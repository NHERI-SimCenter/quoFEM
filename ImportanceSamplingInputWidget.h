#ifndef IMPORTANCESAMPLINGINPUTWIDGET_H
#define IMPORTANCESAMPLINGINPUTWIDGET_H

#include <UQ_MethodInputWidget.h>
#include <QLineEdit>
#include <QComboBox>

class ImportanceSamplingInputWidget : public UQ_MethodInputWidget
{
    Q_OBJECT
public:
    explicit ImportanceSamplingInputWidget(QWidget *parent = nullptr);

signals:

public slots:

    // SimCenterWidget interface
public:
    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);

    // UQ_MethodInputWidget interface
public:
    int getNumberTasks();
private:
    QLineEdit *randomSeed;
    QLineEdit *numSamples;
    QComboBox* isMethod;
};

#endif // IMPORTANCESAMPLINGINPUTWIDGET_H
