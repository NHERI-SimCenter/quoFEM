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

#include <GlobalReliabilityWidget.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>
#include <QButtonGroup>
#include <QCheckBox>


GlobalReliabilityWidget::GlobalReliabilityWidget(QWidget *parent) 
: UQ_MethodInputWidget(parent)
{
     QGridLayout *layout = new QGridLayout();

    // create layout label and entry for # samples
    QHBoxLayout *methodLayout= new QHBoxLayout;
    QLabel *label2 = new QLabel();
    label2->setText(QString("GP Approximation"));
    gpApproximation = new QComboBox();
    gpApproximation->addItem(tr("x-space"));
    gpApproximation->addItem(tr("u-space"));
    gpApproximation->setToolTip("Create Gaussian Proccess Approximation in x-space or u-space");

    layout->addWidget(label2, 0,1);
    layout->addWidget(gpApproximation, 0,2);

    /*
    QButtonGroup *theButtonGroup = new QButtonGroup();
    checkedResponseLevel = new QCheckBox();
    checkedProbabilityLevel = new QCheckBox();
    checkedProbabilityLevel->setChecked(true);
    theButtonGroup->addButton(checkedResponseLevel);
    theButtonGroup->addButton(checkedProbabilityLevel);

    responseLevel = new QLineEdit();
    layout->addWidget(label3, 1,1);
    layout->addWidget(mppMethod, 1,2);

    layout->addWidget(checkedResponseLevel, 2,0);
    layout->addWidget(new QLabel("Response Levels"), 2, 1);
    layout->addWidget(responseLevel, 2, 2);
    layout->addWidget(checkedProbabilityLevel, 3,0);
    */

    probabilityLevel = new QLineEdit();
    probabilityLevel->setText("");

    layout->addWidget(new QLabel("Response Levels"), 1, 1);
    layout->addWidget(probabilityLevel, 1, 2);

    layout->setColumnStretch(2,2);
    layout->setColumnStretch(3,4);
    layout->setRowStretch(5,1);


    this->setLayout(layout);
}

GlobalReliabilityWidget::~GlobalReliabilityWidget()
{

}

bool
GlobalReliabilityWidget::outputToJSON(QJsonObject &jsonObj){

    bool result = true;
    jsonObj["gpApproximation"]=gpApproximation->currentText();

    QJsonArray responseLevel;
    QStringList responseLevelList = QStringList(probabilityLevel->text().split(" "));
    for (int i = 0; i < responseLevelList.size(); ++i)
        responseLevel.push_back(responseLevelList.at(i).toDouble());
    jsonObj["responseLevel"]=responseLevel;

    return result;    
}

bool
GlobalReliabilityWidget::inputFromJSON(QJsonObject &jsonObject){

    bool result = false;
    if ( (jsonObject.contains("gpApproximation"))
         && (jsonObject.contains("probabilityLevel"))
         ) {

        //responseLevel->setText("");
        probabilityLevel->setText("");

        QString scheme=jsonObject["gpApproximation"].toString();
        gpApproximation->setCurrentIndex(gpApproximation->findText(scheme));

        QJsonArray probLevels;

        QJsonValue probLevelVal = jsonObject["responseLevel"];
        if (probLevelVal.isArray()) {

            QStringList levelList;
            QJsonArray levels = probLevelVal.toArray();

            for (int i=0; i<levels.count(); i++) {
                QJsonValue value = levels.at(i);
                double levelV = value.toDouble();
                levelList << QString::number(levelV);
            }
            probabilityLevel->setText(levelList.join(" "));
        } else {
            qDebug() << "FORM Input: Probability level not json array";
        }

        return true;
    }

    return result;
}

void
GlobalReliabilityWidget::clear(void)
{

}



int
GlobalReliabilityWidget::getNumberTasks()
{
    return 1;
}
