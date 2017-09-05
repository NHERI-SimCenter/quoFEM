// Written: fmckenna

#include "SamplingMethodInputWidget.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>


SamplingMethodInputWidget::SamplingMethodInputWidget(QWidget *parent) : QWidget(parent)
{
    //femSpecific = 0;

    layout = new QHBoxLayout();

    QVBoxLayout *methodLayout= new QVBoxLayout;
    QLabel *label1 = new QLabel();
    label1->setText(QString("Sampling Method"));
    samplingMethod = new QComboBox();
    samplingMethod->addItem(tr("LHS"));
    samplingMethod->addItem(tr("Monte Carlo"));

    methodLayout->addWidget(label1);
    methodLayout->addWidget(samplingMethod);
    
   // connect(samplingMethod, SIGNAL(currentIndexChanged(QString)), this, SLOT(femProgramChanged(QString)));

    QVBoxLayout *samplesLayout= new QVBoxLayout;
    QLabel *label2 = new QLabel();
    label2->setText(QString("#samples"));
    numSamples = new QLineEdit();
    numSamples->setMaximumWidth(100);
    numSamples->setMinimumWidth(100);

    samplesLayout->addWidget(label2);
    samplesLayout->addWidget(numSamples);

    QVBoxLayout *seedLayout= new QVBoxLayout;
    QLabel *label3 = new QLabel();
    label3->setText(QString("seed"));
    randomSeed = new QLineEdit();
    randomSeed->setText(tr("0"));
    randomSeed->setMaximumWidth(100);
    randomSeed->setMinimumWidth(100);

    seedLayout->addWidget(label3);
    seedLayout->addWidget(randomSeed);

    layout->addLayout(methodLayout);
    layout->addLayout(samplesLayout);
    layout->addLayout(seedLayout);

     layout->addStretch();
    run = new QPushButton();
    run->setText(tr("RUN"));
    layout->addWidget(run);

   // layout->addStretch();
    this->setLayout(layout);
    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Fixed);
    this->setSizePolicy(sp);
}

SamplingMethodInputWidget::~SamplingMethodInputWidget()
{

}


void SamplingMethodInputWidget::clear(void)
{

}



void
SamplingMethodInputWidget::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject uq;
    uq["method"]=samplingMethod->currentText();
    uq["samples"]=numSamples->text();
    uq["seed"]=randomSeed->text();
    jsonObject["uqMethod"]=uq;
}


void
SamplingMethodInputWidget::inputFromJSON(QJsonObject &rvObject)
{
  this->clear();

}

