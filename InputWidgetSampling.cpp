// Written: fmckenna

#include "InputWidgetSampling.h"
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
#include <InputWidgetEDP.h>

#include <iostream>
#include <sstream>
#include <fstream>


InputWidgetSampling::InputWidgetSampling(QWidget *parent)
    : SimCenterWidget(parent),uqSpecific(0)
{
    layout = new QVBoxLayout();

    QVBoxLayout *methodLayout= new QVBoxLayout;
    QLabel *label1 = new QLabel();
    label1->setText(QString("Method"));
    samplingMethod = new QComboBox();
    samplingMethod->addItem(tr("LHS"));
    samplingMethod->addItem(tr("Monte Carlo"));
    
    methodLayout->addWidget(label1);
    methodLayout->addWidget(samplingMethod);
    
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

    layout->addLayout(mLayout);

    theEdpWidget = new InputWidgetEDP();
    layout->addWidget(theEdpWidget,1);

    this->setLayout(layout);
}

InputWidgetSampling::~InputWidgetSampling()
{

}


void InputWidgetSampling::clear(void)
{

}



void
InputWidgetSampling::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject uq;
    uq["method"]=samplingMethod->currentText();
    uq["samples"]=numSamples->text().toInt();
    uq["seed"]=randomSeed->text().toDouble();
    theEdpWidget->outputToJSON(uq);
    jsonObject["samplingMethodData"]=uq;
}


void
InputWidgetSampling::inputFromJSON(QJsonObject &jsonObject)
{
  this->clear();

  QJsonObject uq = jsonObject["samplingMethodData"].toObject();
  QString method =uq["method"].toString();
  int samples=uq["samples"].toInt();
  double seed=uq["seed"].toDouble();
  
  numSamples->setText(QString::number(samples));
  randomSeed->setText(QString::number(seed));
  int index = samplingMethod->findText(method);
  samplingMethod->setCurrentIndex(index);

  theEdpWidget->inputFromJSON(uq);
}

void InputWidgetSampling::uqSelectionChanged(const QString &arg1)
{

}

int InputWidgetSampling::processResults(QString &filenameResults) {

    std::ifstream fileResults("/Users/simcenter/Downloads/pedro/dakota.out");
    if (!fileResults.is_open()) {
        qDebug() << "Could not open file";
           return -1;
    }
    int numEDP = theEdpWidget->getNumEDP();
    double *data = new double[numEDP*4];

    for (int i=0; i<numEDP; i++) {
        double mean, stdDev, num3, num4;
        std::string haystack;
        fileResults >> haystack >> mean >> stdDev >> num3 >> num4;

       data[i*4] = mean;
       data[i*4+1]=stdDev;
       data[i*4+2]=num3;
       data[i*4+3]=num4;
    }
    theEdpWidget->processResults(data);

    delete [] data;
    return 0;
}
/*
std::ifstream fileResults(filenameOUT.);
    if (!fileResults.is_open())
    {
        std::cerr << "Failed to open file!\n";
        return -1;
    }

    // Since this is a read-only variable,
    // also mark is as `const` to make intentions
    // clear to the readers.
    //
    const std::string needle = "Kurtosis";

    // Main reading loop:
    //
    std::string haystack;
    while (std::getline(file, haystack))
    {
        if (haystack.find(needle) != std::string::npos)
        {
            std::cout << haystack << "\n";
            break;
            // Prints
            //  <a class="member-modal" href="https://website.net/users/membercard/890520">Phil Jankins</a>
            // once
        }
    }
    */
