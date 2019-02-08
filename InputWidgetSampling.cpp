// Written: fmckenna

// added and modified: padhye

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

#include "InputWidgetSampling.h"
#include <DakotaResultsSampling.h>
#include <RandomVariableInputWidget.h>


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


InputWidgetSampling::InputWidgetSampling(QWidget *parent)
: InputWidgetDakotaMethod(parent),uqSpecific(0)
{
    layout = new QVBoxLayout();

    //
    // create layout for selection box for method type to layout
    //

    QVBoxLayout *methodLayout= new QVBoxLayout;
    QLabel *label1 = new QLabel();
    label1->setText(QString("Method"));
    samplingMethod = new QComboBox();
    samplingMethod->setStyleSheet("QComboBox::down-arrow {image: url(C://Users//nikhil//NHERI/uqFEM//images//pulldownarrow.PNG);heigth:50px;width:100px;}");
    samplingMethod->addItem(tr("LHS"));
    samplingMethod->addItem(tr("Monte Carlo"));
    
    methodLayout->addWidget(label1);
    methodLayout->addWidget(samplingMethod);
    
    //
    // create layout label and entry for # samples
    //

    QVBoxLayout *samplesLayout= new QVBoxLayout;
    QLabel *label2 = new QLabel();
    label2->setText(QString("# Samples"));
    numSamples = new QLineEdit();
    numSamples->setText(tr("10"));
    numSamples->setMaximumWidth(100);
    numSamples->setMinimumWidth(100);
    
    samplesLayout->addWidget(label2);
    samplesLayout->addWidget(numSamples);
    
    //
    // create label and entry for seed to layout
    //

    QVBoxLayout *seedLayout= new QVBoxLayout;
    QLabel *label3 = new QLabel();
    label3->setText(QString("Seed"));
    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;

    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setMaximumWidth(100);
    randomSeed->setMinimumWidth(100);
    
    seedLayout->addWidget(label3);
    seedLayout->addWidget(randomSeed);


    // add a checkbox for Sobolev index
    sobolevCheckBox =new QCheckBox("Sobolev Index");
    connect(sobolevCheckBox,SIGNAL(clicked(bool)),this,SLOT(setSobolevFlag(bool)));
    flagForSobolevIndices=0;
    
    //
    // create main layout and add previously created layouts to it
    //

    QHBoxLayout *mLayout = new QHBoxLayout();
    mLayout->addLayout(methodLayout);
    mLayout->addLayout(samplesLayout);
    mLayout->addLayout(seedLayout);
    mLayout->addStretch(1);
    mLayout->addWidget(sobolevCheckBox);
    mLayout->addStretch(5);

    layout->addLayout(mLayout);

    // finally add the EDP layout & set widget layout

    theEdpWidget = new InputWidgetEDP();
    layout->addWidget(theEdpWidget,1);

    this->setLayout(layout);
}

InputWidgetSampling::~InputWidgetSampling()
{

}

int 
InputWidgetSampling::getMaxNumParallelTasks(void){
  return numSamples->text().toInt();
}

void InputWidgetSampling::clear(void)
{

}


bool
InputWidgetSampling::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    //note method will always return true as initial values set

    //
    // in a json object we will place the values and then add object to input
    //

    QJsonObject uq;
    uq["method"]=samplingMethod->currentText();
    uq["samples"]=numSamples->text().toInt();
    uq["seed"]=randomSeed->text().toDouble();
    if (flagForSobolevIndices == 1)
      uq["sobelov_indices"]=flagForSobolevIndices;
    result = theEdpWidget->outputToJSON(uq);
    jsonObject["samplingMethodData"]=uq;
    return result;
}


bool
InputWidgetSampling::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = false;
    this->clear();

    //
    // get sampleingMethodData, if not present it's an error

    if (jsonObject.contains("samplingMethodData")) {
        QJsonObject uq = jsonObject["samplingMethodData"].toObject();

        //
        // get method, #sample and seed, if not present an error
        // given method, set selection box to point to it and finnally input the EDPs
        //

        if (uq.contains("method") && uq.contains("samples") && uq.contains("seed")) {

            QString method =uq["method"].toString();
            int samples=uq["samples"].toInt();
            double seed=uq["seed"].toDouble();
            if (uq.contains("sobolev_indices") && uq.contains("samples")) {
                flagForSobolevIndices = uq["sobolev_indices"].toInt();
                sobolevCheckBox->setChecked(true);
                // set the sobolevFlag
            }

            else {
                flagForSobolevIndices = 0;
                sobolevCheckBox->setChecked(false);
            }
		

            numSamples->setText(QString::number(samples));
            randomSeed->setText(QString::number(seed));
            int index = samplingMethod->findText(method);
            if (index == -1) {
                emit sendErrorMessage("ERROR: Sampling Input Widget - \"samplingMethodData\" invalid type .. keeping current");
                return false;
            }

            samplingMethod->setCurrentIndex(index);
            return theEdpWidget->inputFromJSON(uq);

            // initializing the correlation matrix here


        } else {
            emit sendErrorMessage("ERROR: Sampling Input Widget - no \"method\" ,\"samples\" or \"seed\" data");
            return false;
        }

    } else {
        emit sendErrorMessage("ERROR: Sampling Input Widget - no \"samplingMethodData\" input");
        return false;
    }

    // should never get here .. if do my logic is screwy and need to return a false
    emit sendErrorMessage("ERROR - faulty logic - contact code developers");
    return result;
}

void InputWidgetSampling::uqSelectionChanged(const QString &arg1)
{
    // if more data than just num samples and seed code would go here to add or remove widgets from layout
}

int InputWidgetSampling::processResults(QString &filenameResults, QString &filenameTab) {

    return 0;
}

DakotaResults *
InputWidgetSampling::getResults(void) {
    return new DakotaResultsSampling();
}

RandomVariableInputWidget *
InputWidgetSampling::getParameters(void) {
    QString classType("Uncertain");
  return new RandomVariableInputWidget(classType);
}

void InputWidgetSampling::setSobolevFlag(bool value) {
    if(value) {
      flagForSobolevIndices=1;
    }
    return;
}
