// Written: fmckenna

// added and modified: padhye

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

#include "InputWidgetSampling.h"
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
#include <InputWidgetEDP.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>


#include <QStackedWidget>
#include <MonteCarloInputWidget.h>
#include <LatinHypercubeInputWidget.h>

InputWidgetSampling::InputWidgetSampling(QWidget *parent)
: InputWidgetDakotaMethod(parent),uqSpecific(0)
{
    layout = new QVBoxLayout();
    mLayout = new QVBoxLayout();
    //
    // create layout for selection box for method type to layout
    //

    QVBoxLayout *methodLayout= new QVBoxLayout;
    QLabel *label1 = new QLabel();
    label1->setText(QString("Method"));
    samplingMethod = new QComboBox();
    samplingMethod->setMaximumWidth(200);
    samplingMethod->setMinimumWidth(200);
    samplingMethod->addItem(tr("LHS"));
    samplingMethod->addItem(tr("Monte Carlo"));

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

    mLayout->addLayout(methodLayout);
    mLayout->addStretch(1);

    //
    // qstacked widget to hold all widgets
    //

    theStackedWidget = new QStackedWidget();
    theLHS = new LatinHypercubeInputWidget();


    theStackedWidget->addWidget(theLHS);
    theMC = new MonteCarloInputWidget();
    theStackedWidget->addWidget(theMC);

    // set current widget to index 0
    theCurrentMethod = theLHS;


    mLayout->addWidget(theStackedWidget);
    layout->addLayout(mLayout);

    // finally add the EDP layout & set widget layout
    theEdpWidget = new InputWidgetEDP();
    layout->addWidget(theEdpWidget,1);

    this->setLayout(layout);

    connect(samplingMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(onTextChanged(QString)));

  // add a checkbox for Sobolev index
  qDebug() << "Creating Sobolev\n";
  sobolevCheckBox =new QCheckBox("Sobolev Index");
  connect(sobolevCheckBox,SIGNAL(clicked(bool)),this,SLOT(setSobolevFlag(bool)));
  flagForSobolevIndices=0;

}

void InputWidgetSampling::onTextChanged(QString text)
{
  if (text=="Latin Hypercube Sampling") {
    theStackedWidget->setCurrentIndex(0);
    theCurrentMethod = theLHS;
  }
  else if (text=="Monte Carlo Sampling") {
    theStackedWidget->setCurrentIndex(1);
    theCurrentMethod = theMC;  
  }
    /*
    } else if (text=="Quadrature") {

        // create layout label and entry for dimension
        QVBoxLayout *dimLayout= new QVBoxLayout;
        QLabel *label2 = new QLabel();
        label2->setText(QString("Dimension"));
        numSamples = new QLineEdit();
        numSamples->setMaximumWidth(100);
        numSamples->setMinimumWidth(100);
        dimLayout->addWidget(label2);
        dimLayout->addWidget(numSamples);

        mLayout->addLayout(dimLayout);
        mLayout->addStretch(1);

        // create label and entry for level
        QVBoxLayout *levelLayout= new QVBoxLayout;
        QLabel *label3 = new QLabel();
        label3->setText(QString("Level"));
        srand(time(NULL));
        randomSeed = new QLineEdit();
        randomSeed->setMaximumWidth(100);
        randomSeed->setMinimumWidth(100);
        levelLayout->addWidget(label3);
        levelLayout->addWidget(randomSeed);

        mLayout->addLayout(levelLayout);
        mLayout->addStretch(1);

    } else if (text=="Sparse Grid Quadrature") {

        // create layout label and entry for dimension
        QVBoxLayout *dimLayout= new QVBoxLayout;
        QLabel *label2 = new QLabel();
        label2->setText(QString("Dimension"));
        numSamples = new QLineEdit();
        numSamples->setMaximumWidth(100);
        numSamples->setMinimumWidth(100);
        dimLayout->addWidget(label2);
        dimLayout->addWidget(numSamples);

        mLayout->addLayout(dimLayout);
        mLayout->addStretch(1);

        // create label and entry for level
        QVBoxLayout *levelLayout= new QVBoxLayout;
        QLabel *label3 = new QLabel();
        label3->setText(QString("Level"));
        srand(time(NULL));
        randomSeed = new QLineEdit();
        randomSeed->setMaximumWidth(100);
        randomSeed->setMinimumWidth(100);
        levelLayout->addWidget(label3);
        levelLayout->addWidget(randomSeed);

        mLayout->addLayout(levelLayout);
        mLayout->addStretch(1);

    } else if (text=="Surrogate - Polynomial Chaos") {

        // create layout label and entry for dimension
        QVBoxLayout *dimLayout= new QVBoxLayout;
        QLabel *label2 = new QLabel();
        label2->setText(QString("Dimension"));
        numSamples = new QLineEdit();
        numSamples->setMaximumWidth(100);
        numSamples->setMinimumWidth(100);
        dimLayout->addWidget(label2);
        dimLayout->addWidget(numSamples);

        mLayout->addLayout(dimLayout);
        mLayout->addStretch(1);

        // create label and entry for level
        QVBoxLayout *chaosLayout= new QVBoxLayout;
        QLabel *label3 = new QLabel();
        label3->setText(QString("Pol. chaos order"));
        srand(time(NULL));
        randomSeed = new QLineEdit();
        randomSeed->setMaximumWidth(100);
        randomSeed->setMinimumWidth(100);
        chaosLayout->addWidget(label3);
        chaosLayout->addWidget(randomSeed);

        mLayout->addLayout(chaosLayout);
        mLayout->addStretch(1);

    } else if (text=="Surrogate - Gaussian Process") {

        // create layout label and entry for dimension
        QVBoxLayout *covKLayout= new QVBoxLayout;
        QLabel *label2 = new QLabel();
        label2->setText(QString("Cov Kernel Type"));
        numSamples = new QLineEdit();
        numSamples->setMaximumWidth(100);
        numSamples->setMinimumWidth(100);
        covKLayout->addWidget(label2);
        covKLayout->addWidget(numSamples);

        mLayout->addLayout(covKLayout);
        mLayout->addStretch(1);


        // create label and entry for level
        QVBoxLayout *corrLayout= new QVBoxLayout;
        QLabel *label3 = new QLabel();
        label3->setText(QString("Correlation length"));
        srand(time(NULL));
        randomSeed = new QLineEdit();
        randomSeed->setMaximumWidth(100);
        randomSeed->setMinimumWidth(100);
        corrLayout->addWidget(label3);
        corrLayout->addWidget(randomSeed);

        mLayout->addLayout(corrLayout);
        mLayout->addStretch(1);

    } else if (text=="Importance Sampling") {

        // create layout label and entry for # samples
        QVBoxLayout *samplesLayout= new QVBoxLayout;
        QLabel *label2 = new QLabel();
        label2->setText(QString("# Samples"));
        numSamples = new QLineEdit();
        numSamples->setText(tr("1000"));
        numSamples->setMaximumWidth(100);
        numSamples->setMinimumWidth(100);
        samplesLayout->addWidget(label2);
        samplesLayout->addWidget(numSamples);

        mLayout->addLayout(samplesLayout);
        mLayout->addStretch(1);

        // create label and entry for seed to layout
        QVBoxLayout *seedLayout= new QVBoxLayout;
        QLabel *label3 = new QLabel();
        label3->setText(QString("Seed"));
        srand(time(NULL));
        int randomNumber = rand() % 1000 + 1;
        randomSeed = new QLineEdit();
        randomSeed->setText(QString::number(randomNumber));
        randomSeed->setMaximumWidth(100);
        randomSeed->setMinimumWidth(100);
        seedLayout->addWidget(label3);
        seedLayout->addWidget(randomSeed);

        mLayout->addLayout(seedLayout);
        mLayout->addStretch(1);

    } else if (text=="Multilevel Monte Carlo") {

        // create layout label and entry for # samples
        QVBoxLayout *samplesLayout= new QVBoxLayout;
        QLabel *label2 = new QLabel();
        label2->setText(QString("# Samples"));
        numSamples = new QLineEdit();
        numSamples->setText(tr("1000"));
        numSamples->setMaximumWidth(100);
        numSamples->setMinimumWidth(100);
        samplesLayout->addWidget(label2);
        samplesLayout->addWidget(numSamples);

        mLayout->addLayout(samplesLayout);
        mLayout->addStretch(1);

        // create label and entry for seed to layout
        QVBoxLayout *seedLayout= new QVBoxLayout;
        QLabel *label3 = new QLabel();
        label3->setText(QString("Seed"));
        srand(time(NULL));
        int randomNumber = rand() % 1000 + 1;
        randomSeed = new QLineEdit();
        randomSeed->setText(QString::number(randomNumber));
        randomSeed->setMaximumWidth(100);
        randomSeed->setMinimumWidth(100);
        seedLayout->addWidget(label3);
        seedLayout->addWidget(randomSeed);

        mLayout->addLayout(seedLayout);
        mLayout->addStretch(1);
    }
    */
}

