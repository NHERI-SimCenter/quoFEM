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


#include "DakotaResultsBayesianCalibration.h"
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

DakotaResultsBayesianCalibration::DakotaResultsBayesianCalibration(int numBurn, QWidget *parent)
    : UQ_Results(parent)
{
    // title & add button

    theDataTable = NULL;
    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget,1);

//    mLeft = true;
//    col1 = 0;
//    col2 = 0;
    burnInSamples = numBurn;

//     spreadsheet = new MyTableWidget();
//     spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);
//     connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));

//     spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);
//     connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));

//     chart = new QChart();
//     chart->setAnimationOptions(QChart::AllAnimations);

//     QChartView *chartView = new QChartView(chart);
//     chartView->setRenderHint(QPainter::Antialiasing);
//     chartView->chart()->legend()->hide();

//     //
//     // create a widget into which we place the chart and the spreadsheet
//     //

//     QWidget *widget = new QWidget();
//     QGridLayout *layout = new QGridLayout(widget);
//     QPushButton* save_spreadsheet = new QPushButton();
//     save_spreadsheet->setText("Save Data");
//     save_spreadsheet->setToolTip(tr("Save data into file in a CSV format"));
//     save_spreadsheet->resize(30,30);
//     connect(save_spreadsheet,SIGNAL(clicked()),this,SLOT(onSaveSpreadsheetClicked()));

//     layout->setContentsMargins(0,0,0,0);
//     layout->setSpacing(3);

//     layout->addWidget(chartView, 0,0,1,1);
//     layout->addWidget(save_spreadsheet,1,0,Qt::AlignLeft);
//     layout->addWidget(spreadsheet,2,0,1,1);

//     tabWidget->addTab(widget, tr("Data Values"));
}

DakotaResultsBayesianCalibration::~DakotaResultsBayesianCalibration()
{

}


void DakotaResultsBayesianCalibration::clear(void)
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

    tabWidget->clear();
    //dakotaText->clear();
}



bool
DakotaResultsBayesianCalibration::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    int numEDP = theNames.count();

    // quick return .. noEDP -> no analysis done -> no results out
    if (numEDP == 0)
      return true;

    jsonObject["resultType"]=QString(tr("DakotaResultsBayesianCalibration"));

    //
    // add summary data
    //

    QJsonArray resultsData;
    for (int i=0; i<numEDP; i++) {
        QJsonObject edpData;
        edpData["name"]=theNames.at(i);
        edpData["mean"]=theMeans.at(i);
        edpData["stdDev"]=theStdDevs.at(i);
        resultsData.append(edpData);
    }
    jsonObject["summary"]=resultsData;

    // add general data
    jsonObject["general"]=dakotaText->toPlainText();

    //
    // add spreadsheet data
    //

    if(theDataTable != NULL) {
        theDataTable->outputToJSON(jsonObject);
    }
    return result;
}

