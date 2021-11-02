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

// Written: Michael Gardner

#include "UQ_JsonEngine.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLabel>

#include <QDebug>

#include "CustomUQ_Results.h"

UQ_JsonEngine::UQ_JsonEngine(QWidget *parent)
: UQ_Engine(parent)
{

    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *applicationLayout = new QHBoxLayout();
    
    //
    // the selection part
    //
    theApplicationName = new QLineEdit();
    auto applicationNameLabel = new QLabel();
    applicationNameLabel->setText(tr("UQ Application Name"));
    theApplicationName->setToolTip(tr("Input the UQ application name here to match what has been set in WorkflowApplications.json"));

    applicationLayout->addWidget(applicationNameLabel);
    applicationLayout->addWidget(theApplicationName);
    applicationLayout->addStretch();
      
    QString classType("Uncertain");    
    theRandomVariables = new RandomVariablesContainer(classType);
    theJsonConfigureWidget = new JsonConfiguredWidget( theRandomVariables );
    
    layout->addLayout(applicationLayout);
    layout->addWidget(theJsonConfigureWidget);
    layout->addStretch();
    this->setLayout(layout);
}

UQ_JsonEngine::~UQ_JsonEngine()
{}

int
UQ_JsonEngine::getMaxNumParallelTasks(void) {
  return 1;
    // return theCurrentEngine->getMaxNumParallelTasks();
}

bool
UQ_JsonEngine::outputToJSON(QJsonObject &rvObject) {
  return theJsonConfigureWidget->outputToJSON(rvObject);
}

bool
UQ_JsonEngine::inputFromJSON(QJsonObject &rvObject) {
  return theJsonConfigureWidget->inputFromJSON(rvObject);
}

bool
UQ_JsonEngine::outputAppDataToJSON(QJsonObject &jsonObject) {
  jsonObject["Application"] = theApplicationName->text();
  QJsonObject dataObj;
  jsonObject["ApplicationData"] = dataObj;

  return true;  
}

bool
UQ_JsonEngine::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return true;
}

int
UQ_JsonEngine::processResults( QString &filenameResults, QString &filenameTab) {
  return 0;
}

RandomVariablesContainer *
UQ_JsonEngine::getParameters() {
  QString classType("Uncertain");    
  theRandomVariables = new RandomVariablesContainer(classType);  
  return theRandomVariables;
}

UQ_Results *
UQ_JsonEngine::getResults(void) {
  return new CustomUQ_Results(theRandomVariables);
}

QString
UQ_JsonEngine::getProcessingScript() {
  return QString("runCustomUQ.py");
}

QString
UQ_JsonEngine::getMethodName() {
  return QString("runCustomUQ");
}
