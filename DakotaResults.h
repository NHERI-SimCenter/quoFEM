#ifndef DAKOTA_RESULTS_H
#define DAKOTA_RESULTS_H

// Written: fmckenna

#include <SimCenterWidget.h>
class QVBoxLayout;

class DakotaResults : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit DakotaResults(QWidget *parent = 0);
    virtual ~DakotaResults();

    virtual void outputToJSON(QJsonObject &rvObject);
    virtual void inputFromJSON(QJsonObject &rvObject);

    virtual int processResults(QString &filenameResults, QString &filenameTab);

    void setResultWidget(DakotaResults *result);

signals:

public slots:

protected:
    DakotaResults *resultWidget;
    QVBoxLayout *layout;
};

#endif // DAKOTA_RESULTS_SAMPLING_H