bool
DakotaResultsBayesianCalibration::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;
    this->clear();

     if (!jsonObject.contains("summary")) {
        return true;
     }

     //
     // create a summary widget in which place basic output (name, mean, stdDev)
     //

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    QWidget *summary = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summaryLayout->setContentsMargins(0,0,0,0); // adding back
    summary->setLayout(summaryLayout);
    sa->setWidget(summary);

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

        QWidget *theWidget = this->createResultParameterWidget(name, mean, stdDev);
        summaryLayout->addWidget(theWidget);
    }
    summaryLayout->addStretch();

    //
    // into a QTextEdit place more detailed Dakota text
    //
    dakotaText = new QTextEdit();
    dakotaText->setReadOnly(true); // make it so user cannot edit the contents
    QJsonValue theValue = jsonObject["general"];
    dakotaText->setText(theValue.toString());

    //
    // into a spreadsheet place all the data returned
    //

    QJsonValue spreadsheetValue = jsonObject["spreadsheet"].toObject();
    if (spreadsheetValue.isNull()) { // ok .. if saved files but did not run a simulation
        return true;
    }

    theDataTable = new ResultsDataChart(spreadsheetValue.toObject());

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(dakotaText, tr("General"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

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

int DakotaResultsBayesianCalibration::processResults(QString &dirName)
{
  qDebug() << "DakotaResultsBayesianCalibration::processResults dir" << dirName;
  QString filenameOut = dirName + QDir::separator() + tr("dakota.out");
  QString filenameTAB = dirName + QDir::separator() + tr("dakotaTab.out");
  return this->processResults(filenameOut, filenameTAB);
}

int DakotaResultsBayesianCalibration::processResults(QString &filenameResults, QString &filenameTab) {

    statusMessage(tr("Processing Dakota Bayesian Calibration Results"));

    //
    // check it actually ran with no errors
    //

    QFileInfo fileTabInfo(filenameTab);
    QString filenameErrorString = fileTabInfo.absolutePath() + QDir::separator() + QString("dakota.err");

    QFileInfo filenameErrorInfo(filenameErrorString);
    if (!filenameErrorInfo.exists()) {
        errorMessage("No dakota.err file - dakota did not run - problem with dakota setup or the applications failed with inputs provided");
	return 0;
    }


    QFileInfo filenameTabInfo(filenameTab);
    if (!filenameTabInfo.exists()) {
        errorMessage("No dakotaTab.out file - dakota failed .. possibly no QoI");
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
        errorMessage(QString(QString("Error Running Dakota: ") + line));
        return 0;
    }

    //
    // open Dakota output file
    //

    std::ifstream fileResults(filenameResults.toStdString().c_str());
    if (!fileResults.is_open()) {
        qDebug() << "Could not open file: " << filenameResults;
        return -1;
    }

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    summary = new QWidget();
    summaryLayout = new QVBoxLayout();
    summaryLayout->setContentsMargins(0,0,0,0);
    summary->setLayout(summaryLayout);

    sa->setWidget(summary);

    dakotaText = new QTextEdit();
    dakotaText->setReadOnly(true); // make it so user cannot edit the contents



    // now ignore every line until Kurtosis found

    const std::string needle = "Kurtosis";
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

    dakotaText->append(haystack.c_str());

    bool isSummaryDone = false;

    while (std::getline(fileResults, haystack)) {
        dakotaText->append(haystack.c_str());
        if (isSummaryDone == false) {

            if ( strlen(haystack.c_str()) == 0) {
                isSummaryDone = true;
            } else {
                //
                // add sumary info
                //

                std::istringstream iss(haystack);
                std::string subs;

                iss >> subs;
                QString  nameString(QString::fromStdString(subs));

                iss >> subs;
                QString meanText(QString::fromStdString(subs));
                double mean = meanText.toDouble();

                iss >> subs;
                QString stdDevText(QString::fromStdString(subs));
                double stdDev = stdDevText.toDouble();

                QWidget *theWidget = this->createResultParameterWidget(nameString, mean, stdDev);
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


    // open file containing tab data
    std::ifstream tabResults(filenameTab.toStdString().c_str());
    if (!tabResults.is_open()) {
        errorMessage("Could not open dakotaTab.out file");
        return -1;
    }


    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //

    theDataTable = new ResultsDataChart(filenameTab);

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(dakotaText, tr("General"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

    statusMessage(tr("Results Displayed"));

    return true;
}



extern QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit);

QWidget *
DakotaResultsBayesianCalibration::createResultParameterWidget(QString &name, double mean, double stdDev) {
    QWidget *edp = new QWidget;
    QHBoxLayout *edpLayout = new QHBoxLayout();

    edpLayout->setContentsMargins(0,0,0,0);
    edpLayout->setSpacing(3);

    edp->setLayout(edpLayout);

    QLineEdit *nameLineEdit;
    QWidget *nameWidget = addLabeledLineEdit(QString("Parameter Name"), &nameLineEdit);
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
    QWidget *stdDevWidget = addLabeledLineEdit(QString("Standard Deviation"), &stdDevLineEdit);
    stdDevLineEdit->setText(QString::number(stdDev));
    stdDevLineEdit->setDisabled(true);
    theStdDevs.append(stdDev);
    edpLayout->addWidget(stdDevWidget);

    edpLayout->addStretch();

    return edp;
}
