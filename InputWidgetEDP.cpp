
// Written: fmckenna

#include "InputWidgetEDP.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QFrame>
#include <sectiontitle.h>
#include <QSpacerItem>

#include <QDebug>


InputWidgetEDP::InputWidgetEDP(QWidget *parent) : SimCenterWidget(parent)
{
    verticalLayout = new QVBoxLayout();
    this->setLayout(verticalLayout);
    this->makeEDP();
}

InputWidgetEDP::~InputWidgetEDP()
{
// qDebug() << "InputWidgetEDP::DESTRCUTOR CALLED\n";
}

void
InputWidgetEDP::makeEDP(void)
{
    // title & add button
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *title=new SectionTitle();
    title->setText(tr("Engineeering Demand Parameters"));
    title->setMinimumWidth(250);
    QSpacerItem *spacer1 = new QSpacerItem(50,10);
    QSpacerItem *spacer2 = new QSpacerItem(20,10);

    QPushButton *addEDP = new QPushButton();
    addEDP->setMinimumWidth(75);
    addEDP->setMaximumWidth(75);
    addEDP->setText(tr("Add"));
    connect(addEDP,SIGNAL(clicked()),this,SLOT(addEDP()));

    QPushButton *removeEDP = new QPushButton();
    removeEDP->setMinimumWidth(75);
    removeEDP->setMaximumWidth(75);
    removeEDP->setText(tr("Remove"));
    connect(removeEDP,SIGNAL(clicked()),this,SLOT(removeEDP()));

    titleLayout->addWidget(title);
    titleLayout->addItem(spacer1);
    titleLayout->addWidget(addEDP);
    titleLayout->addItem(spacer2);
    titleLayout->addWidget(removeEDP);
    titleLayout->addStretch();

    verticalLayout->addLayout(titleLayout);

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    edp = new QFrame();
    edp->setFrameShape(QFrame::NoFrame);
    edp->setLineWidth(0);

    edpLayout = new QVBoxLayout;

    //setLayout(layout);
    edp->setLayout(edpLayout);
    edpLayout->addStretch();
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


int InputWidgetEDP::getNumEDP(void)
{
    return theEDPs.length();
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


void InputWidgetEDP::removeEDP(void)
{
    // find the ones selected & remove them
    int numEDPs = theEDPs.size();
    for (int i = numEDPs-1; i >= 0; i--) {
        EDP *theEDP = theEDPs.at(i);
        if (theEDP->isSelectedForRemoval()) {
            theEDP->close();
            edpLayout->removeWidget(theEDP);
            theEDPs.remove(i);
            //theEDP->setParent(0);
            delete theEDP;
        }
    }
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

int
InputWidgetEDP::processResults(double *data) {
    qDebug() << "InputWidgetEDP::processResults";

    for (int i = 0; i <theEDPs.size(); ++i) {
        theEDPs.at(i)->setResults(&data[i*4]);
    }
}
