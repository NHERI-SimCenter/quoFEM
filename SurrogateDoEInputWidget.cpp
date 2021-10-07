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

#include <SurrogateDoEInputWidget.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <QRadioButton>
#include <QSpinBox>

SurrogateDoEInputWidget::SurrogateDoEInputWidget(QWidget *parent)
: UQ_MethodInputWidget(parent)
{
    auto layout = new QGridLayout();

    //
    // Random Seed
    //
    int wid = 0; // widget id

    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Set the seed");
    randomSeed->setMaximumWidth(150);

    layout->addWidget(new QLabel("Random Seed"), wid, 0);
    layout->addWidget(randomSeed, wid++, 1);

    //
    // create convergence criteria
    //

    accuracyMeasure = new QLineEdit();
    accuracyMeasure->setText(tr("0.02"));
    accuracyMeasure->setValidator(new QDoubleValidator);
    accuracyMeasure->setToolTip("NRMSE: normalized root mean square error");
    accuracyMeasure->setMaximumWidth(150);

    layout->addWidget(new QLabel("Target Accuracy (Normalized Err) "), wid, 0);
    layout->addWidget(accuracyMeasure, wid++, 1);

    //
    // create layout label and entry for # samples
    //

    numSamples = new QLineEdit();
    numSamples->setText(tr("150"));
    numSamples->setValidator(new QIntValidator);
    numSamples->setToolTip("Specify the number of samples");
    numSamples->setMaximumWidth(150);

    layout->addWidget(new QLabel("Max Number of Model Runs"), wid, 0);
    layout->addWidget(numSamples, wid++, 1);

    //
    // Max computation time (approximate)
    //

    timeMeasure = new QLineEdit();
    timeMeasure->setText(tr("60"));
    timeMeasure->setValidator(new QIntValidator);
    timeMeasure->setToolTip("Max Computation Time (minutes)");
    timeMeasure->setMaximumWidth(150);
    timeMeasure->setMinimumWidth(150);

    layout->addWidget(new QLabel("Max Computation Time (minutes)    "), wid, 0);
    layout->addWidget(timeMeasure, wid++, 1);

    //
    // Parallel Execution
    //

    parallelCheckBox = new QCheckBox();
    parallelCheckBox->setChecked(true);
    layout->addWidget(new QLabel("Parallel Execution"), wid, 0);
    layout->addWidget(parallelCheckBox, wid++, 1);

    //
    // Advanced options
    //

    theAdvancedCheckBox = new QCheckBox();
    theAdvancedTitle=new QLabel("\n     Advanced Options for Gaussian Process Model ");
    theAdvancedTitle->setStyleSheet("font-weight: bold; color: grey");
    layout->addWidget(theAdvancedTitle, wid, 0, 1, 3,Qt::AlignBottom);
    layout->addWidget(theAdvancedCheckBox, wid++, 0, 1, 3, Qt::AlignBottom);

    lineA= new QFrame;
    lineA->setFrameShape(QFrame::HLine);
    lineA->setFrameShadow(QFrame::Sunken);
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
    gpKernel->setCurrentIndex(0);

    layout->addWidget(theKernelLabel, wid, 0);
    layout->addWidget(gpKernel, wid++, 1);
    theKernelLabel->setVisible(false);
    gpKernel->setVisible(false);

    //
    // Use Linear trending function
    //

    theLinearLabel=new QLabel("Add Linear Trend Function");

    theLinearCheckBox = new QCheckBox();
    theLinearCheckBox->setToolTip("Default is no trending function");

    layout->addWidget(theLinearLabel, wid, 0);
    layout->addWidget(theLinearCheckBox, wid++, 1);
    theLinearLabel->setVisible(false);
    theLinearCheckBox->setVisible(false);

    //
    // Use Log transform
    //

    //theLogtLabel=new QLabel("Responses are always positive");
    //theLogtLabel2=new QLabel("     (allow log-transform)");
    theLogtLabel=new QLabel("Log-space Transform of QoI");
    theLogtLabel2=new QLabel("      (check this box only when all responses are always positive)");

    theLogtCheckBox = new QCheckBox();
    layout->addWidget(theLogtLabel, wid, 0);
    layout->addWidget(theLogtLabel2, wid, 1,1,-1);
    layout->addWidget(theLogtCheckBox, wid++, 1);
    theLogtLabel->setVisible(false);
    theLogtLabel2->setVisible(false);
    theLogtCheckBox->setVisible(false);

    //
    // # of Initial DoE (Space filling)
    //

//    theInitialLabel=new QLabel("Number of Initial Samples (DoE)");

//    initialDoE = new QLineEdit();
//    initialDoE->setValidator(new QIntValidator);
//    initialDoE->setToolTip("Set the number of initial DoE (Space filling)");
//    initialDoE->setPlaceholderText("(Optional)");
//    initialDoE->setMaximumWidth(150);
//    layout->addWidget(theInitialLabel, wid, 0);
//    layout->addWidget(initialDoE, wid++, 1);
//    initialDoE->setVisible(false);
//    theInitialLabel->setVisible(false);

    //
    // DoE Options
    //

    theDoELabel=new QLabel("DoE Options");
    theDoESelection = new QComboBox();

    theDoESelection->addItem(tr("Pareto"),0);
    theDoESelection->addItem(tr("IMSEw"),1);
    theDoESelection->addItem(tr("MMSEw"),2);
    theDoESelection->addItem(tr("None"),3);
    theDoESelection->setMaximumWidth(150);
    theDoESelection->setCurrentIndex(0);

    //theDoEMsg= new QLabel("Provide the number of initial samples (DoE)");
    //theDoEMsg= new QLabel("");
    initialDoE = new QLineEdit();
    initialDoE->setToolTip("Provide the number of initial samples");
    initialDoE->setPlaceholderText("(Optional) Initial DoE #");
    initialDoE->setMaximumWidth(150);
    initialDoE->setMinimumWidth(150);

    layout->addWidget(theDoELabel, wid, 0);
    layout->addWidget(theDoESelection, wid, 1);
    //layout->addWidget(theDoEMsg, wid, 2);
    layout->addWidget(initialDoE, wid++, 2,1,3);

    theDoELabel->setVisible(false);
    theDoESelection->setVisible(false);
    //theDoEMsg->setVisible(false);
    initialDoE->setVisible(false);
    connect(theDoESelection,SIGNAL(currentIndexChanged(int)),this,SLOT(showDoEBox(int)));

    //
    // Nugget function
    //


    theNuggetLabel=new QLabel("Nugget Variances");
    theNuggetSelection = new QComboBox();

    theNuggetSelection->addItem(tr("Optimize"),0);
    theNuggetSelection->addItem(tr("Fixed Values"),1);
    theNuggetSelection->addItem(tr("Fixed Bounds"),2);
    theNuggetSelection->addItem(tr("Zero"),3);

    theNuggetSelection->setMaximumWidth(150);
    theNuggetSelection->setCurrentIndex(0);

    theNuggetVals = new QLineEdit();
    theNuggetVals->setToolTip("Provide nugget values");
    //theNuggetVals->setMaximumWidth(150);
    theNuggetMsg= new QLabel("in the log-transformed space");

    layout->addWidget(theNuggetLabel, wid, 0);
    layout->addWidget(theNuggetSelection, wid, 1);
    layout->addWidget(theNuggetMsg, wid++, 2);
    layout->addWidget(theNuggetVals, wid++, 1,1,3);

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
    // Use Existing Initial DoE
    //

    theExistingCheckBox = new QCheckBox();
    theExistingTitle = new QLabel("\n     Start with Existing Dataset");
    theExistingTitle ->setStyleSheet("font-weight: bold; color: grey");
    layout->addWidget(theExistingTitle, wid, 0, 1, 2, Qt::AlignBottom);
    layout->addWidget(theExistingCheckBox, wid++, 0, Qt::AlignBottom);

    lineB = new QFrame;
    lineB->setFrameShape(QFrame::HLine);
    lineB->setFrameShadow(QFrame::Sunken);
    layout->addWidget(lineB, wid++, 0, 1, 3);
    lineB->setVisible(false);

    //
    // Input data
    //

    inpFileDir = new QLineEdit();
    chooseInpFile = new QPushButton("Choose");
    connect(chooseInpFile, &QPushButton::clicked, this, [=](){
        QString fileName = QFileDialog::getOpenFileName(this, "Open Simulation Model", "", "All files (*.*)");
        inpFileDir->setText(fileName);
        this->checkValidityData(fileName);
        setWindowFilePath(fileName);
    });
    inpFileDir->setMaximumWidth(150);
    theInputLabel=new QLabel("Training Points (Input RV)");
    layout->addWidget(theInputLabel,wid,0);
    layout->addWidget(inpFileDir,wid,1);
    layout->addWidget(chooseInpFile,wid++,2,Qt::AlignLeft);
    theInputLabel->setVisible(false);
    inpFileDir->setVisible(false);
    chooseInpFile->setVisible(false);
    //
    // Output data
    //

    outFileDir = new QLineEdit();
    chooseOutFile = new QPushButton("Choose");
    connect(chooseOutFile, &QPushButton::clicked, this, [=](){
        outFileDir->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
        this->checkValidityData(outFileDir->text());
    });
    outFileDir->setMaximumWidth(150);
    theOutputLabel = new QLabel("System Responses (Output QoI)");
    layout->addWidget(theOutputLabel,wid,0,Qt::AlignTop);
    layout->addWidget(outFileDir,wid,1,Qt::AlignTop);
    layout->addWidget(chooseOutFile,wid++,2,Qt::AlignLeft);
    theOutputLabel->setVisible(false);
    outFileDir->setVisible(false);
    chooseOutFile->setVisible(false);

    errMSG=new QLabel("Unrecognized file format");
    errMSG->setStyleSheet({"color: red"});
    layout->addWidget(errMSG,wid++,1,1,2,Qt::AlignLeft);
    errMSG->hide();

    //
    // Finish
    //

    layout->setRowStretch(wid, 1);
    layout->setColumnStretch(5, 1);
    this->setLayout(layout);
    connect(theAdvancedCheckBox,SIGNAL(toggled(bool)),this,SLOT(doAdvancedGP(bool)));
    connect(theExistingCheckBox,SIGNAL(toggled(bool)),this,SLOT(doExistingGP(bool)));

}

