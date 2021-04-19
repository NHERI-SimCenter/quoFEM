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

    //
    // High-fidelity data
    //

    QGroupBox *theHFbox = new QGroupBox("High-fidelity dataset");
    QGridLayout *theHFlayout = new QGridLayout();
    theHFbox->setLayout(theHFlayout);
    layout->addWidget(theHFbox,0,0,1,-1);

    // Input

    inpFileDir_HF = new QLineEdit();
    QPushButton *chooseInpFile = new QPushButton("Choose");
    connect(chooseInpFile, &QPushButton::clicked, this, [=](){
        inpFileDir_HF->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
        //this->parseInputDataForRV(inpFileDir->text());
    });
    inpFileDir_HF->setMinimumWidth(600);
    inpFileDir_HF->setReadOnly(true);
    theHFlayout->addWidget(new QLabel("Training Points (Input) File"),2,0);
    theHFlayout->addWidget(inpFileDir_HF,2,1,1,3);
    theHFlayout->addWidget(chooseInpFile,2,4);

    // Output

    outFileDir_HF = new QLineEdit();
    QPushButton *chooseOutFile = new QPushButton("Choose");
    connect(chooseOutFile, &QPushButton::clicked, this, [=](){
        outFileDir_HF->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
        //this->parseOutputDataForQoI(outFileDir->text());
    });
    outFileDir_HF->setMinimumWidth(600);
    outFileDir_HF->setReadOnly(true);
    theHFlayout->addWidget(new QLabel("System Results (Output) File     "),3,0,Qt::AlignTop);
    theHFlayout->addWidget(outFileDir_HF,3,1,1,3,Qt::AlignTop);
    theHFlayout->addWidget(chooseOutFile,3,4,Qt::AlignTop);

    theHFlayout->setRowStretch(4, 1);
    theHFlayout->setColumnStretch(5, 1);

    //
    // Low-fidelity data
    //

    QGroupBox *theLFbox = new QGroupBox("Low-fidelity dataset");
    QGridLayout *theLFlayout = new QGridLayout();
    theLFbox->setLayout(theLFlayout);
    layout->addWidget(theLFbox,1,0,1,-1);

    // Input

    int id_lf = 0;

    inpFileDir_LF = new QLineEdit();
    chooseInpFile_LF = new QPushButton("Choose");
    connect(chooseInpFile_LF, &QPushButton::clicked, this, [=](){
        inpFileDir_LF->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
        if (!theCheckButton->isChecked()) {
            this->parseInputDataForRV(inpFileDir_LF->text());
        }
    });
    inpFileDir_LF->setMinimumWidth(600);
    inpFileDir_LF->setReadOnly(true);
    theLFlayout->addWidget(new QLabel("Training Points (Input) File"),id_lf,0);
    theLFlayout->addWidget(inpFileDir_LF,id_lf,1,1,3);
    theLFlayout->addWidget(chooseInpFile_LF,id_lf++,4);

    // Output - Data

    outFileDir_LF = new QLineEdit();
    chooseOutFile_LF = new QPushButton("Choose");
    connect(chooseOutFile_LF, &QPushButton::clicked, this, [=](){
        outFileDir_LF->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
        if (!theCheckButton->isChecked()) {
            this->parseOutputDataForQoI(outFileDir_LF->text());
        }
    });
    outFileDir_LF->setMinimumWidth(600);
    outFileDir_LF->setReadOnly(true);
    theLFlayout->addWidget(new QLabel("System Results (Output) File     "),id_lf,0,Qt::AlignTop);
    theLFlayout->addWidget(outFileDir_LF,id_lf,1,1,3,Qt::AlignTop);
    theLFlayout->addWidget(chooseOutFile_LF,id_lf++,4,Qt::AlignTop);

    //
    // Output - Model
    //

    theCheckLabel = new QLabel("     Simulate low-fidelity model");
    theCheckButton = new QCheckBox();
    theCheckLabel -> setStyleSheet("font-weight: bold; color: grey");

    line1= new QFrame;
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);

    theLFoptions = new QWidget();
    QGridLayout *theLFoptions_grid = new QGridLayout(theLFoptions);
    theLFoptions_grid->setMargin(0);

    theLFlayout->addWidget(theCheckLabel,id_lf,1);
    theLFlayout->addWidget(theCheckButton,id_lf,1);
    theLFlayout->addWidget(theLFoptions,id_lf++,2,3,-1);
    theLFlayout->addWidget(line1, id_lf++, 1);

    line1->hide();

    theLFlayout->setRowStretch(4, 1);
    theLFlayout->setColumnStretch(5, 1);



    // FEM model

    int id_fe=0;
    QLabel * theFEMLabel = new QLabel("   Please specify the model at the FEM Tab");
    theLFoptions_grid->addWidget(theFEMLabel, id_fe++, 0,1,2);
    theFEMLabel->setStyleSheet("font-style: italic");

    // Random Seed

    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Set the seed");
    randomSeed->setMaximumWidth(150);

    theLFoptions_grid->addWidget(new QLabel("   Random Seed"), id_fe, 0);
    theLFoptions_grid->addWidget(randomSeed, id_fe++, 1);

    // create convergence criteria

    accuracyMeasure = new QLineEdit();
    accuracyMeasure->setText(tr("0.02"));
    accuracyMeasure->setValidator(new QDoubleValidator);
    accuracyMeasure->setToolTip("NRMSE: normalized root mean square error");
    accuracyMeasure->setMaximumWidth(150);

    theLFoptions_grid->addWidget(new QLabel("   Target Accuracy (Normalized Error)   "), id_fe, 0);
    theLFoptions_grid->addWidget(accuracyMeasure, id_fe++, 1);

    // create layout label and entry for # samples

    numSamples = new QLineEdit();
    numSamples->setText(tr("150"));
    numSamples->setValidator(new QIntValidator);
    numSamples->setToolTip("Specify the number of samples");
    numSamples->setMaximumWidth(150);

    theLFoptions_grid->addWidget(new QLabel("   Max Number of Model Runs"), id_fe, 0);
    theLFoptions_grid->addWidget(numSamples, id_fe++, 1);

    // Max computation time (approximate)

    timeMeasure = new QLineEdit();
    timeMeasure->setText(tr("60"));
    timeMeasure->setValidator(new QIntValidator);
    timeMeasure->setToolTip("Max Computation Time (minutes)");
    timeMeasure->setMaximumWidth(150);

    theLFoptions_grid->addWidget(new QLabel("   Max Computation Time (minutes)    "), id_fe, 0);
    theLFoptions_grid->addWidget(timeMeasure, id_fe++, 1);


    // Existing Dataset

    theExistingCheckBox = new QCheckBox();
    theLFoptions_grid->addWidget(new QLabel("   Start with Existing Dataset"), id_fe, 0);
    theLFoptions_grid->addWidget(theExistingCheckBox, id_fe++, 1);


    // Do DoE

    theDoECheckBox = new QCheckBox();
    theLFoptions_grid->addWidget(new QLabel("   Do Adaptive Design of Experiments"), id_fe, 0);
    theLFoptions_grid->addWidget(theDoECheckBox, id_fe++, 1);

    theLFoptions_grid->setRowStretch(id_fe, 1);
    theLFoptions_grid->setColumnStretch(2, 1);
    theLFoptions->hide();

    //
    // Errors
    //

    errMSG=new QLabel("Unrecognized file format");
    errMSG->setStyleSheet({"color: red"});
    layout->addWidget(errMSG,2,0,Qt::AlignLeft);
    errMSG->hide();

    //
    // Advanced options
    //
    theAdvancedCheckBox = new QCheckBox();
    theAdvancedTitle=new QLabel("\n    Advanced Options for Gaussian Process Model");
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

    theLinearLabel=new QLabel("Use Linear Trend Function");

    theLinearCheckBox = new QCheckBox();
    layout->addWidget(theLinearLabel, 6, 0);
    layout->addWidget(theLinearCheckBox, 6, 1);
    //theLinearLabel->setStyleSheet("color: gray");
    //theLinearCheckBox->setDisabled(1);
    theLinearLabel->setVisible(false);
    theLinearCheckBox->setVisible(false);

    // Use Log transform

    theLogtLabel=new QLabel("Responses are always positive");
    theLogtLabel2=new QLabel("     (allow log-transform)");

    theLogtCheckBox = new QCheckBox();
    layout->addWidget(theLogtLabel, 7, 0);
    layout->addWidget(theLogtLabel2, 7, 1);
    layout->addWidget(theLogtCheckBox, 7, 1);
    theLogtLabel->setVisible(false);
    theLogtLabel2->setVisible(false);
    theLogtCheckBox->setVisible(false);

    //
    // Finish
    //

    layout->setRowStretch(8, 1);
    layout->setColumnStretch(7, 1);
    this->setLayout(layout);

    connect(theCheckButton,SIGNAL(toggled(bool)),this,SLOT(setLowFidDir(bool)));
    connect(theAdvancedCheckBox,SIGNAL(toggled(bool)),this,SLOT(doAdvancedGP(bool)));
    connect(theExistingCheckBox,SIGNAL(toggled(bool)),this,SLOT(doExistingGP(bool)));
    theExistingCheckBox->setChecked(true);

}


