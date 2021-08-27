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

#include <SurrogateMFInputWidget.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QPushButton>
#include <QFileDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <iostream>
#include <fstream>
#include <regex>
#include <iterator>
#include <string>
#include <sstream>
#include <InputWidgetParameters.h>
#include <InputWidgetEDP.h>
#include <InputWidgetFEM.h>

SurrogateMFInputWidget::SurrogateMFInputWidget(InputWidgetParameters *param,InputWidgetFEM *femwidget,InputWidgetEDP *edpwidget, QWidget *parent)
: UQ_MethodInputWidget(parent), theParameters(param), theEdpWidget(edpwidget), theFemWidget(femwidget)
{
    auto layout = new QGridLayout();

    theHighSimLabel = new QLabel("     Simulation model");
    theHighSimButton = new QCheckBox();
    theHighSimLabel -> setStyleSheet("font-weight: bold; color: grey");

    theLowSimLabel = new QLabel("     Simulation model");
    theLowSimButton = new QCheckBox();
    theLowSimLabel -> setStyleSheet("font-weight: bold; color: grey");


    // /////////////////////////////////////
    // HIGH FIDELITY
    // /////////////////////////////////////

    // create a high fidelity box
    QGroupBox *theHFbox = new QGroupBox("High-fidelity dataset");
    QGridLayout *theHFlayout = new QGridLayout();
    theHFbox->setLayout(theHFlayout);
    layout->addWidget(theHFbox,0,0,1,-1);

    //
    // DATA
    //

    // Input

    int id_hf = 0;
    inpFileDir_HF = new QLineEdit();
    chooseInpFile_HF = new QPushButton("Choose");
    connect(chooseInpFile_HF, &QPushButton::clicked, this, [=](){
        inpFileDir_HF->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"));
        if ((!theHighSimButton->isChecked()) && (!theLowSimButton->isChecked()) )
            this->parseInputDataForRV(inpFileDir_HF->text());
        else
            this->countColumn(inpFileDir_HF->text()); // to give error
    });
    inpFileDir_HF->setMinimumWidth(600);
    inpFileDir_HF->setReadOnly(true);
    theHFlayout->addWidget(new QLabel("Training Points (Input) File"),id_hf,0);
    theHFlayout->addWidget(inpFileDir_HF,id_hf,1,1,3);
    theHFlayout->addWidget(chooseInpFile_HF,id_hf++,4);

    // Output

    outFileDir_HF = new QLineEdit();
    chooseOutFile_HF = new QPushButton("Choose");
    connect(chooseOutFile_HF, &QPushButton::clicked, this, [=](){
        outFileDir_HF->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"));
        if ((!theHighSimButton->isChecked()) && (!theLowSimButton->isChecked()) )
            this->parseOutputDataForQoI(outFileDir_HF->text());
        else
            this->countColumn(outFileDir_HF->text()); // to give error
    });
    outFileDir_HF->setMinimumWidth(600);
    outFileDir_HF->setReadOnly(true);
    theHFlayout->addWidget(new QLabel("System Results (Output) File     "),id_hf,0,Qt::AlignTop);
    theHFlayout->addWidget(outFileDir_HF,id_hf,1,1,3,Qt::AlignTop);
    theHFlayout->addWidget(chooseOutFile_HF,id_hf++,4,Qt::AlignTop);

    //
    // Model
    //

    line0= new QFrame;
    line0->setFrameShape(QFrame::HLine);
    line0->setFrameShadow(QFrame::Sunken);

    theHighSimOptions = new QWidget();
    QGridLayout *theHighSimGrid = new QGridLayout(theHighSimOptions);
    theHighSimGrid->setMargin(0);

    theHFlayout->addWidget(theHighSimLabel,id_hf,1);
    theHFlayout->addWidget(theHighSimButton,id_hf,1);
    theHFlayout->addWidget(theHighSimOptions,id_hf++,2,3,-1);
    theHFlayout->addWidget(line0, id_hf++, 1);


    theHFlayout->setRowStretch(4, 1);
    theHFlayout->setColumnStretch(5, 1);
    line0->hide();

    // FEM model

    int id_fe=0;
    QLabel * theFEMLabelHF = new QLabel("   Please specify the model at the FEM Tab");
    theHighSimGrid->addWidget(theFEMLabelHF, id_fe++, 0,1,2);
    theFEMLabelHF->setStyleSheet("font-style: italic");

    // Create layout label and entry for # samples

    numSamples_HF = new QLineEdit();
    numSamples_HF->setText(tr("30"));
    numSamples_HF->setValidator(new QIntValidator);
    numSamples_HF->setToolTip("Specify the number of samples");
    numSamples_HF->setMaximumWidth(150);

    theHighSimGrid->addWidget(new QLabel("   Max Number of Model Runs"), id_fe, 0);
    theHighSimGrid->addWidget(numSamples_HF, id_fe++, 1);


    // Existing Dataset

    theExistingCheckBox_HF = new QCheckBox();
    theHighSimGrid->addWidget(new QLabel("   Start with Existing Dataset"), id_fe, 0);
    theHighSimGrid->addWidget(theExistingCheckBox_HF, id_fe++, 1);


    theHighSimGrid->setRowStretch(id_fe, 1);
    theHighSimGrid->setColumnStretch(2, 1);
    theHighSimOptions->hide();

    // /////////////////////////////////////
    // LOW FIDELITY - DATA
    // /////////////////////////////////////

    QGroupBox *theLFbox = new QGroupBox("Low-fidelity dataset");
    QGridLayout *theLFlayout = new QGridLayout();
    theLFbox->setLayout(theLFlayout);
    layout->addWidget(theLFbox,1,0,1,-1);

    // Input

    int id = 0;

    inpFileDir_LF = new QLineEdit();
    chooseInpFile_LF = new QPushButton("Choose");
    connect(chooseInpFile_LF, &QPushButton::clicked, this, [=](){
        inpFileDir_LF->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"));

        if ((!theHighSimButton->isChecked()) && (!theLowSimButton->isChecked()) )
            this->parseInputDataForRV(inpFileDir_LF->text());
        else
            this->countColumn(inpFileDir_LF->text()); // to give error

    });
    inpFileDir_LF->setMinimumWidth(600);
    inpFileDir_LF->setReadOnly(true);
    theLFlayout->addWidget(new QLabel("Training Points (Input) File"),id,0);
    theLFlayout->addWidget(inpFileDir_LF,id,1,1,3);
    theLFlayout->addWidget(chooseInpFile_LF,id++,4);

    outFileDir_LF = new QLineEdit();
    chooseOutFile_LF = new QPushButton("Choose");
    connect(chooseOutFile_LF, &QPushButton::clicked, this, [=](){
        outFileDir_LF->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"));

        if ((!theHighSimButton->isChecked()) && (!theLowSimButton->isChecked()) )
            this->parseOutputDataForQoI(outFileDir_LF->text());
        else
            this->countColumn(outFileDir_LF->text()); // to give error

    });
    outFileDir_LF->setMinimumWidth(600);
    outFileDir_LF->setReadOnly(true);
    theLFlayout->addWidget(new QLabel("System Results (Output) File     "),id,0,Qt::AlignTop);
    theLFlayout->addWidget(outFileDir_LF,id,1,1,3,Qt::AlignTop);
    theLFlayout->addWidget(chooseOutFile_LF,id++,4,Qt::AlignTop);

    // /////////////////////////////////////
    // LOW FIDELITY - Model
    // /////////////////////////////////////

    line1= new QFrame;
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);

    theLowSimOptions = new QWidget();
    QGridLayout *theLowSimGrid = new QGridLayout(theLowSimOptions);
    theLowSimGrid->setMargin(0);

    theLFlayout->addWidget(theLowSimLabel,id,1);
    theLFlayout->addWidget(theLowSimButton,id,1);
    theLFlayout->addWidget(theLowSimOptions,id++,2,3,-1);
    theLFlayout->addWidget(line1, id++, 1);

    line1->hide();

    theLFlayout->setRowStretch(4, 1);
    theLFlayout->setColumnStretch(5, 1);

    // FEM model

    id_fe=0;
    QLabel * theFEMLabel_LF = new QLabel("   Please specify the model at the FEM Tab");
    theLowSimGrid->addWidget(theFEMLabel_LF, id_fe++, 0,1,2);
    theFEMLabel_LF->setStyleSheet("font-style: italic");


    numSamples_LF = new QLineEdit();
    numSamples_LF->setText(tr("150"));
    numSamples_LF->setValidator(new QIntValidator);
    numSamples_LF->setToolTip("Specify the number of samples");
    numSamples_LF->setMaximumWidth(150);

    theLowSimGrid->addWidget(new QLabel("   Max Number of Model Runs"), id_fe, 0);
    theLowSimGrid->addWidget(numSamples_LF, id_fe++, 1);


    // Existing Dataset

    theExistingCheckBox_LF = new QCheckBox();
    theLowSimGrid->addWidget(new QLabel("   Start with Existing Dataset"), id_fe, 0);
    theLowSimGrid->addWidget(theExistingCheckBox_LF, id_fe++, 1);


    theLowSimGrid->setRowStretch(id_fe, 1);
    theLowSimGrid->setColumnStretch(2, 1);
    theLowSimOptions->hide();

    //
    // Errors
    //

    errMSG=new QLabel("Unrecognized file format");
    errMSG->setStyleSheet({"color: red"});
    layout->addWidget(errMSG,2,0,Qt::AlignLeft);
    errMSG->hide();


    // /////////////////////////////////////
    // SIMULATION OPTIONS
    // /////////////////////////////////////

    theSimBox = new QGroupBox("Simulation Options");
    QGridLayout *theSimGrid = new QGridLayout();
    theSimBox->setLayout(theSimGrid);
    layout->addWidget(theSimBox,2,0,1,-1);
    theSimBox->setVisible(false);

    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Set the seed");
    randomSeed->setMaximumWidth(150);

    theSimGrid->addWidget(new QLabel("   Random Seed"), id_fe, 0);
    theSimGrid->addWidget(randomSeed, id_fe++, 1);

