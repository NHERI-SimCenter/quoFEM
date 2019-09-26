
#include "PCEInputWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>
#include <QGroupBox>
#include <QVBoxLayout>

PCEInputWidget::PCEInputWidget(QWidget *parent) : UQ_MethodInputWidget(parent)
{
  QVBoxLayout *layout = new QVBoxLayout();

    // specify method to generate training data
    // create layout label and entry for # samples
    QGroupBox *trainingDataGroup = new QGroupBox("Surrogate Training Data");
    trainingDataLayout = new QGridLayout();

    trainingDataLayout->addWidget(new QLabel("Method for Data Generation"), 0, 0);
    dataMethod = new QComboBox();
    dataMethod->addItem("Quadrature");
    dataMethod->addItem("Sparse Grid Quadrature");
    connect(dataMethod,SIGNAL(currentIndexChanged(int)),this,SLOT(trainingDataMethodChanged(int)));
    trainingDataLayout->addWidget(dataMethod, 0, 1);

    // create layout label and entry for level
    levelLabel = new QLabel("Quadrature Order");
    level = new QLineEdit();
    level->setText(tr("5"));
    level->setValidator(new QIntValidator);
    level->setToolTip("Specify the quadrature levels");
    //trainingDataLayout->addWidget(new QLabel("Quadrature Level"), 2, 0);
    trainingDataLayout->addWidget(levelLabel, 1, 0);
    trainingDataLayout->addWidget(level, 1, 1);

    trainingDataLayout->setColumnStretch(2, 1);

    trainingDataGroup->setLayout(trainingDataLayout);
    layout->addWidget(trainingDataGroup);

    QGroupBox *samplingDataGroup = new QGroupBox("Surrogate Sampling Data");
    samplingDataLayout = new QGridLayout();

    samplingDataLayout->addWidget(new QLabel("Method for Data Generation"), 0, 0);
    dataMethodSampling = new QComboBox();
    dataMethodSampling->addItem("LHS");
    dataMethodSampling->addItem("Monte Carlo");
    //connect(dataMethodSampling,SIGNAL(currentIndexChanged(int)),this,SLOT(samplingDataMethodChanged(int)));
    samplingDataLayout->addWidget(dataMethodSampling, 0, 1);

    // create layout label and entry for # samples
    numSamplesSampling = new QLineEdit();
    numSamplesSampling->setText(tr("1000"));
    numSamplesSampling->setValidator(new QIntValidator);
    numSamplesSampling->setToolTip("Specify the number of samples");
    samplingDataLayout->addWidget(new QLabel("# Samples"), 1, 0);
    samplingDataLayout->addWidget(numSamplesSampling, 1, 1);


    // create label and entry for seed to layout
    srand(time(NULL));
    int randomNumberSampling = rand() % 1000 + 1;
    randomSeedSampling = new QLineEdit();
    randomSeedSampling->setText(QString::number(randomNumberSampling));
    randomSeedSampling->setValidator(new QIntValidator);
    randomSeedSampling->setToolTip("Set the seed");
    samplingDataLayout->addWidget(new QLabel("Seed"), 2, 0);
    samplingDataLayout->addWidget(randomSeedSampling, 2, 1);

    // create layout label and entry for level
    levelSampling = new QLineEdit();
    levelSampling->setText(tr("5"));
    levelSampling->setValidator(new QIntValidator);
    levelSampling->setToolTip("Specify the quadrature levels");
    samplingDataLayout->addWidget(new QLabel("Quadrature Level"), 3, 0);
    samplingDataLayout->addWidget(levelSampling, 3, 1);
    samplingDataLayout->itemAtPosition(3,0)->widget()->hide();
    samplingDataLayout->itemAtPosition(3,1)->widget()->hide();

    samplingDataLayout->setRowStretch(4, 1);
    samplingDataLayout->setColumnStretch(2, 1);

    samplingDataGroup->setLayout(samplingDataLayout);
    layout->addWidget(samplingDataGroup);
    layout->addStretch();


    this->setLayout(layout);
}


bool PCEInputWidget::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    jsonObject["dataMethod"]=dataMethod->currentText();
    jsonObject["level"]=level->text().toInt();

    jsonObject["samplesSampling"]=numSamplesSampling->text().toInt();
    jsonObject["seedSampling"]=randomSeedSampling->text().toDouble();
    jsonObject["dataMethodSampling"]=dataMethodSampling->currentText();

    return result;
}

bool PCEInputWidget::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = false;
    if ( (jsonObject.contains("level"))
         && (jsonObject.contains("dataMethod"))
         && (jsonObject.contains("samplesSampling"))
         && (jsonObject.contains("seedSampling"))
         && (jsonObject.contains("dataMethodSampling")) ) {

        int levelV=jsonObject["level"].toInt();

        level->setText(QString::number(levelV));

        QString method=jsonObject["dataMethod"].toString();
        dataMethod->setCurrentIndex(dataMethod->findText(method));

        int samplesS=jsonObject["samplesSampling"].toInt();
        double seedS=jsonObject["seedSampling"].toDouble();
        numSamplesSampling->setText(QString::number(samplesS));
        randomSeedSampling->setText(QString::number(seedS));

        QString methodS=jsonObject["dataMethodSampling"].toString();
        dataMethodSampling->setCurrentIndex(dataMethod->findText(methodS));


        return true;

    }
    return result;
}

int PCEInputWidget::getNumberTasks()
{
    return 0;
}

void PCEInputWidget::trainingDataMethodChanged(int method) {

    if (method == 0) {
       levelLabel->setText("Quadrature Order");
    } else {
        levelLabel->setText("Quadrature Level");
    }
}

void PCEInputWidget::samplingDataMethodChanged(int method) {

    if (method == 0 || method == 1) {
        samplingDataLayout->itemAtPosition(1,0)->widget()->show();
        samplingDataLayout->itemAtPosition(1,1)->widget()->show();
        samplingDataLayout->itemAtPosition(2,0)->widget()->show();
        samplingDataLayout->itemAtPosition(2,1)->widget()->show();
        samplingDataLayout->itemAtPosition(3,0)->widget()->hide();
        samplingDataLayout->itemAtPosition(3,1)->widget()->hide();
    } else {
        samplingDataLayout->itemAtPosition(1,0)->widget()->hide();
        samplingDataLayout->itemAtPosition(1,1)->widget()->hide();
        samplingDataLayout->itemAtPosition(2,0)->widget()->hide();
        samplingDataLayout->itemAtPosition(2,1)->widget()->hide();
        samplingDataLayout->itemAtPosition(3,0)->widget()->show();
        samplingDataLayout->itemAtPosition(3,1)->widget()->show();
    }
}

