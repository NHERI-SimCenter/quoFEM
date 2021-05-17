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

#include "DakotaInputCalibration.h"
#include <DakotaResultsCalibration.h>

#include <QLabel>
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
#include <QValidator>
#include <RandomVariablesContainer.h>

DakotaInputCalibration::DakotaInputCalibration(QWidget *parent)
    : UQ_Engine(parent)
{
  QGridLayout *layout = new QGridLayout();

  calibrationMethod = new QComboBox();
  calibrationMethod->addItem(tr("OPT++GaussNewton"));
  calibrationMethod->addItem(tr("NL2SOL"));
  layout->addWidget(new QLabel("Method"), 0, 0);
  layout->addWidget(calibrationMethod, 0, 1, 1, 2);


  // The methods below are not yet handled by parseDAKOTA.py
  //calibrationMethod->addItem(tr("ColinyPattern"));
  //calibrationMethod->addItem(tr("ConjugateGradient"));
  //calibrationMethod->addItem(tr("Coliny_EA"));

  connect(calibrationMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(comboboxItemChanged(QString)));
  maxIterations = new QLineEdit();
  maxIterations->setText(tr("1000"));
  maxIterations->setValidator(new QIntValidator);
  maxIterations->setToolTip("Max number of iterations");

  layout->addWidget(new QLabel("Max # Iterations"), 1, 0, 1, 2);
  layout->addWidget(maxIterations ,1, 1, 1, 2);
    
  convergenceTol = new QLineEdit();
  convergenceTol->setText(QString::number(1.0e-4));
  layout->addWidget(new QLabel("Convergence Tol"), 2, 0, 1, 2);
  layout->addWidget(convergenceTol, 2, 1, 1, 2);
  convergenceTol->setValidator(new QDoubleValidator);
  convergenceTol->setToolTip("Specify the convergence tolerance");


  scalingFactors = new QLineEdit();
  scalingFactors->setPlaceholderText("(Optional)");
  scalingFactors->setToolTip("Scaling factors on responses, one for each QoI must be provided or values ignored, default is 1.0 for all");
  layout->addWidget(new QLabel("Scaling Factors"), 3, 0, 1, 2);
  layout->addWidget(scalingFactors, 3, 1, 1, 2);

//  readCalibrationDataCheckBox = new QCheckBox();
//  readCalibrationDataCheckBox->setChecked(false);
//  layout->addWidget(new QLabel("Read calibration data from file"), 4, 0);
//  layout->addWidget(readCalibrationDataCheckBox, 4, 1);

//  readCalibrationDataCheckBox = new QCheckBox();
//  readCalibrationDataCheckBox->setChecked(false);
//  layout->addWidget(new QLabel("Read calibration data from file"), 4, 0, 1, 1);
//  layout->addWidget(readCalibrationDataCheckBox, 4, 1, 1, 1);
//  connect(readCalibrationDataCheckBox,SIGNAL(toggled(bool)),this,SLOT(showNumExp(bool)));

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

  layout->addWidget(calDataFileLabel, 4, 0, 1, 1);
  layout->addWidget(calDataFileEdit, 4, 1, 1, 4);
  layout->addWidget(chooseCalDataFile, 4, 5, 1, 1);


//  int bottom = 1;
//  QIntValidator *posIntValidator = new QIntValidator();
//  posIntValidator->setBottom(bottom);

//  numExperiments = new QLineEdit();
//  numExperiments->setText(tr("1"));
//  numExperiments->setValidator(posIntValidator);
//  numExperiments->setToolTip("Number of calibration data files to read per response");


//  numExperimentsLabel = new QLabel();
//  numExperimentsLabel->setText(tr("# Experiments"));
//  numExperiments->setVisible(false);
//  numExperimentsLabel->setVisible(false);


//  layout->addWidget(numExperimentsLabel, 4, 2, 1, 1);
//  layout->addWidget(numExperiments, 4, 3, 1, 2);


  layout->setColumnStretch(4, 1);
  layout->setColumnStretch(6, 1);

  layout->setRowStretch(5,1);

  this->setLayout(layout);
}

int
DakotaInputCalibration::getMaxNumParallelTasks(void){
  return 1;
}

