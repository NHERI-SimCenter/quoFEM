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

#include <iostream>
#include "InputWidgetFEM.h"
#include <QGridLayout>
#include <QComboBox>
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
#include <QFileInfo>
#include <QVectorIterator>
#include <OpenSeesPyParser.h>

#include <OpenSeesParser.h>
#include <FEAPpvParser.h>

#include <InputWidgetParameters.h>

InputWidgetFEM::InputWidgetFEM(InputWidgetParameters *param, QWidget *parent)
  : SimCenterWidget(parent), theParameters(param), numInputs(1)
{
    femSpecific = 0;

    layout = new QVBoxLayout();        
    QVBoxLayout *name= new QVBoxLayout;

    // text and add button at top
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *textFEM=new SectionTitle();
    textFEM->setText(tr("Finite Element Method Application"));
    //textFEM->setMinimumWidth(250);
    QSpacerItem *spacer = new QSpacerItem(50,10);

    femSelection = new QComboBox();
   // femSelection->setMaximumWidth(400);
    //femSelection->setMinimumWidth(200);
    femSelection->setToolTip(tr("Remote Application to Run"));
    titleLayout->addWidget(textFEM);
    titleLayout->addItem(spacer);
    titleLayout->addWidget(femSelection);
    titleLayout->addStretch();
    titleLayout->setSpacing(0);
    titleLayout->setMargin(0);

    layout->addLayout(titleLayout);

    layout->setSpacing(10);
    layout->setMargin(0);
    // name->addStretch();

    femSelection->addItem(tr("OpenSees"));
    femSelection->addItem(tr("FEAPpv"));
    femSelection->addItem(tr("OpenSeesPy"));
    femSelection->addItem(tr("Custom"));

    connect(femSelection, SIGNAL(currentIndexChanged(QString)), this, SLOT(femProgramChanged(QString)));

    layout->addLayout(name);
    this->femProgramChanged(tr("OpenSees"));

    // layout->addStretch();
    // layout->setSpacing(10);
    layout->setMargin(10);
    layout->addStretch();

    this->setLayout(layout);

}

InputWidgetFEM::~InputWidgetFEM()
{
}


void InputWidgetFEM::clear(void)
{

}



bool
InputWidgetFEM::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject fem;
    QJsonArray apps;
    fem["program"]=femSelection->currentText();
    fem["numInputs"]=numInputs;

    if (femSelection->currentText() == "Custom") {
       // Add driver script and post-processing script to JSON file
       QString driverScript      = inputFilenames.at(0)->text();
       QString postProcessScript = postprocessFilenames.at(0)->text();
       fem["inputFile"]          = driverScript;
       fem["postprocessScript"]  = postProcessScript;

       QFileInfo driverInfo(driverScript);

       fem["mainInput"] = driverInfo.fileName();
       QString path     = driverInfo.absolutePath();
       fem["dir"]       = path;

       QFileInfo postProcessInfo(postProcessScript);
       fem["mainPostprocessScript"] = postProcessInfo.fileName();

       // Add all additional input files to JSON
       for (auto const val : customInputFiles) {
	 apps.append(val->text());
       }
       fem["fileInfo"] = apps;
       
    } else if (numInputs == 1) {
        QString fileName1=inputFilenames.at(0)->text();
        QString fileName2=postprocessFilenames.at(0)->text();

        fem["inputFile"]=fileName1;
        fem["postprocessScript"]=fileName2;

        QFileInfo fileInfo(fileName1);

        fem["mainInput"]=fileInfo.fileName();
        QString path = fileInfo.absolutePath();
        fem["dir"]=path;

        QFileInfo fileInfo2(fileName2);
        fem["mainPostprocessScript"]=fileInfo2.fileName();

        if (femSelection->currentText() == "OpenSeesPy") {

             QString fileName3=parametersFilenames.at(0)->text();
             QFileInfo pFileInfo(fileName3);
             fem["parametersFile"]=pFileInfo.fileName();
             fem["parametersScript"]=fileName3;
        }

    } else {
        for (int i=0; i<numInputs; i++) {
            QJsonObject obj;
            QString fileName1=inputFilenames.at(i)->text();
            QString fileName2=postprocessFilenames.at(i)->text();

            obj["inputFile"]=fileName1;
            obj["postprocessScript"]=fileName2;

            QFileInfo fileInfo(fileName1);

            obj["mainInput"]=fileInfo.fileName();
            QString path = fileInfo.absolutePath();
            obj["dir"]=path;

            QFileInfo fileInfo2(fileName2);
            obj["mainPostprocessScript"]=fileInfo2.fileName();
            apps.append(obj);


        }
        fem["fileInfo"] = apps;
    }

    jsonObject["fem"]=fem;

    return true;
}