//    create convergence criteria

    accuracyMeasure = new QLineEdit();
    accuracyMeasure->setText(tr("0.02"));
    accuracyMeasure->setValidator(new QDoubleValidator);
    accuracyMeasure->setToolTip("NRMSE: normalized root mean square error");
    accuracyMeasure->setMaximumWidth(150);

    theSimGrid->addWidget(new QLabel("   Target Accuracy (Normalized Error)   "), id_fe, 0);
    theSimGrid->addWidget(accuracyMeasure, id_fe++, 1);

//    Max computation time (approximate)

    timeMeasure = new QLineEdit();
    timeMeasure->setText(tr("60"));
    timeMeasure->setValidator(new QIntValidator);
    timeMeasure->setToolTip("Max Computation Time (minutes)");
    timeMeasure->setMaximumWidth(150);

    theSimGrid->addWidget(new QLabel("   Max Computation Time (minutes)    "), id_fe, 0);
    theSimGrid->addWidget(timeMeasure, id_fe++, 1);

    // Do DoE

    theDoECheckBox = new QCheckBox();
    theSimGrid->addWidget(new QLabel("   Do Adaptive Design of Experiments"), id_fe, 0);
    theSimGrid->addWidget(theDoECheckBox, id_fe++, 1);

    theSimGrid->setColumnStretch(2,1);
    theSimGrid->setRowStretch(4,1);


    //
    // Parallel Execution
    //

    parallelCheckBox = new QCheckBox();
    parallelCheckBox -> setChecked(true);
    theSimGrid->addWidget(new QLabel("   Parallel Execution"), id_fe, 0);
    theSimGrid->addWidget(parallelCheckBox, id_fe++, 1);

    // /////////////////////////////////////
    // ADVANCED
    // /////////////////////////////////////

    theAdvancedCheckBox = new QCheckBox();
    theAdvancedTitle=new QLabel("\n     Advanced Options for Gaussian Process Model");
    theAdvancedTitle->setStyleSheet("font-weight: bold; color: gray");
    layout->addWidget(theAdvancedTitle, 3, 0,1,3,Qt::AlignBottom);
    layout->addWidget(theAdvancedCheckBox, 3, 0,Qt::AlignBottom);

    lineA = new QFrame;
    lineA->setFrameShape(QFrame::HLine);
    lineA->setFrameShadow(QFrame::Sunken);
    lineA->setMaximumWidth(420);
    layout->addWidget(lineA, 4, 0, 1, 3);
    lineA->setVisible(false);

    // Selection of GP kernel

    theKernelLabel=new QLabel("Kernel Function");

    gpKernel = new QComboBox();
    gpKernel->addItem(tr("Matern 5/2"));
    gpKernel->addItem(tr("Matern 3/2"));
    gpKernel->addItem(tr("Radial Basis"));
    gpKernel->addItem(tr("Exponential"));
    gpKernel->setMaximumWidth(150);
    layout->addWidget(theKernelLabel, 5, 0);
    layout->addWidget(gpKernel, 5, 1);
    gpKernel->setCurrentIndex(0);
    theKernelLabel->setVisible(false);
    gpKernel->setVisible(false);

    // Use Linear trending function

    theLinearLabel=new QLabel("Add Linear Trend Function");

    theLinearCheckBox = new QCheckBox();
    layout->addWidget(theLinearLabel, 6, 0);
    layout->addWidget(theLinearCheckBox, 6, 1);
    //theLinearLabel->setStyleSheet("color: gray");
    //theLinearCheckBox->setDisabled(1);
    theLinearLabel->setVisible(false);
    theLinearCheckBox->setVisible(false);

    // Use Log transform

    theLogtLabel=new QLabel("Log-space Transform of QoI");
    theLogtLabel2=new QLabel("     (check this box only when all response qunatities are always positive)");

    theLogtCheckBox = new QCheckBox();
    layout->addWidget(theLogtLabel, 7, 0);
    layout->addWidget(theLogtLabel2, 7, 1);
    layout->addWidget(theLogtCheckBox, 7, 1);
    theLogtLabel->setVisible(false);
    theLogtLabel2->setVisible(false);
    theLogtCheckBox->setVisible(false);


    //
    // Nugget function
    //

    theNuggetLabel=new QLabel("Nugget Values for each QoI");
    theNuggetSelection = new QComboBox();

    theNuggetSelection->addItem(tr("Optimize"),0);
    theNuggetSelection->addItem(tr("Fixed Values"),1);
    theNuggetSelection->addItem(tr("Fixed Bounds"),2);
    theNuggetSelection->setMaximumWidth(150);
    theNuggetSelection->setCurrentIndex(0);

    theNuggetVals = new QLineEdit();
    theNuggetVals->setToolTip("Provide nugget values");
    //theNuggetVals->setMaximumWidth(150);

    layout->addWidget(theNuggetLabel, 8, 0);
    layout->addWidget(theNuggetSelection, 8, 1);
    layout->addWidget(theNuggetVals, 9, 1,1,2);

    theNuggetLabel->setVisible(false);
    theNuggetSelection->setVisible(false);
    theNuggetVals->setVisible(false);

    connect(theNuggetSelection,SIGNAL(currentIndexChanged(int)),this,SLOT(showNuggetBox(int)));

    //
    // Finish
    //

    layout->setRowStretch(10, 1);
    layout->setColumnStretch(7, 1);
    this->setLayout(layout);

    connect(theLowSimButton,SIGNAL(toggled(bool)),this,SLOT(setLowSim(bool)));
    connect(theHighSimButton,SIGNAL(toggled(bool)),this,SLOT(setHighSim(bool)));
    //connect(theLowDataButton,SIGNAL(toggled(bool)),this,SLOT(setHighSim(bool)));
    //connect(theHighDataButton,SIGNAL(toggled(bool)),this,SLOT(setHighData(bool)));

    connect(theAdvancedCheckBox,SIGNAL(toggled(bool)),this,SLOT(doAdvancedGP(bool)));
    connect(theExistingCheckBox_LF,SIGNAL(toggled(bool)),this,SLOT(doExistingLF(bool)));
    connect(theExistingCheckBox_HF,SIGNAL(toggled(bool)),this,SLOT(doExistingHF(bool)));
    theExistingCheckBox_LF->setChecked(true);
    theExistingCheckBox_HF->setChecked(true);

}