SurrogateDoEInputWidget::~SurrogateDoEInputWidget()
{

}

void
SurrogateDoEInputWidget::showNuggetBox(int idx)
{
    theNuggetVals->clear();
    if (idx == 0) {
        theNuggetVals->hide();
    } else if (idx==1){
        theNuggetVals->show();
        theNuggetVals->setPlaceholderText("QoI₁, QoI₂,..");
    } else if (idx==2) {
        theNuggetVals->show();
        theNuggetVals->setPlaceholderText("[QoI₁ˡᵇ,QoI₁ᵘᵇ], [QoI₂ˡᵇ,QoI₂ᵘᵇ],..");
    }
    if ((theLogtCheckBox->isChecked()) && (idx!=0))
        theNuggetMsg -> setVisible(true);
    else
        theNuggetMsg -> setVisible(false);
};


void
SurrogateDoEInputWidget::showDoEBox(int idx)
{
    initialDoE->clear();
    if (idx == 3) {
        initialDoE->hide();
    } else {
        initialDoE->show();
    }
};

// SLOT function
void SurrogateDoEInputWidget::doAdvancedGP(bool tog)
{
    if (tog) {
        theAdvancedTitle->setStyleSheet("font-weight: bold; color: black");

    } else {
        theAdvancedTitle->setStyleSheet("font-weight: bold; color: grey");

        gpKernel->setCurrentIndex(0);
        theNuggetSelection->setCurrentIndex(0);
        theLinearCheckBox->setChecked(false);
        theLogtCheckBox->setChecked(false);
        initialDoE-> setText("");
    }

        lineA->setVisible(tog);
        gpKernel-> setVisible(tog);
        theLinearCheckBox-> setVisible(tog);
        theLogtCheckBox-> setVisible(tog);
        initialDoE-> setVisible(tog);
        theLinearLabel->setVisible(tog);
        theLogtLabel->setVisible(tog);
        theLogtLabel2->setVisible(tog);
        theKernelLabel->setVisible(tog);
        //theInitialLabel->setVisible(tog);
        theNuggetLabel->setVisible(tog);
        theNuggetSelection->setVisible(tog);
        theDoELabel->setVisible(tog);
        theDoESelection->setVisible(tog);
}

