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
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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
#include <QApplication>
#include <math.h>

#include <ResultsDataChart.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include "MyTableWidget.h"
#include <QHeaderView>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <QValueAxis>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
#include <QAreaSeries>
using namespace QtCharts;

ResultsDataChart::ResultsDataChart(QString filenameTab, QWidget *parent)
    : SimCenterWidget(parent)
{
    //QString filenameTab="";
    //auto layout = new QGridLayout();

    col1 = 0;
    col2 = 0;
    spreadsheet = new MyTableWidget();
    this->readTableFromTab(filenameTab);
    this->makeChart();

}

ResultsDataChart::ResultsDataChart(QJsonObject spread, QWidget *parent)
    : SimCenterWidget(parent)
{
    //QString filenameTab="";
    //auto layout = new QGridLayout();

    col1 = 0;
    col2 = 0;
    spreadsheet = new MyTableWidget();
    this->readTableFromJson(spread);
    this->makeChart();

}

void
ResultsDataChart::makeChart() {

    mLeft = true;
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
    //chartView->chart()->legend()->hide();

    //
    // creat buttons to save data
    //

    //QWidget *widget = new QWidget();
    //QGridLayout *layout = new QGridLayout(widget);
    QGridLayout *layout = new QGridLayout();
    QPushButton* save_spreadsheet = new QPushButton();
    save_spreadsheet->setText("Save Table");
    save_spreadsheet->setToolTip(tr("Save data into file in a CSV format"));
    save_spreadsheet->resize(30,30);
    connect(save_spreadsheet,SIGNAL(clicked()),this,SLOT(onSaveSpreadsheetClicked()));

    QPushButton* save_columns = new QPushButton();
    save_columns->setText("Save Columns Separately");
    save_columns->setToolTip(tr("Select an existing folder"));
    save_columns->resize(30,30);
    connect(save_columns,SIGNAL(clicked()),this,SLOT(onSaveSpreadsheetSeparatelyClicked()));

    layout->addWidget(chartView, 0,0,1,3);
    layout->addWidget(save_spreadsheet,1,0,Qt::AlignLeft);
    layout->addWidget(save_columns,1,1,Qt::AlignLeft);
    layout->addWidget(spreadsheet,2,0,1,3);
    layout->setColumnStretch(2,1);

    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //

    this->setLayout(layout);
}

QVector<QVector<double>>
ResultsDataChart::getStatistics() {
    //
    // determine summary statistics for each edp
    //
    QVector<QVector<double>> statistics;
//    QVector<double> mean_vector;
//    QVector<double> sd_vector;
//    QVector<double> skewness_vector;
//    QVector<double> kurtosis_vector;
    QVector<QString> var_names;

    for (int col = 0; col<colCount; ++col) { // +1 for first col which is not an RV
        // compute the mean
        double sum_value=0;
        for(int row=0;row<rowCount;++row) {
            QTableWidgetItem *item_index = spreadsheet->item(row,col);
            double value_item = item_index->text().toDouble();
            sum_value=sum_value+value_item;
        }

        double mean_value=sum_value/rowCount;

        double sd_value=0;
        double kurtosis_value=0;
        double skewness_value = 0;
        for(int row=0; row<rowCount;++row) {
            QTableWidgetItem *item_index = spreadsheet->item(row,col);
            double value_item = item_index->text().toDouble();
            double tmp = value_item - mean_value;
            double tmp2 = tmp*tmp;
            sd_value += tmp2;
            skewness_value += tmp*tmp2;
            kurtosis_value += tmp2*tmp2;
        }

        double n = rowCount;
        double tmpV = sd_value/n;

        if (rowCount > 1)
            sd_value = sd_value/(n-1);
        sd_value=sqrt(sd_value);

        // biased Kurtosis
        kurtosis_value = kurtosis_value/(n*tmpV*tmpV);
        // unbiased kurtosis value as calculated by Matlab
        if (n > 3)
            kurtosis_value = (n-1)/((n-2)*(n-3))*((n+1)*kurtosis_value - 3*(n-1)) + 3;

        tmpV = sqrt(tmpV);
        // biased skewness
        skewness_value = skewness_value/(n*tmpV*tmpV*tmpV);
        // unbiased skewness like Matlab
        if (n > 3)
            skewness_value *= sqrt(n*(n-1))/(n-2);

        QString variableName = theHeadings.at(col);

    //    mean_vector.push_back(mean_value);
    //    sd_vector.push_back(sd_value);
    //    skewness_vector.push_back(skewness_value);
    //    kurtosis_vector.push_back(kurtosis_value);
    //    var_names.push_back(variableName);
    //  statistics_tmp.push_back(mean_value);
    //  statistics_tmp.push_back(sd_value);
    //  statistics_tmp.push_back(skewness_value);
    //  statistics_tmp.push_back(kurtosis_value);
          statistics.push_back({mean_value, sd_value, skewness_value, kurtosis_value});
          var_names.push_back(variableName);
    //    QWidget *theWidget = this->createResultEDPWidget(variableName, mean_value, sd_value, skewness_value, kurtosis_value);
    //    summaryLayout->addWidget(theWidget);
    }
    return statistics;
    //summaryLayout->addStretch();
}