SurrogateMFInputWidget::~SurrogateMFInputWidget()
{

}


void
SurrogateMFInputWidget::showNuggetBox(int idx)
{

    if (idx == 0) {
        theNuggetVals->hide();
    } else if (idx==1){
        theNuggetVals->show();
        theNuggetVals->setPlaceholderText("QoI1, QoI2,..");
    } else if (idx==2) {
        theNuggetVals->show();
        theNuggetVals->setPlaceholderText("[QoI1_LB,QoI1_UB], [QoI2_LB,QoI2_UB],..");
    }

};


void SurrogateMFInputWidget::doExistingLF(bool tog)
{
    if (tog) {
        inpFileDir_LF->setDisabled(0);
        outFileDir_LF->setDisabled(0);
        chooseInpFile_LF->setDisabled(0);
        chooseInpFile_LF->setStyleSheet("font-color: white");
        chooseOutFile_LF->setDisabled(0);
        chooseOutFile_LF->setStyleSheet("font-color: white");
    } else {
        inpFileDir_LF->setDisabled(1);
        outFileDir_LF->setDisabled(1);
        chooseInpFile_LF->setDisabled(1);
        chooseInpFile_LF->setStyleSheet("background-color: lightgrey;border-color:grey");
        chooseOutFile_LF->setDisabled(1);
        chooseOutFile_LF->setStyleSheet("background-color: lightgrey;border-color:grey");
    }
}