InputWidgetSampling::~InputWidgetSampling()
{

}

int 
InputWidgetSampling::getMaxNumParallelTasks(void){
  return theCurrentMethod->getNumberTasks();
}

void InputWidgetSampling::clear(void)
{

}


bool
InputWidgetSampling::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject uq;
    uq["method"]=samplingMethod->currentText();
    theCurrentMethod->outputToJSON(uq);


    if (flagForSobolevIndices == 1)
      uq["sobelov_indices"]=flagForSobolevIndices;
    result = theEdpWidget->outputToJSON(uq);

    jsonObject["samplingMethodData"]=uq;

    return result;
}


bool
InputWidgetSampling::inputFromJSON(QJsonObject &jsonObject)
{
  bool result = false;
  this->clear();
  std::cerr << "STARTED";
  //
  // get sampleingMethodData, if not present it's an error
  
  if (jsonObject.contains("samplingMethodData")) {
    QJsonObject uq = jsonObject["samplingMethodData"].toObject();
    if (uq.contains("method")) {
      QString method =uq["method"].toString();
      this->uqSelectionChanged(method);
      theCurrentMethod->inputFromJSON(uq);
  std::cerr << "sobolev";
      if (uq.contains("sobolev_indices") && uq.contains("samples")) {
	std::cerr << "HELLO SOBOLEV\n";
	flagForSobolevIndices = uq["sobolev_indices"].toInt();
	sobolevCheckBox->setChecked(true);
	// set the sobolevFlag
      }
      else {
	std::cerr << "HELLO NO SOBOLEV\n";
	flagForSobolevIndices = 0;
	sobolevCheckBox->setChecked(false);
      }

      result = theEdpWidget->inputFromJSON(uq);
    }
  }
  return result;
}

void InputWidgetSampling::uqSelectionChanged(const QString &arg1)
{
    // if more data than just num samples and seed code would go here to add or remove widgets from layout
}

int InputWidgetSampling::processResults(QString &filenameResults, QString &filenameTab) {

    return 0;
}

DakotaResults *
InputWidgetSampling::getResults(void) {
    return new DakotaResultsSampling();
}

RandomVariablesContainer *
InputWidgetSampling::getParameters(void) {
    QString classType("Uncertain");
  return new RandomVariablesContainer(classType);
}
