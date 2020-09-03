/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna

// added and modified: padhye


#include "DakotaResultsCalibration.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>

#include <QTabWidget>
#include <QTextEdit>
#include <MyTableWidget.h>
#include <QDebug>
#include <QHBoxLayout>
#include <QColor>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
using namespace QtCharts;
#include <math.h>
#include <QValueAxis>
#include <QPushButton>
#include <QFileDialog>

#include <QXYSeries>
#define NUM_DIVISIONS 10


DakotaResultsCalibration::DakotaResultsCalibration(QWidget *parent)
    : UQ_Results(parent)
{
    // title & add button
    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget,1);
    mLeft = true;
    col1 = 0;
    col2 = 0;

    summary = new QWidget();
    summaryLayout = new QVBoxLayout();
    summaryLayout->setContentsMargins(0,0,0,0);
    summary->setLayout(summaryLayout);
    tabWidget->addTab(summary,tr("Summary"));

    dakotaText = new QTextEdit();
    dakotaText->setReadOnly(true); // make it so user cannot edit the contents
    tabWidget->addTab(dakotaText, tr("General"));


     spreadsheet = new MyTableWidget();
     spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);
     connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));

     spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);
     connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));

     chart = new QChart();
     chart->setAnimationOptions(QChart::AllAnimations);

     QChartView *chartView = new QChartView(chart);
     chartView->setRenderHint(QPainter::Antialiasing);
     chartView->chart()->legend()->hide();

     //
     // create a widget into which we place the chart and the spreadsheet
     //

     QWidget *widget = new QWidget();
     QGridLayout *layout = new QGridLayout(widget);
     QPushButton* save_spreadsheet = new QPushButton();
     save_spreadsheet->setText("Save Data");
     save_spreadsheet->setToolTip(tr("Save data into file in a CSV format"));
     save_spreadsheet->resize(30,30);
     connect(save_spreadsheet,SIGNAL(clicked()),this,SLOT(onSaveSpreadsheetClicked()));

     layout->setContentsMargins(0,0,0,0);
     layout->setSpacing(3);

     layout->addWidget(chartView, 0,0,1,1);
     layout->addWidget(save_spreadsheet,1,0,Qt::AlignLeft);
     layout->addWidget(spreadsheet,2,0,1,1);

     tabWidget->addTab(widget, tr("Data Values"));
}

DakotaResultsCalibration::~DakotaResultsCalibration()
{

}


void DakotaResultsCalibration::clear(void)
{
    /*
    QWidget *res=tabWidget->widget(0);
    QWidget *gen=tabWidget->widget(0);
    QWidget *dat=tabWidget->widget(0);

    tabWidget->clear();
    delete dat;
    delete gen;
    delete res;
    */
    spreadsheet->clear();
    dakotaText->clear();

}

bool
DakotaResultsCalibration::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    int numEDP = theNames.count();

    // quick return .. noEDP -> no analysis done -> no results out
    if (numEDP == 0)
      return true;

    jsonObject["resultType"]=QString(tr("DakotaResultsCalibration"));

    //
    // add summary data
    //

    QJsonArray resultsData;
    for (int i=0; i<numEDP; i++) {
        QJsonObject edpData;
        edpData["name"]=theNames.at(i);
        edpData["best_value"]=theBestValues.at(i);
        resultsData.append(edpData);
    }
    jsonObject["summary"]=resultsData;

    // add general data
    jsonObject["general"]=dakotaText->toPlainText();

    //
    // add spreadsheet data
    //

    QJsonObject spreadsheetData;

    int numCol = spreadsheet->columnCount();
    int numRow = spreadsheet->rowCount();

    spreadsheetData["numRow"]=numRow;
    spreadsheetData["numCol"]=numCol;

    QJsonArray headingsArray;
    for (int i = 0; i <theHeadings.size(); ++i) {
        headingsArray.append(QJsonValue(theHeadings.at(i)));
    }

    spreadsheetData["headings"]=headingsArray;

    QJsonArray dataArray;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int row = 0; row < numRow; ++row) {
        for (int column = 0; column < numCol; ++column) {
            QTableWidgetItem *theItem = spreadsheet->item(row,column);
            QString textData = theItem->text();
            dataArray.append(textData.toDouble());
        }
    }
    QApplication::restoreOverrideCursor();
    spreadsheetData["data"]=dataArray;

    jsonObject["spreadsheet"] = spreadsheetData;
    return result;
}


