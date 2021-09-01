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

#include "UQ_Results.h"
#include <QVBoxLayout>
#include <QJsonObject>
#include <UQ_Results.h>
#include <QMessageBox>
#include <QDebug>
#include <RandomVariablesContainer.h>

UQ_Results::UQ_Results(QWidget *parent)
: SimCenterWidget(parent), resultWidget(0)
{
    layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    this->setLayout(layout);
    resultWidget = 0;
}

UQ_Results::~UQ_Results()
{

}


bool
UQ_Results::outputToJSON(QJsonObject &jsonObject)
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
UQ_Results::inputFromJSON(QJsonObject &jsonObject)
{   
    bool result = false;
    if (jsonObject.contains("uqResults")) {
        QJsonValue uqValue = jsonObject["uqResults"];

        QJsonObject uq = uqValue.toObject();
        QString resultType = uq["resultType"].toString();
        //UQ_Results *newResultWidget = 0;

        if (resultType == "NONE") {
           // resultWidget=0;
            return true; // no results saved
        }

	if (resultWidget != 0) {
	  result = resultWidget->inputFromJSON(uq);
	} else {
	  emit sendErrorMessage("ERROR: reading Dakota Results - no result widget set!");
	}

    } else {
        emit sendErrorMessage("ERROR: Dakota Results - no \"uqResults\" entry");
        return false;
    }


    // no error if no results .. maybe none actually in file
    return result;
}

int 
UQ_Results::processResults(QString &filenameResults, QString &filenameTab) {

    if (resultWidget != 0)
        return resultWidget->processResults(filenameResults, filenameTab);
    else {
        QMessageBox::warning(this, tr("Application"),
                             tr("BUG - No Results Set!"));

        return 0;
    }
}

void
UQ_Results::setResultWidget(UQ_Results *result) {
    if (resultWidget != 0) {
        layout->removeWidget(resultWidget);
        delete resultWidget;
        resultWidget = 0;

    } else {
        qDebug() << "ResultWidget NULL";
    }

    if (result != 0) {
        layout->addWidget(result);
        resultWidget = result;
    }
}