void SurrogateMFInputWidget::doExistingHF(bool tog)
{
    if (tog) {
        inpFileDir_HF->setDisabled(0);
        outFileDir_HF->setDisabled(0);
        chooseInpFile_HF->setDisabled(0);
        chooseInpFile_HF->setStyleSheet("font-color: white");
        chooseOutFile_HF->setDisabled(0);
        chooseOutFile_HF->setStyleSheet("font-color: white");
    } else {
        inpFileDir_HF->setDisabled(1);
        outFileDir_HF->setDisabled(1);
        chooseInpFile_HF->setDisabled(1);
        chooseInpFile_HF->setStyleSheet("background-color: lightgrey;border-color:grey");
        chooseOutFile_HF->setDisabled(1);
        chooseOutFile_HF->setStyleSheet("background-color: lightgrey;border-color:grey");
    }
}

// SLOT function
void SurrogateMFInputWidget::doAdvancedGP(bool tog)
{
    if (tog) {
        theAdvancedTitle->setStyleSheet("font-weight: bold; color: black");

    } else {
        theAdvancedTitle->setStyleSheet("font-weight: bold; color: gray");
        gpKernel->setCurrentIndex(0);
        theLinearCheckBox->setChecked(false);
        theLogtCheckBox->setChecked(false);
    }

    lineA->setVisible(tog);
    gpKernel->setVisible(tog);
    theLinearCheckBox->setVisible(tog);
    theLogtCheckBox->setVisible(tog);
    theLinearLabel->setVisible(tog);
    theLogtLabel->setVisible(tog);
    theLogtLabel2->setVisible(tog);
    theKernelLabel->setVisible(tog);
    theNuggetLabel->setVisible(tog);
    theNuggetVals->setVisible(tog);
}


