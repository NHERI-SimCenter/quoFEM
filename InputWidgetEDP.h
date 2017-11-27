#ifndef INPUTWIDGETEDP_H
#define INPUTWIDGETEDP_H

// Written: fmckenna

#include <SimCenterWidget.h>

#include "EDP.h"
#include <QGroupBox>
#include <QVector>
#include <QVBoxLayout>

class InputWidgetEDP : public SimCenterWidget
{
    Q_OBJECT

public:
    explicit InputWidgetEDP(QWidget *parent = 0);
    ~InputWidgetEDP();

    void outputToJSON(QJsonObject &rvObject);
    void inputFromJSON(QJsonObject &rvObject);

    int processResults(double *data);
    int getNumEDP(void);

signals:

public slots:
   void addEDP(void);
   void removeEDP(void);
   void clear(void);

private:
    void makeEDP(void);
    QVBoxLayout *verticalLayout;
    QVBoxLayout *edpLayout;
    //QGroupBox *edp;
    QFrame *edp;
    //QWidget *edp;

    QVector<EDP *>theEDPs;
};

#endif // INPUTWIDGETEDP_H
