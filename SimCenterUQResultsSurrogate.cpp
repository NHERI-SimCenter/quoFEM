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

#include "SimCenterUQResultsSurrogate.h"
//#include "InputWidgetFEM.h"
#include "qbarset.h"
#include <QBarCategoryAxis>
#include <QBarSeries>

#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>

#include <QFileDialog>
#include <QTabWidget>
#include <QTextEdit>
#include <MyTableWidget.h>
#include <QDebug>
#include <QHBoxLayout>
#include <QColor>
#include <QMenuBar>
#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QProcess>
#include <QScrollArea>
#include <QJsonDocument>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

//#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>
//#include <InputWidgetFEM.h>
#include <InputWidgetUQ.h>
#include <MainWindow.h>

//#include <InputWidgetFEM.h>
#include <InputWidgetUQ.h>
#include <MainWindow.h>
#include <QHeaderView>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
using namespace QtCharts;
#include <math.h>
#include <QValueAxis>

#include <QXYSeries>
#include <RandomVariablesContainer.h>

#define NUM_DIVISIONS 10



//QLabel *best_fit_label_text;


SimCenterUQResultsSurrogate::SimCenterUQResultsSurrogate(RandomVariablesContainer *theRandomVariables, QWidget *parent)
    : UQ_Results(parent), theRVs(theRandomVariables)
{
    // title & add button
    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget,1);
    mLeft = true;
    col1 = 0;
    col2 = 0;
}

SimCenterUQResultsSurrogate::~SimCenterUQResultsSurrogate()
{

}


