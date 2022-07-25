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

#include "OpenSeesPyFEM.h"
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

#include <OpenSeesPyParser.h>
#include <RandomVariablesContainer.h>

//#include <InputWidgetParameters.h>

OpenSeesPyFEM::OpenSeesPyFEM(QWidget *parent)
  : SimCenterAppWidget(parent)
{

    QGridLayout *layout = new QGridLayout();

    QLabel *label1 = new QLabel("Input Script");
    inputScript = new QLineEdit;
    QPushButton *chooseMS_Button = new QPushButton();
    chooseMS_Button->setText(tr("Choose"));
    connect(chooseMS_Button, &QPushButton::clicked, this, [this](){
      QString selectedFile = QFileDialog::getOpenFileName(this,
							  tr("MainScript"),
                              "",
							  "All files (*.py)");

        if(!selectedFile.isEmpty()) {
            inputScript->setText(selectedFile);
        }
    });
    
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
                              "",
							  "All files (*.py)");

        if(!selectedFile.isEmpty()) {
            postprocessScript->setText(selectedFile);
        }
    });


    QLabel *label3 = new QLabel("Parameters Script");
    parametersScript = new QLineEdit;
    parametersScript->setPlaceholderText("(Optional)");        
    QPushButton *choosParametersScript = new QPushButton();
    choosParametersScript->setText(tr("Choose"));
    connect(choosParametersScript, SIGNAL(clicked()), this, SLOT(chooseParametersScript()));
    
    layout->addWidget(label2,1,0);
    layout->addWidget(postprocessScript,1,1);
    layout->addWidget(choosePostprocessScript,1,2);    
    layout->setRowStretch(2,1.0);

    layout->addWidget(label3,2,0);
    layout->addWidget(parametersScript,2,1);
    layout->addWidget(choosParametersScript,2,2);    
    layout->setRowStretch(3,1.0);
    
    layout->setColumnStretch(1,3);
    layout->setColumnStretch(3,1);        
    this->setLayout(layout);
}

OpenSeesPyFEM::~OpenSeesPyFEM()
{

}


void OpenSeesPyFEM::clear(void)
{
  QStringList names;
  for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
         names.append(varNamesAndValues.at(i));
  }

  RandomVariablesContainer *theRVs=RandomVariablesContainer::getInstance();
  theRVs->removeRandomVariables(names);

  postprocessScript->setText("");
  inputScript->setText("");
  parametersScript->setText("");

}

bool
OpenSeesPyFEM::inputFromJSON(QJsonObject &jsonObject)
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
OpenSeesPyFEM::outputToJSON(QJsonObject &jsonObject) {
  return true;
}


bool
OpenSeesPyFEM::outputAppDataToJSON(QJsonObject &jsonObject) {

    bool result = true;
    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["Application"] = "OpenSeesPy";
    QJsonObject dataObj;

    QString fileName = inputScript->text();
    QFileInfo fileInfo(fileName);
    
    if (fileInfo.exists() && fileInfo.isFile()) {
      dataObj["mainScript"]=fileInfo.fileName();
      dataObj["MS_Path"]=fileInfo.path();
    } else {
      QString msg = QString("OpenSeesPy - mainScript " ) + fileName + QString(" does not exist!");
      this->errorMessage(msg);
      dataObj["mainScript"]=fileName;
      dataObj["MS_Path"]=QString("");
      result = false;
    }

    QString fileName1 = postprocessScript->text();
    QFileInfo fileInfo1(fileName1);

    if (fileInfo1.exists() && fileInfo1.isFile()) {
        dataObj["postprocessScript"]=fileInfo1.fileName();
        dataObj["PS_Path"]=fileInfo1.path();
    } else {
        if (fileName1.isEmpty())
            dataObj["postprocessScript"]=QString("");
        else {
            QString msg = QString("OpenSeesPy - postprocessScript " ) + fileName1 + QString(" does not exist!");
            this->errorMessage(msg);
            dataObj["postprocessScript"]=fileName1;
            result = false;
        }
    }
    
    QString fileName2 = parametersScript->text();
    QFileInfo fileInfo2(fileName2);

    if (fileInfo2.exists() && fileInfo2.isFile()) {
        dataObj["parametersScript"]=fileInfo2.fileName();
        dataObj["PA_Path"]=fileInfo2.path();
    } else {
        if (fileName2.isEmpty())
            dataObj["parametersScript"]=QString("");
        else {
            QString msg = QString("OpenSeesPy - parametersScript " ) + fileName2 + QString(" does not exist!");
            this->errorMessage(msg);
            dataObj["parametersScript"]=fileName2;
            result = false;
        }
    }

    
    jsonObject["ApplicationData"] = dataObj;

    return true; // copyFiles returns false, if not true JSON object is empty!
}

 bool
