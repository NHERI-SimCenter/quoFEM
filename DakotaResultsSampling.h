#ifndef DAKOTA_RESULTS_SAMPLING_H
#define DAKOTA_RESULTS_SAMPLING_H

// Written: fmckenna

#include <SimCenterWidget.h>

class QTextEdit;
class QTabWidget;
class QTableWidget;

class DakotaResultsSampling : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit DakotaResultsSampling(QWidget *parent = 0);
    ~DakotaResultsSampling();

    void outputToJSON(QJsonObject &rvObject);
    void inputFromJSON(QJsonObject &rvObject);

    int processResults(QString &filenameResults);

signals:

public slots:
   void clear(void);


private:
   QTabWidget *tabWidget;
   QTextEdit *dakotaText;
   QTableWidget *spreadsheet;

};

#endif // DAKOTA_RESULTS_SAMPLING_H