//// SLOT function
//void DakotaInputCalibration::showNumExp(bool tog)
//{
//    if (tog) {
//        numExperiments->setVisible(true);
//        numExperimentsLabel->setVisible(true);
//    } else {
//        numExperiments->setVisible(false);
//        numExperimentsLabel->setVisible(false);
//    }
//}

bool
DakotaInputCalibration::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject uq;
    uq["method"]=calibrationMethod->currentText();
    uq["maxIterations"]=maxIterations->text().toInt();
    uq["convergenceTol"]=convergenceTol->text().toDouble();
    uq["factors"]=scalingFactors->text();
    uq["calibrationDataFile"]=calDataFileEdit->text();
    if (calDataFileEdit->text().isEmpty()) {
        sendErrorMessage("ERROR: Calibration data file not provided");
        result = false;
    }

//    uq["readCalibrationData"]=readCalibrationDataCheckBox->isChecked();
//    uq["numExperiments"]=numExperiments->text().toInt();

    /*
    if(calibrationMethod->currentText()=="ColinyPattern")
    {
        uq["initialDelta"] =initial_step_size_value->text().toDouble();
        uq["thresholdDelta"] = final_step_size_value->text().toDouble() ;
        uq["contractionFactor"] = contraction_factor->text().toDouble();
        uq["maxFunEvals"] = max_function_evals_ColonyPattern->text().toInt() ;
        uq["patternMove"] = exploratory_moves->currentText();
    }
    if(calibrationMethod->currentText()=="Coliny_EA")
    {
        uq["maxFunEvals"]=max_function_evals_ColonyEA->text().toInt();
        uq["seed"]=seed_ColonyEA->text().toInt();
        uq["popSize"]=pop_sizeColonyEA->text().toInt();
        uq["fitnessType"]=fitness_typeColonyEA->currentText();
        uq["mutationType"]=mutation_typeColonyEA->currentText();
        uq["mutationRate"]=mutation_rateColonyEA->text().toDouble();
        uq["crossoverType"]=crossover_typeColonyEA->currentText();
        uq["crossoverRate"]=crossover_rateColonyEA->text().toDouble();
        uq["replacementType"]=replacement_typeColonyEA->currentText();
        uq["replacementValue"]=replacement_type_value_ColonyEA->text().toInt();
    }
    */

    jsonObject["calibrationMethodData"]=uq;

    return result;
}

void
DakotaInputCalibration::comboboxItemChanged(QString value)
{
    return;
}



DakotaInputCalibration::~DakotaInputCalibration()
{

}


void DakotaInputCalibration::clear(void)
{

}

bool
DakotaInputCalibration::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;
    this->clear();

    QJsonObject uq = jsonObject["calibrationMethodData"].toObject();
    QString method =uq["method"].toString();
    int maxIter=uq["maxIterations"].toInt();
    double convTol=uq["convergenceTol"].toDouble();
    QString fact = uq["factors"].toString();
    if (uq.contains("calibrationDataFile")) {
        QString calDataFile = uq["calibrationDataFile"].toString();
        calDataFileEdit->setText(calDataFile);
    }

    maxIterations->setText(QString::number(maxIter));
    convergenceTol->setText(QString::number(convTol));
    int index = calibrationMethod->findText(method);
    calibrationMethod->setCurrentIndex(index);
    scalingFactors->setText(fact);

//    int numExp = 1;
//    bool readCalDataBool = false;
//    if (uq.contains("readCalibrationData")) {
//        if(uq["readCalibrationData"].toBool()) {
//            readCalDataBool = true;
//            if (uq.contains("numExperiments")) {
//                numExp = uq["numExperiments"].toInt();
//            }
//        }
//    }
//    readCalibrationDataCheckBox->setChecked(readCalDataBool);
//    numExperiments->setText(QString::number(numExp));

    return result;

}

void 
DakotaInputCalibration::methodChanged(const QString &arg1)
{

}

int 
DakotaInputCalibration::processResults(QString &filenameResults, QString &filenameTab) 
{
    return 0;
}

UQ_Results *
DakotaInputCalibration::getResults(void) {
  return new DakotaResultsCalibration();
}

RandomVariablesContainer *
DakotaInputCalibration::getParameters(void) {
  QString classType("Design");
  return new RandomVariablesContainer(classType,QString("Dakota"));
}

QString
DakotaInputCalibration::getMethodName(void){
  return QString("calibration");
}
