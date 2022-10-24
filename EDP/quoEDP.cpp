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

#include "quoEDP.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QJsonObject>
#include <QFrame>
#include <QRadioButton>
#include <QDebug>
#include <QGridLayout>
#include <QValidator>
#include <QPushButton>

//
// headers for quoEDPDistribution subclasses that user can select
//

static QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit){

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
    //theLayout->setMargin(0);

    QWidget *theWidget = new QWidget();
    theWidget->setLayout(theLayout);

    *theLineEdit = theEdit;
    return theWidget;
}

QWidget *addOneEDP(QString theLabelName, QLineEdit **theLineEdit,
                   QString theLabelLength, QLineEdit **theLength){
//QWidget *addOneEDP(QString theLabelName, QLineEdit **theLineEdit,
//                   QString theLabelLength, QLineEdit **theLength,
//                   QString theLabelIndCoords, QLineEdit **theIndCoords,
//                   QString theLabelVarType, QLineEdit **theVarType){

//    QVBoxLayout *theLayout = new QVBoxLayout();
    QGridLayout *theLayout = new QGridLayout();
    QLabel *theLabel = new QLabel();
    theLabel->setText(theLabelName);
    QLineEdit *theEdit = new QLineEdit();
    theLabel->setMaximumWidth(200);
    theEdit->setMaximumWidth(200);
    theLabel->setMinimumWidth(40);
    theEdit->setMinimumWidth(40);

    int bottom = 1;
    QIntValidator *posIntValidator = new QIntValidator();
    posIntValidator->setBottom(bottom);

    QLabel *lengthLabel = new QLabel();
    lengthLabel->setText(theLabelLength);
    QLineEdit *lengthEdit = new QLineEdit();
    lengthEdit->setText("1");
    lengthEdit->setValidator(posIntValidator);
    lengthEdit->setMaximumWidth(60);
    lengthLabel->setMaximumWidth(60);
    lengthEdit->setMinimumWidth(60);
    lengthLabel->setMinimumWidth(60);

//    QLabel *numIndCoordsLabel = new QLabel();
//    numIndCoordsLabel->setText(theLabelIndCoords);
//    QLineEdit *numIndCoordsEdit = new QLineEdit();
//    numIndCoordsEdit->setValidator(posIntValidator);

//    QLabel *expVarTypeLabel = new QLabel();
//    expVarTypeLabel->setText(theLabelVarType);
//    QLineEdit *expVarTypeEdit = new QLineEdit();

    theLayout->addWidget(theLabel, 0, 0);
    theLayout->addWidget(theEdit, 1, 0);
    theLayout->addWidget(lengthLabel, 0, 1);
    theLayout->addWidget(lengthEdit, 1, 1);
//    theLayout->addWidget(numIndCoordsLabel, 0, 2);
//    theLayout->addWidget(numIndCoordsEdit, 1, 2);
//    theLayout->addWidget(expVarTypeLabel, 0, 3);
//    theLayout->addWidget(expVarTypeEdit, 1, 3);

//    theLayout->setSpacing(0);
    theLayout->setHorizontalSpacing(10);
    theLayout->setVerticalSpacing(0);
    //theLayout->setMargin(0);

    theLayout->setColumnStretch(0, 4);
    theLayout->setColumnStretch(1, 1);
    theLayout->setColumnStretch(2, 1);
    theLayout->setColumnStretch(3, 1);

    QWidget *theWidget = new QWidget();
    theWidget->setLayout(theLayout);

    *theLineEdit = theEdit;
    *theLength = lengthEdit;
//    *theIndCoords = numIndCoordsEdit;
//    *theVarType = expVarTypeEdit;
    return theWidget;
}

// abs - changing the EDP method to support field data
//EDP::EDP(QString EDPname, QWidget *parent)
//    :QWidget(parent),resultsSet(false),mean(0),stdDev(0)
quoEDP::quoEDP(QString EDPname, QWidget *parent)
    :QWidget(parent),resultsSet(false),mean(0),stdDev(0)
{
    //
    // create a vertical layout to deal with variable name
    //

    mainLayout = new QHBoxLayout;

    // abs - changing the widget for EDP definition
//    QWidget *nameWidget = addLabeledLineEdit(QString("Variable Name"), &variableName);
    QWidget *nameWidget = addOneEDP(QString("Variable Name"), &variableName, QString("Length"), &varLength);
//    QWidget *nameWidget = addOneEDP(QString("Variable Name"), &variableName, QString("Length"), &varLength,
//                                    QString("# Coordinates"), &varNumIndCoords,
//                                    QString("Variance Type"), &varExperimentVarianceType);

    variableName->setText(EDPname);

    //button = new QRadioButton();
    //mainLayout->addWidget(button);

    //
    // create remove button (not used any more - sy)
    //
    QPushButton *removeButton = new QPushButton("×");
    const QSize BUTTON_SIZE = QSize(15, 15);
    removeButton->setFixedSize(BUTTON_SIZE);
    removeButton->setStyleSheet("QPushButton { font-size:15px;  font-weight: bold;padding: 0px 0px 2px 0px; }");
    mainLayout->addWidget(removeButton);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(xButtonClicked()) );


    mainLayout->addWidget(nameWidget);;
    mainLayout->addStretch();

    mainLayout->setSpacing(10);
    //mainLayout->setMargin(0);

    this->setLayout(mainLayout);
}

quoEDP::~quoEDP()
{

}

void
quoEDP::xButtonClicked(void){
    emit removeEDPclicked(this);
}

bool
quoEDP::isSelectedForRemoval(void)
{
    return button->isChecked();
}

bool quoEDP::outputToJSON(QJsonObject &jsonObject){
    jsonObject["name"]=variableName->text().simplified();
    if (resultsSet == true) {
        jsonObject["mean"]=mean->text().toDouble();
        jsonObject["stdDev"]=stdDev->text().toDouble();
    }

    // abs - adding for new EDP definition
    if (varLength->text().toInt() > 1) {
        jsonObject["type"]="field";
//        jsonObject["numIndCoords"]=varNumIndCoords->text().toInt();
    } else {
        jsonObject["type"]="scalar";
    }
    jsonObject["length"]=varLength->text().toInt();
//    jsonObject["expVarianceType"]=varExperimentVarianceType->text().simplified();

    return true;
}

bool quoEDP::inputFromJSON(QJsonObject &jsonObject){
    QJsonValue theMeanValue = jsonObject["name"];
    variableName->setText(theMeanValue.toString().simplified());

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

    // abs - adding for new EDP definition
    if (jsonObject.contains("type")) {
        QJsonValue theVarLength = jsonObject["length"];
        // add the length
        varLength->setText(QString::number(theVarLength.toInt()));
//        QJsonValue theVarType = jsonObject["expVarianceType"];
//        varExperimentVarianceType->setText(theVarType.toString().simplified());
//        if (jsonObject.contains("numIndCoords")) {
//            QJsonValue theNumIndCoords = jsonObject["numIndCoords"];
//            varNumIndCoords->setText(QString::number(theNumIndCoords.toInt()));
//        }
    }
    else {
        varLength->setText("1");
//        varExperimentVarianceType->setText("");
//        varNumIndCoords->setText("");
    }
    return true;
}

void quoEDP::setResults(double *data)
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