SurrogateMFInputWidget::~SurrogateMFInputWidget()
{

}

void SurrogateMFInputWidget::doExistingGP(bool tog)
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

// SLOT function
void SurrogateMFInputWidget::doAdvancedGP(bool tog)
{
    if (tog) {
        theAdvancedTitle->setStyleSheet("font-weight: bold; color: black");
        lineA->setVisible(true);
        gpKernel->setVisible(true);
        theLinearCheckBox->setVisible(true);
        theLogtCheckBox->setVisible(true);
        theLinearLabel->setVisible(true);
        theLogtLabel->setVisible(true);
        theLogtLabel2->setVisible(true);
        theKernelLabel->setVisible(true);
    } else {
        theAdvancedTitle->setStyleSheet("font-weight: bold; color: gray");

        lineA->setVisible(false);
        gpKernel->setVisible(false);
        theLinearCheckBox->setVisible(false);
        theLogtCheckBox->setVisible(false);
        theLinearLabel->setVisible(false);
        theLogtLabel->setVisible(false);
        theLogtLabel2->setVisible(false);
        theKernelLabel->setVisible(false);
        gpKernel->setCurrentIndex(0);
        theLinearCheckBox->setChecked(false);
        theLogtCheckBox->setChecked(false);
    }
}


void SurrogateMFInputWidget::setLowFidDir(bool tog)
{
    if (tog) {
        theExistingCheckBox->setChecked(false);
        theEdpWidget->setGPQoINames(QStringList("") );
        theFemWidget->setFEMforGP("GPmodel");
        theFemWidget->femProgramChanged("OpenSees");
        theEdpWidget->setGPQoINames(QStringList({}) );// remove GP RVs
        theParameters->setGPVarNamesAndValues(QStringList({}));// remove GP RVs
        errMSG->hide();
        theLFoptions->show();
        line1->show();
        theCheckLabel -> setStyleSheet("font-weight: bold; color: black");
    } else {
        theExistingCheckBox->setChecked(true);
        theFemWidget->setFEMforGP("GPdata");
        parseInputDataForRV(inpFileDir_LF->text());
        parseOutputDataForQoI(outFileDir_LF->text());
        theLFoptions->hide();
        line1->hide();
        theCheckLabel -> setStyleSheet("font-weight: bold; color: grey");

    }
}

