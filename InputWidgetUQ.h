#ifndef SAMPLINGMETHODINPUTWIDGET_H
#define SAMPLINGMETHODINPUTWIDGET_H

// Written: fmckenna

#include <SimCenterWidget.h>

#include "EDP.h"
#include <QGroupBox>
#include <QVector>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>

class SimCenterWidget;
class InputWidgetSampling;
class DakotaResults;

class InputWidgetUQ : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit InputWidgetUQ(QWidget *parent = 0);
    ~InputWidgetUQ();

    void outputToJSON(QJsonObject &rvObject);
    void inputFromJSON(QJsonObject &rvObject);

    int processResults(QString &filenameResults, QString &filenameTab);
    DakotaResults *getResults(void);

signals:

public slots:
   void clear(void);
    void uqSelectionChanged(const QString &arg1);
 //  void uqMethodChanged(const QString &arg1);

private:
    QVBoxLayout *layout;
    QWidget     *methodSpecific;
    QComboBox   *samplingMethod;
    QLineEdit   *numSamples;
    QLineEdit   *randomSeed;
    QPushButton *run;

    QComboBox   *uqSelection;

    SimCenterWidget     *uqType;
    InputWidgetSampling *samplingWidget;
};

#endif // SAMPLINGMETHODINPUTWIDGET_H
