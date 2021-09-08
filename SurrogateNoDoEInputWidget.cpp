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

#include <SurrogateNoDoEInputWidget.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QPushButton>
#include <QFileDialog>
#include <QCheckBox>
#include <QComboBox>
#include <iostream>
#include <fstream>
#include <regex>
#include <iterator>
#include <string>
#include <sstream>
#include <InputWidgetParameters.h>
#include <InputWidgetEDP.h>
#include <InputWidgetFEM.h>

SurrogateNoDoEInputWidget::SurrogateNoDoEInputWidget(InputWidgetParameters *param,InputWidgetFEM *femwidget,InputWidgetEDP *edpwidget, QWidget *parent)
: UQ_MethodInputWidget(parent), theParameters(param), theEdpWidget(edpwidget), theFemWidget(femwidget)
{
    auto layout = new QGridLayout();
    int wid = 0; // widget id

    //
    // Create Input LineEdit
    //

    inpFileDir = new QLineEdit();
    QPushButton *chooseInpFile = new QPushButton("Choose");
    connect(chooseInpFile, &QPushButton::clicked, this, [=](){
        inpFileDir->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"));
        this->parseInputDataForRV(inpFileDir->text());
    });
    inpFileDir->setMinimumWidth(600);
    inpFileDir->setReadOnly(true);
    layout->addWidget(new QLabel("Training Points (Input) File"),wid,0);
    layout->addWidget(inpFileDir,wid,1,1,3);
    layout->addWidget(chooseInpFile,wid++,4);

    //
    // Create Output selection checkbox
    //

    QHBoxLayout *theSelectionLayout = new QHBoxLayout();

    theCheckButton = new QCheckBox();

    QLabel *theLabel = new QLabel();
    theLabel->setText("Get results from datafile");

    theSelectionLayout->addWidget(theCheckButton,0);
    theSelectionLayout->addWidget(theLabel,1);


    //
    // Create Output LineEdit
    //
    outFileDir = new QLineEdit();
    chooseOutFile = new QPushButton("Choose");
    connect(chooseOutFile, &QPushButton::clicked, this, [=](){
        outFileDir->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"));
        this->parseOutputDataForQoI(outFileDir->text());
    });
    outFileDir->setMinimumWidth(600);
    outFileDir->setReadOnly(true);
    layout->addWidget(new QLabel("System Results (Output) File     "),wid,0,Qt::AlignTop);
    layout->addWidget(outFileDir,wid,1,1,3,Qt::AlignTop);
    layout->addWidget(chooseOutFile,wid,4,Qt::AlignTop);
    layout->addWidget(theCheckButton,wid,5,Qt::AlignTop);
    layout->addWidget(theLabel,wid++,6,Qt::AlignTop);

    errMSG=new QLabel("Unrecognized file format");
    errMSG->setStyleSheet({"color: red"});
    layout->addWidget(errMSG,wid++,1,Qt::AlignLeft);
    errMSG->hide();

    //
    // Create Advanced options
    //

    theAdvancedCheckBox = new QCheckBox();
    theAdvancedTitle=new QLabel("\n     Advanced Options for Gaussian Process Model");
    theAdvancedTitle->setStyleSheet("font-weight: bold; color: gray");
    layout->addWidget(theAdvancedTitle, wid, 0,1,3,Qt::AlignBottom);
    layout->addWidget(theAdvancedCheckBox, wid++, 0,Qt::AlignBottom);

    lineA = new QFrame;
    lineA->setFrameShape(QFrame::HLine);
    lineA->setFrameShadow(QFrame::Sunken);
    lineA->setMaximumWidth(420);
    layout->addWidget(lineA, wid++, 0, 1, 3);
    lineA->setVisible(false);

    //
    // Selection of GP kernel
    //

    theKernelLabel=new QLabel("Kernel Function");

    gpKernel = new QComboBox();
    gpKernel->addItem(tr("Matern 5/2"));
    gpKernel->addItem(tr("Matern 3/2"));
    gpKernel->addItem(tr("Radial Basis"));
    gpKernel->addItem(tr("Exponential"));
    gpKernel->setMaximumWidth(150);
    layout->addWidget(theKernelLabel, wid, 0);
    layout->addWidget(gpKernel, wid++, 1, 1,2);
    gpKernel->setCurrentIndex(0);
    theKernelLabel->setVisible(false);
    gpKernel->setVisible(false);

    //
    // Use Linear trending function
    //

    theLinearLabel=new QLabel("Add Linear Trend Function");

    theLinearCheckBox = new QCheckBox();
    layout->addWidget(theLinearLabel, wid, 0);
    layout->addWidget(theLinearCheckBox, wid++, 1);
    //theLinearLabel->setStyleSheet("color: gray");
    //theLinearCheckBox->setDisabled(1);
    theLinearLabel->setVisible(false);
    theLinearCheckBox->setVisible(false);

    //
    // Use Log transform
    //

    theLogtLabel=new QLabel("Log-space Transform of QoI");
    theLogtLabel2=new QLabel("     (check this box only when all responses are always positive)");

    theLogtCheckBox = new QCheckBox();
    layout->addWidget(theLogtLabel, wid, 0);
    layout->addWidget(theLogtLabel2, wid, 1,1,-1,Qt::AlignLeft);
    layout->addWidget(theLogtCheckBox, wid++, 1);
    theLogtLabel->setVisible(false);
    theLogtLabel2->setVisible(false);
    theLogtCheckBox->setVisible(false);


    //
    // Nugget function
    //

    theNuggetLabel=new QLabel("Nugget Variances");
    theNuggetSelection = new QComboBox();

    theNuggetSelection->addItem(tr("Optimize"),0);
    theNuggetSelection->addItem(tr("Fixed Values"),1);
    theNuggetSelection->addItem(tr("Fixed Bounds"),2);
    theNuggetSelection->setMaximumWidth(150);
    theNuggetSelection->setCurrentIndex(0);

    theNuggetVals = new QLineEdit();
    theNuggetVals->setToolTip("Provide nugget values");
    //theNuggetVals->setMaximumWidth(150);
    theNuggetMsg= new QLabel("in the log-transformed space");

    layout->addWidget(theNuggetLabel, wid, 0);
    layout->addWidget(theNuggetSelection, wid, 1);
    layout->addWidget(theNuggetMsg, wid++, 2,Qt::AlignLeft);
    layout->addWidget(theNuggetVals, wid++, 1,1,2);

    theNuggetLabel->setVisible(false);
    theNuggetSelection->setVisible(false);
    theNuggetVals->setVisible(false);
    theNuggetMsg->setVisible(false);

    connect(theNuggetSelection,SIGNAL(currentIndexChanged(int)),this,SLOT(showNuggetBox(int)));
    connect(theLogtCheckBox, &QCheckBox::toggled, this, [=](bool tog)  {
        if (tog && (theNuggetSelection->currentIndex()!=0))
            theNuggetMsg -> setVisible(true);
        else
            theNuggetMsg -> setVisible(false);
    });
    //
    // Finish
    //

    layout->setRowStretch(wid, 1);
    layout->setColumnStretch(6, 1);
    this->setLayout(layout);


    outFileDir->setDisabled(1);
    chooseOutFile->setDisabled(1);
    chooseOutFile->setStyleSheet("background-color: lightgrey;border-color:grey");

    connect(theCheckButton,SIGNAL(toggled(bool)),this,SLOT(setOutputDir(bool)));
    connect(theAdvancedCheckBox,SIGNAL(toggled(bool)),this,SLOT(doAdvancedGP(bool)));


}


