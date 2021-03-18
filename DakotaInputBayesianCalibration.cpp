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

    int bottom = 1;
    QIntValidator *posIntValidator = new QIntValidator();
    posIntValidator->setBottom(bottom);

    int minChains = 3;
    QIntValidator *minChainValidator = new QIntValidator();
    minChainValidator->setBottom(minChains);

    chains = new QLineEdit();
//    chains->size;
    chains->setText(tr("3"));
    chains->setValidator(minChainValidator);
    layout->addWidget(new QLabel("# Chains (>2)"), 1, 0, 1, 2);
    layout->addWidget(chains, 1, 1, 1, 2);



    chainSamples = new QLineEdit();
    chainSamples->setText(tr("1000"));
    chainSamples->setValidator(posIntValidator);
    layout->addWidget(new QLabel("# Chain Samples"), 2, 0, 1, 2);
    layout->addWidget(chainSamples, 2, 1, 1, 2);


    burnInSamples = new QLineEdit();
    burnInSamples->setText("0");
    burnInSamples->setValidator(posIntValidator);
    burnInSamples->setToolTip("Set Random Seed Value");
    layout->addWidget(new QLabel("# Burn in Samples"), 3, 0, 1, 2);
    layout->addWidget(burnInSamples, 3, 1, 1, 2);


    jumpStep = new QLineEdit();
    jumpStep->setText(tr("5"));
    jumpStep->setValidator(posIntValidator);
    jumpStep->setToolTip("Jump Step");
    layout->addWidget(new QLabel("Jump Step"), 4, 0, 1, 2);
    layout->addWidget(jumpStep, 4, 1, 1, 2);



    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Set Random Seed Value");

    layout->addWidget(new QLabel("Seed"), 5, 0, 1, 2);
    layout->addWidget(randomSeed, 5, 1, 1, 2);

//    calibrationData = new QLineEdit();
//    layout->addWidget(new QLabel("Calibration data file"), 7, 0);
//    layout->addWidget(calibrationData, 7, 1, 1, 2);

    readCalibrationDataCheckBox = new QCheckBox();
    readCalibrationDataCheckBox->setChecked(false);
    layout->addWidget(new QLabel("Read calibration data from file"), 6, 0, 1, 1);
    layout->addWidget(readCalibrationDataCheckBox, 6, 1, 1, 1);
    connect(readCalibrationDataCheckBox,SIGNAL(toggled(bool)),this,SLOT(showNumExp(bool)));


    numExperiments = new QLineEdit();
    numExperiments->setText(tr("1"));
    numExperiments->setValidator(posIntValidator);
    numExperiments->setToolTip("Number of calibration data files to read per response");


    numExperimentsLabel = new QLabel();
    numExperimentsLabel->setText(tr("# Experiments"));
    numExperiments->setVisible(false);
    numExperimentsLabel->setVisible(false);

    layout->addWidget(numExperimentsLabel, 6, 2, 1, 1);
    layout->addWidget(numExperiments, 6, 3, 1, 1);

//    readCalDataWidget = new QHBoxLayout();
//    readCalDataWidget->addWidget(new QLabel("Read calibration data from file"));
//    readCalDataWidget->addWidget(readCalibrationDataCheckBox);
//    readCalDataWidget->addStretch();
//    readCalDataWidget->addWidget(numExperimentsLabel);
//    readCalDataWidget->addWidget(numExperiments);
//    layout->addLayout(readCalDataWidget, 7, 0, 1, 2);

//    readCovarianceDataLabel = new QLabel();
//    readCovarianceDataLabel->setText(tr("Read error covariance data from file"));
//    readCovarianceDataCheckBox = new QCheckBox();
//    readCovarianceDataCheckBox->setChecked(false);

//    readCovarianceDataLabel->setVisible(false);
//    readCovarianceDataCheckBox->setVisible(false);
//    connect(readCovarianceDataCheckBox,SIGNAL(toggled(bool)),this,SLOT(showCalibrateCovMultiplierCheckBox(bool)));

//    calibrateCovarianceMultiplierLabel = new QLabel();
//    calibrateCovarianceMultiplierLabel->setText(tr("Calibrate multipliers on error covariance"));
//    calibrateCovarianceMultiplierCheckBox = new QCheckBox();
//    calibrateCovarianceMultiplierCheckBox->setChecked(false);

//    calibrateCovarianceMultiplierLabel->setVisible(false);
//    calibrateCovarianceMultiplierCheckBox->setVisible(false);

