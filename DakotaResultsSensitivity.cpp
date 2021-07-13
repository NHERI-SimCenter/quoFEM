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

#include "DakotaResultsSensitivity.h"
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

#define NUM_DIVISIONS 10


DakotaResultsSensitivity::DakotaResultsSensitivity(RandomVariablesContainer *theRandomVariables, QWidget *parent)
    : UQ_Results(parent), theRVs(theRandomVariables)
{
    // title & add button
    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget,1);
}

DakotaResultsSensitivity::~DakotaResultsSensitivity()
{

}


void DakotaResultsSensitivity::clear(void)
{
    // delete any existing widgets
    int count = tabWidget->count();
    if (count > 0) {
        for (int i=0; i<count; i++) {
            QWidget *theWidget = tabWidget->widget(count);
            delete theWidget;
        }
    }
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

int DakotaResultsSensitivity::processResults(QString &filenameResults, QString &filenameTab)
{
    emit sendStatusMessage(tr("Processing Results ... "));

    this->clear();

    //
    // check it actually ran with errors
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

    //
    // read data from file filename
    //

    // open file containing data
    std::ifstream fileResults(filenameResults.toStdString().c_str());
    if (!fileResults.is_open()) {
        std::cerr << "DakotaResultsReliability: Could not open file";
        return -1;
    }

    /********************** LOOKING FOR THE FOLLOWING
Global sensitivity indices for each response function:
Node_3_Disp Sobol' indices:
                                  Main             Total
                      2.4039246953e-01  2.1539126172e-01 E
                      3.1111165023e-01  2.5059985880e-01 P
                     -1.0433587063e-01  4.5879211975e-02 A1
                      5.5975158418e-02  2.3683900870e-01 A2
Node_2_Disp Sobol' indices:
                                  Main             Total
                      2.8398436472e-01  2.1746571906e-01 E
                      3.3651270362e-01  2.5289844553e-01 P
                     -1.2721174582e-01  6.9156108718e-02 A1
                      4.2512526616e-03  1.2984354691e-01 A2

<<<<< Iterator random_sampling completed.

              ******************************************** */

    const std::string needleStart = "Global sensitivity indices for each response function:";
    std::string haystack;

    while (std::getline(fileResults, haystack))
        if (haystack.find(needleStart) != std::string::npos)
            break;

    if (fileResults.eof()) {
        std::cerr << "DakotaResultsReliability: No sensitivity results in output";
        return -1;
    }

    //
    // Read from out file
    //

    const std::string needleSobol = "Sobol'";
    bool done = false;
    numEDP = 0;
    numRV = 0;

    QVector<double> sobols_main_vec;
    QVector<double> sobols_tot_vec;

    while (done == false) {
        std::getline(fileResults, haystack);
        if (haystack.find(needleSobol) != std::string::npos) {
            //QString h(QString::fromStdString(haystack));
            edpname_list << QString::fromStdString(haystack);
            std::getline(fileResults, haystack);
            if (numEDP>0) {
                sobols_tot.push_back(sobols_tot_vec);
                sobols_main.push_back(sobols_main_vec);
            }
            numRV = 0;
            numEDP++;
            sobols_main_vec.clear();
            sobols_tot_vec.clear();
            rvname_list.clear();
        } else {
            std::istringstream iss(haystack); // main/total
            QString h(QString::fromStdString(haystack));
            if (h.length() == 0) {
                sobols_tot.push_back(sobols_tot_vec);
                sobols_main.push_back(sobols_main_vec);
                done = true;
            } else {
                std::string data1, data2, data3, data4;

                iss >> data1 >> data2 >> data3;
                QString d1(QString::fromStdString(data1)); //main
                QString d2(QString::fromStdString(data2)); //total
                QString d3(QString::fromStdString(data3)); // label

                sobols_main_vec.push_back(d1.toDouble());
                sobols_tot_vec.push_back(d2.toDouble());
                rvname_list << d3;
                numRV++;
            }
        }
    }

    fileResults.close();

    QScrollArea *sa = new QScrollArea;

    //HEREHEREHEREHEREHERE
    gsaChart(*&sa);
    /*************
    while (done == false) {
        std::getline(fileResults, haystack);
        if (haystack.find(needleSobol) != std::string::npos) {
            QString h(QString::fromStdString(haystack));

            groupBox = new QGroupBox(h);
            summaryLayout->addWidget(groupBox);


            // Labels & QlineEdit Option in a QGrid
            trainingDataLayout = new QGridLayout();
            QLabel *l1 = new QLabel("Random Variable");
            QLabel *l2 = new QLabel("Main");
            QLabel *l3 = new QLabel("Total");
            trainingDataLayout->addWidget(l1, 0,0);
            trainingDataLayout->addWidget(l2, 0,1);
            trainingDataLayout->addWidget(l3, 0,2);
            QFont font;
            font.setBold(true);
            l1->setAlignment(Qt::AlignCenter);
            l1->setFont(font);
            l2->setAlignment(Qt::AlignCenter);
            l2->setFont(font);
            l3->setAlignment(Qt::AlignCenter);
            l3->setFont(font);

            groupBox->setLayout(trainingDataLayout);

            // set num EDP and read a useleass line
            numEDP = 0;
            std::getline(fileResults, haystack);

        } else {
            std::istringstream iss(haystack);
            QString h(QString::fromStdString(haystack));
            if (h.length() == 0) {
                done = true;
            } else {
                std::string data1, data2, data3, data4;

                iss >> data1 >> data2 >> data3;
                QString d1(QString::fromStdString(data1));
                QString d2(QString::fromStdString(data2));
                QString d3(QString::fromStdString(data3));
                QLineEdit *e1 = new QLineEdit(d3); e1->setReadOnly(true); e1->setAlignment(Qt::AlignCenter);
                QLineEdit *e2 = new QLineEdit(d1); e2->setReadOnly(true);  e2->setAlignment(Qt::AlignRight);
                QLineEdit *e3 = new QLineEdit(d2); e3->setReadOnly(true); e3->setAlignment(Qt::AlignRight);

                trainingDataLayout->addWidget(e1, numEDP+1,0);
                trainingDataLayout->addWidget(e2, numEDP+1,1);
                trainingDataLayout->addWidget(e3, numEDP+1,2);
                trainingDataLayout->setColumnStretch(1,.5);
                trainingDataLayout->setColumnStretch(2,.5);
                trainingDataLayout->setColumnStretch(3,1);


                numEDP++;
            }
        }
    }
    *************/


    //
    // create spreadsheet,  a QTableWidget showing RV and results for each run
    //


    //theDataTable = new ResultsDataChart(filenameTab);
    theDataTable = new ResultsDataChart(filenameTab, isSurrogate, theRVs->getNumRandomVariables());


    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //



    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

    emit sendStatusMessage(tr(""));

    return 0;
}

void DakotaResultsSensitivity::gsaChart(QScrollArea *&sa) {

    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    QGroupBox *groupBox = NULL;
    QWidget *summary = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summaryLayout->setContentsMargins(0,0,0,0); // adding back
    summary->setLayout(summaryLayout);
    sa->setWidget(summary);

    QGridLayout * trainingDataLayout = NULL;
    QStringList theTableHeadings;
    theTableHeadings << "Random Variable" << "Main" << "Total";

    for (int ne=0; ne< numEDP; ne++) {
        QString h;
        groupBox = new QGroupBox(edpname_list[ne]);
        summaryLayout->addWidget(groupBox);
        // Labels & QlineEdit Option in a QGrid
        trainingDataLayout = new QGridLayout();
        QLabel *l1 = new QLabel("Random Variable");
        QLabel *l2 = new QLabel("Main");
        QLabel *l3 = new QLabel("Total");
        trainingDataLayout->addWidget(l1, 0,0);
        trainingDataLayout->addWidget(l2, 0,1);
        trainingDataLayout->addWidget(l3, 0,2);
        QFont font;
        font.setBold(true);
        l1->setAlignment(Qt::AlignCenter);
        l1->setFont(font);
        l2->setAlignment(Qt::AlignCenter);
        l2->setFont(font);
        l3->setAlignment(Qt::AlignCenter);
        l3->setFont(font);

        groupBox->setLayout(trainingDataLayout);

        for (int nrv=0; nrv< numRV; nrv++) {
            std::string data1, data2, data3, data4;

            QLineEdit *e1 = new QLineEdit(rvname_list[nrv]); e1->setReadOnly(true); e1->setAlignment(Qt::AlignCenter);
            QLineEdit *e2 = new QLineEdit(QString::number(sobols_main[ne][nrv])); e2->setReadOnly(true);  e2->setAlignment(Qt::AlignRight);
            QLineEdit *e3 = new QLineEdit(QString::number(sobols_tot[ne][nrv])); e3->setReadOnly(true); e3->setAlignment(Qt::AlignRight);

            trainingDataLayout->addWidget(e1, nrv+1,0);
            trainingDataLayout->addWidget(e2, nrv+1,1);
            trainingDataLayout->addWidget(e3, nrv+1,2);
            trainingDataLayout->setColumnStretch(1,.5);
            trainingDataLayout->setColumnStretch(2,.5);
            trainingDataLayout->setColumnStretch(3,1);
        }
    }
    summaryLayout->addStretch();
}




// padhye
// this function is called if you decide to say save the data from UI into a json object
bool
DakotaResultsSensitivity::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["resultType"]=QString(tr("DakotaResultsSensitivity"));
    jsonObject["isSurrogate"]=isSurrogate;

    if (spreadsheet == NULL)
        return true;

    //
    // add summary data
    //

    QJsonArray sobols_tot_vec, sobols_main_vec;
    for (int nq=0; nq<numEDP; nq++){
        for (int nc=0; nc<numRV; nc++){
            sobols_tot_vec.append(sobols_tot[nq][nc]);
            sobols_main_vec.append(sobols_main[nq][nc]);
        }
    }
    jsonObject["sobols_main"]=sobols_main_vec;
    jsonObject["sobols_tot"]=sobols_tot_vec;

    QJsonArray QoIlist;
    for (int nq=0; nq<numEDP; nq++){
        QoIlist.append(edpname_list[nq]);
    }
    jsonObject["QoIlist"]=QoIlist;

    QJsonArray rvlist;
    for (int nc=0; nc<numRV; nc++){
        rvlist.append(rvname_list[nc]);
    }
    jsonObject["RVlist"]=rvlist;

    jsonObject["numQoI"]=numEDP;
    jsonObject["numRV"]=numRV;

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
DakotaResultsSensitivity::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;

    this->clear();

    QJsonValue spreadsheetValue = jsonObject["spreadsheet"];
    if (spreadsheetValue.isNull())
        return true;

    //
    // create a summary widget in which place basic output (name, mean, stdDev)
    //

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
    // sobols
    //

    numEDP=jsonObject["numQoI"].toInt();
    numRV=jsonObject["numRV"].toInt();

    QJsonArray sobols_tot_vals=jsonObject["sobols_tot"].toArray();
    QJsonArray sobols_main_vals=jsonObject["sobols_main"].toArray();

    int n=0;
    for (int nq=0; nq<numEDP; nq++){
        QVector<double> sobols_tot_vec, sobols_main_vec;
        for (int nc=0; nc<numRV; nc++){
            sobols_tot_vec.push_back(sobols_tot_vals[nc].toDouble());
            sobols_main_vec.push_back(sobols_main_vals[nc].toDouble());
        }
        sobols_tot.push_back(sobols_tot_vec);
        sobols_main.push_back(sobols_main_vec);
    }

    QJsonArray QoIlist=jsonObject["QoIlist"].toArray();
    for (int nq=0; nq<numEDP; nq++) {
        edpname_list.push_back(QoIlist[nq].toString());
    }

    QJsonArray comblist=jsonObject["RVlist"].toArray();
    for (int nc=0; nc<numRV; nc++) {
        rvname_list.push_back(comblist[nc].toString());
    }

    QScrollArea *sa = new QScrollArea;
    gsaChart(*&sa);


    //theDataTable = new ResultsDataChart(spreadsheetValue.toObject());

    isSurrogate=jsonObject["isSurrogate"].toBool();
    theDataTable = new ResultsDataChart(spreadsheetValue.toObject(), isSurrogate, theRVs->getNumRandomVariables());

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

    return result;
}


extern QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit);
