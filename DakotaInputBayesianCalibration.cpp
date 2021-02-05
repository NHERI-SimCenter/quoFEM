// Written: fmckenna

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

#include "DakotaInputBayesianCalibration.h"
#include <DakotaResultsBayesianCalibration.h>
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
#include <QGridLayout>
#include <QComboBox>
#include <QValidator>

#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>


DakotaInputBayesianCalibration::DakotaInputBayesianCalibration(QWidget *parent)
    : UQ_Engine(parent)
{
    layout = new QGridLayout();


    calibrationMethod = new QComboBox();
    calibrationMethod->addItem(tr("DREAM"));
    connect(calibrationMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(onMethodChanged(QString)));
    layout->addWidget(new QLabel("Method"), 0, 0);
    layout->addWidget(calibrationMethod, 0, 1, 1, 2);

    /*
    //calibrationMethod->addItem("QUESO"); QUESO GPL!
    layout->addWidget(new QLabel("Method"), 0, 0);
    layout->addWidget(calibrationMethod, 0, 1, 1, 2);

    connect(calibrationMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(onMethodChanged(QString)));

    mcmcMethod = new QComboBox();
    mcmcMethod->addItem(tr("DRAM"));
    mcmcMethod->addItem("Delayed Rejection");
    mcmcMethod->addItem("Adaptive Metropolis");
    mcmcMethod->addItem("Metropolis Hastings");
    mcmcMethod->addItem("Multilevel");
    layout->addWidget(new QLabel("MCMC Algorithm"), 2, 0);
    layout->addWidget(mcmcMethod, 2, 1);
    */
    chains = new QLineEdit();
    chains->setText(tr("3"));
    layout->addWidget(new QLabel("# Chains (>2)"), 2, 0);
    layout->addWidget(chains, 2, 1);



    chainSamples = new QLineEdit();
    chainSamples->setText(tr("1000"));
    layout->addWidget(new QLabel("# Chain Samples"), 3,0);
    layout->addWidget(chainSamples, 3,1);

    burnInSamples = new QLineEdit();
    burnInSamples->setText("0");
    layout->addWidget(new QLabel("# Burn in Samples"), 4,0);
    layout->addWidget(burnInSamples, 4,1);
    burnInSamples->setValidator(new QIntValidator);
    burnInSamples->setToolTip("Set Random Seed Value");

    jumpStep = new QLineEdit();
    jumpStep->setText(tr("5"));
    layout->addWidget(new QLabel("Jump Step"), 5,0);
    layout->addWidget(jumpStep, 5,1);
    burnInSamples->setValidator(new QIntValidator);
    burnInSamples->setToolTip("Jump Step");


    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Set Random Seed Value");

    layout->addWidget(new QLabel("Seed"), 6, 0);
    layout->addWidget(randomSeed, 6, 1);
    /*
    emulator = new QComboBox();
    emulator->addItem(tr("Gaussian Process"));
    emulator->addItem("Polynomial Chaos");
    emulator->addItem("Multilevel Polynomial Chaos");
    emulator->addItem("Multifidelity Polynomial Chaos");
    emulator->addItem("Stochastic Collocation");
    layout->addWidget(new QLabel("Emulator"), 6, 0);
    layout->addWidget(emulator, 6, 1);

    maxIterations = new QLineEdit();
    maxIterations->setText("100");
    maxIterations->setValidator(new QIntValidator);
    maxIterations->setToolTip("Max number of iterations");

    layout->addWidget(new QLabel("Max # Iterations"), 8, 0);
    layout->addWidget(maxIterations, 8,1);

    convTol = new QLineEdit();
    convTol->setText("1e-4");
    convTol->setValidator(new QDoubleValidator);
    convTol->setToolTip("Specify the convergence tolerance");

    layout->addWidget(new QLabel("Convergence Tol"), 9, 0);
    layout->addWidget(convTol, 9,1);

    scalingFactors = new QLineEdit();
    scalingFactors->setToolTip("Scaling factors on responses, one for each QoI must be provided or values ignored, default is 1.0 for all");
    layout->addWidget(new QLabel("Scaling Factors"), 10, 0);
    layout->addWidget(scalingFactors, 10, 1);

    layout->itemAtPosition(2,0)->widget()->hide();
    layout->itemAtPosition(2,1)->widget()->hide();
    */

    layout->setColumnStretch(1, 2);
    layout->setColumnStretch(2, 1);
    layout->setColumnStretch(3,4);

    layout->setRowStretch(7,1);

    this->setLayout(layout);
}