void SimCenterUQResultsSurrogate::clear(void)
{
    // delete any existing widgets
    int count = tabWidget->count();
    if (count > 0) {
        for (int i=0; i<count; i++) {
            QWidget *theWidget = tabWidget->widget(count);
            delete theWidget;
        }
    }
    theHeadings.clear();
    theMeans.clear();
    theStdDevs.clear();
    theKurtosis.clear();

    tabWidget->clear();

    spreadsheet = NULL;

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

// if sobelov indices are selected then we would need to do some processing outselves

int SimCenterUQResultsSurrogate::processResults(QString &filenameResults, QString &filenameTab)
{
    emit sendStatusMessage(tr("Processing Results ... "));

    this->clear();
    mLeft = true;
    col1 = 0;
    col2 = 0;
    lastPath = "";
    //
    // check it actually ran with errors
    //

    QFileInfo fileTabInfo(filenameTab);
    QString filenameErrorString = fileTabInfo.absolutePath() + QDir::separator() + QString("dakota.err");
    workingDir=fileTabInfo.absolutePath()+ QDir::separator();

    QFileInfo filenameErrorInfo(filenameErrorString);
    if (!filenameErrorInfo.exists()) {
        emit sendErrorMessage("No dakota.err file - SimCenterUQ did not run - problem with dakota setup or the applicatins failed with inputs provied");
        return 0;
    }
    QFile fileError(filenameErrorString);
    QString line("");
    if (fileError.open(QIODevice::ReadOnly)) {
        QTextStream in(&fileError);
        while (!in.atEnd()) {
            line = in.readLine();
        }
        fileError.close();
    }

    if (line.length() != 0) {
        qDebug() << line.length() << " " << line;
        emit sendErrorMessage(QString(QString("Error Running SimCenterUQ: ") + line));
        return 0;
    }

    QFileInfo filenameTabInfo(filenameTab);
    if (!filenameTabInfo.exists()) {
        emit sendErrorMessage("No dakotaTab.out file - dakota failed .. possibly no QoI or a permission issue. Check out Jobs Directory");
        return 0;
    }

    //
    // create summary, a QWidget for summary data, the EDP name, mean, stdDev, kurtosis info
    //

    // create a scrollable windows, place summary inside it
    QScrollArea *sa = new QScrollArea;

    //
    // read data from file filename
    //

    //
    // open file
    //

    QFile file(filenameResults);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QString message = QString("Error: could not open file") + filenameResults;
    }

    // place contents of file into json object
    QString val;
    val=file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    jsonObj = doc.object();

    // close file
    file.close();

    // check file contains valid object
    if (jsonObj.isEmpty()) {
        QString message = QString("ERROR: file either empty or malformed JSON");
    }

    summarySurrogate(*&sa);

    //QHBoxLayout *gsaLayout = new QHBoxLayout();

    //
    // create spreadsheet,  a QTableWidget showing RV and results for each run
    //

    spreadsheet = new MyTableWidget();

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

    bool includesInterface = false;
    do {
        std::string subs;
        iss >> subs;
        if (colCount > 0) {
            //if (subs != " ") {
                //if (subs != "interface")
                    theHeadings << subs.c_str();
                //else
                    //includesInterface = true;
            //}
        }
        colCount++;
    } while (iss);

    if (includesInterface == true)
        colCount = colCount-2;
    else
        colCount = colCount -1;

    spreadsheet->setColumnCount(colCount);
    spreadsheet->setHorizontalHeaderLabels(theHeadings);
    spreadsheet->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    spreadsheet->verticalHeader()->setVisible(false);

    // now until end of file, read lines and place data into spreadsheet

    int rowCount = 0;
    while (std::getline(tabResults, inputLine)) {
        std::istringstream is(inputLine);
        int col=0;
        //qDebug()<<"\n the inputLine is        "<<inputLine.c_str();

        spreadsheet->insertRow(rowCount);
        for (int i=0; i<colCount+2; i++) {
            //std::string data;
            double data;
            is >> data;
            //if ((includesInterface == true && i != 1) || (includesInterface == false)) {
                QModelIndex index = spreadsheet->model()->index(rowCount, col);
                //spreadsheet->model()->setData(index, data.c_str());
                spreadsheet->model()->setData(index, QString::number(data));
                col++;
            //}
        }
        rowCount++;
    }
    tabResults.close();

    // this is where we are connecting edit triggers
    spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));

    //
    // create a chart, setting data points from first and last col of spreadsheet
    //

    chart = new QChart();
    chart->setAnimationOptions(QChart::AllAnimations);

    // by default the constructor is called and it plots the graph of the last column on Y-axis w.r.t first column on the
    // X-axis
    this->onSpreadsheetCellClicked(0,colCount-1);

    // to control the properties, how your graph looks you must click and study the onSpreadsheetCellClicked

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->chart()->legend()->hide();


    QWidget *widget = new QWidget();
    QGridLayout *layout = new QGridLayout(widget);
    QPushButton* save_spreadsheet = new QPushButton();
    save_spreadsheet->setText("Save Data");
    save_spreadsheet->setToolTip(tr("Save data into file in a CSV format"));
    save_spreadsheet->resize(30,30);
    connect(save_spreadsheet,SIGNAL(clicked()),this,SLOT(onSaveSpreadsheetClicked()));

    layout->addWidget(chartView, 0,0,1,1);
    layout->addWidget(save_spreadsheet,1,0,Qt::AlignLeft);
    layout->addWidget(spreadsheet,2,0,1,1);

    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(widget, tr("Data Values"));
    tabWidget->adjustSize();

    emit sendStatusMessage(tr(""));

    return 0;
}


void
SimCenterUQResultsSurrogate::onSaveSpreadsheetClicked()
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
                //     qDebug()<<value;
            }
            stream<<endl;
        }
    }

    lastPath =  QFileInfo(fileName).path();

}

void
SimCenterUQResultsSurrogate::onSaveModelClicked()
{


    QString fileName = QFileDialog::getSaveFileName(this,
                                                   tr("Save Data"), lastPath+"/SimGpModel",
                                                   tr("Pickle File (*.pkl)"));
    QString fileName2 = fileName;

    fileName2.replace(".pkl",".json");

    QFileInfo fileInfo(fileName);
    QString path = fileInfo.absolutePath();

    QFile::copy(workingDir+QString("SimGpModel.pkl"), fileName);
    QFile::copy(workingDir+QString("dakota.out"), fileName2);
    //QFile::copy(workingDir+QString("templatedir"), path+"templatedir_SIM");

    QString workflowDir1 = workingDir+QString("templatedir");
    QString workflowDir2 = path+QString("/templatedir_SIM");
    bool directoryCopied = copyPath(workflowDir1, workflowDir2, true);

    lastPath =  QFileInfo(fileName).path();

}

