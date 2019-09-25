
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
    dataMethod->addItem("LHS");
    dataMethod->addItem("Monte Carlo");
    dataMethod->addItem("Sparse Grid Quadrature");
    connect(dataMethod,SIGNAL(currentIndexChanged(int)),this,SLOT(trainingDataMethodChanged(int)));
    trainingDataLayout->addWidget(dataMethod, 0, 1);

    // create layout label and entry for # samples
    numSamples = new QLineEdit();
    numSamples->setText(tr("1000"));
    numSamples->setValidator(new QIntValidator);
    numSamples->setToolTip("Specify the number of samples");
    trainingDataLayout->addWidget(new QLabel("# Samples"), 1, 0);
    trainingDataLayout->addWidget(numSamples, 1, 1);


    // create label and entry for seed to layout
    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Set the seed");
    trainingDataLayout->addWidget(new QLabel("Seed"), 2, 0);
    trainingDataLayout->addWidget(randomSeed, 2, 1);

    // create layout label and entry for level
    level = new QLineEdit();
    level->setText(tr("5"));
    level->setValidator(new QIntValidator);
    level->setToolTip("Specify the quadrature levels");
    trainingDataLayout->addWidget(new QLabel("Quadrature Level"), 3, 0);
    trainingDataLayout->addWidget(level, 3, 1);
    trainingDataLayout->itemAtPosition(3,0)->widget()->hide();
    trainingDataLayout->itemAtPosition(3,1)->widget()->hide();

    trainingDataLayout->setRowStretch(4, 1);
    trainingDataLayout->setColumnStretch(2, 1);

    trainingDataGroup->setLayout(trainingDataLayout);
    layout->addWidget(trainingDataGroup);



    QGroupBox *samplingDataGroup = new QGroupBox("Surrogate Sampling Data");
    samplingDataLayout = new QGridLayout();

    trainingDataLayout->addWidget(new QLabel("Method for Data Generation"), 0, 0);
    dataMethodSampling = new QComboBox();
    dataMethodSampling->addItem("LHS");
    dataMethodSampling->addItem("Monte Carlo");
    dataMethodSampling->addItem("Sparse Grid Quadrature");
    connect(dataMethodSampling,SIGNAL(currentIndexChanged(int)),this,SLOT(samplingDataMethodChanged(int)));
    samplingDataLayout->addWidget(dataMethodSampling, 0, 1);

    // create layout label and entry for # samples
    numSamplesSampling = new QLineEdit();
    numSamplesSampling->setText(tr("1000"));
    numSamplesSampling->setValidator(new QIntValidator);
    numSamplesSampling->setToolTip("Specify the number of samples");
    samplingDataLayout->addWidget(new QLabel("# Samples"), 1, 0);
    samplingDataLayout->addWidget(numSamples, 1, 1);


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
    samplingDataLayout->addWidget(level, 3, 1);
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
    jsonObject["samples"]=numSamples->text().toInt();
    jsonObject["seed"]=randomSeed->text().toDouble();
    jsonObject["dataMethod"]=dataMethod->currentText();
    jsonObject["level"]=level->text().toInt();

    jsonObject["samplesSampling"]=numSamplesSampling->text().toInt();
    jsonObject["seedSampling"]=randomSeedSampling->text().toDouble();
    jsonObject["dataMethodSampling"]=dataMethodSampling->currentText();
    jsonObject["levelSampling"]=levelSampling->text().toInt();

    return result;
}

bool PCEInputWidget::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = false;
    if ( (jsonObject.contains("samples"))
         && (jsonObject.contains("seed"))
         && (jsonObject.contains("level"))
         && (jsonObject.contains("dataMethod"))
         && (jsonObject.contains("samplesSampling"))
         && (jsonObject.contains("seedSampling"))
         && (jsonObject.contains("levelSampling"))
         && (jsonObject.contains("dataMethodSampling")) ) {

        int samples=jsonObject["samples"].toInt();
        int levelV=jsonObject["level"].toInt();
        double seed=jsonObject["seed"].toDouble();
        numSamples->setText(QString::number(samples));
        randomSeed->setText(QString::number(seed));
        level->setText(QString::number(levelV));

        QString method=jsonObject["dataMethod"].toString();
        dataMethod->setCurrentIndex(dataMethod->findText(method));

        int samplesS=jsonObject["samplesSampling"].toInt();
        int levelVS=jsonObject["levelSampling"].toInt();
        double seedS=jsonObject["seedSampling"].toDouble();
        numSamplesSampling->setText(QString::number(samplesS));
        randomSeedSampling->setText(QString::number(seedS));
        levelSampling->setText(QString::number(levelVS));

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

    if (method == 0 || method == 1) {
        trainingDataLayout->itemAtPosition(1,0)->widget()->show();
        trainingDataLayout->itemAtPosition(1,1)->widget()->show();
        trainingDataLayout->itemAtPosition(2,0)->widget()->show();
        trainingDataLayout->itemAtPosition(2,1)->widget()->show();
        trainingDataLayout->itemAtPosition(3,0)->widget()->hide();
        trainingDataLayout->itemAtPosition(3,1)->widget()->hide();
    } else {
        trainingDataLayout->itemAtPosition(1,0)->widget()->hide();
        trainingDataLayout->itemAtPosition(1,1)->widget()->hide();
        trainingDataLayout->itemAtPosition(2,0)->widget()->hide();
        trainingDataLayout->itemAtPosition(2,1)->widget()->hide();
        trainingDataLayout->itemAtPosition(3,0)->widget()->show();
        trainingDataLayout->itemAtPosition(3,1)->widget()->show();
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