OpenSeesPyFEM::inputAppDataFromJSON(QJsonObject &jsonObject) {
  
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
    
    // Main ==

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

    // Post ==

    if (dataObject.contains("postprocessScript")) {
      QJsonValue theName = dataObject["postprocessScript"];
      fileName = theName.toString();
      
      if (dataObject.contains("PS_Path")) {
        QJsonValue theName = dataObject["PS_Path"];
        filePath = theName.toString();
        postprocessScript->setText(QDir(filePath).filePath(fileName));
      } else {
        postprocessScript->setText(fileName);
      }
    } else  {
      postprocessScript->setText("");
    }

    // Params ==

    if (dataObject.contains("parametersScript")) {
      QJsonValue theName = dataObject["parametersScript"];
      fileName = theName.toString();

      if (dataObject.contains("PA_Path")) {
        QJsonValue theName = dataObject["PA_Path"];
        filePath = theName.toString();
        parametersScript->setText(QDir(filePath).filePath(fileName));
      } else {
        parametersScript->setText(fileName);
      }
    } else  {
        parametersScript->setText("");
    }

  } else
    return false;

  return true;
}


void
OpenSeesPyFEM::setParametersScript(QString name1){

    // set file name & ebtry in qLine edit

    parametersScript->setText(name1);

    //
    // parse file for random variables and add them
    //

    OpenSeesPyParser theParser;
    varNamesAndValues = theParser.getVariables(name1);

    RandomVariablesContainer *theRVs=RandomVariablesContainer::getInstance();    
    theRVs->addRVsWithValues(varNamesAndValues);

    return;
}

void
OpenSeesPyFEM::chooseParametersScript(void) {
  QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),
                        "",
						"All files (*.py)");
  if(!fileName.isEmpty()) {
      this->setParametersScript(fileName);
  }
}
 
bool
OpenSeesPyFEM::copyFiles(QString &dirName) {

     QString fileName = inputScript->text();

     if (fileName.isEmpty()) {
         this->errorMessage("OpenSeesPy - copyFiles - no file set");
         return false;
     }
     
     QFileInfo fileInfo(fileName);
     if (!fileInfo.exists() || !fileInfo.isFile()) {
       QString msg = QString("OpenSeesPy - copyFiles - file does not exist: ") + fileName;
       this->errorMessage(msg);	   
       return false;
     }       

     QString theFile = fileInfo.fileName();
     QString thePath = fileInfo.path();

     SimCenterAppWidget::copyPath(thePath, dirName, false);

     RandomVariablesContainer *theRVs=RandomVariablesContainer::getInstance();
     QStringList varNames = theRVs->getRandomVariableNames();

     // now do interesting stuff
     
     QString fileNameP = parametersScript->text();
     QFileInfo fileInfo1(fileName);
     if (fileInfo1.exists() && fileInfo1.isFile()) {

       QString newName = dirName + QDir::separator() + "tmpSimCenter.params";
       OpenSeesPyParser theParser;
       theParser.writeFile(fileNameP, newName, varNames);
       
     } else {
       
       QString newName = dirName + QDir::separator() + "tmpSimCenter.script";
       QFile::copy(fileName, newName);
       
     }

     return true;
}