void
SimCenterUQResultsSurrogate::onSaveInfoClicked()
{

    QString fileName = QFileDialog::getSaveFileName(this,
                                                   tr("Save Data"), lastPath+"/GPresults",
                                                   tr("Text File (*.out)"));
    QFile::copy(workingDir+QString("GPresults.out"), fileName);
    lastPath =  QFileInfo(fileName).path();
}

void
SimCenterUQResultsSurrogate::onSaveXClicked()
{

    QString fileName = QFileDialog::getSaveFileName(this,
                                                   tr("Save Data"), lastPath+"/X",
                                                   tr("Text File (*.txt)"));
    QFile::copy(workingDir+QString("inputTab.out"), fileName);
    lastPath =  QFileInfo(fileName).path();
}

void
SimCenterUQResultsSurrogate::onSaveYClicked()
{

    QString fileName = QFileDialog::getSaveFileName(this,
                                                   tr("Save Data"), lastPath+"/Y",
                                                   tr("Output File (*.txt)"));
    QFile::copy(workingDir+QString("outputTab.out"), fileName);
    lastPath =  QFileInfo(fileName).path();
}


void SimCenterUQResultsSurrogate::onSpreadsheetCellClicked(int row, int col)
{
    mLeft = spreadsheet->wasLeftKeyPressed();

    //  best_fit_instructions->clear();
    //see the file MyTableWiget.cpp in order to find the function wasLeftKeyPressed();
    //qDebug()<<"\n the value of mLeft       "<<mLeft;
    //qDebug()<<"\n I am inside the onSpreadsheetCellClicked routine  and I am exiting!!  ";
    //  exit(1);
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
        oldCol= col2;   //
        //oldCol=0;   // padhye trying. I don't think it makes sense to use coldCol as col2, i.e., something that was
        // previously selected?
        col2 = col; // col is the one that comes in te function, based on the click made after clicking

        //    qDebug()<<"\n the value of oldcol is  "<<oldCol;
        //    qDebug()<<"\n the value of col2 is    "<<col2;
        //    qDebug()<<"\t the value of col is     "<<col;

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
            QTableWidgetItem *itemX = spreadsheet->item(i,col1);    //col1 goes in x-axis, col2 on y-axis
            //col1=0;
            QTableWidgetItem *itemY = spreadsheet->item(i,col2);
            QTableWidgetItem *itemOld = spreadsheet->item(i,oldCol);
            itemOld->setData(Qt::BackgroundRole, QColor(Qt::white));
            itemX->setData(Qt::BackgroundRole, QColor(Qt::lightGray));
            itemY->setData(Qt::BackgroundRole, QColor(Qt::lightGray));

            series->append(itemX->text().toDouble(), itemY->text().toDouble());
        }
        chart->addSeries(series);
        series->setName("Samples");

        QValueAxis *axisX = new QValueAxis();
        QValueAxis *axisY = new QValueAxis();

        axisX->setTitleText(theHeadings.at(col1));
        axisY->setTitleText(theHeadings.at(col2));

        // padhye adding ranges 8/25/2018
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

        //  qDebug()<<"\n the value of xRange is     ";
        //qDebug()<<xRange;

        //qDebug()<<"\n the value of yRange is     ";
        //qDebug()<<yRange;

        // if the column is not the run number, i.e., 0 column, then adjust the x-axis differently

        if(col1!=0)
        {
            axisX->setRange(minX - 0.01*xRange, maxX + 0.1*xRange);
        }
        else{

            axisX->setRange(int (minX - 1), int (maxX +1));
            // axisX->setTickCount(1);

        }
        // adjust y with some fine precision
        axisY->setRange(minY - 0.1*yRange, maxY + 0.1*yRange);
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
                //histogram[i]/rowCount;
                if (histogram[i] > maxPercent)
                    maxPercent = histogram[i];
            }
            for (int i=0; i<NUM_DIVISIONS; i++) {
                series->append(min+i*dRange, 0);
                series->append(min+i*dRange, histogram[i]);
                series->append(min+(i+1)*dRange, histogram[i]);
                series->append(min+(i+1)*dRange, 0);
            }


            chart->addSeries(series);
            series->setName("Histogram");

            QValueAxis *axisX = new QValueAxis();
            QValueAxis *axisY = new QValueAxis();

            axisX->setRange(min-(max-min)*.1, max+(max-min)*.1);
            axisY->setRange(0, 1.1*maxPercent);
            axisY->setTitleText("Frequency %");
            axisX->setTitleText(theHeadings.at(col1));
            axisX->setTickCount(NUM_DIVISIONS+1);
            chart->setAxisX(axisX, series);
            chart->setAxisY(axisY, series);

        } else {
            // cumulative distribution
            mergesort(dataValues, rowCount);
            series->append(min-(max-min)*0.1, 0.0);
            for (int i=0; i<rowCount-1; i++) {
                series->append(dataValues[i], 1.0*(i)/rowCount);
                series->append(dataValues[i+1], 1.0*(i)/rowCount);
                //qDebug()<<"\n the dataValues[i] and rowCount is     "<<dataValues[i]<<"\t"<<rowCount<<"";
            }
            series->append(dataValues[rowCount-1], 1.0);
            series->append(max+(max-min)*0.1, 1.0);

            delete [] dataValues;
            chart->addSeries(series);
            QValueAxis *axisX = new QValueAxis();
            QValueAxis *axisY = new QValueAxis();
            // padhye, make these consistent changes all across.
            axisX->setRange(min-(max-min)*0.1, max+(max-min)*0.1);
            axisY->setRange(0, 1);
            axisY->setTitleText("Cumulative Probability");
            axisX->setTitleText(theHeadings.at(col1));
            axisX->setTickCount(NUM_DIVISIONS+1);
            chart->setAxisX(axisX, series);
            chart->setAxisY(axisY, series);
            series->setName("Cumulative Frequency Distribution");
        }
    }
}