bool
InputWidgetFEM::inputFromJSON(QJsonObject &jsonObject)
{
    this->clear();

    if (jsonObject.contains("fem")) {

        QJsonObject femObject = jsonObject["fem"].toObject();

        QString program=femObject["program"].toString();

        int index = femSelection->findText(program);
        femSelection->setCurrentIndex(index);
        this->femProgramChanged(program);

        numInputs = 1;
        if (femObject.contains("numInputs") )
            numInputs = femObject["numInputs"].toInt();

        if (numInputs == 1) {	  
            QString fileName1=femObject["inputFile"].toString();;
            QString fileName2=femObject["postprocessScript"].toString();

            inputFilenames.at(0)->setText(fileName1);
            postprocessFilenames.at(0)->setText(fileName2);

            if (femSelection->currentText() == "OpenSeesPy") {
                QString fileName3=femObject["parametersScript"].toString();
                parametersFilenames.at(0)->setText(fileName3);
            }

	    if (femSelection->currentText() == "Custom") {
	      // Check how many input files
	      customInputFiles.resize(femObject["fileInfo"].toArray().count());

	      int count = 0;
	      for (auto const& val : femObject["fileInfo"].toArray()) {
		customInputFiles.at(count)->setText(val.toString());
		count++;
	      }
	    } 	    

            this->parseInputfilesForRV(fileName1);
        }
        // call setFilename1 so parser works on input file

    } else {
        emit sendErrorMessage("ERROR: FEM Input - no fem section in input file");
        return false;
    }
    return true;
}

