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

// added and modified: padhye


#include "InputWidgetParameters.h"
#include <QVBoxLayout>
#include <QJsonObject>
#include <RandomVariableInputWidget.h>
#include <QMessageBox>
#include <QDebug>

InputWidgetParameters::InputWidgetParameters(QWidget *parent)
    : SimCenterWidget(parent), theParameters(0)
{
    layout = new QVBoxLayout();
    this->setLayout(layout);
}

InputWidgetParameters::~InputWidgetParameters()
{

}

bool
InputWidgetParameters::outputToJSON(QJsonObject &jsonObject)
{
  if (theParameters != 0) 
    return theParameters->outputToJSON(jsonObject);
  return true;
}


bool
InputWidgetParameters::inputFromJSON(QJsonObject &jsonObject)
{   
  qDebug() << "InputWidgetParameters::inputFromJSON";
  if (theParameters != 0) 
    return theParameters->inputFromJSON(jsonObject);
  return true;
}

void
InputWidgetParameters::setParametersWidget(RandomVariableInputWidget *param) {

    if (theParameters != 0) {
        layout->removeWidget(theParameters);
        delete theParameters;
        theParameters = 0;
    }

    if (param != 0) {
        layout->addWidget(param);
        theParameters = param;
        theParameters->setInitialConstantRVs(varNamesAndValues);
    //    connect(this,SLOT(errorMessage(QString)),param,SIGNAL(sendErrorMessage(QString)));
        connect(param,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    }
}

void
InputWidgetParameters::setInitialVarNamesAndValues(QStringList theList){
    varNamesAndValues=theList;
    theParameters->setInitialConstantRVs(varNamesAndValues);
}

QStringList
InputWidgetParameters::getParametereNames(void)
{
    if (theParameters != 0)
        return theParameters->getRandomVariableNames();
    else {
        QStringList empty;
        return empty;
    }
}

void
InputWidgetParameters::errorMessage(QString message){
    qDebug() << "InputWidgetParameters" << message;
    emit this->sendErrorMessage(message);
}