// padhye
// this function is called if you decide to say save the data from UI into a json object
bool
SimCenterUQResultsSurrogate::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["resultType"]=QString(tr("SimCenterUQResultsSurrogate"));

    if (this->spreadsheet == nullptr)
        return true;
    //
    // add summary data
    //

    jsonObject["summary"] = jsonObj;
    /*
    QJsonArray resultsData;
    int numEDP = theNames.count();
    for (int i=0; i<numEDP; i++) {
        QJsonObject edpData;
        edpData["name"]=theNames.at(i);
        edpData["mean"]=theMeans.at(i);
        edpData["stdDev"]=theStdDevs.at(i);
        edpData["kurtosis"]=theKurtosis.at(i);
        resultsData.append(edpData);
    }

    jsonObject["summary"]=resultsData;
    */
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



// if you already have a json data file then you can populate the UI with the entries from json.

bool
SimCenterUQResultsSurrogate::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;

    this->clear();

    QJsonValue theNameValue = jsonObject["spreadsheet"];
    if (theNameValue.isNull())
        return true;

    //
    // create a summary widget in which place basic output (name, mean, stdDev)
    //

    jsonObj = jsonObject["summary"].toObject();

    QScrollArea *sa = new QScrollArea;
    summarySurrogate(*&sa);
    saveModelButton ->setDisabled(true);
    saveResultButton ->setDisabled(true);
    saveXButton->setDisabled(true);
    saveYButton->setDisabled(true);
    saveModelButton ->setStyleSheet({ "background-color: lightgrey; border: none;" });
    saveResultButton ->setStyleSheet({ "background-color: lightgrey; border: none;" });
    saveXButton ->setStyleSheet({ "background-color: lightgrey; border: none;" });
    saveYButton ->setStyleSheet({ "background-color: lightgrey; border: none;" });

    /*
    QWidget *summary = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summary->setLayout(summaryLayout);

    QJsonArray edpArray = jsonObject["summary"].toArray();
    foreach (const QJsonValue &edpValue, edpArray) {
        QString name;
        double mean, stdDev;
        QJsonObject edpObject = edpValue.toObject();
        QJsonValue theNameValue = edpObject["name"];
        name = theNameValue.toString();

        QJsonValue theMeanValue = edpObject["mean"];
        mean = theMeanValue.toDouble();

        QJsonValue theStdDevValue = edpObject["stdDev"];
        stdDev = theStdDevValue.toDouble();

        QJsonValue theKurtosis = edpObject["kurtosis"];
        double kurtosis = theKurtosis.toDouble();

        QWidget *theWidget = this->createResultEDPWidget(name, mean, stdDev, kurtosis);
        summaryLayout->addWidget(theWidget);
    }
    summaryLayout->addStretch();
    */

    //
    // into a spreadsheet place all the data returned
    //

    spreadsheet = new MyTableWidget();
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
    spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));

    //
    // create a chart, setting data points from first and last col of spreadsheet
    //

    chart = new QChart();
    chart->setAnimationOptions(QChart::AllAnimations);
    //QScatterSeries *series = new QScatterSeries;
    col1 = 0;           // col1 is initialied as the first column in spread sheet
    col2 = numCol-1;    // col2 is initialized as the second column in spread sheet
    mLeft = true;       // left click

    this->onSpreadsheetCellClicked(0,numCol-1);

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

    layout->addWidget(chartView, 0,0,1,1);
    layout->addWidget(save_spreadsheet,1,0,Qt::AlignLeft);
    layout->addWidget(spreadsheet,2,0,1,1);


    //layout->addWidget(analysis_message,1);
    // add 3 Widgets to TabWidget
    //

    //tabWidget->addTab(summary,tr("Summary"));
    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(widget, tr("Data Values"));

    tabWidget->adjustSize();

    //qDebug()<<"\n debugging the values: result is  \n"<<result<<"\n";

    return result;
}