bool
DakotaResultsCalibration::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;
    this->clear();

    if (!jsonObject.contains("summary")) { // no saving of analysis data
       return true;
    }

    //
    // create a summary widget in which place basic output (name, mean, stdDev)
    //

    QJsonArray edpArray = jsonObject["summary"].toArray();
    foreach (const QJsonValue &edpValue, edpArray) {
        QString name;
        double bestValue;
        QJsonObject edpObject = edpValue.toObject();
        QJsonValue theNameValue = edpObject["name"];
        name = theNameValue.toString();

        QJsonValue theBestValue = edpObject["best_value"];
        bestValue = theBestValue.toDouble();

        QWidget *theWidget = this->createResultParameterWidget(name, bestValue);
        summaryLayout->addWidget(theWidget);
    }
    summaryLayout->addStretch();

    //
    // into dakotaText place more detailed Dakota text
    //

    QJsonValue theValue = jsonObject["general"];
    dakotaText->setText(theValue.toString());

    //
    // into a spreadsheet place all the data returned
    //

    QJsonObject spreadsheetData = jsonObject["spreadsheet"].toObject();
    int numRow = spreadsheetData["numRow"].toInt();
    int numCol = spreadsheetData["numCol"].toInt();
    spreadsheet->setColumnCount(numCol);
    spreadsheet->setRowCount(numRow);

    QJsonArray headingData= spreadsheetData["headings"].toArray();
    for (int i=0; i<numCol; i++) {
        theHeadings << headingData.at(i).toString();
    }

    spreadsheet->setHorizontalHeaderLabels(theHeadings);

    QJsonArray dataData= spreadsheetData["data"].toArray();
    int dataCount =0;
    for (int row =0; row<numRow; row++) {
        for (int col=0; col<numCol; col++) {
            QModelIndex index = spreadsheet->model()->index(row, col);
            spreadsheet->model()->setData(index, dataData.at(dataCount).toDouble());
            dataCount++;
        }
    }

    //
    // create a chart, setting data points from first and last col of spreadsheet
    //

    col1 = 0;
    col2 = numCol-1;
    mLeft = true;
    this->onSpreadsheetCellClicked(0,numCol-1);

    tabWidget->adjustSize();
    return result;
}


static void merge_helper(double *input, int left, int right, double *scratch)
{
    // if one element: done  else: recursive call and then merge
    if(right == left + 1) {
        return;
    } else {
        int length = right - left;
        int midpoint_distance = length/2;
        /* l and r are to the positions in the left and right subarrays */
        int l = left, r = left + midpoint_distance;

        // sort each subarray
        merge_helper(input, left, left + midpoint_distance, scratch);
        merge_helper(input, left + midpoint_distance, right, scratch);

        // merge the arrays together using scratch for temporary storage
        for(int i = 0; i < length; i++) {
            /* Check to see if any elements remain in the left array; if so,
            * we check if there are any elements left in the right array; if
            * so, we compare them.  Otherwise, we know that the merge must
            * use take the element from the left array */
            if(l < left + midpoint_distance &&
                    (r == right || fmin(input[l], input[r]) == input[l])) {
                scratch[i] = input[l];
                l++;
            } else {
                scratch[i] = input[r];
                r++;
            }
        }
        // Copy the sorted subarray back to the input
        for(int i = left; i < right; i++) {
            input[i] = scratch[i - left];
        }
    }
}

static int mergesort(double *input, int size)
{
    double *scratch = new double[size];
    if(scratch != NULL) {
        merge_helper(input, 0, size, scratch);
        delete [] scratch;
        return 1;
    } else {
        return 0;
    }
}

