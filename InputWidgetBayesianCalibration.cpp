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

#include "InputWidgetBayesianCalibration.h"
#include <DakotaResultsBayesianCalibration.h>
#include <RandomVariablesContainer.h>


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


InputWidgetBayesianCalibration::InputWidgetBayesianCalibration(QWidget *parent)
    : InputWidgetDakotaMethod(parent),uqSpecific(0)
{
    layout = new QVBoxLayout();

    QHBoxLayout *methodLayout= new QHBoxLayout;
    QLabel *label1 = new QLabel();
    label1->setText(QString("Method"));
    calibrationMethod = new QComboBox();
    calibrationMethod->addItem(tr("DREAM"));
   // calibrationMethod->addItem(tr("QUESO"));
    
    methodLayout->addWidget(label1);
    methodLayout->addWidget(calibrationMethod,2);
    methodLayout->addStretch(4);
    
    QGridLayout *otherLayout= new QGridLayout;
    QLabel *label2 = new QLabel();
    label2->setText(QString("#Chain Samples"));
    chainSamples = new QLineEdit();
    chainSamples->setText(tr("10"));
    //chainSamples->setMaximumWidth(100);
    //chainSamples->setMinimumWidth(100);

    otherLayout->addWidget(label2, 0,0);
    otherLayout->addWidget(chainSamples, 0,1);
    
    QLabel *label3 = new QLabel();
    label3->setText(QString("Seed"));
    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;

    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    //    randomSeed->setMaximumWidth(100);
    //    randomSeed->setMinimumWidth(100);
    
    otherLayout->addWidget(label3, 1,0);
    otherLayout->addWidget(randomSeed, 1,1);
    otherLayout->setColumnStretch(2,1);
    otherLayout->setRowStretch(2,1);

    layout->addLayout(methodLayout);
    layout->addLayout(otherLayout);

    this->setLayout(layout);
}

InputWidgetBayesianCalibration::~InputWidgetBayesianCalibration()
{

}

int 
InputWidgetBayesianCalibration::getMaxNumParallelTasks(void){
  return 1;
}

void InputWidgetBayesianCalibration::clear(void)
{

}


bool
InputWidgetBayesianCalibration::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    QJsonObject uq;
    uq["method"]=calibrationMethod->currentText();
    uq["chain_samples"]=chainSamples->text().toInt();
    uq["seed"]=randomSeed->text().toDouble();

    jsonObject["bayesian_calibration_method_data"]=uq;
    return result;

}


bool
InputWidgetBayesianCalibration::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;
    this->clear();

    if (jsonObject.contains("bayesian_calibration_method_data")) {
        QJsonObject uq = jsonObject["bayesian_calibration_method_data"].toObject();
        QString method =uq["method"].toString();
        int samples=uq["chain_samples"].toInt();
        double seed=uq["seed"].toDouble();

        chainSamples->setText(QString::number(samples));
        randomSeed->setText(QString::number(seed));

        int index = calibrationMethod->findText(method);
        calibrationMethod->setCurrentIndex(index);

    } else {
        emit sendErrorMessage("ERROR: Bayesian Calibration INput - no \"bayesian_calibration_method\" data");
        return false;
    }

    return true;
}

void InputWidgetBayesianCalibration::uqSelectionChanged(const QString &arg1)
{

}

int InputWidgetBayesianCalibration::processResults(QString &filenameResults, QString &filenameTab) {

    return 0;
}

DakotaResults *
InputWidgetBayesianCalibration::getResults(void) {
  return new DakotaResultsBayesianCalibration();
}

RandomVariablesContainer *
InputWidgetBayesianCalibration::getParameters(void) {
  QString classType("Uncertain");
  return new RandomVariablesContainer(classType);
}
