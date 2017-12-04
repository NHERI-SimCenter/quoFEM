#ifndef DAKOTA_RESULTS_SAMPLING_H
#define DAKOTA_RESULTS_SAMPLING_H

// Written: fmckenna

#include <DakotaResults.h>
#include <QtCharts/QChart>
using namespace QtCharts;

class QTextEdit;
class QTabWidget;
class MyTableWidget;
//class QChart;

class DakotaResultsSampling : public DakotaResults
{
    Q_OBJECT
public:
    explicit DakotaResultsSampling(QWidget *parent = 0);
    ~DakotaResultsSampling();

    void outputToJSON(QJsonObject &rvObject);
    void inputFromJSON(QJsonObject &rvObject);

    int processResults(QString &filenameResults, QString &filenameTab);
    QWidget *createResultEDPWidget(QString &name, double mean, double stdDev);

signals:

public slots:
   void clear(void);
   void onSpreadsheetCellClicked(int, int);

private:
   QTabWidget *tabWidget;
   QTextEdit  *dakotaText;
   MyTableWidget *spreadsheet;
   QChart *chart;

   int col1, col2;
   bool mLeft;
   QStringList theHeadings;

   QVector<QString>theNames;
   QVector<double>theMeans;
   QVector<double>theStdDevs;
};

#endif // DAKOTA_RESULTS_SAMPLING_H