int DakotaResultsCalibration::processResults(QString &filenameResults, QString &filenameTab) {


    //
    // open Dakota output file
    //


    std::ifstream fileResults(filenameResults.toStdString().c_str());
    if (!fileResults.is_open()) {
        qDebug() << "Could not open file: " << filenameResults;
        return -1;
    }

    // now ignore every line until Best Parameters

    const std::string needle = "Best parameters          =";
    const std::string needle2 = "Best residual term";
    const std::string needle3 = "Original";
    std::string haystack;

    while (std::getline(fileResults, haystack)) {
        if (haystack.find(needle) != std::string::npos) {
            break;
        }
    }

    //
    // now copy line and every subsequent line into text editor
    //  - also if still collecting sumary data, add each EDP's sumary info
    //

    dakotaText->setText("\n");
    dakotaText->append(haystack.c_str());

    bool isSummaryDone = false;

    while (std::getline(fileResults, haystack)) {
        std::cerr << haystack << "\n";
        dakotaText->append(haystack.c_str());
        if (isSummaryDone == false) {

            if (haystack.find(needle2) != std::string::npos || haystack.find(needle3) != std::string::npos) {
                isSummaryDone = true;
            } else {
                //
                // add sumary info
                //

                std::istringstream iss(haystack);
                std::string subs;

                iss >> subs;
                QString bestText(QString::fromStdString(subs));
                double best = bestText.toDouble();

                iss >> subs;
                QString  nameString(QString::fromStdString(subs));

                QWidget *theWidget = this->createResultParameterWidget(nameString, best);
                summaryLayout->addWidget(theWidget);
            }
        }
    }
    summaryLayout->addStretch();

    // close input file
    fileResults.close();

    //
    // now into a QTableWidget copy the random variable and edp's of each black box run
    //

    //spreadsheet = new MyTableWidget();

    // open file containing tab data
    std::ifstream tabResults(filenameTab.toStdString().c_str());
    if (!tabResults.is_open()) {
        qDebug() << "Could not open file";
        return -1;
    }

    //
    // read first line and set headings (ignoring second column for now)
    //

    std::string inputLine;
    std::getline(tabResults, inputLine);
    std::istringstream iss(inputLine);
    int colCount = 0;
    theHeadings << "Run #";
    do
    {
        std::string subs;
        iss >> subs;
        if (colCount > 1) {
            if (subs != " ") {
                theHeadings << subs.c_str();
            }
        }
        colCount++;
    } while (iss);

    colCount = colCount-2;
    spreadsheet->setColumnCount(colCount);
    spreadsheet->setHorizontalHeaderLabels(theHeadings);

    // now until end of file, read lines and place data into spreadsheet

    int rowCount = 0;
    while (std::getline(tabResults, inputLine)) {
        std::istringstream is(inputLine);
        int col=0;
        spreadsheet->insertRow(rowCount);
        for (int i=0; i<colCount+2; i++) {
            std::string data;
            is >> data;
            if (i != 1) {
                QModelIndex index = spreadsheet->model()->index(rowCount, col);
                spreadsheet->model()->setData(index, data.c_str());
                col++;
            }
        }
        rowCount++;
    }
    tabResults.close();


    this->onSpreadsheetCellClicked(0,colCount-1);


    tabWidget->adjustSize();

    return 0;
}

