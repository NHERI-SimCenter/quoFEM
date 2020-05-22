
#include "GaussianProcessInputWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>
#include <QGroupBox>
#include <QDebug>

GaussianProcessInputWidget::GaussianProcessInputWidget(QWidget *parent) : UQ_MethodInputWidget(parent)
{
    //auto layout = new QGridLayout();
    QVBoxLayout *layout = new QVBoxLayout();

    // create layout label and entry for # samples
    QGroupBox *trainingDataGroup = new QGroupBox("Surrogate Training Data");
    QGridLayout * trainingDataLayout = new QGridLayout();
    numSamples = new QLineEdit();
    numSamples->setText(tr("500"));
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
    //    dataMethod->addItem("Other");
    trainingDataLayout->addWidget(dataMethod,2,1);
    trainingDataGroup->setLayout(trainingDataLayout);

    layout->addWidget(trainingDataGroup);
    //    layout->addStretch();

    // create layout label and entry for # samples
    QGroupBox *sampleDataGroup = new QGroupBox("Surrogate Sampling Data");
    QGridLayout * sampleDataLayout = new QGridLayout();
    numSamples2 = new QLineEdit();
    numSamples2->setText(tr("1000"));
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
    // dataMethod2->addItem("Other");
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

  bool result = false;
  if ( (jsonObject.contains("samples"))
       && (jsonObject.contains("seed"))
       && (jsonObject.contains("samples2"))
       && (jsonObject.contains("seed2"))
       && (jsonObject.contains("dataMethod"))
       && (jsonObject.contains("dataMethod2")) ) {

    int samples=jsonObject["samples"].toInt();
    double seed=jsonObject["seed"].toDouble();
    numSamples->setText(QString::number(samples));
    randomSeed->setText(QString::number(seed));

    int samples2=jsonObject["samples2"].toInt();
    double seed2=jsonObject["seed2"].toDouble();
    numSamples2->setText(QString::number(samples2));
    randomSeed2->setText(QString::number(seed2));

    QString method1=jsonObject["dataMethod"].toString();
    QString method2=jsonObject["dataMethod2"].toString();
    dataMethod->setCurrentIndex(dataMethod->findText(method1));
    dataMethod2->setCurrentIndex(dataMethod2->findText(method2));

    result = true;
  }

  return result;
}

int GaussianProcessInputWidget::getNumberTasks()
{
    return numSamples->text().toInt();
}