void InputWidgetFEM::femProgramChanged(const QString &arg1)
{
    //
    // remove old widget and clear file names
    //
    if (femSpecific != nullptr) {      
      layout->removeWidget(femSpecific);
    }
    
    inputFilenames.clear();
    postprocessFilenames.clear();
    parametersFilenames.clear();
    customInputFiles.clear();

    auto newFemSpecific = new QWidget();
    auto oldFemSpecific = femSpecific;

    if (arg1 == QString("Custom")) {
      theCustomInputNumber = new QSpinBox();
      theCustomLayout = new QVBoxLayout();
      theCustomFileInputs = new QVBoxLayout();
      theCustomFileInputWidget = new QWidget();      

      QLabel *customNumberLabel = new QLabel();
      customNumberLabel->setText(tr("Number of input files and executables required by driver script"));      
      QHBoxLayout *numberLayout = new QHBoxLayout();
      numberLayout->addWidget(customNumberLabel);
      numberLayout->addWidget(theCustomInputNumber);
      numberLayout->addStretch();
      
      theCustomLayout->addLayout(numberLayout);

      theCustomFileInputs = new QVBoxLayout();
      theCustomFileInputWidget = new QWidget();
      theCustomFileInputWidget->setLayout(theCustomFileInputs);
      theCustomLayout->addWidget(theCustomFileInputWidget);
           
      QVBoxLayout *customLayout = new QVBoxLayout();      

      // Add location to add driver script
      SectionTitle *textCustom=new SectionTitle();
      textCustom->setText(tr("Input driver script that runs and post-processes application"));
      QSpacerItem *customSpacer = new QSpacerItem(50,10);
      
      QLabel *driverLabel = new QLabel();
      driverLabel->setText(tr("Driver script"));
      QLineEdit *driverFile = new QLineEdit;
      QPushButton *chooseDriver = new QPushButton();
      chooseDriver->setText(tr("Choose"));
      connect(chooseDriver, &QPushButton::clicked, this, [=](){
                driverFile->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
            });      
      chooseDriver->setToolTip(tr("Push to choose a file from your file system"));
      
      QGridLayout *driverLayout = new QGridLayout();
      driverLayout->addWidget(driverLabel, 1, 0);
      driverLayout->addWidget(driverFile, 1, 1);
      driverLayout->addWidget(chooseDriver, 1, 2);
      driverLayout->setColumnStretch(3,1);
      driverLayout->setColumnStretch(1,3);

      // Add location to add post-processing script
      QLabel *postProcessingLabel = new QLabel();
      postProcessingLabel->setText("Postprocessing script");
      QLineEdit *postProcessScript = new QLineEdit;
      QPushButton *choosePostProcessing = new QPushButton();
      
      connect(choosePostProcessing, &QPushButton::clicked, this, [=](){
            postProcessScript->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
							});
      choosePostProcessing->setText(tr("Choose"));
      choosePostProcessing->setToolTip(tr("Push to choose a file from your file system"));

      QGridLayout *postProcessLayout = new QGridLayout();
      postProcessLayout->addWidget(postProcessingLabel, 1, 0);
      postProcessLayout->addWidget(postProcessScript, 1, 1);
      postProcessLayout->addWidget(choosePostProcessing, 1, 2);
      postProcessLayout->setColumnStretch(3,1);
      postProcessLayout->setColumnStretch(1,3);      

      // Add custom layout for specifying additional inputs
      customLayout->addWidget(textCustom);
      customLayout->addItem(customSpacer);
      customLayout->addLayout(driverLayout);
      customLayout->addLayout(postProcessLayout);
      customLayout->addLayout(theCustomLayout);
      customLayout->addStretch();

      connect(theCustomInputNumber, SIGNAL(valueChanged(int)), this, SLOT(customInputNumberChanged(int)));      
      theCustomInputNumber->setValue(0);                
      inputFilenames.append(driverFile);
      postprocessFilenames.append(postProcessScript);
      newFemSpecific->setLayout(customLayout);
      
    } else if (numInputs == 1) {

        QGridLayout *femLayout = new QGridLayout();

        QLabel *label1 = new QLabel();
        QLineEdit *file1 = new QLineEdit;
        QPushButton *chooseFile1 = new QPushButton();
        chooseFile1->setText(tr("Choose"));
         if ((arg1 == QString("FEAPpv")) || (arg1 == QString("OpenSees"))){
            connect(chooseFile1, &QPushButton::clicked, this, [=](){
                file1->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
                this->parseInputfilesForRV(file1->text());
            });
         } else {
             connect(chooseFile1, &QPushButton::clicked, this, [=](){
                 file1->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
                 // this->parseInputfilesForRV(file1->text());
             });
         }

        chooseFile1->setToolTip(tr("Push to choose a file from your file system"));

        femLayout->addWidget(label1, 0,0);
        femLayout->addWidget(file1,  0,1);
        femLayout->addWidget(chooseFile1, 0, 2);

        QLabel *label2 = new QLabel();
        label2->setText("Postprocess Script");
        QLineEdit *file2 = new QLineEdit;
        QPushButton *chooseFile2 = new QPushButton();

        connect(chooseFile2, &QPushButton::clicked, this, [=](){
            file2->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
        });
         chooseFile2->setText(tr("Choose"));
        chooseFile2->setToolTip(tr("Push to choose a file from your file system"));

        if (arg1 == QString("FEAPpv")){
            label1->setText("Input File");
            file1->setToolTip(tr("Name of FEAPpv input file"));
            file2->setToolTip(tr("Name of Python script that will process FEAPpv output file for UQ engine"));
        } else if (arg1 == "OpenSees") {
            label1->setText("Input Script");
            file1->setToolTip(tr("Name of OpenSees input script"));
            file2->setToolTip(tr("Name of Python/Tcl script that will process OpenSees output file for UQ engine"));
        } else {
            label1->setText("Input Script");
            file1->setToolTip(tr("Name of OpenSeesPy input script"));
            file2->setToolTip(tr("Name of Python script that will process OpenSeesPy output"));
        }

        femLayout->addWidget(label2, 1,0);
        femLayout->addWidget(file2, 1,1);
        femLayout->addWidget(chooseFile2, 1,2);


        if (arg1 == "OpenSeesPy") {
            QLabel *label3 = new QLabel();
            label3->setText("Parameters File");
            QLineEdit *file3 = new QLineEdit;
            file2->setToolTip(tr("Name of Python script that contains defined parameters"));
            QPushButton *chooseFile3 = new QPushButton();

            connect(chooseFile3, &QPushButton::clicked, this, [=](){
                file3->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
                this->parseInputfilesForRV(file3->text());
            });
            chooseFile3->setText(tr("Choose"));
            chooseFile3->setToolTip(tr("Push to choose a file from your file system"));

            parametersFilenames.append(file3);

            femLayout->addWidget(label3, 2,0);
            femLayout->addWidget(file3, 2,1);
            femLayout->addWidget(chooseFile3, 2,2);

            femLayout->setColumnStretch(3,1);
            femLayout->setColumnStretch(1,3);

        } else {
            femLayout->setColumnStretch(3,1);
            femLayout->setColumnStretch(1,3);
        }


        inputFilenames.append(file1);
        postprocessFilenames.append(file2);
        newFemSpecific->setLayout(femLayout);

    } else {

        QVBoxLayout *multiLayout = new QVBoxLayout();
        newFemSpecific->setLayout(multiLayout);

        for (int i=0; i< numInputs; i++) {
            QGridLayout *femLayout = new QGridLayout();
            QLabel *label1 = new QLabel();

            QLabel *label2 = new QLabel();
            label2->setText("Postprocess Script");

            QLineEdit *file1 = new QLineEdit;
            QPushButton *chooseFile1 = new QPushButton();
            chooseFile1->setText(tr("Choose"));
            connect(chooseFile1, &QPushButton::clicked, this, [=](){
                file1->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
                this->parseInputfilesForRV(file1->text());
            });

            if (arg1 == QString("FEAPpv")){
                label1->setText("Input File");
                file1->setToolTip(tr("Name of FEAPpv input file"));
            } else {
                label1->setText("Input Script");
                file1->setToolTip(tr("Name of OpenSees input script"));
            }

            chooseFile1->setToolTip(tr("Push to choose a file from your file system"));

            QLineEdit *file2 = new QLineEdit;
            QPushButton *chooseFile2 = new QPushButton();

            connect(chooseFile2, &QPushButton::clicked, this, [=](){
                file2->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
            });

            file2->setToolTip(tr("Name of Python script that will process FEAPpv output file for UQ engine"));
            chooseFile2->setToolTip(tr("Push to choose a file from your file system"));

            chooseFile2->setText(tr("Choose"));

            femLayout->addWidget(label1, 0,0);
            femLayout->addWidget(file1,  0,1);
            femLayout->addWidget(chooseFile1, 0, 2);
            femLayout->addWidget(label2, 1,0);
            femLayout->addWidget(file2, 1,1);
            femLayout->addWidget(chooseFile2, 1,2);

            femLayout->setColumnStretch(3,1);
            femLayout->setColumnStretch(1,3);

            inputFilenames.append(file1);
            postprocessFilenames.append(file2);
            multiLayout->addLayout(femLayout);

        }
        newFemSpecific->setLayout(multiLayout);
    }

    layout->insertWidget(1, newFemSpecific);
    femSpecific = newFemSpecific;
    oldFemSpecific->deleteLater();
}