void
DakotaResultsCalibration::onSpreadsheetCellClicked(int row, int col)
{
    mLeft = spreadsheet->wasLeftKeyPressed();

    // create a new series
    chart->removeAllSeries();
    //chart->removeA
    QAbstractAxis *oldAxisX=chart->axisX();
    if (oldAxisX != 0)
        chart->removeAxis(oldAxisX);
    QAbstractAxis *oldAxisY=chart->axisY();
    if (oldAxisY != 0)
        chart->removeAxis(oldAxisY);


    // QScatterSeries *series;//= new QScatterSeries;

    int oldCol;
    if (mLeft == true) {
        oldCol= col2;
        col2 = col;
    } else {
        oldCol= col1;
        col1 = col;
    }

    int rowCount = spreadsheet->rowCount();
    if (col1 != col2) {
        QScatterSeries *series = new QScatterSeries;

        // adjust marker size and opacity based on the number of samples
        if (rowCount < 10) {
            series->setMarkerSize(15.0);
            series->setColor(QColor(0, 114, 178, 200));
        } else if (rowCount < 100) {
            series->setMarkerSize(11.0);
            series->setColor(QColor(0, 114, 178, 160));
        } else if (rowCount < 1000) {
            series->setMarkerSize(8.0);
            series->setColor(QColor(0, 114, 178, 100));
        } else if (rowCount < 10000) {
            series->setMarkerSize(6.0);
            series->setColor(QColor(0, 114, 178, 70));
        } else if (rowCount < 100000) {
            series->setMarkerSize(5.0);
            series->setColor(QColor(0, 114, 178, 50));
        } else {
            series->setMarkerSize(4.5);
            series->setColor(QColor(0, 114, 178, 30));
        }
        
        series->setBorderColor(QColor(255,255,255,0));

        for (int i=0; i<rowCount; i++) {
            QTableWidgetItem *itemX = spreadsheet->item(i,col1);
            QTableWidgetItem *itemY = spreadsheet->item(i,col2);
            QTableWidgetItem *itemOld = spreadsheet->item(i,oldCol);
            itemOld->setData(Qt::BackgroundRole, QColor(Qt::white));
            itemX->setData(Qt::BackgroundRole, QColor(Qt::lightGray));
            itemY->setData(Qt::BackgroundRole, QColor(Qt::lightGray));

            series->append(itemX->text().toDouble(), itemY->text().toDouble());
        }
        chart->addSeries(series);

        QValueAxis *axisX = new QValueAxis();
        QValueAxis *axisY = new QValueAxis();

        axisX->setTitleText(theHeadings.at(col1));
        axisY->setTitleText(theHeadings.at(col2));
        axisY->setLabelFormat("%e");

        //padhye adding ranges 8/25/2018
        // finding the range for X and Y axis
        // now the axes will look a bit clean.


        double minX, maxX;
        double minY, maxY;

        for (int i=0; i<rowCount; i++) {
            QTableWidgetItem *itemX = spreadsheet->item(i,col1);
            QTableWidgetItem *itemY = spreadsheet->item(i,col2);
            double value1 = itemX->text().toDouble();
            double value2 = itemY->text().toDouble();
            if (i == 0) {
                minX=value1;
                maxX=value1;
                minY=value2;
                maxY=value2;
                        }
            if(value1<minX){minX=value1;}
            if(value1>maxX){maxX=value1;}
            if(value2<minY){minY=value2;}
            if(value2>maxY){maxY=value2;}
        }

        // if value is constant, adjust axes
        if (minX==maxX) {
            double axisMargin=abs(minX)*0.1;
            minX=minX-axisMargin;
            maxX=maxX+axisMargin;
        }
        if (minY==maxY) {
            double axisMargin=abs(minY)*0.1;
            minY=minY-axisMargin;
            maxY=maxY+axisMargin;
        }

        double xRange=maxX-minX;
        double yRange=maxY-minY;

        if(col1!=0)
        {
        axisX->setRange(minX - 0.01*xRange, maxX + 0.1*xRange);
        }
        else{

        axisX->setRange(int (minX - 1), int (maxX +1));
       // axisX->setTickCount(1);

        }

        // adjust y with some fine precision
        axisY->setRange(minY - 0.2*yRange, maxY + 0.2*yRange);


        chart->setAxisX(axisX, series);
        chart->setAxisY(axisY, series);

    } else {

        QLineSeries *series= new QLineSeries;

        static double NUM_DIVISIONS_FOR_DIVISION = 10.0;
        double *dataValues = new double[rowCount];
        double histogram[NUM_DIVISIONS];
        for (int i=0; i<NUM_DIVISIONS; i++)
            histogram[i] = 0;

        double min = 0;
        double max = 0;
        for (int i=0; i<rowCount; i++) {
            QTableWidgetItem *itemX = spreadsheet->item(i,col1);
            QTableWidgetItem *itemOld = spreadsheet->item(i,oldCol);
            itemOld->setData(Qt::BackgroundRole, QColor(Qt::white));
            itemX->setData(Qt::BackgroundRole, QColor(Qt::lightGray));
            double value = itemX->text().toDouble();
            dataValues[i] =  value;

            if (i == 0) {
                min = value;
                max = value;
            } else if (value < min) {
                min = value;
            } else if (value > max) {
                max = value;
            }
        }
        if (mLeft == true) {

            // frequency distribution
            double range = max-min;
            double dRange = range/NUM_DIVISIONS_FOR_DIVISION;

            for (int i=0; i<rowCount; i++) {
                // compute block belongs to, watch under and overflow due to numerics
                int block = floor((dataValues[i]-min)/dRange);
                if (block < 0) block = 0;
                if (block > NUM_DIVISIONS-1) block = NUM_DIVISIONS-1;
                histogram[block] += 1;
            }

            double maxPercent = 0;
            for (int i=0; i<NUM_DIVISIONS; i++) {
                histogram[i]/rowCount;
                if (histogram[i] > maxPercent)
                    maxPercent = histogram[i];
            }
            for (int i=0; i<NUM_DIVISIONS; i++) {
                series->append(min+i*dRange, 0);
                series->append(min+i*dRange, histogram[i]);
                series->append(min+(i+1)*dRange, histogram[i]);
                series->append(min+(i+1)*dRange, 0);
            }

            delete [] dataValues;

            chart->addSeries(series);
            QValueAxis *axisX = new QValueAxis();
            QValueAxis *axisY = new QValueAxis();
            axisY->setLabelFormat("%.2f");


            axisX->setRange(min, max);
            axisY->setRange(0, maxPercent);
            axisY->setTitleText("Frequency %");
            axisX->setTitleText(theHeadings.at(col1));
            axisX->setTickCount(NUM_DIVISIONS+1);
            chart->setAxisX(axisX, series);
            chart->setAxisY(axisY, series);
    } else {

            // cumulative distributionn
            mergesort(dataValues, rowCount);

            for (int i=0; i<rowCount; i++) {
                series->append(dataValues[i], 1.0*i/rowCount);
            }

            delete []dataValues;

            chart->addSeries(series);
            QValueAxis *axisX = new QValueAxis();
            QValueAxis *axisY = new QValueAxis();
            axisY->setLabelFormat("%.2f");


            axisX->setRange(min, max);
            axisY->setRange(0, 1);
            axisY->setTitleText("Cumulative Probability");
            axisX->setTitleText(theHeadings.at(col1));
            axisX->setTickCount(NUM_DIVISIONS+1);
            chart->setAxisX(axisX, series);
            chart->setAxisY(axisY, series);
        }


    }
}

