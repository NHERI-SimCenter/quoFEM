#ifndef INPUTWIDGETEDP_H
#define INPUTWIDGETEDP_H

// Written: fmckenna

#include <QWidget>

#include "EDP.h"
#include <QGroupBox>
#include <QVector>
#include <QVBoxLayout>

class InputWidgetEDP : public QWidget
{
    Q_OBJECT
public:
    explicit InputWidgetEDP(QWidget *parent = 0);
    ~InputWidgetEDP();

    void outputToJSON(QJsonObject &rvObject);
    void inputFromJSON(QJsonObject &rvObject);

signals:

public slots:
   void addEDP(void);
   void clear(void);

private:
    void makeEDP(void);
    QVBoxLayout *verticalLayout;
    QVBoxLayout *edpLayout;
    QGroupBox *edp;

    QVector<EDP *>theEDPs;
};

#endif // INPUTWIDGETEDP_H
