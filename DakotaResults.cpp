// Written: fmckenna

#include "DakotaResults.h"
#include <QVBoxLayout>
#include <QJsonObject>
#include <DakotaResultsSampling.h>
#include <QMessageBox>
#include <QDebug>

DakotaResults::DakotaResults(QWidget *parent)
    : SimCenterWidget(parent), resultWidget(0)
{
    layout = new QVBoxLayout();
    this->setLayout(layout);
    resultWidget = 0;
}

DakotaResults::~DakotaResults()
{

}


void
DakotaResults::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject uq;

    if (resultWidget != 0) {
        resultWidget->outputToJSON(uq);
    } else {
        uq["resultType"]=QString(tr("NONE"));
    }
    jsonObject["uqResults"]=uq;
}


void
DakotaResults::inputFromJSON(QJsonObject &jsonObject)
{   
    QJsonValue uqValue = jsonObject["uqResults"];
    if ( uqValue.isUndefined())// !! uqValue.isNull() )
        return;

    QJsonObject uq = uqValue.toObject();
    QString resultType = uq["resultType"].toString();
    DakotaResults *newResultWidget = 0;
    if (resultType == QString("DakotaResultsSampling")) {
        newResultWidget = new DakotaResultsSampling();
        newResultWidget->inputFromJSON(uq);
    }

    this->setResultWidget(newResultWidget);
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

