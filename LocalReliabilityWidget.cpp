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

#include <LocalReliabilityWidget.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QButtonGroup>
#include <QCheckBox>
#include <QJsonArray>
#include <QDebug>

LocalReliabilityWidget::LocalReliabilityWidget(QWidget *parent) 
: UQ_MethodInputWidget(parent)
{
  layout = new QGridLayout();

  // create layout label and entry for # samples

  layout->addWidget(new QLabel("Local Method"), 0, 1);
  method = new QComboBox();
  method->addItem("Mean Value");
  method->addItem("Most Probable Point");
  connect(method, SIGNAL(currentIndexChanged(QString)), this, SLOT(onMethodSelectionChanged(QString)));
  layout->addWidget(method, 0, 2);

  QLabel *label3 = new QLabel();
  label3->setText(QString("MPP Search Method"));
  mppMethod = new QComboBox();
  //  mppMethod->setMaximumWidth(100);
  //  mppMethod->setMinimumWidth(100);
  mppMethod->addItem(tr("no_approx"));
  mppMethod->addItem(tr("x_taylor_mean"));
  mppMethod->addItem(tr("u_taylor_mean"));
  mppMethod->addItem(tr("x_taylor_mpp"));
  mppMethod->addItem(tr("u_taylor_mpp"));
  mppMethod->setToolTip("Set the search method for the Most Probable Point");

  layout->addWidget(label3, 1,1);
  layout->addWidget(mppMethod, 1,2);

  QLabel *label2 = new QLabel();
  label2->setText(QString("Integration Method"));
  integrationMethod = new QComboBox();
  integrationMethod->addItem(tr("First Order"));
  integrationMethod->addItem(tr("Second Order"));
  integrationMethod->setToolTip("Set integration method: first order or second order");

  layout->addWidget(label2, 2,1);
  layout->addWidget(integrationMethod, 2,2);

  /*
  QButtonGroup *theButtonGroup = new QButtonGroup();
  checkedResponseLevel = new QCheckBox();
  checkedProbabilityLevel = new QCheckBox();
  checkedProbabilityLevel->setChecked(true);
  theButtonGroup->addButton(checkedResponseLevel);
  theButtonGroup->addButton(checkedProbabilityLevel);
  */

  //responseLevel = new QLineEdit();
  levelType = new QComboBox();
  levelType->addItem("Probability Levels");
  levelType->addItem("Response Levels");

  probabilityLevel = new QLineEdit();
  probabilityLevel->setText(".02 .20 .40 .60 0.80 0.99");


 // layout->addWidget(new QLabel("Probability Levels"), 3, 1);
  layout->addWidget(levelType, 3, 1);
  layout->addWidget(probabilityLevel, 3, 2);

  layout->setColumnStretch(2,2);
  layout->setColumnStretch(3,4);
  layout->setRowStretch(5,1);

  for(int i = 1; i < 3; i++)
      {
         layout->itemAtPosition(i,1)->widget()->hide();
         layout->itemAtPosition(i,2)->widget()->hide();
      }

  this->setLayout(layout);
  method->setCurrentIndex(1);
}

LocalReliabilityWidget::~LocalReliabilityWidget()
{


}

void LocalReliabilityWidget::onMethodSelectionChanged(const QString &method)
{
    if (method == QString("Mean Value")) {
        for(int i = 1; i < 3; i++) {
            layout->itemAtPosition(i,1)->widget()->hide();
            layout->itemAtPosition(i,2)->widget()->hide();
        }
    } else {
        for(int i = 1; i < 3; i++) {
            layout->itemAtPosition(i,1)->widget()->show();
            layout->itemAtPosition(i,2)->widget()->show();
        }

    }
}

bool
LocalReliabilityWidget::outputToJSON(QJsonObject &jsonObj){

    bool result = true;
    jsonObj["localMethod"]=method->currentText();
    jsonObj["integrationMethod"]=integrationMethod->currentText();
    jsonObj["mpp_Method"]=mppMethod->currentText();
    jsonObj["levelType"]=levelType->currentText();

    QJsonArray probLevel;
    QStringList probLevelList = QStringList(probabilityLevel->text().split(" "));
    for (int i = 0; i < probLevelList.size(); ++i)
        probLevel.push_back(probLevelList.at(i).toDouble());
    jsonObj["probabilityLevel"]=probLevel;

    /*
    QJsonArray respLevel;
    QStringList respLevelList = QStringList(responseLevel->text().split(" "));
    for (int i = 0; i < respLevelList.size(); ++i)
        respLevel.push_back(respLevelList.at(i).toDouble());
    jsonObj["responseLevel"]=respLevel;
*/

  /*
    if (checkedResponseLevel->isChecked())
        jsonObj["activeLevel"]=QString("ResponseLevel");
    else
        jsonObj["activeLevel"]=QString("ProbabilityLevel");
*/
    return result;    
}

bool
LocalReliabilityWidget::inputFromJSON(QJsonObject &jsonObject){
    bool result = false;

    if ( (jsonObject.contains("integrationMethod"))
         && (jsonObject.contains("mpp_Method"))
         && (jsonObject.contains("localMethod"))
         && (jsonObject.contains("levelType"))
         && (jsonObject.contains("probabilityLevel"))
         ) {

        //responseLevel->setText("");
        probabilityLevel->setText("");

        QString localMethod=jsonObject["localMethod"].toString();
        integrationMethod->setCurrentIndex(integrationMethod->findText(localMethod));

        QString scheme=jsonObject["integrationMethod"].toString();
        integrationMethod->setCurrentIndex(integrationMethod->findText(scheme));

        QString method=jsonObject["mpp_Method"].toString();
        mppMethod->setCurrentIndex(mppMethod->findText(method));

        QString levelT=jsonObject["levelType"].toString();
        levelType->setCurrentIndex(levelType->findText(levelT));

        QStringList respLevelList;
        QJsonArray probLevels;

        QJsonValue probLevelVal = jsonObject["probabilityLevel"];
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
        /*
        QJsonValue respLevelVal = jsonObject["responseLevel"];
        if (respLevelVal.isArray()) {

            QStringList levelList;
            QJsonArray levels = respLevelVal.toArray();

            for (int i=0; i<levels.count(); i++) {
                QJsonValue value = levels.at(i);
                double levelV = value.toDouble();
                levelList << QString::number(levelV);
            }
            responseLevel->setText(levelList.join(" "));
        } else {
            qDebug() << "FORM Input: Response level not json array";
        }
        */

        return true;
    }

    return result;
}

void
LocalReliabilityWidget::clear(void)
{
}



int
LocalReliabilityWidget::getNumberTasks()
{
    return 1;
}
