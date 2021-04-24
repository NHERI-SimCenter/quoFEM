
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

#include "SimCenterUQInputSurrogate.h"
#include "SimCenterUQResultsSurrogate.h"
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

#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>


#include <QStackedWidget>
#include <SurrogateDoEInputWidget.h>
#include <SurrogateNoDoEInputWidget.h>
#include <SurrogateMFInputWidget.h>
#include <InputWidgetFEM.h>
#include <InputWidgetParameters.h>
#include <InputWidgetEDP.h>

SimCenterUQInputSurrogate::SimCenterUQInputSurrogate(InputWidgetParameters *param,InputWidgetFEM *femwidget,InputWidgetEDP *edpwidget, QWidget *parent)
: UQ_Engine(parent),uqSpecific(0), theParameters(param), theEdpWidget(edpwidget), theFemWidget(femwidget)
{

    layout = new QVBoxLayout();
    mLayout = new QVBoxLayout();

    //
    // create layout for input selection box
    //

    QHBoxLayout *methodLayout1= new QHBoxLayout;
    QLabel *label1 = new QLabel();
    label1->setText(QString("Training Dataset"));
    inpMethod = new QComboBox();
    inpMethod->addItem(tr("Sampling and Simulation"));
    inpMethod->addItem(tr("Import Data File"));
    inpMethod->addItem(tr("Import Multi-fidelity Data File"));

    methodLayout1->addWidget(label1);
    methodLayout1->addWidget(inpMethod,2);
    methodLayout1->addStretch(4);

    mLayout->addLayout(methodLayout1);

    //
    // input selection widgets
    //

    theStackedWidget = new QStackedWidget();

    theDoE = new SurrogateDoEInputWidget();
    theStackedWidget->addWidget(theDoE);
    theData = new SurrogateNoDoEInputWidget(theParameters,theFemWidget,theEdpWidget);
    theStackedWidget->addWidget(theData);
    theMultiFidelity = new SurrogateMFInputWidget(theParameters,theFemWidget,theEdpWidget);
    theStackedWidget->addWidget(theMultiFidelity);

    theStackedWidget->setCurrentIndex(0);

    theInpCurrentMethod = theDoE;

    mLayout->addWidget(theStackedWidget);

    //
    //
    //

    layout->addLayout(mLayout);

    this->setLayout(layout);

    connect(inpMethod, SIGNAL(currentIndexChanged(QString)), this, SLOT(onIndexChanged(QString)));
}

void SimCenterUQInputSurrogate::onIndexChanged(const QString &text)
{
    if (text=="Sampling and Simulation") {
        theStackedWidget->setCurrentIndex(0);
        theInpCurrentMethod = theDoE;

        theFemWidget->setFEMforGP("GPmodel"); // reset FEM
        //theEdpWidget->setGPQoINames(QStringList({})); // remove GP RVs
    }
    else if (text=="Import Data File") {
        delete theData;
        theData = new SurrogateNoDoEInputWidget(theParameters,theFemWidget,theEdpWidget);
        theStackedWidget->insertWidget(1,theData);

        theStackedWidget->setCurrentIndex(1);
        theInpCurrentMethod = theData;
    }
    else if (text=="Import Multi-fidelity Data File") {
        delete theMultiFidelity;
        theMultiFidelity = new SurrogateMFInputWidget(theParameters,theFemWidget,theEdpWidget);
        theStackedWidget->insertWidget(2,theMultiFidelity);

        theStackedWidget->setCurrentIndex(2);
        theInpCurrentMethod = theMultiFidelity;
        theFemWidget->setFEMforGP("GPdata");
    }

    //theParameters->setGPVarNamesAndValues(QStringList({}));// remove GP RVs
}


SimCenterUQInputSurrogate::~SimCenterUQInputSurrogate()
{

}

int
SimCenterUQInputSurrogate::getMaxNumParallelTasks(void){
  return theInpCurrentMethod->getNumberTasks();
}

void SimCenterUQInputSurrogate::clear(void)
{

}

void SimCenterUQInputSurrogate::numModelsChanged(int numModels) {
    emit onNumModelsChanged(numModels);
}

bool
SimCenterUQInputSurrogate::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject uq;
    uq["method"]=inpMethod->currentText();
    theInpCurrentMethod->outputToJSON(uq);

    jsonObject["surrogateMethodInfo"]=uq;

    return result;
}


bool
SimCenterUQInputSurrogate::inputFromJSON(QJsonObject &jsonObject)
{
  bool result = false;
  this->clear();

  //
  // get sampleingMethodData, if not present it's an error
  //

  if (jsonObject.contains("surrogateMethodInfo")) {
      QJsonObject uq = jsonObject["surrogateMethodInfo"].toObject();
      if (uq.contains("method")) {

          QString method =uq["method"].toString();
          int index = inpMethod->findText(method);
          if (index == -1) {
              return false;
          }
          inpMethod->setCurrentIndex(index);
          result = theInpCurrentMethod->inputFromJSON(uq);
          if (result == false)
              return result;

      }
  }

  return result;
}

bool
SimCenterUQInputSurrogate::outputAppDataToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["Application"] = "SimCenterUQ-UQ";
    QJsonObject uq;
    uq["method"]=inpMethod->currentText();
    theInpCurrentMethod->outputToJSON(uq);
    jsonObject["ApplicationData"] = uq;

    return result;
}


bool
SimCenterUQInputSurrogate::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    bool result = false;
    this->clear();
    //
    // get sampleingMethodData, if not present it's an error

    if (jsonObject.contains("ApplicationData")) {
        QJsonObject uq = jsonObject["ApplicationData"].toObject();

        if (uq.contains("method")) {
          QString method = uq["method"].toString();
          int index = inpMethod->findText(method);

          if (index == -1) {
              emit sendErrorMessage(QString("ERROR: Unknown Method") + method);
              return false;
          }
          inpMethod->setCurrentIndex(index);
          return theInpCurrentMethod->inputFromJSON(uq);
        }

    } else {
        emit sendErrorMessage("ERROR: Surrogate Input Widget - no \"surrogateMethodData\" input");
        return false;
    }

    return result;
}



int SimCenterUQInputSurrogate::processResults(QString &filenameResults, QString &filenameTab) {

    return 0;
}

UQ_Results *
SimCenterUQInputSurrogate::getResults(void) {
    qDebug() << "RETURNED SimCenterUQRESULTSSURROGATE";
    return new SimCenterUQResultsSurrogate(theRandomVariables);
}

RandomVariablesContainer *
SimCenterUQInputSurrogate::getParameters(void) {
  QString classType("Uncertain");
  theRandomVariables =  new RandomVariablesContainer(classType,tr("SimCenterUQ"));
  return theRandomVariables;
}



QString
SimCenterUQInputSurrogate::getMethodName(void){
    //if (inpMethod->currentIndex()==0){
    //    return QString("surrogateModel");
    //} else if (inpMethod->currentIndex()==1){
    //    return QString("surrogateData");
    //}
    return QString("surrogate");
}