extern QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit);

QWidget *
SimCenterUQResultsSurrogate::createResultEDPWidget(QString &name, double mean, double stdDev, double kurtosis) {
    QWidget *edp = new QWidget;
    QHBoxLayout *edpLayout = new QHBoxLayout();

    edp->setLayout(edpLayout);

    QLineEdit *nameLineEdit;
    QWidget *nameWidget = addLabeledLineEdit(QString("Name"), &nameLineEdit);
    nameLineEdit->setText(name);
    nameLineEdit->setDisabled(true);
    theNames.append(name);
    edpLayout->addWidget(nameWidget);

    QLineEdit *meanLineEdit;
    QWidget *meanWidget = addLabeledLineEdit(QString("Mean"), &meanLineEdit);
    meanLineEdit->setText(QString::number(mean));
    meanLineEdit->setDisabled(true);
    theMeans.append(mean);
    edpLayout->addWidget(meanWidget);

    QLineEdit *stdDevLineEdit;
    QWidget *stdDevWidget = addLabeledLineEdit(QString("StdDev"), &stdDevLineEdit);
    stdDevLineEdit->setText(QString::number(stdDev));
    stdDevLineEdit->setDisabled(true);
    theStdDevs.append(stdDev);
    edpLayout->addWidget(stdDevWidget);

    QLineEdit *kurtosisLineEdit;
    QWidget *kurtosisWidget = addLabeledLineEdit(QString("Kurtosis"), &kurtosisLineEdit);
    kurtosisLineEdit->setText(QString::number(kurtosis));
    kurtosisLineEdit->setDisabled(true);
    theKurtosis.append(kurtosis);
    edpLayout->addWidget(kurtosisWidget);

    edpLayout->addStretch();

    return edp;
}

bool SimCenterUQResultsSurrogate::copyPath(QString sourceDir, QString destinationDir, bool overWriteDirectory)
{
    QDir originDirectory(sourceDir);

    if (! originDirectory.exists())
    {
        return false;
    }

    QDir destinationDirectory(destinationDir);

    if(destinationDirectory.exists() && !overWriteDirectory)
    {
        return false;
    }
    else if(destinationDirectory.exists() && overWriteDirectory)
    {
        destinationDirectory.removeRecursively();
    }

    originDirectory.mkpath(destinationDir);

    foreach (QString directoryName, originDirectory.entryList(QDir::Dirs | \
                                                              QDir::NoDotAndDotDot))
    {
        QString destinationPath = destinationDir + "/" + directoryName;
        originDirectory.mkpath(destinationPath);
        copyPath(sourceDir + "/" + directoryName, destinationPath, overWriteDirectory);
    }

    foreach (QString fileName, originDirectory.entryList(QDir::Files))
    {
        QFile::copy(sourceDir + "/" + fileName, destinationDir + "/" + fileName);
    }

    QDir finalDestination(destinationDir);
    finalDestination.refresh();

    if(finalDestination.exists())
    {
        return true;
    }

    return false;
}

