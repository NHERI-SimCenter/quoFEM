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

#include "UCSD_Engine.h"
#include "UCSD_Results.h"
#include "UQ_MethodInputWidget.h"
#include "RandomVariablesContainer.h"
#include <QStackedWidget>
#include <QComboBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QLabel>

#include <QDebug>

#include <UCSD_TMMC.h>

UCSD_Engine::UCSD_Engine(InputWidgetParameters *param,InputWidgetFEM *femWidget,InputWidgetEDP *edpWidget, QWidget *parent)
: UQ_Engine(parent), theCurrentMethod(0), theParameters(param), theFemWidget(femWidget), theEdpWidget(edpWidget)
{

    QString classType("Uncertain");
    theRandomVariables =  new RandomVariablesContainer(classType);
  
    QVBoxLayout *layout = new QVBoxLayout();

    //
    // the selection part
    //

    QHBoxLayout *theSelectionLayout = new QHBoxLayout();
    QLabel *label = new QLabel();
    label->setText(QString("Method"));
    theMethodSelection = new QComboBox();
    theMethodSelection->addItem(tr("Transitional Markov chain Monte Carlo"));
    theMethodSelection->setMinimumWidth(600);

    theSelectionLayout->addWidget(label);
    theSelectionLayout->addWidget(theMethodSelection);
    theSelectionLayout->addStretch();
    layout->addLayout(theSelectionLayout);

    parallelCheckBox = new QCheckBox();
    parallelCheckBox->setChecked(true);

    //
    // create the stacked widget
    //

    theStackedWidget = new QStackedWidget();

    //
    // create the individual widgets add to stacked widget
    //

    theTMMC = new UCSD_TMMC(theParameters, theFemWidget, theEdpWidget);
    theStackedWidget->addWidget(theTMMC);

    layout->addWidget(theStackedWidget);
    this->setLayout(layout);
    theCurrentMethod=theTMMC;

    connect(theMethodSelection,
	    SIGNAL(currentIndexChanged(QString)),
	    this,
	    SLOT(engineSelectionChanged(QString)));
}

UCSD_Engine::~UCSD_Engine()
{

}


void UCSD_Engine::methodChanged(const QString &arg1)
{
    UQ_MethodInputWidget *theOldEngine = theCurrentMethod;

    if ((arg1 == QString("TMMC")) || 
	(arg1 == QString("Transitional Markov chain Monte Carlo"))) {
      theStackedWidget->setCurrentIndex(0);
      theCurrentMethod = theTMMC;   
    } 
    else {
        QString errorMsg = QString("UCSD_Engine Selection type: ") + arg1 + QString(" unknown");
        errorMessage(errorMsg);
        qDebug() << "ERROR .. UCSD_Engine selection .. type unknown: " << arg1;
    }

    // emit signal if engine changed
    if (theCurrentMethod != theOldEngine)
        emit onMethodChanged();
}


int
UCSD_Engine::getMaxNumParallelTasks(void) {
    return theCurrentMethod->getNumberTasks();
}

bool
UCSD_Engine::outputToJSON(QJsonObject &jsonObject) {

    jsonObject["uqType"] = theMethodSelection->currentText();
    jsonObject["parallelExecution"] = parallelCheckBox->isChecked();
    return theCurrentMethod->outputToJSON(jsonObject);
}

bool
UCSD_Engine::inputFromJSON(QJsonObject &jsonObject) {
    bool result = false;

    QString selection = jsonObject["uqType"].toString();

    int index = theMethodSelection->findText(selection);
    theMethodSelection->setCurrentIndex(index);
    this->methodChanged(selection);
    if (theCurrentMethod != 0)
        result = theCurrentMethod->inputFromJSON(jsonObject);
    else
        result = false; // don't emit error as one should have been generated

    parallelCheckBox->setChecked(true);
    if (jsonObject.contains("parallelExecution")) {
        bool checkedState = jsonObject["parallelExecution"].toBool();
        parallelCheckBox->setChecked(checkedState);
    }

    return result;
}


bool
UCSD_Engine::outputAppDataToJSON(QJsonObject &jsonObject)
{
    jsonObject["Application"] = "UCSD-UQ";
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;

    return true;
}

bool
UCSD_Engine::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return true;
}

int
UCSD_Engine::processResults(QString &filenameResults, QString &filenameTab) {
    //return theCurrentMethod->processResults(filenameResults, filenameTab);
    return 0;
}

RandomVariablesContainer *
UCSD_Engine::getParameters() {
    QString classType("Uncertain");
    theRandomVariables =  new RandomVariablesContainer(classType);

    return theRandomVariables;
}

UQ_Results *
UCSD_Engine::getResults(void) {
    return new UCSD_Results(theRandomVariables);
}

QString
UCSD_Engine::getProcessingScript() {
    return QString("parseDAKOTA.py");
}

QString
UCSD_Engine::getMethodName() {
    return QString("UCSD");
}

bool
UCSD_Engine::copyFiles(QString &fileName) {
    return theCurrentMethod->copyFiles(fileName);
}
