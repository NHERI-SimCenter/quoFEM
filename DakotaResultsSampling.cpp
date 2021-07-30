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

#include "DakotaResultsSampling.h"
//#include "InputWidgetFEM.h"

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

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
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
#include <QFileInfo>
#include <QFile>

// QLabel *best_fit_label_text;
//#define NUM_DIVISIONS 10

DakotaResultsSampling::DakotaResultsSampling(RandomVariablesContainer *theRandomVariables, QWidget *parent)
  : UQ_Results(parent), theRVs(theRandomVariables), tabWidget(0)
{
    // title & add button
    theDataTable = NULL;
    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget,1);
}

DakotaResultsSampling::~DakotaResultsSampling()
{

}


void DakotaResultsSampling::clear(void)
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
    theSkewness.clear();

    tabWidget->clear();
    theDataTable = NULL;
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

int DakotaResultsSampling::processResults(QString &filenameResults, QString &filenameTab)
{
    emit sendStatusMessage(tr("Processing Sampling Results"));

    this->clear();

    //
    // check it actually ran with n errors
    //

    QFileInfo fileTabInfo(filenameTab);
    QString filenameErrorString = fileTabInfo.absolutePath() + QDir::separator() + QString("dakota.err");

    QFileInfo filenameErrorInfo(filenameErrorString);
    if (!filenameErrorInfo.exists()) {
        emit sendErrorMessage("No dakota.err file - dakota did not run - problem with dakota setup or the applicatins failed with inputs provied");
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
        emit sendErrorMessage(QString(QString("Error Running Dakota: ") + line));

        // check if there is an error message from surrogate modeling
//        QFileInfo surrogateErrorInfo(fileTabInfo.absolutePath() + QDir::separator() + QString("surrogate.err"));
//        if (surrogateErrorInfo.exists()) {
//            QFile surrogateError(filenameErrorString);
//            if (surrogateError.open(QIODevice::ReadOnly)) {
//               QTextStream in(&fileError);
//               line = in.readLine();
//               surrogateError.close();
//            }
//            emit sendErrorMessage(QString(QString("Error Running Surrogate Simulation: ") + line));
//        }

        return 0;
    }

    QFileInfo filenameTabInfo(filenameTab);
    if (!filenameTabInfo.exists()) {
        emit sendErrorMessage("No dakotaTab.out file - dakota failed .. possibly no QoI");
        return 0;
    }

    // If surrogate model is used, display additional info.
    QDir tempFolder(filenameTabInfo.absolutePath());
    QFileInfo surrogateTabInfo(tempFolder.filePath("surrogateTab.out"));
    if (surrogateTabInfo.exists()) {
        filenameTab = tempFolder.filePath("surrogateTab.out");
        isSurrogate = true;
    }

    //
    // create summary, a QWidget for summary data, the EDP name, mean, stdDev, kurtosis info
    //

    // create a scrollable windows, place summary inside it
    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    QWidget *summary = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summaryLayout->setContentsMargins(0,0,0,0); // adding back
    summary->setLayout(summaryLayout);

    sa->setWidget(summary);

    //
    // create spreadsheet,  a QTableWidget showing RV and results for each run
    //

    theDataTable = new ResultsDataChart(filenameTab, isSurrogate, theRVs->getNumRandomVariables());

    //
    // determine summary statistics for each edp
    //

    QVector<QVector<double>> statisticsVector = theDataTable->getStatistics();
    QVector<QString> NamesVector = theDataTable->getNames();
    for (int col = theRVs->getNumRandomVariables()+1; col<NamesVector.size(); ++col) { // +
        QWidget *theWidget = this->createResultEDPWidget(NamesVector[col], statisticsVector[col]);
        summaryLayout->addWidget(theWidget);
    }
    summaryLayout->addStretch();

    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

    emit sendStatusMessage(tr(""));

    return 0;
}



// padhye
// this function is called if you decide to say save the data from UI into a json object
bool
DakotaResultsSampling::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;


    jsonObject["resultType"]=QString(tr("SimCenterUQResultsSampling"));
    jsonObject["isSurrogate"]=isSurrogate;

    //
    // add summary data
    //

    QJsonArray resultsData;
    int numEDP = theNames.count();
    for (int i=0; i<numEDP; i++) {
        QJsonObject edpData;
        edpData["name"]=theNames.at(i);
        edpData["mean"]=theMeans.at(i);
        edpData["stdDev"]=theStdDevs.at(i);
        edpData["kurtosis"]=theKurtosis.at(i);
        edpData["skewness"]=theSkewness.at(i);
        resultsData.append(edpData);
    }

    jsonObject["summary"]=resultsData;

    //
    // add spreadsheet data
    //

    if(theDataTable != NULL) {
        theDataTable->outputToJSON(jsonObject);
    }
    return result;
}



// if you already have a json data file then you can populate the UI with the entries from json.

bool
DakotaResultsSampling::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;

    this->clear();

    //
    // check any data exists
    //

    QJsonObject &theObject = jsonObject;

    QJsonValue uqValue;
    if (jsonObject.contains("uqResults")) {
        uqValue = jsonObject["uqResults"];
        jsonObject = uqValue.toObject();
    } else
        theObject = jsonObject;
    

    QJsonValue spreadsheetValue = theObject["spreadsheet"];
    if (spreadsheetValue.isNull()) { // ok .. if saved files but did not run a simulation
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

    isSurrogate=jsonObject["isSurrogate"].toBool();
    theDataTable = new ResultsDataChart(spreadsheetValue.toObject(), isSurrogate, theRVs->getNumRandomVariables());

    //
    // determine summary statistics for each edp
    //

    QVector<QVector<double>> statisticsVector = theDataTable->getStatistics();
    QVector<QString> NamesVector = theDataTable->getNames();
    for (int col = theRVs->getNumRandomVariables()+1; col<NamesVector.size(); ++col) { // +
        QWidget *theWidget = this->createResultEDPWidget(NamesVector[col], statisticsVector[col]);
        summaryLayout->addWidget(theWidget);
    }
    summaryLayout->addStretch();

    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

    return result;
}

extern QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit);

QWidget *
DakotaResultsSampling::createResultEDPWidget(QString &name, QVector<double> statistics) {

    double mean = statistics[0];
    double stdDev = statistics[1];
    double skewness = statistics[2];
    double kurtosis = statistics[3];

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

    QLineEdit *skewnessLineEdit;
    QWidget *skewnessWidget = addLabeledLineEdit(QString("Skewness"), &skewnessLineEdit);
    skewnessLineEdit->setText(QString::number(skewness));
    skewnessLineEdit->setDisabled(true);
    theSkewness.append(skewness);
    edpLayout->addWidget(skewnessWidget);

    QLineEdit *kurtosisLineEdit;
    QWidget *kurtosisWidget = addLabeledLineEdit(QString("Kurtosis"), &kurtosisLineEdit);
    kurtosisLineEdit->setText(QString::number(kurtosis));
    kurtosisLineEdit->setDisabled(true);
    theKurtosis.append(kurtosis);
    edpLayout->addWidget(kurtosisWidget);

    edpLayout->addStretch();

    return edp;
}
