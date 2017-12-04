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
#include <QRadioButton>
#include <QDebug>

//
// headers for EDPDistribution subclasses that user can select
//

QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit){

    QVBoxLayout *theLayout = new QVBoxLayout();
    QLabel *theLabel = new QLabel();
    theLabel->setText(theLabelName);
    QLineEdit *theEdit = new QLineEdit();
    theLabel->setMaximumWidth(200);
    theLabel->setMinimumWidth(200);
    theEdit->setMaximumWidth(200);
    theEdit->setMinimumWidth(200);
    theLayout->addWidget(theLabel);
    theLayout->addWidget(theEdit);
    theLayout->setSpacing(0);
    theLayout->setMargin(0);
    theLayout->addStretch();

    QWidget *theWidget = new QWidget();
    theWidget->setLayout(theLayout);

    *theLineEdit = theEdit;
    return theWidget;
}

EDP::EDP(QWidget *parent)
    :QWidget(parent),resultsSet(false),mean(0),stdDev(0)
{
    //
    // create a vertical layout to deal with variable name
    //

    mainLayout = new QHBoxLayout;

    QWidget *nameWidget = addLabeledLineEdit(QString("Variable Name"), &variableName);

    button = new QRadioButton();
    mainLayout->addWidget(button);
    mainLayout->addWidget(nameWidget);;
    mainLayout->addStretch();

    mainLayout->setSpacing(5);
    mainLayout->setMargin(5);

    this->setLayout(mainLayout);
}

EDP::~EDP()
{

}

bool
EDP::isSelectedForRemoval(void)
{
    return button->isChecked();
}

void EDP::outputToJSON(QJsonObject &jsonObject){
    jsonObject["name"]=variableName->text();
    if (resultsSet == true) {
        jsonObject["mean"]=mean->text().toDouble();
        jsonObject["stdDev"]=stdDev->text().toDouble();
    }
}

void EDP::inputFromJSON(QJsonObject &jsonObject){
    QJsonValue theMeanValue = jsonObject["name"];
    variableName->setText(theMeanValue.toString());

    if (jsonObject.contains("mean")) {

        // add mean and stdDevl labels
        QWidget *meanWidget = addLabeledLineEdit(QString("Mean"), &mean);
        QJsonValue theMeanValue = jsonObject["mean"];
        mean->setText(QString::number(theMeanValue.toDouble()));

        mainLayout->insertWidget(2, meanWidget);

        QWidget *stdDevWidget = addLabeledLineEdit(QString("StdDev"), &stdDev);
        QJsonValue theStdDevValue = jsonObject["stdDev"];
        stdDev->setText(QString::number(theStdDevValue.toDouble()));

        mainLayout->insertWidget(3, stdDevWidget);
        mainLayout->addStretch();
    }
}

void EDP::setResults(double *data)
{
    if (mean == 0) {
        // create mean and stdDev boxes, fill in
        QWidget *meanWidget = addLabeledLineEdit(QString("Mean"), &mean);
        mean->setText(QString::number(data[0]));
        mainLayout->insertWidget(2, meanWidget);

        QWidget *stdDevWidget = addLabeledLineEdit(QString("StdDev"), &stdDev);
        stdDev->setText(QString::number(data[1]));

        mainLayout->insertWidget(3, stdDevWidget);
        mainLayout->addStretch();
    } else {
        // set text in mean and stdDev boxes
        stdDev->setText(QString::number(data[1]));
        mean->setText(QString::number(data[0]));

    }
}
