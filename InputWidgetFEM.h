#ifndef INPUTWIDGETFEM_H
#define INPUTWIDGETFEM_H

// Written: fmckenna

#include <QWidget>

#include "EDP.h"
#include <QGroupBox>
#include <QVector>
#include <QVBoxLayout>
#include <QComboBox>

class InputWidgetFEM : public QWidget
{
    Q_OBJECT
public:
    explicit InputWidgetFEM(QWidget *parent = 0);
    ~InputWidgetFEM();

    void outputToJSON(QJsonObject &rvObject);
    void inputFromJSON(QJsonObject &rvObject);

signals:

public slots:
   void clear(void);
   void femProgramChanged(const QString &arg1);
   void chooseFileName1(void);
   void chooseFileName2(void);

private:
    QVBoxLayout *layout;
    QWidget     *femSpecific;
    QComboBox   *femSelection;

    QLineEdit *file1;
    QLineEdit *file2;

    QString fileName1;
    QString fileName2;
};

#endif // INPUTWIDGETFEM_H
