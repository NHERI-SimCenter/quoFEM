// Written: fmckenna

#include "InputWidgetUQ.h"
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

#include <SimCenterWidget.h>
#include <InputWidgetSampling.h>


InputWidgetUQ::InputWidgetUQ(QWidget *parent)
    : SimCenterWidget(parent),uqType(0)
{
    //femSpecific = 0;

    layout = new QVBoxLayout();

    QVBoxLayout *name= new QVBoxLayout;

    // text and add button at top
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *textFEM=new SectionTitle();
    textFEM->setText(tr("UQ Method"));
    textFEM->setMinimumWidth(250);
    QSpacerItem *spacer = new QSpacerItem(50,10);

    uqSelection = new QComboBox();
    uqSelection->setMaximumWidth(100);
    uqSelection->setMinimumWidth(100);

    titleLayout->addWidget(textFEM);
    titleLayout->addItem(spacer);
    titleLayout->addWidget(uqSelection);
    titleLayout->addStretch();
    titleLayout->setSpacing(0);
    titleLayout->setMargin(0);

    name->addLayout(titleLayout);

    name->setSpacing(10);
    name->setMargin(0);

    uqSelection->addItem(tr("Sampling"));
    uqSelection->addItem(tr("Reliability"));
    uqSelection->addItem(tr("Calibration"));

    connect(uqSelection, SIGNAL(currentIndexChanged(QString)), this, SLOT(uqSelectionChanged(QString)));

    layout->addLayout(name);

    layout->addStretch();
 //   run = new QPushButton();
 //   run->setText(tr("RUN"));
 //   layout->addWidget(run);

    this->setLayout(layout);
   // QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Fixed);
   // this->setSizePolicy(sp);

    this->uqSelectionChanged(tr("Sampling"));
    //layout->setSpacing(10);
    layout->setMargin(0);
    //layout->addStretch();
}

InputWidgetUQ::~InputWidgetUQ()
{

}


void InputWidgetUQ::clear(void)
{

}



void
InputWidgetUQ::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject uq;

    if (uqType != 0) {
        uqType->outputToJSON(uq);
    }
    uq["uqType"] = uqSelection->currentText();
    jsonObject["uqMethod"]=uq;
}


void
InputWidgetUQ::inputFromJSON(QJsonObject &jsonObject)
{
  this->clear();

    QJsonObject uq = jsonObject["uqMethod"].toObject();
    QString selection = uq["uqType"].toString();
    this->uqSelectionChanged(selection);
    if (uqType != 0)
         uqType->inputFromJSON(uq);
}

int InputWidgetUQ::processResults(QString &filenameResults)
{
    if (samplingWidget != 0)
        return samplingWidget->processResults(filenameResults);
}

void InputWidgetUQ::uqSelectionChanged(const QString &arg1)
{
    if (uqType != 0) {
       // layout->rem
        samplingWidget = 0;
        layout->removeWidget(uqType);
        delete uqType;
        uqType = 0;
    }

    //uqSpecific = new QWidget();

    if (arg1 == QString("Sampling")) {
      //uqType = new InputWidgetSampling();
      samplingWidget = new InputWidgetSampling();
      uqType = samplingWidget;
    }

    if (uqType != 0)
        layout->insertWidget(1, uqType,1);
}
