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
    calibrationMethod = new QComboBox();
    calibrationMethod->addItem(tr("DREAM"));
    connect(calibrationMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(onMethodChanged(QString)));

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
    chains->setText(tr("3"));
    chains->setValidator(minChainValidator);

    chainSamples = new QLineEdit();
    chainSamples->setText(tr("1000"));
    chainSamples->setValidator(posIntValidator);

    burnInSamples = new QLineEdit();
    burnInSamples->setText("0");
    burnInSamples->setValidator(posIntValidator);
    burnInSamples->setToolTip("Set Random Seed Value");

    jumpStep = new QLineEdit();
    jumpStep->setText(tr("5"));
    jumpStep->setValidator(posIntValidator);
    jumpStep->setToolTip("Jump Step"); 

    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Set Random Seed Value");

//    calibrationData = new QLineEdit();
//    layout->addWidget(new QLabel("Calibration data file"), 7, 0);
//    layout->addWidget(calibrationData, 7, 1, 1, 2);

    readCalibrationDataCheckBox = new QCheckBox();
    readCalibrationDataCheckBox->setChecked(false);
    connect(readCalibrationDataCheckBox,SIGNAL(toggled(bool)),this,SLOT(showNumExp(bool)));

    numExperiments = new QLineEdit();
    numExperiments->setText(tr("1"));
    numExperiments->setValidator(posIntValidator);
    numExperiments->setToolTip("Number of calibration data files to read per response");

    numExperimentsLabel = new QLabel();
    numExperimentsLabel->setText(tr("# Experiments"));
    numExperiments->setVisible(false);
    numExperimentsLabel->setVisible(false);

    calibrateMultiplierText = new QLabel();
    calibrateMultiplierText->setText(tr("Error Multiplier Calibration:"));

    calibratePerResponseLabel = new QLabel();
    calibratePerResponseLabel->setText(tr("Per Response"));
    calibratePerResponseCheckBox = new QCheckBox();
    calibratePerResponseCheckBox->setChecked(true);

    calibratePerExperimentLabel = new QLabel();
    calibratePerExperimentLabel->setText(tr("Per Experiment"));
    calibratePerExperimentCheckBox = new QCheckBox();
    calibratePerExperimentCheckBox->setChecked(false);

    // Provide calibration data file as input
    QLabel *calDataFileLabel = new QLabel();
    calDataFileLabel->setText("Calibration data file");
    calDataFileEdit = new QLineEdit;
    QPushButton *chooseCalDataFile = new QPushButton();

    connect(chooseCalDataFile, &QPushButton::clicked, this, [=](){
          calDataFileEdit->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*)"));
                          });
    chooseCalDataFile->setText(tr("Choose"));
    chooseCalDataFile->setToolTip(tr("Push to choose a file from your file system"));


//    readCovarianceDataLabel = new QLabel();
//    readCovarianceDataLabel->setText(tr("Read error covariance from file"));
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


//    layout->addWidget(readCovarianceDataLabel, 7, 0, 1, 1);
//    layout->addWidget(readCovarianceDataCheckBox, 7, 1, 1, 1);
//    layout->addWidget(calibrateCovarianceMultiplierLabel, 7, 2, 1, 2);
//    layout->addWidget(calibrateCovarianceMultiplierCheckBox, 7, 4, 1, 1);


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

    layout = new QGridLayout();
    layout->addWidget(new QLabel("Method"), 0, 0, 1, 1);
    layout->addWidget(calibrationMethod, 0, 1, 1, 3);
    layout->addWidget(new QLabel("# Chains (>2)"), 1, 0, 1, 1);
    layout->addWidget(chains, 1, 1, 1, 3);
    layout->addWidget(new QLabel("# Chain Samples"), 2, 0, 1, 1);
    layout->addWidget(chainSamples, 2, 1, 1, 3);
    layout->addWidget(new QLabel("# Burn in Samples"), 3, 0, 1, 1);
    layout->addWidget(burnInSamples, 3, 1, 1, 3);
    layout->addWidget(new QLabel("Jump Step"), 4, 0, 1, 1);
    layout->addWidget(jumpStep, 4, 1, 1, 3);
    layout->addWidget(new QLabel("Seed"), 5, 0, 1, 1);
    layout->addWidget(randomSeed, 5, 1, 1, 3);
    layout->addWidget(calDataFileLabel, 6, 0, 1, 1);
    layout->addWidget(calDataFileEdit, 6, 1, 1, 4);
    layout->addWidget(chooseCalDataFile, 6, 5, 1, 1);

//    layout->addWidget(new QLabel("Read calibration data from file"), 6, 0, 1, 2);
//    layout->addWidget(readCalibrationDataCheckBox, 6, 2, 1, 1);
//    layout->addWidget(numExperimentsLabel, 6, 3, 1, 2);
//    layout->addWidget(numExperiments, 6, 5, 1, 1);
//    layout->addWidget(calibrateMultiplierText, 7, 0, 1, 2);
//    layout->addWidget(calibratePerResponseLabel, 8, 0, 1, 1);
//    layout->addWidget(calibratePerResponseCheckBox, 8, 1, 1, 1);
//    layout->addWidget(calibratePerExperimentLabel, 9, 0, 1, 1);
//    layout->addWidget(calibratePerExperimentCheckBox, 9, 1, 1, 1);

