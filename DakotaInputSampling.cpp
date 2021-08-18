
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

#include "DakotaInputSampling.h"
#include <DakotaResultsSampling.h>
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
#include <MonteCarloInputWidget.h>
#include <LatinHypercubeInputWidget.h>
//#include <ImportanceSamplingInputWidget.h>
#include <GaussianProcessInputWidget.h>
#include <PCEInputWidget.h>
#include <MultiFidelityMonteCarlo.h>

DakotaInputSampling::DakotaInputSampling(QWidget *parent)
: UQ_Engine(parent),uqSpecific(0)
{
    layout = new QVBoxLayout();
    mLayout = new QVBoxLayout();

    //
    // create layout for selection box for method type to layout
    //

    QHBoxLayout *methodLayout= new QHBoxLayout;
    QLabel *label1 = new QLabel();
    label1->setText(QString("Method"));
    samplingMethod = new QComboBox();
    samplingMethod->setMaximumWidth(200);
    samplingMethod->setMinimumWidth(200);
    samplingMethod->addItem(tr("LHS"));
    samplingMethod->addItem(tr("Monte Carlo"));
    //samplingMethod->addItem(tr("Importance Sampling"));
    samplingMethod->addItem(tr("Gaussian Process Regression"));
    samplingMethod->addItem(tr("Polynomial Chaos Expansion"));
    // samplingMethod->addItem(tr("Multi Fidelity Monte Carlo"));

    /*
    samplingMethod->addItem(tr("Multilevel Monte Carlo"));
    samplingMethod->addItem(tr("Importance Sampling"));
    samplingMethod->addItem(tr("Quadrature"));
    samplingMethod->addItem(tr("Sparse Grid Quadrature"));
    samplingMethod->addItem(tr("Surrogate - Polynomial Chaos"));
    samplingMethod->addItem(tr("Surrogate - Gaussian Process"));
    */

    methodLayout->addWidget(label1);
    methodLayout->addWidget(samplingMethod);
    methodLayout->addStretch(1);

    mLayout->addLayout(methodLayout);

    //
    // qstacked widget to hold all widgets
    //

    theStackedWidget = new QStackedWidget();

    theLHS = new LatinHypercubeInputWidget();
    theStackedWidget->addWidget(theLHS);

    theMC = new MonteCarloInputWidget();
    theStackedWidget->addWidget(theMC);

    //theIS = new ImportanceSamplingInputWidget();
    //theStackedWidget->addWidget(theIS);

    theGP = new GaussianProcessInputWidget();
    theStackedWidget->addWidget(theGP);

    thePCE = new PCEInputWidget();
    theStackedWidget->addWidget(thePCE);

    theMFMC = new MultiFidelityMonteCarlo();
    theStackedWidget->addWidget(theMFMC);

    // set current widget to index 0
    theCurrentMethod = theLHS;


    mLayout->addWidget(theStackedWidget);
    layout->addLayout(mLayout);

    this->setLayout(layout);

    connect(theMFMC, SIGNAL(onNumModelsChanged(int)), this, SLOT(numModelsChanged(int)));
    connect(samplingMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(onTextChanged(QString)));

}

void DakotaInputSampling::onTextChanged(const QString &text)
{
  if (text=="LHS") {
    theStackedWidget->setCurrentIndex(0);
    theCurrentMethod = theLHS;
  }
  else if (text=="Monte Carlo") {
    theStackedWidget->setCurrentIndex(1);
    theCurrentMethod = theMC;  
  }
  //else if (text=="Importance Sampling") {
  //  theStackedWidget->setCurrentIndex(2);
  //  theCurrentMethod = theIS;
  //}
  else if (text=="Gaussian Process Regression") {
    theStackedWidget->setCurrentIndex(3);
    theCurrentMethod = theGP;
  }
  else if (text=="Polynomial Chaos Expansion") {
    theStackedWidget->setCurrentIndex(4);
    theCurrentMethod = thePCE;
  }
  else if (text=="Multi Fidelity Monte Carlo") {
    theStackedWidget->setCurrentIndex(5);
    theCurrentMethod = theMFMC;
  }
}

DakotaInputSampling::~DakotaInputSampling()
{

}

int 
DakotaInputSampling::getMaxNumParallelTasks(void){
  return theCurrentMethod->getNumberTasks();
}

void DakotaInputSampling::clear(void)
{

}

void DakotaInputSampling::numModelsChanged(int numModels) {
    emit onNumModelsChanged(numModels);
}

bool
DakotaInputSampling::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject uq;
    uq["method"]=samplingMethod->currentText();
    theCurrentMethod->outputToJSON(uq);

    jsonObject["samplingMethodData"]=uq;

    return result;
}


bool
DakotaInputSampling::inputFromJSON(QJsonObject &jsonObject)
{
  bool result = false;
  this->clear();

  //
  // get sampleingMethodData, if not present it's an error
  //
  
  if (jsonObject.contains("samplingMethodData")) {
      QJsonObject uq = jsonObject["samplingMethodData"].toObject();
      if (uq.contains("method")) {

          QString method =uq["method"].toString();
          int index = samplingMethod->findText(method);
          if (index == -1) {
              return false;
          }
          samplingMethod->setCurrentIndex(index);
          result = theCurrentMethod->inputFromJSON(uq);
          if (result == false)
              return result;

      }
  }
  
  return result;
}

bool
DakotaInputSampling::outputAppDataToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["Application"] = "Dakota-UQ";
    QJsonObject uq;
    uq["method"]=samplingMethod->currentText();
    theCurrentMethod->outputToJSON(uq);
    jsonObject["ApplicationData"] = uq;

    return result;
}


bool
DakotaInputSampling::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    bool result = false;
    this->clear();
    //
    // get sampleingMethodData, if not present it's an error

    if (jsonObject.contains("ApplicationData")) {
        QJsonObject uq = jsonObject["ApplicationData"].toObject();

        if (uq.contains("method")) {
          QString method = uq["method"].toString();
          int index = samplingMethod->findText(method);

          if (index == -1) {
              errorMessage(QString("ERROR: Unknown Method") + method);
              return false;
          }
          samplingMethod->setCurrentIndex(index);
          return theCurrentMethod->inputFromJSON(uq);
        }

    } else {
        errorMessage("ERROR: Sampling Input Widget - no \"samplingMethodData\" input");
        return false;
    }

    return result;
}



int DakotaInputSampling::processResults(QString &filenameResults, QString &filenameTab) {

    return 0;
}

UQ_Results *
DakotaInputSampling::getResults(void) {
    qDebug() << "RETURNED DAKOTARESULTSSAMPLING";
    return new DakotaResultsSampling(theRandomVariables);
}

RandomVariablesContainer *
DakotaInputSampling::getParameters(void) {
  QString classType("Uncertain");
  theRandomVariables =  new RandomVariablesContainer(classType);
  return theRandomVariables;
}

QString
DakotaInputSampling::getMethodName(void){
  return QString("sampling");
}
