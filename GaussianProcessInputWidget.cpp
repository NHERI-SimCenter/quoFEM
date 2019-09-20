
#include "GaussianProcessInputWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>
#include <QGroupBox>

GaussianProcessInputWidget::GaussianProcessInputWidget(QWidget *parent) : UQ_MethodInputWidget(parent)
{
    //auto layout = new QGridLayout();
    QHBoxLayout *layout = new QHBoxLayout();

    // create layout label and entry for # samples
    QGroupBox *trainingDataGroup = new QGroupBox("Surrogate Training Data");
    QGridLayout * trainingDataLayout = new QGridLayout();
    numSamples = new QLineEdit();
    numSamples->setText(tr("10"));
    numSamples->setValidator(new QIntValidator);
    numSamples->setToolTip("Specify the number of samples");
    trainingDataLayout->addWidget(new QLabel("# Samples "), 0, 0);
    trainingDataLayout->addWidget(numSamples, 0, 1);

    // create label and entry for seed to layout
    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Set the seed");
    trainingDataLayout->addWidget(new QLabel("Seed"), 1, 0);
    trainingDataLayout->addWidget(randomSeed, 1, 1);

    trainingDataLayout->addWidget(new QLabel("Method for Data Generation"), 2, 0);
    dataMethod = new QComboBox();
    dataMethod->addItem("LHS");
    dataMethod->addItem("Monte Carlo");
    dataMethod->addItem("Other");
    trainingDataLayout->addWidget(dataMethod,2,1);
    trainingDataGroup->setLayout(trainingDataLayout);

    layout->addWidget(trainingDataGroup);
    //    layout->addStretch();

    // create layout label and entry for # samples
    QGroupBox *sampleDataGroup = new QGroupBox("Surrogate Sampling Data");
    QGridLayout * sampleDataLayout = new QGridLayout();
    numSamples2 = new QLineEdit();
    numSamples2->setText(tr("10"));
    numSamples2->setValidator(new QIntValidator);
    numSamples2->setToolTip("Specify the number of samples");
    sampleDataLayout->addWidget(new QLabel("# Samples "), 0, 0);
    sampleDataLayout->addWidget(numSamples2, 0, 1);

    // create label and entry for seed to layout
    srand(time(NULL));
    int randomNumber2 = rand() % 1000 + 1;
    randomSeed2 = new QLineEdit();
    randomSeed2->setText(QString::number(randomNumber2));
    randomSeed2->setValidator(new QIntValidator);
    randomSeed2->setToolTip("Set the seed");
    sampleDataLayout->addWidget(new QLabel("Seed"), 1, 0);
    sampleDataLayout->addWidget(randomSeed2, 1, 1);

    sampleDataLayout->addWidget(new QLabel("Method for Data Generation"), 2, 0);
    dataMethod2 = new QComboBox();
    dataMethod2->addItem("LHS");
    dataMethod2->addItem("Monte Carlo");
    dataMethod2->addItem("Other");
    sampleDataLayout->addWidget(dataMethod2,2,1);
    sampleDataGroup->setLayout(sampleDataLayout);

    layout->addWidget(sampleDataGroup);
    layout->addStretch();

    this->setLayout(layout);
}


bool GaussianProcessInputWidget::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    jsonObject["samples"]=numSamples->text().toInt();
    jsonObject["seed"]=randomSeed->text().toDouble();
    jsonObject["dataMethod"]=dataMethod->currentText();
    jsonObject["samples2"]=numSamples2->text().toInt();
    jsonObject["seed2"]=randomSeed2->text().toDouble();
    jsonObject["dataMethod2"]=dataMethod2->currentText();
    return result;
}

bool GaussianProcessInputWidget::inputFromJSON(QJsonObject &jsonObject)
{
}

int GaussianProcessInputWidget::getNumberTasks()
{
}



