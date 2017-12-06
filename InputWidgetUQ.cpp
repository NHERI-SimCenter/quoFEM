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


InputWidgetUQ::InputWidgetUQ(QWidget *parent)
    : SimCenterWidget(parent),uqType(0)
{
    //femSpecific = 0;

    layout = new QVBoxLayout();

    QVBoxLayout *name= new QVBoxLayout;

    // text and add button at top
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *textFEM=new SectionTitle();
    textFEM->setText(tr("UQ Method"));
    textFEM->setMinimumWidth(250);
    QSpacerItem *spacer = new QSpacerItem(50,10);

    uqSelection = new QComboBox();
    uqSelection->setMaximumWidth(100);
    uqSelection->setMinimumWidth(100);

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
    uqSelection->addItem(tr("Reliability"));
    uqSelection->addItem(tr("Calibration"));

    connect(uqSelection, SIGNAL(currentIndexChanged(QString)), this, SLOT(uqSelectionChanged(QString)));

    layout->addLayout(name);

    layout->addStretch();
    //   run = new QPushButton();
    //   run->setText(tr("RUN"));
    //   layout->addWidget(run);

    this->setLayout(layout);
    // QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Fixed);
    // this->setSizePolicy(sp);

    this->uqSelectionChanged(tr("Sampling"));
    //layout->setSpacing(10);
    layout->setMargin(0);
    //layout->addStretch();
}

InputWidgetUQ::~InputWidgetUQ()
{

}


void InputWidgetUQ::clear(void)
{

}



void
InputWidgetUQ::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject uq;

    if (uqType != 0) {
        uqType->outputToJSON(uq);
    }
    uq["uqType"] = uqSelection->currentText();
    jsonObject["uqMethod"]=uq;
}


void
InputWidgetUQ::inputFromJSON(QJsonObject &jsonObject)
{
    this->clear();

    QJsonObject uq = jsonObject["uqMethod"].toObject();
    QString selection = uq["uqType"].toString();
    this->uqSelectionChanged(selection);
    if (uqType != 0)
        uqType->inputFromJSON(uq);
}

int InputWidgetUQ::processResults(QString &filenameResults, QString &filenameTab)
{
    if (samplingWidget != 0)
        return samplingWidget->processResults(filenameResults, filenameTab);
}

void InputWidgetUQ::uqSelectionChanged(const QString &arg1)
{
    if (uqType != 0) {
        // layout->rem
        samplingWidget = 0;
        layout->removeWidget(uqType);
        delete uqType;
        uqType = 0;
    }


    if (arg1 == QString("Sampling")) {
        //uqType = new InputWidgetSampling();
        samplingWidget = new InputWidgetSampling();
        uqType = samplingWidget;
    }

    if (uqType != 0)
        layout->insertWidget(1, uqType,1);
}

DakotaResults *
InputWidgetUQ::getResults(void)
{
    if (samplingWidget != 0) {
        return samplingWidget->getResults();
    }

    return 0;
}