int InputWidgetFEM::parseInputfilesForRV(QString name1){
    QString fileName1 = name1;
    //  file1->setText(name1);

    // if OpenSees or FEAP parse the file for the variables
    QString pName = femSelection->currentText();
    if (pName == "OpenSees" || pName == "OpenSees-2") {
        OpenSeesParser theParser;
        varNamesAndValues = theParser.getVariables(fileName1);
    }  else if (pName == "FEAPpv") {
        FEAPpvParser theParser;
        varNamesAndValues = theParser.getVariables(fileName1);
    } else if (pName == "OpenSeesPy") {
        OpenSeesPyParser theParser;
        varNamesAndValues = theParser.getVariables(fileName1);
    } else if (pName == "Custom") {
      // No need to do anything here for custom
    }
      
    // qDebug() << "VARNAMESANDVALUES: " << varNamesAndValues;
    if (pName != "Custom") {
      theParameters->setInitialVarNamesAndValues(varNamesAndValues);      
    }

    return 0;
}

void
InputWidgetFEM::numModelsChanged(int newNum) {
    numInputs = newNum;
    this->femProgramChanged(femSelection->currentText());
}

void InputWidgetFEM::customInputNumberChanged(int numCustomInputs) {
  // numInputs = numCustomInputs;
  auto tempLayout = theCustomFileInputs;

  // Delete old inputs
  QLayoutItem * item;
  while ((item = tempLayout->takeAt(0)) != nullptr) {
    if (item->layout() != nullptr) {
      auto sublayout = item->layout();
      while (sublayout->takeAt(0) != nullptr) {
  	if (sublayout->takeAt(0)->widget() != nullptr) {
  	  auto widget = sublayout->takeAt(0)->widget();
  	  sublayout->removeWidget(widget);
  	  widget->deleteLater();
  	}
      }
      tempLayout->removeItem(item);
      item->layout()->deleteLater();
    } else if (item->widget() != nullptr) {
      tempLayout->removeItem(item);
      item->widget()->deleteLater();
    }
  }

  tempLayout->deleteLater();
  tempLayout = nullptr;

  
  // Create new file inputs
  QVBoxLayout * newLayout = new QVBoxLayout();
  customInputFiles.clear();
  for (int i = 0; i < numCustomInputs; ++i) {
    auto inputLabel = new QLabel();
    auto inputFile = new QLineEdit();
    inputLabel->setText(tr("Input location"));
    auto *chooseFile = new QPushButton();
    chooseFile->setText(tr("Choose"));
    connect(chooseFile, &QPushButton::clicked, this, [=](){
                inputFile->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
            });      
    chooseFile->setToolTip(tr("Push to choose a file from your file system"));
    
    auto inputLayout = new QHBoxLayout();
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(inputFile);
    inputLayout->addWidget(chooseFile);
    newLayout->addLayout(inputLayout);
    customInputFiles.append(inputFile);    
  }
  newLayout->addStretch();  

  // Replace outdated widget with new one containing new file input layout
  theCustomFileInputs = newLayout;
  theCustomFileInputWidget->layout()->deleteLater();

  theCustomLayout->removeWidget(theCustomFileInputWidget);  
  theCustomFileInputWidget->deleteLater();

  theCustomFileInputWidget = new QWidget();
  theCustomFileInputWidget->setLayout(theCustomFileInputs);
  theCustomLayout->addWidget(theCustomFileInputWidget);
    
  newLayout = nullptr;
}