void SurrogateDoEInputWidget::doExistingGP(bool tog)
{
    if (tog) {
        theExistingTitle->setStyleSheet("font-weight: bold; color: black");
    } else {
        theExistingTitle->setStyleSheet("font-weight: bold; color: grey");
        inpFileDir-> setText("");
        outFileDir-> setText("");
    }
        lineB->setVisible(tog);
        inpFileDir->setVisible(tog);
        outFileDir->setVisible(tog);
        theInputLabel->setVisible(tog);
        theOutputLabel->setVisible(tog);
        chooseInpFile->setVisible(tog);
        chooseOutFile->setVisible(tog);
}

void
SurrogateDoEInputWidget::checkValidityData(QString name1){

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
}

bool
SurrogateDoEInputWidget::outputToJSON(QJsonObject &jsonObj){

    bool result = true;

    jsonObj["samples"]=numSamples->text().toInt();
    jsonObj["seed"]=randomSeed->text().toInt();
    jsonObj["timeLimit"]=timeMeasure->text().toDouble();
    jsonObj["accuracyLimit"]=accuracyMeasure->text().toDouble();
    jsonObj["parallelExecution"]=parallelCheckBox->isChecked();

    jsonObj["advancedOpt"]=theAdvancedCheckBox->isChecked();
    if (theAdvancedCheckBox->isChecked())
    {
        jsonObj["advancedOpt"]=true;
        jsonObj["kernel"]=gpKernel->currentText();
        jsonObj["DoEmethod"]=theDoESelection->currentText();
        jsonObj["initialDoE"]=initialDoE->text().toDouble();
        jsonObj["linear"]=theLinearCheckBox->isChecked();
        jsonObj["logTransform"]=theLogtCheckBox->isChecked();
        jsonObj["nuggetOpt"]=theNuggetSelection->currentText();
        jsonObj["nuggetString"]=theNuggetVals->text();

    }

    jsonObj["existingDoE"]=theExistingCheckBox->isChecked();
    if (theExistingCheckBox->isChecked())
    {
        jsonObj["inpFile"]=inpFileDir->text();
        jsonObj["outFile"]=outFileDir->text();
    }
    return result;    
}

