#ifndef PCEINPUTWIDGET_H
#define PCEINPUTWIDGET_H

#include <UQ_MethodInputWidget.h>
#include <QLineEdit>
#include <QComboBox>

class PCEInputWidget : public UQ_MethodInputWidget
{
    Q_OBJECT
public:
    explicit PCEInputWidget(QWidget *parent = nullptr);

signals:

public slots:


public:
    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);

public:
    int getNumberTasks();

private:
    QLineEdit *randomSeed;
    QLineEdit *numSamples;
    QComboBox* dataMethod;

};

#endif // PCEINPUTWIDGET