/*
void InputWidgetFEM::chooseFileName1(void)
{
    QString fileName1=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
    int ok = this->parseInputfilesForRV(fileName1);
}
*/
void
InputWidgetFEM::specialCopyMainInput(QString fileName, QStringList varNames) {
    // if OpenSees or FEAP parse the file for the variables
    if (varNames.size() > 0) {
        QString pName = femSelection->currentText();
        if (pName == "OpenSees" || pName == "OpenSees-2") {
            OpenSeesParser theParser;
            QVectorIterator<QLineEdit *> i(inputFilenames);
            while (i.hasNext())
                theParser.writeFile(i.next()->text(), fileName,varNames);
        }  else if (pName == "FEAPpv") {
            FEAPpvParser theParser;
            QVectorIterator<QLineEdit *> i(inputFilenames);
            while (i.hasNext())
                theParser.writeFile(i.next()->text(), fileName,varNames);

        }  else if (pName == "OpenSeesPy") {
            // do things a little different!
            QFileInfo file(fileName);
            QString fileDir = file.absolutePath();
            //qDebug() << "FILENAME: " << fileName << " path: " << fileDir;
            //qDebug() << "LENGTHS:" << inputFilenames.count() << parametersFilenames.count() << parametersFilenames.length();
            OpenSeesPyParser theParser;
            bool hasParams = false;
            QVectorIterator<QLineEdit *> i(parametersFilenames);
            QString newName = fileDir + QDir::separator() + "tmpSimCenter.params";
            while (i.hasNext()) {
                QString fileName = i.next()->text();
                if (fileName.length() != 0) {
                    theParser.writeFile(fileName, newName,varNames);
                    hasParams = true;
                }
            }

            if (hasParams == false) {
                QString newName = fileDir + QDir::separator() + "tmpSimCenter.script";
                QVectorIterator<QLineEdit *> i(inputFilenames);
                while (i.hasNext()) {
                    QFile::copy(i.next()->text(), newName);
                }
            }
        } else {
	  std::cout << "\nWILL BE ADDING SOMETHING HERE!!!\n";
	}
    }
}



QString InputWidgetFEM::getApplicationName() {
    return femSelection->currentText();
}


QString InputWidgetFEM::getMainInput() {

  QVectorIterator<QLineEdit *> i(inputFilenames);
  std::cout << "\nINPUT FILES:\n";
  while (i.hasNext()) {
    std::cout << (i.next()->text()).toUtf8().constData() << std::endl;    
  }

  std::cout << "\nINPUT FILES:\n";  
  QVectorIterator<QLineEdit *> j(customInputFiles);  
  while (j.hasNext()) {
    std::cout << (j.next()->text()).toUtf8().constData() << std::endl;    
  }
  
  if (inputFilenames.length() != 0) {
      std::cout << "\nMAIN INPUT: " << (inputFilenames.at(0)->text()).toUtf8().constData() << std::endl;
        return inputFilenames.at(0)->text();    
  } else
        return(QString(""));
}