//    layout->setColumnMinimumWidth(0, 50);
    layout->setColumnMinimumWidth(1, 10);
    layout->setColumnMinimumWidth(2, 10);
    layout->setColumnMinimumWidth(3, 10);
//    layout->setColumnMinimumWidth(4, 300);
    layout->setColumnMinimumWidth(4, 50);
//    layout->setColumnMinimumWidth(5, 10);
//    layout->setColumnMinimumWidth(6, 10);

    layout->setRowMinimumHeight(6, 24);
    layout->setRowMinimumHeight(7, 24);

    layout->setColumnStretch(8, 1);
    layout->setRowStretch(10, 1);

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
    uq["calibrationDataFile"]=calDataFileEdit->text();

//    if (uq.contains("calibrationDataFile")) {
//        QString calDataFile = uq["calibrationDataFile"].toString();
//        calDataFileEdit->setText(calDataFile);
//    }



//    uq["readCalibrationData"]=readCalibrationDataCheckBox->isChecked();
//    uq["numExperiments"]=numExperiments->text().toInt();
//    uq["calibrateErrorMultipliersPerResponse"]=calibratePerResponseCheckBox->isChecked();
//    uq["calibrateErrorMultipliersPerExperiment"]=calibratePerExperimentCheckBox->isChecked();


//    uq["readCovariance"]=readCovarianceDataCheckBox->isChecked();
//    uq["calibrateCovarianceMultipliers"]=calibrateCovarianceMultiplierCheckBox->isChecked();

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

        if (uq.contains("calibrationDataFile")) {
            QString calDataFile = uq["calibrationDataFile"].toString();
            calDataFileEdit->setText(calDataFile);
        }

////        bool calibrateCovarianceMultipliersBool = false;
////        bool readCovarianceBool = false;
//        int numExp = 1;
//        bool readCalDataBool = false;
//        if (uq.contains("readCalibrationData")) {
//            if(uq["readCalibrationData"].toBool()) {
//                readCalDataBool = true;
//                if (uq.contains("numExperiments")) {
//                    numExp = uq["numExperiments"].toInt();
//                }
////                if (uq.contains("readCovariance")) {
////                    readCovarianceBool = uq["readCovariance"].toBool();
////                }
////                if (uq.contains("calibrateCovarianceMultipliers")) {
////                    calibrateCovarianceMultipliersBool = uq["calibrateCovarianceMultipliers"].toBool();
////                }
//            }
//        }
//        readCalibrationDataCheckBox->setChecked(readCalDataBool);
//        numExperiments->setText(QString::number(numExp));
////        readCovarianceDataCheckBox->setChecked(readCovarianceBool);
////        calibrateCovarianceMultiplierCheckBox->setChecked(calibrateCovarianceMultipliersBool);

//        bool perResponseBool = false;
//        if (uq.contains("calibrateErrorMultipliersPerResponse")) {
//            if (uq["calibrateErrorMultipliersPerResponse"].toBool()) {
//                perResponseBool = true;
//            }
//        }
//        calibratePerResponseCheckBox->setChecked(perResponseBool);

//        bool perExperimentBool = false;
//        if (uq.contains("calibrateErrorMultipliersPerExperiment")) {
//            if (uq["calibrateErrorMultipliersPerExperiment"].toBool()) {
//                perExperimentBool = true;
//            }
//        }
//        calibratePerExperimentCheckBox->setChecked(perExperimentBool);

//        if (calibrateCovarianceMultipliersBool) {
//            if (!readCovarianceBool){
//                emit sendErrorMessage("ERROR: Bayesian Calibration Input - covariance data must be read from file to calibrate multipliers");
//                result = false;
//            }
//            else {
//                if (!readCalDataBool) {
//                    emit sendErrorMessage("ERROR: Bayesian Calibration Input - calibration data must also be read from file if calibrating multipliers");
//                    result = false;
//                }
//            }
//        }

//        if (readCovarianceBool) {
//            if (!readCalDataBool) {
//                emit sendErrorMessage("ERROR: Bayesian Calibration Input - calibration data must also be read from file if reading error covariance from file");
//                result = false;
//            }
//        }



//        if (uq.contains("calibrateErrorMultipliers"))
//            qDebug() << "\n\n Contains calibrateErrorMultipliers, state: " << uq["calibrateErrorMultipliers"].toBool();
//            if (uq["calibrateErrorMultipliers"].toBool()) {
//                calibrateCovarianceMultiplierCheckBox->setChecked(true);
//                qDebug() << "calibrateErrorCheckBox setChecked to true";
//            }

//        if (jsonObject.contains("advancedOpt")) {
//            if (jsonObject["advancedOpt"].toBool()) {
//              theAdvancedCheckBox->setChecked(true);



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
        result = false;
    }

    return result;
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
