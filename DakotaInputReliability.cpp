
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

#include "DakotaInputReliability.h"
#include <DakotaResultsReliability.h>
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


#include <QStackedWidget>
#include <LocalReliabilityWidget.h>
#include <GlobalReliabilityWidget.h>

DakotaInputReliability::DakotaInputReliability(QWidget *parent)
: UQ_Engine(parent)
{
    layout = new QVBoxLayout();
    mLayout = new QVBoxLayout();

    //
    // create layout for selection box for method type to layout
    //

    QHBoxLayout *methodLayout= new QHBoxLayout;
    QLabel *label1 = new QLabel();
    label1->setText(QString("Reliability Method"));
    reliabilityMethod = new QComboBox();
    reliabilityMethod->setMaximumWidth(200);
    reliabilityMethod->setMinimumWidth(200);
    reliabilityMethod->addItem(tr("Local Reliability"));
    reliabilityMethod->addItem(tr("Global Reliability"));

    methodLayout->addWidget(label1);
    methodLayout->addWidget(reliabilityMethod,2);
    methodLayout->addStretch(4);

    mLayout->addLayout(methodLayout);

    //
    // qstacked widget to hold all widgets
    //

    theStackedWidget = new QStackedWidget();

    theLocal = new LocalReliabilityWidget();
    theStackedWidget->addWidget(theLocal);

    theGlobal = new GlobalReliabilityWidget();
    theStackedWidget->addWidget(theGlobal);

    // set current widget to index 0
    theCurrentMethod = theLocal;

    mLayout->addWidget(theStackedWidget);
    layout->addLayout(mLayout);
    layout->addStretch();

    // finally add the EDP layout & set widget layout

    this->setLayout(layout);

    connect(reliabilityMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(onMethodChanged(QString)));

}

void DakotaInputReliability::onMethodChanged(QString text)
{
    qDebug() << text;

  if (text=="Local Reliability") {
    theStackedWidget->setCurrentIndex(0);
    theCurrentMethod = theLocal;
  } else {
    theStackedWidget->setCurrentIndex(1);
    theCurrentMethod = theGlobal;
  }
}

DakotaInputReliability::~DakotaInputReliability()
{

}

int 
DakotaInputReliability::getMaxNumParallelTasks(void){
  return theCurrentMethod->getNumberTasks();
}

void DakotaInputReliability::clear(void)
{

}


bool
DakotaInputReliability::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject uq;
    uq["method"]=reliabilityMethod->currentText();
    theCurrentMethod->outputToJSON(uq);

    jsonObject["reliabilityMethodData"]=uq;

    return result;
}


bool
DakotaInputReliability::inputFromJSON(QJsonObject &jsonObject)
{
  bool result = false;
  this->clear();

  //
  // get sampleingMethodData, if not present it's an error
  
  if (jsonObject.contains("reliabilityMethodData")) {
      QJsonObject uq = jsonObject["reliabilityMethodData"].toObject();
      if (uq.contains("method")) {
          QString method =uq["method"].toString();
          int index = reliabilityMethod->findText(method);
          if (index == -1) {
              return false;
          }
          reliabilityMethod->setCurrentIndex(index);
          result = theCurrentMethod->inputFromJSON(uq);
          if (result == false)
              return result;
      }
  }
  
  return result;
}


int DakotaInputReliability::processResults(QString &filenameResults, QString &filenameTab) {

    return 0;
}

UQ_Results *
DakotaInputReliability::getResults(void) {
    return new DakotaResultsReliability(theRandomVariables);
}

RandomVariablesContainer *
DakotaInputReliability::getParameters(void) {
  QString classType("Uncertain");
  theRandomVariables =  new RandomVariablesContainer(classType);
  return theRandomVariables;
}
