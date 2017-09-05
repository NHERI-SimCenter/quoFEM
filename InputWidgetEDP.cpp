
// Written: fmckenna

#include "InputWidgetEDP.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>

InputWidgetEDP::InputWidgetEDP(QWidget *parent) : QWidget(parent)
{
    verticalLayout = new QVBoxLayout();
    this->setLayout(verticalLayout);
    this->makeEDP();
}

InputWidgetEDP::~InputWidgetEDP()
{

}

void
InputWidgetEDP::makeEDP(void)
{
    // text and add button at top
    QHBoxLayout *textRandom = new QHBoxLayout();
    QLabel *textEDP = new QLabel();
    textEDP->setText(tr("Output Parameters"));
    textEDP->setMinimumWidth(250);
    textEDP->setMaximumWidth(250);

    QPushButton *addEDP = new QPushButton();
    addEDP->setMinimumWidth(75);
    addEDP->setMaximumWidth(75);
    addEDP->setText(tr("Add"));
    connect(addEDP,SIGNAL(clicked()),this,SLOT(addEDP()));

    textRandom->addWidget(textEDP);
    textRandom->addWidget(addEDP);
    textRandom->addStretch();

    verticalLayout->addLayout(textRandom);

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);

    edp = new QGroupBox(tr(""));

    edpLayout = new QVBoxLayout;
    edpLayout->addStretch();
    //setLayout(layout);
    edp->setLayout(edpLayout);

    this->addEDP();

     sa->setWidget(edp);
     verticalLayout->addWidget(sa);

     verticalLayout->setSpacing(0);
     verticalLayout->setMargin(0);
}

void InputWidgetEDP::addEDP(void)
{
   EDP *theEDP = new EDP();
   theEDPs.append(theEDP);
   edpLayout->insertWidget(edpLayout->count()-1, theEDP);
}

void InputWidgetEDP::clear(void)
{
  // loop over random variables, removing from layout & deleting
  for (int i = 0; i <theEDPs.size(); ++i) {
    EDP *theEDP = theEDPs.at(i);
    edpLayout->removeWidget(theEDP);
    delete theEDP;
  }
  theEDPs.clear();
}


void
InputWidgetEDP::outputToJSON(QJsonObject &jsonObject)
{
    QJsonArray edpArray;
    for (int i = 0; i <theEDPs.size(); ++i) {
        QJsonObject edp;
        theEDPs.at(i)->outputToJSON(edp);
        edpArray.append(edp);
    }
    jsonObject["edps"]=edpArray;
}


void
InputWidgetEDP::inputFromJSON(QJsonObject &rvObject)
{
  this->clear();

    // add the new
    QJsonArray rvArray = rvObject["edps"].toArray();
    foreach (const QJsonValue &rvValue, rvArray) {
        QJsonObject rvObject = rvValue.toObject();
        EDP *theEDP = new EDP();
        theEDP->inputFromJSON(rvObject);
        theEDPs.append(theEDP);
        edpLayout->insertWidget(edpLayout->count()-1, theEDP);
    }
}