void SurrogateMFInputWidget::setLowSim(bool tog)
{
    theExistingCheckBox_LF->setChecked(!tog);

    if (theLowSimButton->isChecked() && theHighSimButton->isChecked()) {
        theEdpWidget->setGPQoINames(QStringList("") );
        theFemWidget->setFEMforGP("GPMFmodel");
        theEdpWidget->setGPQoINames(QStringList({}) );// remove GP RVs
        theParameters->setGPVarNamesAndValues(QStringList({}));// remove GP RVs
        errMSG->hide();
        theSimBox->setVisible(true);
    } else if (!theLowSimButton->isChecked() && !theHighSimButton->isChecked()) {
        theFemWidget->setFEMforGP("GPdata");
        theSimBox->setVisible(false);
    } else {
        theEdpWidget->setGPQoINames(QStringList("") );
        theFemWidget->setFEMforGP("GPmodel");
        //theFemWidget->femProgramChanged("OpenSees");
        theEdpWidget->setGPQoINames(QStringList({}) );// remove GP RVs
        theParameters->setGPVarNamesAndValues(QStringList({}));// remove GP RVs
        errMSG->hide();
        theSimBox->setVisible(true);
    }

    if (theLowSimButton->isChecked()) {
        theLowSimOptions->show();
        line1->show();
        theLowSimLabel -> setStyleSheet("font-weight: bold; color: black");
    } else {
        theExistingCheckBox_LF->setChecked(true);
        parseInputDataForRV(inpFileDir_LF->text());
        parseOutputDataForQoI(outFileDir_LF->text());
        theLowSimOptions->hide();
        line1->hide();
        theLowSimLabel -> setStyleSheet("font-weight: bold; color: grey");
    }
}
void SurrogateMFInputWidget::setHighSim(bool tog) {

    theExistingCheckBox_HF->setChecked(!tog);

    if (theLowSimButton->isChecked() && theHighSimButton->isChecked()) {
        theEdpWidget->setGPQoINames(QStringList("") );
        theFemWidget->setFEMforGP("GPMFmodel");
        theEdpWidget->setGPQoINames(QStringList({}) );// remove GP RVs
        theParameters->setGPVarNamesAndValues(QStringList({}));// remove GP RVs
        errMSG->hide();
        theSimBox->setVisible(true);
    } else if (!theLowSimButton->isChecked() && !theHighSimButton->isChecked()) {
        theFemWidget->setFEMforGP("GPdata");
        theSimBox->setVisible(false);
    } else {
        theEdpWidget->setGPQoINames(QStringList("") );
        theFemWidget->setFEMforGP("GPmodel");
        //theFemWidget->femProgramChanged("OpenSees");
        theEdpWidget->setGPQoINames(QStringList({}) );// remove GP RVs
        theParameters->setGPVarNamesAndValues(QStringList({}));// remove GP RVs
        errMSG->hide();
        theSimBox->setVisible(true);
    }

    if (theHighSimButton->isChecked()) {
        theHighSimOptions->show();
        line0->show();
        theHighSimLabel -> setStyleSheet("font-weight: bold; color: black");
    } else {
        theExistingCheckBox_HF->setChecked(true);
        parseInputDataForRV(inpFileDir_HF->text());
        parseOutputDataForQoI(outFileDir_HF->text());
        theHighSimOptions->hide();
        line0->hide();
        theHighSimLabel -> setStyleSheet("font-weight: bold; color: grey");
    }
}