bool
SurrogateDoEInputWidget::inputFromJSON(QJsonObject &jsonObject){

    bool result = true;

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

    if (jsonObject.contains("parallelExecution")) {
        parallelCheckBox->setChecked(jsonObject["parallelExecution"].toBool());
    } else {
        parallelCheckBox->setChecked(true); // for compatibility. later change it to error. (sy - june 2021)
    }

    if (jsonObject.contains("advancedOpt")) {
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
            double accuracy=jsonObject["initialDoE"].toDouble();
            theDoESelection -> setCurrentText(jsonObject["DoEmethod"].toString());
            initialDoE->setText(QString::number(accuracy));

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
                theNuggetSelection->setCurrentIndex(index);
            }
        } else {
            theAdvancedCheckBox->setChecked(false);
            // for compatibility. Change to give an error later
        }
    } else {
    result = false;
    }

    if (jsonObject.contains("existingDoE")) {
        if (jsonObject["existingDoE"].toBool()) {
            theExistingCheckBox->setChecked(true);
            inpFileDir -> setText(jsonObject["inpFile"].toString());
            outFileDir -> setText(jsonObject["outFile"].toString());
        } else {
            theExistingCheckBox->setChecked(false);
        }
    } else {
        result = false;
    }
    return result;
}

void
SurrogateDoEInputWidget::clear(void)
{

}

bool
SurrogateDoEInputWidget::copyFiles(QString &fileDir) {
    if (theExistingCheckBox->isChecked())
    {
        QFile::copy(inpFileDir->text(), fileDir + QDir::separator() + "inpFile.in");
        QFile::copy(outFileDir->text(), fileDir + QDir::separator() + "outFile.in");
    }
    return true;
}

int
SurrogateDoEInputWidget::getNumberTasks()
{
  return numSamples->text().toInt();
}
