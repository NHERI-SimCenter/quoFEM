#ifndef INPUTWIDGET_SAMPLING_H
#define INPUTWIDGET_SAMPLING_H

// Written: fmckenna

#include <SimCenterWidget.h>

#include "EDP.h"
#include <QGroupBox>
#include <QVector>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>

class InputWidgetEDP;

class InputWidgetSampling : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit InputWidgetSampling(QWidget *parent = 0);
    ~InputWidgetSampling();

    void outputToJSON(QJsonObject &rvObject);
    void inputFromJSON(QJsonObject &rvObject);

    int processResults(QString &filenameResults);

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
    QWidget     *uqSpecific;

    InputWidgetEDP *theEdpWidget;
};

#endif // INPUTWIDGET_SAMPLING_H