bool
SurrogateMFInputWidget::outputToJSON(QJsonObject &jsonObj){

    bool result = true;

    jsonObj["HFfromModel"]=theHighSimButton->isChecked();

    if (theHighSimButton->isChecked())
    {
        jsonObj["samples_HF"]=numSamples_HF->text().toInt();
        jsonObj["existingDoE_HF"]=theExistingCheckBox_HF->isChecked();
        if (theExistingCheckBox_HF->isChecked())
        {
            jsonObj["inpFile_HF"]=inpFileDir_HF->text();
            jsonObj["outFile_HF"]=outFileDir_HF->text();
        }
    } else {
        jsonObj["inpFile_HF"]=inpFileDir_HF->text();
        jsonObj["outFile_HF"]=outFileDir_HF->text();
    }

    jsonObj["LFfromModel"]=theLowSimButton->isChecked();

    if (theLowSimButton->isChecked())
    {
        jsonObj["samples_LF"]=numSamples_LF->text().toInt();
        jsonObj["existingDoE_LF"]=theExistingCheckBox_LF->isChecked();
        if (theExistingCheckBox_LF->isChecked())
        {
            jsonObj["inpFile_LF"]=inpFileDir_LF->text();
            jsonObj["outFile_LF"]=outFileDir_LF->text();
        }
    } else {
        jsonObj["inpFile_LF"]=inpFileDir_LF->text();
        jsonObj["outFile_LF"]=outFileDir_LF->text();
    }

    if (theHighSimButton->isChecked() || theLowSimButton->isChecked()) {
        jsonObj["seed"]=randomSeed->text().toInt();
        jsonObj["timeLimit"]=timeMeasure->text().toDouble();
        jsonObj["accuracyLimit"]=accuracyMeasure->text().toDouble();
        jsonObj["doDoE"]=theDoECheckBox->isChecked();
        jsonObj["parallelExecution"]=parallelCheckBox->isChecked();
    }

    jsonObj["advancedOpt"]=theAdvancedCheckBox->isChecked();
    if (theAdvancedCheckBox->isChecked())
    {
        jsonObj["kernel"]=gpKernel->currentText();
        jsonObj["linear"]=theLinearCheckBox->isChecked();
        jsonObj["logTransform"]=theLogtCheckBox->isChecked();
        jsonObj["nuggetOpt"]=theNuggetSelection->currentText();
        jsonObj["nuggetString"]=theNuggetVals->text();

    }
    return result;    
}


