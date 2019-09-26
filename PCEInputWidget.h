#ifndef PCEINPUTWIDGET_H
#define PCEINPUTWIDGET_H

#include <UQ_MethodInputWidget.h>
#include <QLineEdit>
#include <QComboBox>

class QGridLayout;
class QLabel;

class PCEInputWidget : public UQ_MethodInputWidget
{
    Q_OBJECT
public:
    explicit PCEInputWidget(QWidget *parent = nullptr);

signals:

public slots:
    void trainingDataMethodChanged(int);
    void samplingDataMethodChanged(int);


public:
    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);

public:
    int getNumberTasks();

private:
    QLineEdit *level;
    QLabel *levelLabel;
    QComboBox* dataMethod;

    QLineEdit *randomSeedSampling;
    QLineEdit *numSamplesSampling;
    QLineEdit *levelSampling;
    QComboBox* dataMethodSampling;

    QGridLayout *trainingDataLayout;
    QGridLayout *samplingDataLayout;
};

#endif // PCEINPUTWIDGET