bool
SurrogateMFInputWidget::outputToJSON(QJsonObject &jsonObj){

    bool result = true;

    jsonObj["inpFile_HF"]=inpFileDir_HF->text();
    jsonObj["outFile_HF"]=outFileDir_HF->text();


    jsonObj["LFfromModel"]=theCheckButton->isChecked();

    if (theCheckButton->isChecked())
    {
        jsonObj["samples"]=numSamples->text().toInt();
        jsonObj["seed"]=randomSeed->text().toInt();
        jsonObj["timeLimit"]=timeMeasure->text().toDouble();
        jsonObj["accuracyLimit"]=accuracyMeasure->text().toDouble();
        jsonObj["existingDoE"]=theExistingCheckBox->isChecked();
        if (theExistingCheckBox->isChecked())
        {
            jsonObj["inpFile_LF"]=inpFileDir_LF->text();
            jsonObj["outFile_LF"]=outFileDir_LF->text();
        }
        jsonObj["doDoE"]=theDoECheckBox->isChecked();
    } else {
        jsonObj["inpFile_LF"]=inpFileDir_LF->text();
        jsonObj["outFile_LF"]=outFileDir_LF->text();
    }



    jsonObj["advancedOpt"]=theAdvancedCheckBox->isChecked();
    if (theAdvancedCheckBox->isChecked())
    {
        jsonObj["kernel"]=gpKernel->currentText();
        jsonObj["linear"]=theLinearCheckBox->isChecked();
        jsonObj["logTransform"]=theLogtCheckBox->isChecked();
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
            break;
        }
    }
    // close file
    inFile.close();
    return numberOfColumns_pre;
}

bool
SurrogateMFInputWidget::inputFromJSON(QJsonObject &jsonObject){

    bool result = true;

    if (jsonObject.contains("inpFile_HF")) {
        inpFileDir_HF->setText(jsonObject["inpFile_HF"].toString());
    } else {
        return false;
    }

    if (jsonObject.contains("outFile_HF")) {
        outFileDir_HF->setText(jsonObject["outFile_HF"].toString());
    } else {
        return false;
    }

    if (jsonObject.contains("LFfromModel")) {
      if (jsonObject["LFfromModel"].toBool()) {
          theCheckButton->setChecked(true);
          theFemWidget->setFEMforGP("GPmodel");

          if (jsonObject.contains("samples") && jsonObject.contains("seed")) {
              int samples=jsonObject["samples"].toInt();
              double seed=jsonObject["seed"].toDouble();
              numSamples->setText(QString::number(samples));
              randomSeed->setText(QString::number(seed));
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

          if (jsonObject.contains("existingDoE")) {
              if (jsonObject["existingDoE"].toBool()) {
                  theExistingCheckBox->setChecked(true);
                  inpFileDir_LF -> setText(jsonObject["inpFile_LF"].toString());
                  outFileDir_LF -> setText(jsonObject["outFile_LF"].toString());
              } else {
                  theExistingCheckBox->setChecked(false);
              }
          } else {
              result = false;
          }

          theDoECheckBox->setChecked(jsonObject["doDoE"].toBool());

      } else {
          theCheckButton->setChecked(false);
          outFileDir_LF->setText(jsonObject["outFile_LF"].toString());
          inpFileDir_LF->setText(jsonObject["inpFile_LF"].toString());
          theFemWidget->setFEMforGP("GPdata");
      }
    } else {
      return false;
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
  return numSamples->text().toInt();
}