int SurrogateMFInputWidget::parseInputDataForRV(QString name1){

    double numberOfColumns=countColumn(name1);

    QStringList varNamesAndValues;
    for (int i=0;i<numberOfColumns;i++) {
        varNamesAndValues.append(QString("RV_column%1").arg(i+1));
        varNamesAndValues.append("nan");
    }
    theParameters->setGPVarNamesAndValues(varNamesAndValues);
    //numSamples=0;
    return 0;
}

int SurrogateMFInputWidget::parseOutputDataForQoI(QString name1){
    // get number of columns
    double numberOfColumns=countColumn(name1);
    QStringList qoiNames;
    for (int i=0;i<numberOfColumns;i++) {
        qoiNames.append(QString("QoI_column%1").arg(i+1));
    }
    theEdpWidget->setGPQoINames(qoiNames);
    return 0;
}

int SurrogateMFInputWidget::countColumn(QString name1){
    // get number of columns
    if (name1.isEmpty())
        return 0;

    std::ifstream inFile(name1.toStdString());
    // read lines of input searching for pset using regular expression
    std::string line;
    errMSG->hide();

    int numberOfColumns_pre = -100;
    while (getline(inFile, line)) {
        int  numberOfColumns=1;
        bool previousWasSpace=false;
        //for(int i=0; i<line.size(); i++){
        for(size_t i=0; i<line.size(); i++){
            if(line[i] == '%' || line[i] == '#'){ // ignore header
                numberOfColumns = numberOfColumns_pre;
                break;
            }
            if(line[i] == ' ' || line[i] == '\t' || line[i] == ','){
                if(!previousWasSpace)
                    numberOfColumns++;
                previousWasSpace = true;
            } else {
                previousWasSpace = false;
            }
        }
        if(previousWasSpace)// when there is a blank space at the end of each row
            numberOfColumns--;

        if (numberOfColumns_pre==-100)  // to pass header
        {
            numberOfColumns_pre=numberOfColumns;
            continue;
        }
        if (numberOfColumns != numberOfColumns_pre)// Send an error
        {
            errMSG->show();
            numberOfColumns_pre=0;
            return 0;
        }
    }
    // close file
    if (numberOfColumns_pre<0)// The FIle is empty
    {
        errMSG->show();
        return 0;
    }


    inFile.close();
    return numberOfColumns_pre;
}

