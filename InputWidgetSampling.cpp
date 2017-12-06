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

#include "InputWidgetSampling.h"
#include <DakotaResultsSampling.h>

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
    : SimCenterWidget(parent),uqSpecific(0)
{
    layout = new QVBoxLayout();

    QVBoxLayout *methodLayout= new QVBoxLayout;
    QLabel *label1 = new QLabel();
    label1->setText(QString("Method"));
    samplingMethod = new QComboBox();
    samplingMethod->addItem(tr("LHS"));
    samplingMethod->addItem(tr("Monte Carlo"));
    
    methodLayout->addWidget(label1);
    methodLayout->addWidget(samplingMethod);
    
    QVBoxLayout *samplesLayout= new QVBoxLayout;
    QLabel *label2 = new QLabel();
    label2->setText(QString("# Samples"));
    numSamples = new QLineEdit();
    numSamples->setText(tr("10"));
    numSamples->setMaximumWidth(100);
    numSamples->setMinimumWidth(100);

    
    samplesLayout->addWidget(label2);
    samplesLayout->addWidget(numSamples);
    
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
    
    QHBoxLayout *mLayout = new QHBoxLayout();
    mLayout->addLayout(methodLayout);
    mLayout->addLayout(samplesLayout);
    mLayout->addLayout(seedLayout);
    mLayout->addStretch();

    layout->addLayout(mLayout);

    theEdpWidget = new InputWidgetEDP();
    layout->addWidget(theEdpWidget,1);

    this->setLayout(layout);
}

InputWidgetSampling::~InputWidgetSampling()
{

}


void InputWidgetSampling::clear(void)
{

}



void
InputWidgetSampling::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject uq;
    uq["method"]=samplingMethod->currentText();
    uq["samples"]=numSamples->text().toInt();
    uq["seed"]=randomSeed->text().toDouble();
    theEdpWidget->outputToJSON(uq);
    jsonObject["samplingMethodData"]=uq;
}


void
InputWidgetSampling::inputFromJSON(QJsonObject &jsonObject)
{
    this->clear();

    QJsonObject uq = jsonObject["samplingMethodData"].toObject();
    QString method =uq["method"].toString();
    int samples=uq["samples"].toInt();
    double seed=uq["seed"].toDouble();

    numSamples->setText(QString::number(samples));
    randomSeed->setText(QString::number(seed));
    int index = samplingMethod->findText(method);
    samplingMethod->setCurrentIndex(index);

    theEdpWidget->inputFromJSON(uq);
}

void InputWidgetSampling::uqSelectionChanged(const QString &arg1)
{

}

int InputWidgetSampling::processResults(QString &filenameResults, QString &filenameTab) {

    return 0;
}

DakotaResults *
InputWidgetSampling::getResults(void) {
    return new DakotaResultsSampling();
}
