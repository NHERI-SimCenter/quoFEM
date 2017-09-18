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


InputWidgetUQ::InputWidgetUQ(QWidget *parent)
    : SimCenterWidget(parent),uqSpecific(0)
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
    uqSelection->addItem(tr("Optimization"));

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
    uq["uqType"]=uqSelection->currentText();
    uq["method"]=samplingMethod->currentText();
    uq["samples"]=numSamples->text().toInt();
    uq["seed"]=randomSeed->text().toDouble();
    jsonObject["uqMethod"]=uq;
}


void
InputWidgetUQ::inputFromJSON(QJsonObject &jsonObject)
{
  this->clear();

    QJsonObject uq = jsonObject["uqMethod"].toObject();
    QString selection = uq["uqType"].toString();
    this->uqSelectionChanged(selection);

    int index;
    index = uqSelection->findText(selection);
    uqSelection->setCurrentIndex(index);

    if (selection == tr("Sampling")) {
        QString method =uq["method"].toString();
        int samples=uq["samples"].toInt();
        double seed=uq["seed"].toDouble();

        numSamples->setText(QString::number(samples));
        randomSeed->setText(QString::number(seed));
        index = samplingMethod->findText(method);
        samplingMethod->setCurrentIndex(index);
    }
}

void InputWidgetUQ::uqSelectionChanged(const QString &arg1)
{
    if (uqSpecific != 0) {
       // layout->rem
        layout->removeWidget(uqSpecific);
        delete uqSpecific;
        uqSpecific = 0;
    }

    uqSpecific = new QWidget();

    if (arg1 == QString("Sampling")) {
        QVBoxLayout *methodLayout= new QVBoxLayout;
        QLabel *label1 = new QLabel();
        label1->setText(QString("Method"));
        samplingMethod = new QComboBox();
        samplingMethod->addItem(tr("LHS"));
        samplingMethod->addItem(tr("Monte Carlo"));

        methodLayout->addWidget(label1);
        methodLayout->addWidget(samplingMethod);

       // connect(samplingMethod, SIGNAL(currentIndexChanged(QString)), this, SLOT(uqMethodChanged(QString)));

        QVBoxLayout *samplesLayout= new QVBoxLayout;
        QLabel *label2 = new QLabel();
        label2->setText(QString("# Samples"));
        numSamples = new QLineEdit();
        numSamples->setMaximumWidth(100);
        numSamples->setMinimumWidth(100);

        samplesLayout->addWidget(label2);
        samplesLayout->addWidget(numSamples);

        QVBoxLayout *seedLayout= new QVBoxLayout;
        QLabel *label3 = new QLabel();
        label3->setText(QString("Seed"));
        randomSeed = new QLineEdit();
        randomSeed->setText(tr("0"));
        randomSeed->setMaximumWidth(100);
        randomSeed->setMinimumWidth(100);

        seedLayout->addWidget(label3);
        seedLayout->addWidget(randomSeed);

        QHBoxLayout *mLayout = new QHBoxLayout();
        mLayout->addLayout(methodLayout);
        mLayout->addLayout(samplesLayout);
        mLayout->addLayout(seedLayout);
        mLayout->addStretch();
        uqSpecific->setLayout(mLayout);
    }

    layout->insertWidget(1, uqSpecific);

}
