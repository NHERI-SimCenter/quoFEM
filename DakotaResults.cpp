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

// essentially an abstract class, if no results are available this is the widget that shows up

#include "DakotaResults.h"
#include <QVBoxLayout>
#include <QJsonObject>
#include <DakotaResultsSampling.h>
#include <QMessageBox>
#include <QDebug>
#include <RandomVariablesContainer.h>

DakotaResults::DakotaResults(QWidget *parent)
: SimCenterWidget(parent), resultWidget(0)
{
    layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    this->setLayout(layout);
    resultWidget = 0;
}

DakotaResults::~DakotaResults()
{

}


bool
DakotaResults::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject uq;
    bool result = true;

    if (resultWidget != 0) {
        result = resultWidget->outputToJSON(uq);
    } else {
        uq["resultType"]=QString(tr("NONE"));
    }
    jsonObject["uqResults"]=uq;
    return result;
}


bool
DakotaResults::inputFromJSON(QJsonObject &jsonObject)
{   
    bool result = false;
    if (jsonObject.contains("uqResults")) {
        QJsonValue uqValue = jsonObject["uqResults"];

        QJsonObject uq = uqValue.toObject();
        QString resultType = uq["resultType"].toString();
        DakotaResults *newResultWidget = 0;

        if (resultType == "NONE") {
          return true; // no results saved
        }

        // NOTE: this is kinda kludgy, this class need to know what subclasses enter here
        // so that we can create object of correct type .. this should be done by this class
	/*
        if (resultType == QString("DakotaResultsSampling")) {
            newResultWidget = new DakotaResultsSampling();
            result = newResultWidget->inputFromJSON(uq);
            this->setResultWidget(newResultWidget);
            result = true;
        }
	*/
    } else {
        emit sendErrorMessage("ERROR: Dakota Results - no \"uqResults\" entry");
        return false;
    }


    // no error if no results .. maybe none actually in file
    return result;
}

int 
DakotaResults::processResults(QString &filenameResults, QString &filenameTab) {

    if (resultWidget != 0)
        return resultWidget->processResults(filenameResults, filenameTab);
    else {
        QMessageBox::warning(this, tr("Application"),
                             tr("BUG - No Results Set!"));

        return 0;
    }
}

void
DakotaResults::setResultWidget(DakotaResults *result) {
    if (resultWidget != 0) {
        layout->removeWidget(resultWidget);
        delete resultWidget;
        resultWidget = 0;
    }

    if (result != 0) {
        layout->addWidget(result);
        resultWidget = result;
    }
}

