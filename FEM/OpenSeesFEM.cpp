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

#include "OpenSeesFEM.h"
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

OpenSeesFEM::OpenSeesFEM(QWidget *parent)
  : SimCenterAppWidget(parent)
{

    QGridLayout *layout = new QGridLayout();

    QLabel *label1 = new QLabel("Input Script");
    inputScript = new QLineEdit;
    QPushButton *chooseMS_Button = new QPushButton();
    chooseMS_Button->setText(tr("Choose"));
    connect(chooseMS_Button,SIGNAL(clicked()),this,SLOT(chooseMainScript()));
    layout->addWidget(label1,0,0);
    layout->addWidget(inputScript,0,1);
    layout->addWidget(chooseMS_Button,0,2);

    QLabel *label2 = new QLabel("Postprocess Script");
    postprocessScript = new QLineEdit;
    postprocessScript->setPlaceholderText("(Optional)");
    QPushButton *choosePostprocessScript = new QPushButton();
    choosePostprocessScript->setText(tr("Choose"));
    connect(choosePostprocessScript, &QPushButton::clicked, this, [this](){
      QString selectedFile = QFileDialog::getOpenFileName(this,
							  tr("Postprocess Script"),
							  "C://",
							  "All files (*)");

        if(!selectedFile.isEmpty()) {
            postprocessScript->setText(selectedFile);
        }
    });

    layout->addWidget(label2,1,0);
    layout->addWidget(postprocessScript,1,1);
    layout->addWidget(choosePostprocessScript,1,2);    
    layout->setRowStretch(2,1.0);
    layout->setColumnStretch(1,3);
    layout->setColumnStretch(3,1);        
    this->setLayout(layout);
}

OpenSeesFEM::~OpenSeesFEM()
{

}


void OpenSeesFEM::clear(void)
{
  QStringList names;
  for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
         names.append(varNamesAndValues.at(i));
  }

  RandomVariablesContainer *theRVs=RandomVariablesContainer::getInstance();
  theRVs->removeRandomVariables(names);
}

bool
OpenSeesFEM::inputFromJSON(QJsonObject &jsonObject)
{
  varNamesAndValues.clear();

  if (jsonObject.contains("randomVar")) {
    QJsonArray randomVars = jsonObject["randomVar"].toArray();
    foreach (const QJsonValue & value, randomVars) {
      QJsonObject theRV = value.toObject();
      QString name = theRV["name"].toString();
      QString zero = "0";
      varNamesAndValues.append(name);
      varNamesAndValues.append(zero);
    }
  }
    
  return true;
}

bool
OpenSeesFEM::outputToJSON(QJsonObject &jsonObject) {
  return true;
}


bool
OpenSeesFEM::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["Application"] = "OpenSees";
    QJsonObject dataObj;

    QString fileName = inputScript->text();
    QFileInfo fileInfo(fileName);

    dataObj["mainScript"]=fileInfo.fileName();
    dataObj["MS_Path"]=fileInfo.path();

    QString fileName1 = postprocessScript->text();
    QFileInfo fileInfo1(fileName1);

    if (fileInfo1.exists() && fileInfo1.isFile()) {
	dataObj["postprocessScript"]=fileInfo1.fileName();
	dataObj["PS_Path"]=fileInfo1.path();
    } else
      dataObj["postprocessScript"]=QString("");

    jsonObject["ApplicationData"] = dataObj;

    return true;
}

 bool
OpenSeesFEM::inputAppDataFromJSON(QJsonObject &jsonObject) {
  
  //
  // from ApplicationData
  //
  
  if (jsonObject.contains("ApplicationData")) {
    QJsonObject dataObject = jsonObject["ApplicationData"].toObject();
    
    //
    // retrieve filename and path, set the QLIne Edit
    //
    
    QString fileName;
    QString filePath;
    
    if (dataObject.contains("mainScript")) {
      QJsonValue theName = dataObject["mainScript"];
      fileName = theName.toString();
    } else
      return false;
    
    if (dataObject.contains("MS_Path")) {
      QJsonValue theName = dataObject["MS_Path"];
      filePath = theName.toString();
    } else
      return false;
    
    inputScript->setText(QDir(filePath).filePath(fileName));

    if (dataObject.contains("postprocessScript")) {
      QJsonValue theName = dataObject["postprocessScript"];
      fileName = theName.toString();
    
      if (dataObject.contains("PS_Path")) {
	QJsonValue theName = dataObject["PS_Path"];
	filePath = theName.toString();
	postprocessScript->setText(QDir(filePath).filePath(fileName)); 	
      } else
	postprocessScript->setText(fileName);
    } else {
      postprocessScript->setText("");
    }
  } else
    return false;

  return true;
}


void
OpenSeesFEM::setMainScript(QString name1){

    // remove old random variables
    QStringList names;
    for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
        names.append(varNamesAndValues.at(i));
    }

    RandomVariablesContainer *theRVs= RandomVariablesContainer::getInstance();
    theRVs->removeRandomVariables(names);

    // set file name & ebtry in qLine edit

    inputScript->setText(name1);

    //
    // parse file for random variables and add them
    //

    OpenSeesParser theParser;
    varNamesAndValues = theParser.getVariables(name1);

    theRVs->addRVsWithValues(varNamesAndValues);

    return;
}

void
OpenSeesFEM::chooseMainScript(void) {
    QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),
						  "C://",
						  "All files (*.tcl)");
    this->setMainScript(fileName);
}

void
OpenSeesFEM::specialCopyMainScript(QString fileName, QStringList varNames) {
    // if OpenSees or FEAP parse the file for the variables
    if (varNames.size() > 0) {
        OpenSeesParser theParser;
        theParser.writeFile(inputScript->text(), fileName, varNames);
    }
}

bool
OpenSeesFEM::copyFiles(QString &dirName) {

     QString fileName = inputScript->text();

     if (fileName.isEmpty()) {
         this->errorMessage("OpenSeesInput - no file set");
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

