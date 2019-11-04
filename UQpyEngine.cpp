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

#include "UQpyEngine.h"
#include <QDebug>
#include <RandomVariablesContainer.h>
#include <UQ_Results.h>

UQpyEngine::UQpyEngine(QWidget *parent)
    : UQ_Engine(parent)
{
  /*************************  at some point need to redo so no new
    QString classType("Uncertain");
    theRandomVariables =  new RandomVariablesContainer(classType);
    theResults = new UQ_Results();
  ***************************************************************/
}

UQpyEngine::~UQpyEngine()
{

}

int
UQpyEngine::getMaxNumParallelTasks(void) {
    return 1;
}

bool
UQpyEngine::outputToJSON(QJsonObject &rvObject) {
    return true;
}


bool
UQpyEngine::inputFromJSON(QJsonObject &rvObject) {
    return true;
}


int
UQpyEngine::processResults(QString &filenameResults, QString &filenameTab) {
    return 0;
}


RandomVariablesContainer *
UQpyEngine::getParameters() {
  QString classType("Uncertain");
  RandomVariablesContainer *theRV =  new RandomVariablesContainer(classType);
  return theRV;
}

UQ_Results *UQpyEngine::getResults(void) {
  UQ_Results *theRes = new UQ_Results();
  return theRes;
}

void
UQpyEngine::clear(void) {
    return;
}


QString
UQpyEngine::getProcessingScript() {
    return QString("parseUQpy.py");
}