void
DakotaResultsCalibration::onSaveSpreadsheetClicked()
{

    int rowCount = spreadsheet->rowCount();
    int columnCount = spreadsheet->columnCount();

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Data"), "",
                                                    tr("All Files (*)"));

    QFile file(fileName);
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        for (int j=0; j<columnCount; j++)
        {
            stream <<theHeadings.at(j)<<", ";
        }
        stream <<endl;
        for (int i=0; i<rowCount; i++)
        {
            for (int j=0; j<columnCount; j++)
            {
                QTableWidgetItem *item_value = spreadsheet->item(i,j);
                double value = item_value->text().toDouble();
                stream << value << ", ";
            }
            stream<<endl;
        }
    }
}

extern QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit);

QWidget *
DakotaResultsCalibration::createResultParameterWidget(QString &name, double bestValue) {
    QWidget *edp = new QWidget;
    QHBoxLayout *edpLayout = new QHBoxLayout();
    edpLayout->setContentsMargins(0,0,0,0);
    edpLayout->setSpacing(3);

    edp->setLayout(edpLayout);

    QLineEdit *nameLineEdit;
    QWidget *nameWidget = addLabeledLineEdit(QString("Name"), &nameLineEdit);
    nameLineEdit->setText(name);
    nameLineEdit->setDisabled(true);
    theNames.append(name);
    edpLayout->addWidget(nameWidget);

    QLineEdit *meanLineEdit;
    QWidget *meanWidget = addLabeledLineEdit(QString("Best Parameter"), &meanLineEdit);
    meanLineEdit->setText(QString::number(bestValue));
    meanLineEdit->setDisabled(true);
    theBestValues.append(bestValue);
    edpLayout->addWidget(meanWidget);

    edpLayout->addStretch();

    return edp;
}