QVector<QVector<double>>
ResultsDataChart::getMinMax() {
    //
    // determine min and max value from posterior prediction for each edp
    //
    QVector<QVector<double>> minMax;

    for (int col = 0; col<colCount; ++col) {
        // compute the min and max
        QTableWidgetItem *item_index = spreadsheet->item(0,col);
        double min = item_index->text().toDouble();
        double max = item_index->text().toDouble();

        for(int row=0;row<rowCount;++row) {
            QTableWidgetItem *item_index = spreadsheet->item(row,col);
            double value_item = item_index->text().toDouble();
            if (value_item > max) {
                max = value_item;
            }
            else if (value_item < min) {
                min = value_item;
            }
        }
        minMax.push_back({min, max});
    }
    return minMax;
}


void
ResultsDataChart::readTableFromTab(QString filenameTab) {

    theHeadings << "Run #";

    // open file containing tab data
    std::ifstream tabResults(filenameTab.toStdString().c_str());

    if (!tabResults.is_open()) {
        qDebug() << "Could not open file";
        //return -1;
    }

    //
    // read first line and set headings (ignoring second column for now)
    //

    std::string inputLine;
    std::getline(tabResults, inputLine);
    std::istringstream iss(inputLine);
    colCount = 0;


    bool includesInterface = false;
    do {
        std::string subs;
        iss >> subs;
        if (colCount > 0) {
            if (subs != " ") {
                if (subs != "interface")
                    theHeadings << subs.c_str();
                else
                    includesInterface = true;
            }
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
    spreadsheet->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    spreadsheet->verticalHeader()->setVisible(false);
    // now until end of file, read lines and place data into spreadsheet

    rowCount = 0;
    while (std::getline(tabResults, inputLine)) {
        std::istringstream is(inputLine);
        int col=0;

        spreadsheet->insertRow(rowCount);
        for (int i=0; i<colCount+2; i++) {
            std::string data;
            is >> data;
            if ((includesInterface == true && i != 1) || (includesInterface == false)) {
                QModelIndex index = spreadsheet->model()->index(rowCount, col);
                spreadsheet->model()->setData(index, data.c_str());
                col++;
            }
        }
        rowCount++;
    }
    tabResults.close();
}




void
ResultsDataChart::readTableFromJson(QJsonObject spreadsheetData) {
    //
    // into a spreadsheet place all the data returned
    //

    //spreadsheet = new MyTableWidget();

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

    rowCount = numRow;
    colCount = numCol;
    spreadsheet->verticalHeader()->setVisible(false);

}



QVector<QString>
ResultsDataChart::getNames() {

    QVector<QString> names;

    for (int col =0; col<colCount; ++col) { // +1 for first col which is nit an RV
        QString variableName = theHeadings.at(col);
        names.push_back(variableName);
    }
    return names;
}

void
ResultsDataChart::onSaveSpreadsheetClicked()
{

    int rowCount = spreadsheet->rowCount();
    int columnCount = spreadsheet->columnCount();

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Data"), "",
                                                    tr("CSV (Comma delimited) (*.csv)"));


    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        for (int j=0; j<columnCount; j++)
        {
      if (j == columnCount -1)
            stream <<theHeadings.at(j);
      else
            stream <<theHeadings.at(j)<<", ";
        }
        stream <<endl;
        for (int i=0; i<rowCount; i++)
        {
            for (int j=0; j<columnCount; j++)
            {
                QTableWidgetItem *item_value = spreadsheet->item(i,j);
                double value = item_value->text().toDouble();
        if (j == columnCount-1)
          stream << value ;
        else
          stream << value << ", ";
            }
            stream<<endl;
        }
    file.close();
    }
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
void
ResultsDataChart::onSaveSpreadsheetSeparatelyClicked()
{

    int rowCount = spreadsheet->rowCount();
    int columnCount = spreadsheet->columnCount();

    QString dirName = QFileDialog::getExistingDirectory(this,
                                                    tr("Select directory"), "",
                                                     QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    for (int j=1; j<columnCount; j++)
    {
       QString fileName = dirName + QDir::separator() + theHeadings.at(j) + QString(".txt");
       QFile file(fileName);
       if (file.open(QIODevice::WriteOnly))
       {

           QTextStream stream(&file);
           //stream << "% " << theHeadings.at(j) << endl;
           for (int i=0; i<rowCount; i++)
           {
               QTableWidgetItem *item_value = spreadsheet->item(i,j);
               double value = item_value->text().toDouble();
               stream << value <<endl;
           }
        file.close();
       }
    }

}

void ResultsDataChart::onSpreadsheetCellClicked(int row, int col)
{
    mLeft = spreadsheet->wasLeftKeyPressed();

    chart->removeAllSeries();

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

        double NUM_DIVISIONS_FOR_DIVISION = ceil(sqrt(rowCount));
        if (NUM_DIVISIONS_FOR_DIVISION < 10)
            NUM_DIVISIONS_FOR_DIVISION = 10;
        else if (NUM_DIVISIONS_FOR_DIVISION > 20)
            NUM_DIVISIONS_FOR_DIVISION = 20;

        int NUM_DIVISIONS = NUM_DIVISIONS_FOR_DIVISION;

        double *dataValues = new double[rowCount];
        double histogram[20];
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

        // if constant
        if (min==max) {
            double axisMargin=abs(min)*0.1;
            min=min-axisMargin;
            max=max+axisMargin;
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
                histogram[i] = histogram[i]/rowCount;
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

            //chart->legend()->setVisible(true);
            //chart->legend()->setAlignment(Qt::AlignRight);
        /* ************************************* REMVING BUGGY BEST FIT
            //calling external python script to find the best fit, generating the data and then plotting it.
            // this will be done in the application directory

            QString appDIR = qApp->applicationDirPath(); // this is where the .exe is and also the parseJson.py, and now the fit_distribution.py

            QString data_input_file = appDIR +  QDir::separator() + QString("data_input.txt");
            QString pySCRIPT_dist_fit =  appDIR +  QDir::separator() + QString("fit.py");

            QFile file(data_input_file);
            QTextStream stream(&file);

            if (file.open(QIODevice::ReadWrite))
            {
                for(int i=0;i<rowCount;++i)
                {

                    stream<<dataValues[i];
                    stream<< endl;
                }
            }else {qDebug()<<"\n error in opening file data file for histogram fit  ";exit(1);}


            delete [] dataValues;

            QString file_fitted_path = appDIR +  QDir::separator() + QString("Best_fit.out");

            QFile file_fitted_distribution(file_fitted_path);
            if(!file_fitted_distribution.open(QIODevice::WriteOnly)) {
                QMessageBox::information(0,"error",file.errorString());
            }else
            {

            }
            // make sure to check if Q_OS_WIN or Mac etc. else there will be a bug
            //QProcess process;
            QProcess *process = new QProcess();
            process->setWorkingDirectory(appDIR);

            //pySCRIPT_dist_fit = QString("source $HOME/.bash_profile; source $HOME/.bashrc; python ") + pySCRIPT_dist_fit + QString(" ") + data_input_file;

#ifdef Q_OS_WIN

            QStringList args{pySCRIPT_dist_fit, data_input_file};
            process->execute("python", args);

            //pySCRIPT_dist_fit = QString("python ") + pySCRIPT_dist_fit + QString(" ") + data_input_file;
            //process->execute("cmd", QStringList() << "/C" << pySCRIPT_dist_fit);
#else
            pySCRIPT_dist_fit = QString("source $HOME/.bash_profile; source $HOME/.bashrc; python ") + pySCRIPT_dist_fit + QString(" ") + data_input_file;
            qDebug() << pySCRIPT_dist_fit;
            process->execute("bash", QStringList() << "-c" <<  pySCRIPT_dist_fit);
#endif


            QFile file_fitted_data("Best_fit.out");
            if(!file_fitted_data.exists())
            {
                qDebug()<<"\n The file does not exist and hence exiting";
                exit(1);

            }
            QLineSeries *series_best_fit = new QLineSeries();
            series_best_fit->setName("Best Fit");
            if(file_fitted_data.open(QIODevice::ReadOnly |QIODevice::Text ))
            {

                QTextStream txtStream(&file_fitted_data);
                while(!txtStream.atEnd())
                {
                    QString line = txtStream.readLine();
                    //  double first_value;
                    //   txtStream>>first_value;

                    QStringList list2 = line.split(',', QString::SkipEmptyParts);
                    double value1= list2[0].toDouble();
                    double value2= list2[1].toDouble();

                    series_best_fit->append(value1,value2);

                    //chart->setAxisX(axisX, series_best_fit);
                    //chart->setAxisY(axisY, series_best_fit);
                    //  chart->adjustSize();

                    // qDebug()<<"\n the line read is      "<<line;
                    // qDebug()<<"\n the value1 is         "<<value1;
                    // qDebug()<<"\n the value2 is         "<<value2;
                }

                file_fitted_data.close();

                chart->addSeries(series_best_fit);
                // chart->setTitle("The best fit plot is");
                QString best_fit_info_file = appDIR +  QDir::separator() + QString("data_fit_info.out");

                QFile info_fit_file("data_fit_info.out");
                QTextStream stream(&info_fit_file);

                //QStringList message_fitting_info;

                QString line_from_file="";
                if (info_fit_file.open(QIODevice::ReadWrite))
                {

                    while(!info_fit_file.atEnd())
                    {
                        line_from_file=line_from_file+info_fit_file.readLine();//+"\n";
                        //  qDebug()<<"\n the value of info_fit_file.readLine() is   "<<info_fit_file.readLine();
                        //  qDebug()<<"\n The value of line_from_file is    "<<line_from_file;
                        //   message_fitting_info<<info_fit_file.readLine();
                        //   message_fitting_info<<"\n";
                    }
                    //line_from_file=line_from_file+info_fit_file.readLine()+"\n";
                }
                qDebug() << line_from_file;

                // best_fit_label_text->setText(line_from_file);
                // best_fit_label_text->setStyleSheet("QLabel { background-color : white; color : gray; }");

                // QFont f2("Helvetica [Cronyx]", 10, QFont::Normal);
                // best_fit_label_text->setFont(f2);

                //msgBox.show();
                //  msgBox.exec();

                //     best_fit_instructions->setText("I am fitting the best fit info. here");
                //qDebug()<<"\n\n\n I am about to exit from here   \n\n\n";
                // exit(1);
            }
            //std::ifstream fitted_data_file("Best_fit.out");
            //if(!fitted_data_file.exists)
            // process.execute("python C:/Users/nikhil/NHERI/build-uqFEM-Desktop_Qt_5_11_0_MSVC2015_32bit-Release/debug\\fit.py");
            // exit(1);
            //qDebug()<<"\n   the value of pySCRIPT_dist_fit   is         "<<pySCRIPT_dist_fit;
            //#ifdef Q_OS_WIN
            //   QProcess process;
            //String command = "notepad";
            //process.execute("cmd", QStringList() << "/C" << command);
            //process.start("cmd", QStringList(), QIODevice::ReadWrite);
            //std::cerr << command << "\n";

            //     process.waitForStarted();

            //     qDebug()<<"process has been completed       ";
            //         exit(1);

        ************************************ */
        } else {
            // cumulative distribution
            mergesort(dataValues, rowCount);

            double xAxisMin = min-(max-min)*0.1;
            double xAxisMax =max+(max-min)*0.1;

            // if constant
            if (xAxisMin==xAxisMax) {
                double axisMargin=abs(xAxisMin)*0.1;
                xAxisMin=xAxisMin-axisMargin;
                xAxisMax=xAxisMax+axisMargin;
            }

            series->append(xAxisMin,0);
            for (int i=0; i<rowCount; i++) {
                series->append(dataValues[i], 1.0*i/rowCount);
                series->append(dataValues[i], 1.0*(i+1)/rowCount);
                //qDebug()<<"\n the dataValues[i] and rowCount is     "<<dataValues[i]<<"\t"<<rowCount<<"";
            }
            series->append(xAxisMax, 1.0);
            delete [] dataValues;

            //QAreaSeries *series2 = new QAreaSeries(series);


            chart->addSeries(series);
            QValueAxis *axisX = new QValueAxis();
            QValueAxis *axisY = new QValueAxis();
            // padhye, make these consistent changes all across.
            axisX->setRange(xAxisMin, xAxisMax);
            axisY->setRange(0, 1);
            axisY->setTitleText("Cumulative Probability");
            axisX->setTitleText(theHeadings.at(col1));
            axisX->setTickCount(NUM_DIVISIONS+1);
            chart->setAxisX(axisX, series);
            chart->setAxisY(axisY, series);
            series->setName("Cumulative Frequency Distribution");
        }
    }

    //chart->legend()->setVisible(true);
    //chart->legend()->setAlignment(Qt::AlignRight);
}


ResultsDataChart::~ResultsDataChart()
{

}

bool
ResultsDataChart::outputToJSON(QJsonObject &jsonObj){


    QJsonObject spreadsheetData;

    if (spreadsheet == NULL)
        return true;

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

    jsonObj["spreadsheet"] = spreadsheetData;

    return true;
}

bool
ResultsDataChart::inputFromJSON(QJsonObject &jsonObject){

//  will not be eused
    return true;
}

void
ResultsDataChart::clear(void)
{
    //theHeadings.clear();
    //spreadsheet = NULL;
}



int
ResultsDataChart::getNumberTasks()
{
//  return numSamples->text().toInt();
    return 1;
}