void SimCenterUQResultsSurrogate::summarySurrogate(QScrollArea *&sa)
{

    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    QWidget *summary = new QWidget();
    QGridLayout *summaryLayout = new QGridLayout();
    summaryLayout->setContentsMargins(0,0,0,0); // adding back
    summary->setLayout(summaryLayout);
    sa->setWidget(summary);

    //QJsonObject uqObject = jsonObj["UQ_Method"].toObject();
    int nQoI = jsonObj["ydim"].toInt();
    QJsonArray QoI_tmp = jsonObj["ylabels"].toArray();
    int nSamp = jsonObj["valSamp"].toInt();
    int nSim  = jsonObj["valSim"].toInt();
    double nTime = jsonObj["valTime"].toDouble();
    double NRMSEthr =jsonObj["thrNRMSE"].toDouble();
    QString termCode =jsonObj["terminationCode"].toString();
    QJsonObject valNRMSE = jsonObj["valNRMSE"].toObject();
    QJsonObject valR2 = jsonObj["valR2"].toObject();
    QJsonObject valCorrCoeff = jsonObj["valCorrCoeff"].toObject();
    QJsonObject yExact = jsonObj["yExact"].toObject();
    QJsonObject yPredi = jsonObj["yPredict"].toObject();
    bool isMultiFidelity = jsonObj["doMultiFidelity"].toBool();

    QStringList QoInames;
    foreach (QJsonValue str, QoI_tmp) {
        QoInames << str.toString();
    }

    QGridLayout *resultsLayout = new QGridLayout();

    QString termMsg="\nSurrogate Modeling Completed! ";
    if (termCode=="count"){
        termMsg = termMsg+"- Process ended as the maximum allowable number of simulations is reached";
    } else if (termCode=="time"){
        termMsg = termMsg+"- Process ended as the time limit is exceeded";
    } else if (termCode=="accuracy"){
        termMsg = termMsg+"- Model Converged";
    } else {
        termMsg = termMsg + "termination code unidentified";
    }

    QLabel *surrogateStatusLabel =new QLabel(termMsg);
    surrogateStatusLabel-> setStyleSheet({"font-weight: bold"});
    QFrame *lineA = new QFrame;
    lineA->setFrameShape(QFrame::HLine);
    lineA->setFrameShadow(QFrame::Sunken);

    summaryLayout->addWidget(surrogateStatusLabel, 0, 0,1,2);
    summaryLayout->addWidget(lineA, 1, 0,1,2);
    int idSum=0;
    if (isMultiFidelity) {
        int nSamp_HF = jsonObj["valSamp_HF"].toInt();
        resultsLayout->addWidget(new QLabel("# high-fidelity (HF) samples"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSamp_HF)), idSum++, 1);
        resultsLayout->addWidget(new QLabel("# low-fidelity (LF) samples"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSamp)), idSum++, 1);
        resultsLayout->addWidget(new QLabel("# low-fidelity (LF) model simulations"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSim)), idSum++, 1);
    } else {
        resultsLayout->addWidget(new QLabel("# training samples"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSamp)), idSum++, 1);
        resultsLayout->addWidget(new QLabel("# model simulations"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSim)), idSum++, 1);
    }
    resultsLayout->addWidget(new QLabel("Analysis time"), idSum, 0);
    resultsLayout->addWidget(new QLabel(QString::number(nTime/60, 'f', 1).append(" min.")), idSum++, 1);
    resultsLayout->addWidget(new QLabel(" "), idSum++, 0);

    // blank space

    QLabel *accuMeasureLabel =new QLabel("Goodness-of-Fit");
    accuMeasureLabel-> setStyleSheet({"font-weight: bold"});
    resultsLayout->addWidget(accuMeasureLabel, idSum++, 0);
    resultsLayout->addWidget(new QLabel("Normalized error (NRMSE)"), idSum++, 0);
    resultsLayout->addWidget(new QLabel("R2"), idSum++, 0);
    resultsLayout->addWidget(new QLabel("Correlation coeff"), idSum++, 0);

    QLineEdit *NRMSE;
    QLineEdit *R2;
    QLineEdit *Corr;

    bool warningIdx=false;

    for (int nq=0; nq<nQoI; nq++){
        NRMSE = new QLineEdit();
        R2 = new QLineEdit();
        Corr = new QLineEdit();

        double NRMSEvalue= valNRMSE[QoInames[nq]].toDouble();

        NRMSE -> setText(QString::number(valNRMSE[QoInames[nq]].toDouble(), 'f', 3));
        R2 -> setText(QString::number(valR2[QoInames[nq]].toDouble(), 'f', 3));
        Corr -> setText(QString::number(valCorrCoeff[QoInames[nq]].toDouble(), 'f', 3));

        NRMSE->setAlignment(Qt::AlignRight);
        R2->setAlignment(Qt::AlignRight);
        Corr->setAlignment(Qt::AlignRight);

        NRMSE->setReadOnly(true);
        R2->setReadOnly(true);
        Corr->setReadOnly(true);

        NRMSE ->setMaximumWidth(100);
        R2 ->setMaximumWidth(100);
        Corr ->setMaximumWidth(100);

        if (NRMSEvalue>NRMSEthr) {
            NRMSE -> setStyleSheet({"color: red"});
            R2 -> setStyleSheet({"color: red"});
            Corr -> setStyleSheet({"color: red"});
            warningIdx=true;
        }

        resultsLayout->addWidget(new QLabel(QoInames[nq]), idSum-4, nq+1);
        resultsLayout->addWidget(NRMSE, idSum-3, nq+1);
        resultsLayout->addWidget(R2, idSum-2, nq+1);
        resultsLayout->addWidget(Corr, idSum-1, nq+1);
    }

    idSum += 1;
    if (warningIdx) {
        //surrogateStatusLabel->setText("\nSurrogate analysis finished. - The model may not be accurate");
        QLabel *waringMsgLabel = new QLabel("* Note: Some or all of the QoIs did not converge to the target accuracy (NRMSE="+QString::number(NRMSEthr)+")");
        resultsLayout->addWidget(waringMsgLabel, idSum++, 0,1,-1);
        waringMsgLabel -> setStyleSheet({"color: red"});

    }

    resultsLayout->setRowStretch(idSum, 1);
    resultsLayout->setColumnStretch(nQoI+2, 1);
    summaryLayout->addLayout(resultsLayout,2,0);




    //
    // QScatter plot
    //

    QTabWidget *tabWidgetScatter = new QTabWidget();
    tabWidgetScatter -> setTabPosition(QTabWidget::East);

    int nCVsamps = yExact[QoInames[0]].toArray().size();

    for (int nq=0; nq<nQoI; nq++)
    {
    //int nq=0;
        QScatterSeries *series_CV = new QScatterSeries;
        // adjust marker size and opacity based on the number of samples
        if (nCVsamps < 10) {
            series_CV->setMarkerSize(15.0);
            series_CV->setColor(QColor(0, 114, 178, 200));
        } else if (nCVsamps < 100) {
            series_CV->setMarkerSize(11.0);
            series_CV->setColor(QColor(0, 114, 178, 160));
        } else if (nCVsamps < 1000) {
            series_CV->setMarkerSize(8.0);
            series_CV->setColor(QColor(0, 114, 178, 100));
        } else if (nCVsamps < 10000) {
            series_CV->setMarkerSize(6.0);
            series_CV->setColor(QColor(0, 114, 178, 70));
        } else if (nCVsamps < 100000) {
            series_CV->setMarkerSize(5.0);
            series_CV->setColor(QColor(0, 114, 178, 50));
        } else {
            series_CV->setMarkerSize(4.5);
            series_CV->setColor(QColor(0, 114, 178, 30));
        }

        series_CV->setBorderColor(QColor(255,255,255,0));

        QChart *chart_CV = new QChart;
        QChartView *chartView_CV = new QChartView(chart_CV);

        chart_CV->setAnimationOptions(QChart::AllAnimations);
        chartView_CV->setRenderHint(QPainter::Antialiasing);
        chartView_CV->chart()->legend()->hide();

        QJsonArray yEx= yExact[QoInames[nq]].toArray();
        QJsonArray yPr= yPredi[QoInames[nq]].toArray();
        double maxy=-INFINITY;
        double miny=INFINITY;
        for (int i=0; i<nCVsamps; i++) {
            series_CV->append(yEx[i].toDouble(), yPr[i].toDouble());
            maxy = std::max(maxy,std::max(yEx[i].toDouble(),yPr[i].toDouble()));
            miny = std::min(miny,std::min(yEx[i].toDouble(),yPr[i].toDouble()));
        }
        chart_CV->addSeries(series_CV);
        series_CV->setName("Samples");

        QValueAxis *axisX = new QValueAxis();
        QValueAxis *axisY = new QValueAxis();

        axisX->setTitleText(QString("Exact response"));
        axisY->setTitleText(QString("Predicted response (LOOCV)"));

        axisX->setRange(miny, maxy);
        axisY->setRange(miny, maxy);

        chart_CV->setAxisX(axisX, series_CV);
        chart_CV->setAxisY(axisY, series_CV);

        tabWidgetScatter->addTab(chartView_CV,QoInames[nq]);
    }
    tabWidgetScatter->setMinimumWidth(500);
    tabWidgetScatter->setMinimumHeight(500);
    tabWidgetScatter->setMaximumHeight(500);
    tabWidgetScatter->setMaximumWidth(500);
    QString CVmsg;
    if (isMultiFidelity) {
        CVmsg="\nLeave-One-Out Cross-Validation (LOOCV) Prediction (for the HF sample points)";
    } else {
        CVmsg="\nLeave-One-Out Cross-Validation (LOOCV) Prediction";
    }
    QLabel *CVresultsLabel =new QLabel(CVmsg);
    CVresultsLabel-> setStyleSheet({"font-weight: bold"});
    summaryLayout->addWidget(CVresultsLabel, 3, 0);
    summaryLayout->addWidget(tabWidgetScatter,4,0);

    QLabel *buttonsLabel =new QLabel("\nSaving Options");
    buttonsLabel-> setStyleSheet({"font-weight: bold"});

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    saveModelButton =  new QPushButton("Save GP Model");
    saveResultButton =  new QPushButton("Save GP Info");
    saveXButton =  new QPushButton("RV Data");
    saveYButton =  new QPushButton("QoI Data");

    if (isMultiFidelity) {
        saveXButton->setText("RV Data (LF)");
        saveYButton->setText("QoI Data (LF)");
    }

    connect(saveModelButton,SIGNAL(clicked()),this,SLOT(onSaveModelClicked()));
    connect(saveResultButton,SIGNAL(clicked()),this,SLOT(onSaveInfoClicked()));
    connect(saveXButton,SIGNAL(clicked()),this,SLOT(onSaveXClicked()));
    connect(saveYButton,SIGNAL(clicked()),this,SLOT(onSaveYClicked()));

    saveModelButton->setMinimumWidth(130);
    saveModelButton->setMaximumWidth(130);
    saveResultButton->setMaximumWidth(130);
    saveResultButton->setMinimumWidth(130);
    saveXButton->setMaximumWidth(110);
    saveXButton->setMinimumWidth(110);
    saveYButton->setMaximumWidth(110);
    saveYButton->setMinimumWidth(110);

    buttonsLayout->addWidget(saveModelButton,0,Qt::AlignLeft);
    buttonsLayout->addWidget(saveResultButton,1);
    buttonsLayout->addWidget(saveXButton,2);
    buttonsLayout->addWidget(saveYButton,3);
    buttonsLayout->addStretch(true);
    //buttonsLayout->setStretch(1,1);

    summaryLayout->addWidget(buttonsLabel, 5, 0);
    summaryLayout->addLayout(buttonsLayout, 6, 0,Qt::AlignTop);
    summaryLayout->setRowStretch(7, 1);
    summaryLayout->setColumnStretch(3, 1);

}


