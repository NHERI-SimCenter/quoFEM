#ifndef PCEINPUTWIDGET_H
#define PCEINPUTWIDGET_H

#include <UQ_MethodInputWidget.h>
#include <QLineEdit>
#include <QComboBox>

class QGridLayout;

class PCEInputWidget : public UQ_MethodInputWidget
{
    Q_OBJECT
public:
    explicit PCEInputWidget(QWidget *parent = nullptr);

signals:

public slots:
    void dataMethodChanged(int);


public:
    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);

public:
    int getNumberTasks();

private:
    QLineEdit *randomSeed;
    QLineEdit *quadOd;
    QLineEdit *level;
    QComboBox* dataMethod;
    QGridLayout *layout;

};

#endif // PCEINPUTWIDGET
