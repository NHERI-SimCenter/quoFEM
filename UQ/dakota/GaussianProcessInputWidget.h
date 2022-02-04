#ifndef GAUSSIANPROCESSINPUTWIDGET_H
#define GAUSSIANPROCESSINPUTWIDGET_H

#include <UQ_MethodInputWidget.h>
#include <QLineEdit>
#include <QComboBox>

class GaussianProcessInputWidget : public UQ_MethodInputWidget
{
    Q_OBJECT
public:
    explicit GaussianProcessInputWidget(QWidget *parent = nullptr);

signals:

public slots:


public:
    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);

public:
    int getNumberTasks();

private:
    QLineEdit *trainingSeed;
    QLineEdit *trainingNumSamples;
    QComboBox* trainingMethod;
    QLineEdit *samplingSeed;
    QLineEdit *samplingNumSamples;
    QComboBox *samplingMethod;
    QComboBox *surrogateSurfaceModel;
};

#endif // GAUSSIANPROCESSINPUTWIDGET