bool
SurrogateMFInputWidget::inputFromJSON(QJsonObject &jsonObject){

    bool result = true;

    if (jsonObject.contains("HFfromModel")) {
      if (jsonObject["HFfromModel"].toBool()) {
          theHighSimButton->setChecked(true);

          if (jsonObject.contains("samples_HF")) {
              int samples=jsonObject["samples_HF"].toInt();
              numSamples_HF->setText(QString::number(samples));
          } else {
              result = false;
          }

          if (jsonObject.contains("existingDoE_HF")) {
              if (jsonObject["existingDoE_HF"].toBool()) {
                  theExistingCheckBox_HF->setChecked(true);
                  inpFileDir_HF -> setText(jsonObject["inpFile_HF"].toString());
                  outFileDir_HF -> setText(jsonObject["outFile_HF"].toString());
              } else {
                  theExistingCheckBox_HF->setChecked(false);
              }
          } else {
              result = false;
          }

      } else {
          theLowSimButton->setChecked(false);
          outFileDir_HF->setText(jsonObject["outFile_HF"].toString());
          inpFileDir_HF->setText(jsonObject["inpFile_HF"].toString());
      }
    } else {
      return false;
    }

    if (jsonObject.contains("LFfromModel")) {
      if (jsonObject["LFfromModel"].toBool()) {
          theLowSimButton->setChecked(true);

          if (jsonObject.contains("samples_LF")) {
              int samples=jsonObject["samples_LF"].toInt();
              numSamples_LF->setText(QString::number(samples));
          } else {
              result = false;
          }

          if (jsonObject.contains("existingDoE_LF")) {
              if (jsonObject["existingDoE_LF"].toBool()) {
                  theExistingCheckBox_LF->setChecked(true);
                  inpFileDir_LF -> setText(jsonObject["inpFile_LF"].toString());
                  outFileDir_LF -> setText(jsonObject["outFile_LF"].toString());
              } else {
                  theExistingCheckBox_LF->setChecked(false);
              }
          } else {
              result = false;
          }

      } else {
          theLowSimButton->setChecked(false);
          outFileDir_LF->setText(jsonObject["outFile_LF"].toString());
          inpFileDir_LF->setText(jsonObject["inpFile_LF"].toString());
      }
    } else {
      return false;
    }

    if (jsonObject["LFfromModel"].toBool() || jsonObject["HFfromModel"].toBool())
    {

        if (jsonObject.contains("seed") && jsonObject.contains("doDoE")) {
            double seed=jsonObject["seed"].toDouble();
            randomSeed->setText(QString::number(seed));
            theDoECheckBox->setChecked(jsonObject["doDoE"].toBool());
        } else {
            result = false;
        }

        if (jsonObject.contains("timeLimit") && jsonObject.contains("accuracyLimit")) {
            int time=jsonObject["timeLimit"].toInt();
            double accuracy=jsonObject["accuracyLimit"].toDouble();
            timeMeasure->setText(QString::number(time));
            accuracyMeasure->setText(QString::number(accuracy));
        } else {
            result = false;
        }

        if (jsonObject.contains("parallelExecution")) {
            parallelCheckBox->setChecked(jsonObject["parallelExecution"].toBool());
        } else {
            parallelCheckBox->setChecked(false); // for compatibility. later change it to error. (sy - june 2021)
        }
    }

  if (jsonObject.contains("advancedOpt")) {
      theAdvancedCheckBox->setChecked(jsonObject["advancedOpt"].toBool());
      if (jsonObject["advancedOpt"].toBool()) {
        theAdvancedCheckBox->setChecked(true);
        QString method =jsonObject["kernel"].toString();
        int index = gpKernel->findText(method);
        if (index == -1) {
            return false;
        }
        gpKernel->setCurrentIndex(index);
        theLinearCheckBox->setChecked(jsonObject["linear"].toBool());
        theLogtCheckBox->setChecked(jsonObject["logTransform"].toBool());

        if (jsonObject.contains("nuggetOpt")) {
            QString nuggetOpt =jsonObject["nuggetOpt"].toString();
            index = theNuggetSelection->findText(nuggetOpt);
            if (index == -1) {
                return false;
            }
            theNuggetSelection->setCurrentIndex(index);
            if (index!=0){
                theNuggetVals->setText(jsonObject["nuggetString"].toString());
            }
        } else {
            theNuggetSelection->setCurrentIndex(0);
        }
      }
  } else {
     return false;
  }

  return result;
}

void
SurrogateMFInputWidget::clear(void)
{

}

int
SurrogateMFInputWidget::getNumberTasks()
{
  return numSamples_LF->text().toInt();
}