SurrogateNoDoEInputWidget::~SurrogateNoDoEInputWidget()
{

}


void
SurrogateNoDoEInputWidget::showNuggetBox(int idx)
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

    if ((theLogtCheckBox->isChecked()) && (idx!=0))
        theNuggetMsg -> setVisible(true);
    else
        theNuggetMsg -> setVisible(false);


};


// SLOT function
void SurrogateNoDoEInputWidget::doAdvancedGP(bool tog)
{
    if (tog) {
        theAdvancedTitle->setStyleSheet("font-weight: bold; color: black");
    } else {
        theAdvancedTitle->setStyleSheet("font-weight: bold; color: gray");

        gpKernel->setCurrentIndex(0);
        theNuggetSelection->setCurrentIndex(0);
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
    theNuggetSelection->setVisible(tog);

}


void SurrogateNoDoEInputWidget::setOutputDir(bool tog)
{
    if (tog) {
        outFileDir->setDisabled(0);
        chooseOutFile->setDisabled(0);
        chooseOutFile->setStyleSheet("color: white");
        theFemWidget->setFEMforGP("GPdata");
        parseInputDataForRV(inpFileDir->text());
        parseOutputDataForQoI(outFileDir->text());
    } else {
        outFileDir->setDisabled(1);
        chooseOutFile->setDisabled(1);
        chooseOutFile->setStyleSheet("background-color: lightgrey;border-color:grey");
        theEdpWidget->setGPQoINames(QStringList("") );
        outFileDir->setText(QString("") );
        theFemWidget->setFEMforGP("GPmodel");
        theFemWidget->femProgramChanged("OpenSees");
        theEdpWidget->setGPQoINames(QStringList({}) );// remove GP RVs
        theParameters->setGPVarNamesAndValues(QStringList({}));// remove GP RVs
    }
}

bool
SurrogateNoDoEInputWidget::outputToJSON(QJsonObject &jsonObj){

    bool result = true;

    jsonObj["inpFile"]=inpFileDir->text();

    jsonObj["outputData"]=theCheckButton->isChecked();
    if (theCheckButton->isChecked())
    {
        jsonObj["outFile"]=outFileDir->text();
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
    jsonObj["parallelExecution"]=false;

    return result;    
}


int SurrogateNoDoEInputWidget::parseInputDataForRV(QString name1){

    double numberOfColumns=countColumn(name1);

    QStringList varNamesAndValues;
    for (int i=0;i<numberOfColumns;i++) {
        varNamesAndValues.append(QString("RV_column%1").arg(i+1));
        varNamesAndValues.append("nan");
    }
    theParameters->setGPVarNamesAndValues(varNamesAndValues);
    numSamples=0;
    return 0;
}

int SurrogateNoDoEInputWidget::parseOutputDataForQoI(QString name1){
    // get number of columns
    double numberOfColumns=countColumn(name1);
    QStringList qoiNames;
    for (int i=0;i<numberOfColumns;i++) {
        qoiNames.append(QString("QoI_column%1").arg(i+1));
    }
    theEdpWidget->setGPQoINames(qoiNames);
    return 0;
}

int SurrogateNoDoEInputWidget::countColumn(QString name1){
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
SurrogateNoDoEInputWidget::inputFromJSON(QJsonObject &jsonObject){

    bool result = false;
    if (jsonObject.contains("inpFile")) {
        QString fileDir=jsonObject["inpFile"].toString();
        inpFileDir->setText(fileDir);
        result = true;
    } else {
        return false;
    }

    if (jsonObject.contains("outputData")) {
      if (jsonObject["outputData"].toBool()) {
          theCheckButton->setChecked(true);
          QString fileDir=jsonObject["outFile"].toString();
          outFileDir->setText(fileDir);
          theFemWidget->setFEMforGP("GPdata");
      } else {
          theCheckButton->setChecked(false);
      }
      result = true;
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
     result = true;
  } else {
     return false;
  }

  return result;
}

bool
SurrogateNoDoEInputWidget::copyFiles(QString &fileDir) {
    QFile::copy(inpFileDir->text(), fileDir + QDir::separator() + "inpFile.in");
    if (theCheckButton->isChecked())
    {
        QFile::copy(outFileDir->text(), fileDir + QDir::separator() + "outFile.in");
    }
    return true;
}

void
SurrogateNoDoEInputWidget::clear(void)
{

}

int
SurrogateNoDoEInputWidget::getNumberTasks()
{
  return numSamples;
}