DakotaInputBayesianCalibration::~DakotaInputBayesianCalibration()
{

}

int 
DakotaInputBayesianCalibration::getMaxNumParallelTasks(void){
    return 1;
}

void DakotaInputBayesianCalibration::clear(void)
{

}


bool
DakotaInputBayesianCalibration::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    QJsonObject uq;
    uq["method"]=calibrationMethod->currentText();
    uq["chainSamples"]=chainSamples->text().toInt();
    uq["seed"]=randomSeed->text().toDouble();
    uq["burnInSamples"]=burnInSamples->text().toInt();
    uq["jumpStep"]=jumpStep->text().toInt();
    //uq["mcmcMethod"]=mcmcMethod->currentText();
    uq["chains"]=chains->text().toInt();
    //uq["emulator"]=emulator->currentText();
    //uq["maxIter"]=maxIterations->text().toInt();
    //uq["tol"]=convTol->text().toDouble();
    //uq["factors"]=scalingFactors->text();

    jsonObject["bayesianCalibrationMethodData"]=uq;
    return result;

}


bool
DakotaInputBayesianCalibration::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;
    this->clear();

    if (jsonObject.contains("bayesianCalibrationMethodData")) {

        QJsonObject uq = jsonObject["bayesianCalibrationMethodData"].toObject();

        QString method =uq["method"].toString();
        int index = calibrationMethod->findText(method);
        calibrationMethod->setCurrentIndex(index);

        //QString mcmc = uq["mcmcMethod"].toString();
        //index = mcmcMethod->findText(mcmc);
        //mcmcMethod->setCurrentIndex(index);

        int samples=uq["chainSamples"].toInt();
        chainSamples->setText(QString::number(samples));

        double seed=uq["seed"].toDouble();
        randomSeed->setText(QString::number(seed));

        int burnSamples = uq["burnInSamples"].toInt();
        burnInSamples->setText(QString::number(burnSamples));

        int c = uq["chains"].toInt();
        chains->setText(QString::number(c));

        int j = uq["jumpStep"].toInt();
        jumpStep->setText(QString::number(j));

        //QString em = uq["emulator"].toString();
        //index =emulator->findText(em);
        //emulator->setCurrentIndex(index);

        //int maxIter=uq["maxIter"].toInt();
        //maxIterations->setText(QString::number(maxIter));

        //double tol=uq["tol"].toDouble();
        //convTol->setText(QString::number(tol));

        //QString fact = uq["factors"].toString();
        //scalingFactors->setText(fact);

    } else {
        emit sendErrorMessage("ERROR: Bayesian Calibration INput - no \"bayesian_calibration_method\" data");
        return false;
    }

    return true;
}

void DakotaInputBayesianCalibration::onMethodChanged(const QString &arg1)
{
    /*
    if (arg1.compare(QString("QUESO"))) {
        layout->itemAtPosition(2,0)->widget()->hide();
        layout->itemAtPosition(2,1)->widget()->hide();
        layout->itemAtPosition(3,0)->widget()->show();
        layout->itemAtPosition(3,1)->widget()->show();

    } else {
        layout->itemAtPosition(2,0)->widget()->show();
        layout->itemAtPosition(2,1)->widget()->show();
        layout->itemAtPosition(3,0)->widget()->hide();
        layout->itemAtPosition(3,1)->widget()->hide();
    }
    */

}

int DakotaInputBayesianCalibration::processResults(QString &filenameResults, QString &filenameTab) {

    return 0;
}

UQ_Results *
DakotaInputBayesianCalibration::getResults(void) {
    qDebug() << "new DakotaResultsCalibration";
  return new DakotaResultsBayesianCalibration(burnInSamples->text().toInt());
}

RandomVariablesContainer *
DakotaInputBayesianCalibration::getParameters(void) {
  QString classType("Uncertain");
  return new RandomVariablesContainer(classType);
}


QString
DakotaInputBayesianCalibration::getMethodName(void){
  return QString("Bayesian calibration");
}
