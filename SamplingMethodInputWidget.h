#ifndef SAMPLINGMETHODINPUTWIDGET_H
#define SAMPLINGMETHODINPUTWIDGET_H

// Written: fmckenna

#include <QWidget>

#include "EDP.h"
#include <QGroupBox>
#include <QVector>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>

class SamplingMethodInputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SamplingMethodInputWidget(QWidget *parent = 0);
    ~SamplingMethodInputWidget();

    void outputToJSON(QJsonObject &rvObject);
    void inputFromJSON(QJsonObject &rvObject);

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
};

#endif // SAMPLINGMETHODINPUTWIDGET_H
