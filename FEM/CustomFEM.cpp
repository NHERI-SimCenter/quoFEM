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

#include "CustomFEM.h"
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
#include <string>
#include <sstream>
#include <iostream>
//using namespace std;
#include <QGridLayout>

#include <OpenSeesParser.h>
#include <RandomVariablesContainer.h>

//#include <InputWidgetParameters.h>

CustomFEM::CustomFEM(QWidget *parent)
  : SimCenterAppWidget(parent)
{

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
    QLabel *textCustom=new QLabel();
    textCustom->setStyleSheet("font-weight: bold");

    textCustom->setText(tr("Input driver script that runs and post-processes application"));
    QSpacerItem *customSpacer = new QSpacerItem(50,10);

    QLabel *driverLabel = new QLabel();
    driverLabel->setText(tr("Driver script"));
    driverFile = new QLineEdit;
    QPushButton *chooseDriver = new QPushButton();
    chooseDriver->setText(tr("Choose"));
    connect(chooseDriver, &QPushButton::clicked, this, [=](){
              QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*)");
              if (!fileName.isEmpty()) {
                  driverFile->setText(fileName);
              }
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
        QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*)");
        if (!fileName.isEmpty()) {
            postProcessScript->setText(fileName);
        }
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
    //customLayout->addStretch();

    connect(theCustomInputNumber, SIGNAL(valueChanged(int)), this, SLOT(customInputNumberChanged(int)));
    theCustomInputNumber->setValue(0);

    this->setLayout(customLayout);

}

CustomFEM::~CustomFEM()
{

}


void CustomFEM::clear(void)
{
  QStringList names;
  for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
         names.append(varNamesAndValues.at(i));
  }

  RandomVariablesContainer *theRVs=RandomVariablesContainer::getInstance();
  theRVs->removeRandomVariables(names);
  postprocessScript->setText("");
  driverFile->setText("");
}

bool
CustomFEM::inputFromJSON(QJsonObject &jsonObject)
{
//  varNamesAndValues.clear();

//  if (jsonObject.contains("randomVariables")) {
//    QJsonArray randomVars = jsonObject["randomVariables"].toArray();
//    foreach (const QJsonValue & value, randomVars) {
//      QJsonObject theRV = value.toObject();
//      QString name = theRV["name"].toString();
//      QString zero = "0";
//      varNamesAndValues.append(name);
//      varNamesAndValues.append(zero);
//    }
//  }
    
  return true;
}

bool
CustomFEM::outputToJSON(QJsonObject &jsonObject) {

    // Add driver script and post-processing script to JSON file

  return true;
}


bool
CustomFEM::outputAppDataToJSON(QJsonObject &jsonObject) {


    bool result = true;
    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["Application"] = "customFEM";
    QJsonObject dataObj;

    QString driverScript      = driverFile->text();
    QString postProcessScript = postprocessScript->text();
    dataObj["inputFile"]          = driverScript;
    dataObj["postprocessScript"]  = postProcessScript;

    QFileInfo driverInfo(driverScript);

    dataObj["mainInput"] = driverInfo.fileName();
    QString path     = driverInfo.absolutePath();
    dataObj["dir"]       = path;

    QFileInfo postProcessInfo(postProcessScript);
    dataObj["mainPostprocessScript"] = postProcessInfo.fileName();

    // Add all additional input files to JSON
    QJsonArray apps;
    for (auto const val : customInputFiles) {
         apps.append(val->text());
    }
    dataObj["fileInfo"] = apps;


    jsonObject["ApplicationData"] = dataObj;

    return true; // needed for json file to save, copyFiles will return false
}

 bool
CustomFEM::inputAppDataFromJSON(QJsonObject &jsonObject) {
  
  //
  // from ApplicationData
  //
  
  if (jsonObject.contains("ApplicationData")) {
    QJsonObject dataObject = jsonObject["ApplicationData"].toObject();
    
    //
    // retrieve filename and path, set the QLIne Edit
    //
    
    int count = 0;
    auto fileListArray = dataObject["fileInfo"].toArray();
    auto numFiles = fileListArray.count();
    theCustomInputNumber->setValue(numFiles);
    customInputFiles.resize(numFiles);
    for (auto const& val :  fileListArray) {
        auto b = val.toString();
        //auto inputFile = new QLineEdit(b);
        //customInputFiles.append(inputFile);
        customInputFiles.at(count)->setText(b);
        count++;
    }
  }

  return true;
 }


void
CustomFEM::setMainScript(QString name1){

    return;
}

void
CustomFEM::chooseMainScript(void) {
    QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),
                          "",
						  "All files (*.tcl)");
    if(!fileName.isEmpty()) {
        this->setMainScript(fileName);
    }
}

void
CustomFEM::specialCopyMainScript(QString fileName, QStringList varNames) {
   // pass
}

bool
CustomFEM::copyFiles(QString &dirName) {

     QString fileName = driverFile->text();

     if (fileName.isEmpty()) {
         this->errorMessage("Custom - no file set");
         return false;
     }
     QFileInfo fileInfo(fileName);

     QString theFile = fileInfo.fileName();
     QString thePath = fileInfo.path();

     SimCenterAppWidget::copyPath(thePath, dirName, false);

     RandomVariablesContainer *theRVs=RandomVariablesContainer::getInstance();
     QStringList varNames = theRVs->getRandomVariableNames();

     // now create special copy of original main script that handles the RV
     OpenSeesParser theParser;
     QString copiedFile = dirName + QDir::separator() + theFile;
     theParser.writeFile(fileName, copiedFile, varNames);

     return true;
}


void CustomFEM::customInputNumberChanged(int numCustomInputs) {
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
            QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*)");
            if (!fileName.isEmpty()) {
                inputFile->setText(fileName);
            }
        });
    chooseFile->setToolTip(tr("Push to choose a file from your file system"));

    auto inputLayout = new QHBoxLayout();
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(inputFile);
    inputLayout->addWidget(chooseFile);
    newLayout->addLayout(inputLayout);
    customInputFiles.append(inputFile);
  }
  //newLayout->addStretch();

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


QVector< QString > CustomFEM::getCustomInputs() const {
   QVector< QString > stringOutput(customInputFiles.size());

   unsigned int count = 0;
   for (auto const& val : customInputFiles)
   {
      stringOutput[count] = val->text();
      count++;
   }

   return stringOutput;
}
