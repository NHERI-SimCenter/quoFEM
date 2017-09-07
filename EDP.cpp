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
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

#include "EDP.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QJsonObject>
#include <QFrame>

//
// headers for EDPDistribution subclasses that user can select
//

EDP::EDP(QWidget *parent)
    :QWidget(parent)
{
    //
    // create a vertical layout to deal with variable name
    //

    QVBoxLayout *nameLayout = new QVBoxLayout();
    QLabel *variableLabel = new QLabel();
    variableLabel->setText(QString("Variable Name"));
    variableName = new QLineEdit();
    variableLabel->setMaximumWidth(100);
    variableLabel->setMinimumWidth(100);
    variableName->setMaximumWidth(100);
    variableName->setMinimumWidth(100);
    nameLayout->addWidget(variableLabel);
    nameLayout->addWidget(variableName);
    nameLayout->setSpacing(0);
    nameLayout->setMargin(0);

    QWidget *theWidget = new QWidget();
    QHBoxLayout *widgetLayout = new QHBoxLayout;
    widgetLayout->addLayout(nameLayout);
    widgetLayout->setSpacing(0);
    widgetLayout->setMargin(0);
    theWidget->setLayout(widgetLayout);
    widgetLayout->addStretch();

    mainLayout = new QHBoxLayout;
    mainLayout->addStretch();
    mainLayout->insertWidget(mainLayout->count()-1, theWidget);

    //    theDistribution = new NormalDistribution(this);
    //    mainLayout->insertWidget(mainLayout->count()-1, theDistribution);
    //    mainLayout->addWidget(theDistribution);

    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    this->setLayout(mainLayout);
}

EDP::~EDP()
{

}

void EDP::outputToJSON(QJsonObject &jsonObject){
    jsonObject["name"]=variableName->text();
}

void EDP::inputFromJSON(QJsonObject &jsonObject){
    QJsonValue theMeanValue = jsonObject["name"];
    variableName->setText(theMeanValue.toString());
}

