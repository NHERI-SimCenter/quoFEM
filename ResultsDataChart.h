#ifndef RESULT_DATA_CHART_H
#define RESULT_DATA_CHART_H

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

#include <QtCharts/QChart>
using namespace QtCharts;

#include "MyTableWidget.h"
#include "RandomVariablesContainer.h"
#include <QFileDialog>
#include <QValueAxis>

class QLineEdit;

class ResultsDataChart : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit ResultsDataChart(QString filenameTab, bool isSur=false, int nrv=0, QWidget *parent = 0);
    explicit ResultsDataChart(QJsonObject spreadsheet, bool isSur=false, int nrv=0, QWidget *parent = 0);

    //explicit ResultsDataChart(QWidget *parent = 0);
    ~ResultsDataChart();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    void clear(void);

    int getNumberTasks(void);

    QVector<QVector<double>> getStatistics();
    QVector<QString> getNames();
    QVector<QVector<double>> getMinMax();

signals:

public slots:
   void onSpreadsheetCellClicked(int, int);
   void onSaveSpreadsheetClicked();
   void onSaveSpreadsheetSeparatelyClicked();
   void overlappingPlots(bool isCol1Qoi, bool isCol2Qoi,QValueAxis *axisX, QValueAxis *axisY);

private:

    void readTableFromTab(QString filenameTab);
    void readTableFromJson(QJsonObject jsonobj);
    void makeChart(void);

    //QLineEdit *randomSeed;
    //QLineEdit *numSamples;
    MyTableWidget *spreadsheet;
    QTabWidget *tabWidget;
    QChart *chart;
    QPushButton* save_spreadheet; // save the data from spreadsheet
    QLabel *label;

    int col1, col2;
    bool mLeft;
    QStringList theHeadings;

    QVector<QString>theNames;
    QVector<double>theMeans;
    QVector<double>theStdDevs;
    QVector<double>theKurtosis;
    QVector<double>theSkewness;

    bool isSurrogate;
    int nrv;
    int nqoi;

    int rowCount;
    int colCount;
};

#endif // RESULT_DATA_CHART_H
