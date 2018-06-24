// Written: fmckenna

/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna

#include "InputWidgetCalibration.h"
#include <DakotaResultsCalibration.h>

#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <sectiontitle.h>
#include <InputWidgetEDP.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>

#include <RandomVariableInputWidget.h>


InputWidgetCalibration::InputWidgetCalibration(QWidget *parent)
    : InputWidgetDakotaMethod(parent)
{
    layout = new QVBoxLayout();

    QVBoxLayout *methodLayout= new QVBoxLayout;
    QLabel *label1 = new QLabel();
    label1->setText(QString("Method"));
    calibrationMethod = new QComboBox();
    calibrationMethod->addItem(tr("OPT++GaussNewton"));
    calibrationMethod->addItem(tr("NL2SOL"));
    
    methodLayout->addWidget(label1);
    methodLayout->addWidget(calibrationMethod);
    
    QVBoxLayout *maxIterLayout= new QVBoxLayout;
    QLabel *label2 = new QLabel();
    label2->setText(QString("max # Iterations"));
    maxIterations = new QLineEdit();
    maxIterations->setText(tr("1000"));
    maxIterations->setMaximumWidth(100);
    maxIterations->setMinimumWidth(100);
    
    maxIterLayout->addWidget(label2);
    maxIterLayout->addWidget(maxIterations);
    
    QVBoxLayout *tolLayout= new QVBoxLayout;
    QLabel *label3 = new QLabel();
    label3->setText(QString("Convergence Tol"));
    convergenceTol = new QLineEdit();
    convergenceTol->setText(QString::number(1.0e-2));
    convergenceTol->setMaximumWidth(100);
    convergenceTol->setMinimumWidth(100);
    
    tolLayout->addWidget(label3);
    tolLayout->addWidget(convergenceTol);
    
    QHBoxLayout *mLayout = new QHBoxLayout();
    mLayout->addLayout(methodLayout);
    mLayout->addLayout(maxIterLayout);
    mLayout->addLayout(tolLayout);
    mLayout->addStretch();

    layout->addLayout(mLayout);

    theEdpWidget = new InputWidgetEDP();
    layout->addWidget(theEdpWidget,1);

    this->setLayout(layout);
}

InputWidgetCalibration::~InputWidgetCalibration()
{

}


int 
InputWidgetCalibration::getMaxNumParallelTasks(void){
  return 1;
}

void InputWidgetCalibration::clear(void)
{

}

bool
InputWidgetCalibration::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject uq;
    uq["method"]=calibrationMethod->currentText();
    uq["maxIterations"]=maxIterations->text().toInt();
    uq["convergenceTol"]=convergenceTol->text().toDouble();
    theEdpWidget->outputToJSON(uq);
    jsonObject["calibrationMethodData"]=uq;

    return result;
}


bool
InputWidgetCalibration::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;
    this->clear();

    QJsonObject uq = jsonObject["calibrationMethodData"].toObject();
    QString method =uq["method"].toString();
    int maxIter=uq["maxIterations"].toInt();
    double convTol=uq["convergenceTol"].toDouble();

    maxIterations->setText(QString::number(maxIter));
    convergenceTol->setText(QString::number(convTol));
    int index = calibrationMethod->findText(method);
    calibrationMethod->setCurrentIndex(index);

    result = theEdpWidget->inputFromJSON(uq);
    return result;

}

void 
InputWidgetCalibration::methodChanged(const QString &arg1)
{

}

int 
InputWidgetCalibration::processResults(QString &filenameResults, QString &filenameTab) 
{
    return 0;
}

DakotaResults *
InputWidgetCalibration::getResults(void) {
  return new DakotaResultsCalibration();
}

RandomVariableInputWidget *
InputWidgetCalibration::getParameters(void) {
  QString classType("Design");
  return new RandomVariableInputWidget(classType);
}
