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

#include "InputWidgetUQ.h"
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

#include <SimCenterWidget.h>
#include <InputWidgetSampling.h>
#include <InputWidgetCalibration.h>
#include <InputWidgetBayesianCalibration.h>


InputWidgetUQ::InputWidgetUQ(QWidget *parent)
    : SimCenterWidget(parent),dakotaMethod(0)
{
    layout = new QVBoxLayout();

    QVBoxLayout *name= new QVBoxLayout;

    // text and add button at top
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *textFEM=new SectionTitle();
    textFEM->setText(tr("UQ Method"));
    textFEM->setMinimumWidth(250);
    QSpacerItem *spacer = new QSpacerItem(50,10);

    uqSelection = new QComboBox();
    uqSelection->setMaximumWidth(200);
    uqSelection->setMinimumWidth(200);

    titleLayout->addWidget(textFEM);
    titleLayout->addItem(spacer);
    titleLayout->addWidget(uqSelection);
    titleLayout->addStretch();
    titleLayout->setSpacing(0);
    titleLayout->setMargin(0);

    name->addLayout(titleLayout);

    name->setSpacing(10);
    name->setMargin(0);

    uqSelection->addItem(tr("Sampling"));
    uqSelection->addItem(tr("Calibration"));
    uqSelection->addItem(tr("Bayesian Calibration"));

    connect(uqSelection, SIGNAL(currentIndexChanged(QString)), this, SLOT(uqSelectionChanged(QString)));

    layout->addLayout(name);
    layout->addStretch();

    this->setLayout(layout);

    // set Samlping as the default
    this->uqSelectionChanged(tr("Sampling"));
    layout->setMargin(0);
}

InputWidgetUQ::~InputWidgetUQ()
{

}


void InputWidgetUQ::clear(void)
{

}



bool
InputWidgetUQ::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    //
    // create a json object, fill it and then add this to the object with uqMethod key
    //

    QJsonObject uq;
    if (dakotaMethod != 0) {
        result = dakotaMethod->outputToJSON(uq);
    }
    uq["uqType"] = uqSelection->currentText();

    jsonObject["uqMethod"]=uq;

    return result;
}


bool
InputWidgetUQ::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;

    this->clear();

    if (jsonObject.contains("uqMethod")) {
        QJsonObject uq = jsonObject["uqMethod"].toObject();
        QString selection = uq["uqType"].toString();
        int index = uqSelection->findText(selection);
        uqSelection->setCurrentIndex(index);
        this->uqSelectionChanged(selection);
        if (selectionChangeOK == true && dakotaMethod != 0)
            result = dakotaMethod->inputFromJSON(uq);
        else
            result = false;
    } else {
        qDebug() << "ERROR: UQ Method Input - no \"uqMethod\" entry";
        emit sendErrorMessage("ERROR: UQ Method Input - no \"uqMethod\" entry");
        return false;
    }
    return result;
}

int InputWidgetUQ::processResults(QString &filenameResults, QString &filenameTab)
{
    if (dakotaMethod != 0)
        return dakotaMethod->processResults(filenameResults, filenameTab);
    return 0;
}

void InputWidgetUQ::uqSelectionChanged(const QString &arg1)
{
    selectionChangeOK = true;

    /* ************************************************************
     * old functionailty .. now leaving last active if incorrect ..
     * note way incorrect if reading from bad json input
    if (uqType != 0) {
        // layout->rem
        dakotaMethod = 0;
        layout->removeWidget(uqType);
        delete uqType;
        uqType = 0;
    }
    **************************************************************/
    InputWidgetDakotaMethod *oldMethod = dakotaMethod;

    if (dakotaMethod != 0)
        layout->removeWidget(dakotaMethod);

    if (arg1 == QString("Sampling")) {
        delete dakotaMethod;
        dakotaMethod = new InputWidgetSampling();

    } else if (arg1 == QString("Calibration")) {
        delete dakotaMethod;
        dakotaMethod = new InputWidgetCalibration();

    } else if (arg1 == QString("Bayesian Calibration")) {
        delete dakotaMethod;
        dakotaMethod = new InputWidgetBayesianCalibration();
    } else {
        selectionChangeOK = false;
        emit sendErrorMessage("ERROR: UQ Input - no valid Method provided .. keeping old");
    }

    if (dakotaMethod != 0) {
        this->uqWidgetChanged();
        layout->insertWidget(1, dakotaMethod,1);
        connect(dakotaMethod,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    }

    return;
}

DakotaResults *
InputWidgetUQ::getResults(void)
{
    if (dakotaMethod != 0) {
        return dakotaMethod->getResults();
    }

    return 0;
}

RandomVariableInputWidget *
InputWidgetUQ::getParameters(void)
{
    if (dakotaMethod != 0) {
        return dakotaMethod->getParameters();
    }

    return 0;
}

void
InputWidgetUQ::errorMessage(QString message){
  emit sendErrorMessage(message);
}