//    readErrorCovarianceDataWidget = new QHBoxLayout();
//    readErrorCovarianceDataWidget->addWidget(readCovarianceDataLabel);
//    readErrorCovarianceDataWidget->addWidget(readCovarianceDataCheckBox);
//    readErrorCovarianceDataWidget->addStretch();
//    readErrorCovarianceDataWidget->addWidget(calibrateCovarianceMultiplierLabel);
//    readErrorCovarianceDataWidget->addWidget(calibrateCovarianceMultiplierCheckBox);
//    layout->addLayout(readErrorCovarianceDataWidget, 8, 0, 1, 2);


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

//    layout->setColumnStretch(1, 1);
//    layout->setColumnStretch(2, 1);
//    layout->setColumnStretch(3, 1);
//    layout->setColumnStretch(4, 3);
    layout->setColumnStretch(5, 1);

    layout->setRowStretch(9, 1);

//    layout->setColumnMinimumWidth(0, 100);
//    layout->setColumnMinimumWidth(1, 50);
//    layout->setColumnMinimumWidth(2, 50);
//    layout->setColumnMinimumWidth(3, 50);
//    layout->setColumnMinimumWidth(4, 100);
//    layout->setColumnSizeHint

    layout->setRowMinimumHeight(6, 20);

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

// SLOT function
void DakotaInputBayesianCalibration::showNumExp(bool tog)
{
    if (tog) {
        numExperiments->setVisible(true);
        numExperimentsLabel->setVisible(true);
//        readCovarianceDataLabel->setVisible(true);
//        readCovarianceDataCheckBox->setVisible(true);
//        if (readCovarianceDataCheckBox->isChecked()) {
//            calibrateCovarianceMultiplierLabel->setVisible(true);
//            calibrateCovarianceMultiplierCheckBox->setVisible(true);
//        }
    } else {
        numExperiments->setVisible(false);
        numExperimentsLabel->setVisible(false);
//        readCovarianceDataLabel->setVisible(false);
//        readCovarianceDataCheckBox->setVisible(false);
//        calibrateCovarianceMultiplierLabel->setVisible(false);
//        calibrateCovarianceMultiplierCheckBox->setVisible(false);
    }
}

// SLOT function
void DakotaInputBayesianCalibration::showCalibrateCovMultiplierCheckBox(bool tog)
{
    if (tog) {
        calibrateCovarianceMultiplierLabel->setVisible(true);
        calibrateCovarianceMultiplierCheckBox->setVisible(true);
    } else {
        calibrateCovarianceMultiplierLabel->setVisible(false);
        calibrateCovarianceMultiplierCheckBox->setVisible(false);
    }
}


bool
DakotaInputBayesianCalibration::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    QJsonObject uq;
    uq["method"]=calibrationMethod->currentText();
    uq["chains"]=chains->text().toInt();
    uq["chainSamples"]=chainSamples->text().toInt();
    uq["burnInSamples"]=burnInSamples->text().toInt();
    uq["jumpStep"]=jumpStep->text().toInt();
    uq["seed"]=randomSeed->text().toDouble();
    uq["readCalibrationData"]=readCalibrationDataCheckBox->isChecked();
    uq["numExperiments"]=numExperiments->text().toInt();
//    uq["calibrateErrorMultipliers"]=calibrateErrorCheckBox->isChecked();

    //uq["mcmcMethod"]=mcmcMethod->currentText();

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
//    bool result = true;
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

        int numExp = 1;
        bool readCalDataBool = false;
        if (uq.contains("readCalibrationData")) {
            if(uq["readCalibrationData"].toBool()) {
                readCalDataBool = true;
                if (uq.contains("numExperiments")) {
                    numExp = uq["numExperiments"].toInt();
                }
                numExperiments->setText(QString::number(numExp));
            }
        }
        readCalibrationDataCheckBox->setChecked(readCalDataBool);


//        if (jsonObject.contains("advancedOpt")) {
//            if (jsonObject["advancedOpt"].toBool()) {
//              theAdvancedCheckBox->setChecked(true);

        if (uq.contains("calibrateErrorMultipliers"))
            qDebug() << "\n\n Contains calibrateErrorMultipliers, state: " << uq["calibrateErrorMultipliers"].toBool();
            if (uq["calibrateErrorMultipliers"].toBool()) {
                calibrateCovarianceMultiplierCheckBox->setChecked(true);
                qDebug() << "calibrateErrorCheckBox setChecked to true";
            }



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
        emit sendErrorMessage("ERROR: Bayesian Calibration Input - no \"bayesian_calibration_method\" data");
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
