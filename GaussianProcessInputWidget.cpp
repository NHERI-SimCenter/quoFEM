
#include "GaussianProcessInputWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>
#include <QGroupBox>
#include <QDebug>
#include <QVBoxLayout>

GaussianProcessInputWidget::GaussianProcessInputWidget(QWidget *parent) : UQ_MethodInputWidget(parent)
{
    //auto layout = new QGridLayout();
    QVBoxLayout *layout = new QVBoxLayout();

    // create layout label and entry for # samples
    QGroupBox *trainingDataGroup = new QGroupBox("Surrogate Training Data");
    QGridLayout * trainingDataLayout = new QGridLayout();
    trainingNumSamples = new QLineEdit();
    trainingNumSamples->setText(tr("20"));
    trainingNumSamples->setValidator(new QIntValidator);
    trainingNumSamples->setToolTip("Specify the number of samples");
    trainingDataLayout->addWidget(new QLabel("# Samples"), 0, 0);
    trainingDataLayout->addWidget(trainingNumSamples, 0, 1);

    // create label and entry for seed to layout
    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    trainingSeed = new QLineEdit();
    trainingSeed->setText(QString::number(randomNumber));
    trainingSeed->setValidator(new QIntValidator);
    trainingSeed->setToolTip("Set the seed");
    trainingDataLayout->addWidget(new QLabel("Seed"), 1, 0);
    trainingDataLayout->addWidget(trainingSeed, 1, 1);

    trainingDataLayout->addWidget(new QLabel("Data Generation Method"), 2, 0);
    trainingMethod = new QComboBox();
    trainingMethod->addItem("LHS");
    trainingMethod->addItem("Monte Carlo");
    //    dataMethod->addItem("Other");
    trainingDataLayout->addWidget(trainingMethod,2,1);
    trainingDataGroup->setLayout(trainingDataLayout);

     trainingDataLayout->setColumnStretch(2,1);

    layout->addWidget(trainingDataGroup);

    //    layout->addStretch();

    // create layout label and entry for # samples
    QGroupBox *sampleDataGroup = new QGroupBox("Surrogate Sampling Data");
    QGridLayout * sampleDataLayout = new QGridLayout();

    sampleDataLayout->addWidget(new QLabel("Surface Fitting Method"), 0, 0);
    surrogateSurfaceModel = new QComboBox();
    surrogateSurfaceModel->addItem("Gaussian Process");
    surrogateSurfaceModel->addItem("MARS");
    surrogateSurfaceModel->addItem("MovingLeastSquares");
    surrogateSurfaceModel->addItem("Neural Network");
    surrogateSurfaceModel->addItem("Linear Polynomial");
    surrogateSurfaceModel->addItem("Quadratic Polynomial");
    surrogateSurfaceModel->addItem("Cubic Polynomial");
    //    dataMethod2->addItem("Other");
    sampleDataLayout->addWidget(surrogateSurfaceModel,0,1);

    samplingNumSamples = new QLineEdit();
    samplingNumSamples->setText(tr("1000"));
    samplingNumSamples->setValidator(new QIntValidator);
    samplingNumSamples->setToolTip("Specify the number of samples");
    sampleDataLayout->addWidget(new QLabel("# Samples "), 1, 0);
    sampleDataLayout->addWidget(samplingNumSamples, 1, 1);

    // create label and entry for seed to layout
    srand(time(NULL));
    int randomNumber2 = rand() % 1000 + 1;
    samplingSeed = new QLineEdit();
    samplingSeed->setText(QString::number(randomNumber2));
    samplingSeed->setValidator(new QIntValidator);
    samplingSeed->setToolTip("Set the seed");
    sampleDataLayout->addWidget(new QLabel("Seed"), 2, 0);
    sampleDataLayout->addWidget(samplingSeed, 2, 1);

    sampleDataLayout->addWidget(new QLabel("Data Generation Method"), 3, 0);
    samplingMethod = new QComboBox();
    samplingMethod->addItem("LHS");
    samplingMethod->addItem("Monte Carlo");
    //    dataMethod2->addItem("Other");
    sampleDataLayout->addWidget(samplingMethod,3,1);
    sampleDataGroup->setLayout(sampleDataLayout);


    sampleDataLayout->setColumnStretch(2,1);

    sampleDataGroup->setLayout(sampleDataLayout);
    layout->addWidget(sampleDataGroup);
    layout->addStretch();

    this->setLayout(layout);
}


bool GaussianProcessInputWidget::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    jsonObject["trainingSamples"]=trainingNumSamples->text().toInt();
    jsonObject["trainingSeed"]=trainingSeed->text().toInt();
    jsonObject["trainingMethod"]=trainingMethod->currentText();
    jsonObject["samplingSamples"]=samplingNumSamples->text().toInt();
    jsonObject["samplingSeed"]=samplingSeed->text().toInt();
    jsonObject["samplingMethod"]=samplingMethod->currentText();
    QString text = surrogateSurfaceModel->currentText();

    if (text == "MARS")
        text = "mars";
    else if (text == "MovingLeastSquares")
        text = "moving_least_squares";
    else if (text == "Neural Network")
        text = "neural_network";
    else if (text == "Linear Polynomial")
        text = "polynomial linear";
    else if (text == "Quadratic Polynomial")
        text = "polynomial quadratic";
    else if (text == "Cubic Polynomial")
        text = "polynomial cubic";
    else
        text = "gaussian_process surfpack";

    jsonObject["surrogateSurfaceMethod"]=text;
    return result;
}


bool GaussianProcessInputWidget::inputFromJSON(QJsonObject &jsonObject)
{

  bool result = false;
  if ( (jsonObject.contains("trainingSamples"))
       && (jsonObject.contains("trainingSeed"))
       && (jsonObject.contains("trainingMethod"))
       && (jsonObject.contains("samplingSeed"))
       && (jsonObject.contains("samplingMethod"))
       && (jsonObject.contains("samplingSurfaceMethod"))
       && (jsonObject.contains("samplingSamples")) ) {

    int samples=jsonObject["trainiingSamples"].toInt();
    int seed=jsonObject["trainingSeed"].toInt();
    trainingNumSamples->setText(QString::number(samples));
    trainingSeed->setText(QString::number(seed));

    int samples2=jsonObject["samplingSamples"].toInt();
    double seed2=jsonObject["samplingSeed"].toDouble();
    samplingNumSamples->setText(QString::number(samples2));
    samplingSeed->setText(QString::number(seed2));

    QString method1=jsonObject["trainingMethod"].toString();
    QString method2=jsonObject["samplingMethod"].toString();
    trainingMethod->setCurrentIndex(trainingMethod->findText(method1));
    samplingMethod->setCurrentIndex(samplingMethod->findText(method2));

    QString text=jsonObject["surrogateSurfaceMethod"].toString();
    if (text == "mars")
        text = "MARS";
    else if (text == "moving_least_squares")
        text = "MovingLeastSquares";
    else if (text == "neural_network")
        text = "Neural Network";
    else if (text == "polynomial linear")
        text = "Linear Polynomial";
    else if (text == "polynomial quadratic")
        text = "Quadratic Polynomial";
    else if (text == "polynomial cubic")
        text = "Cubic Polynomial";
    else
        text = "Gaussian Process";

    surrogateSurfaceModel->setCurrentIndex(surrogateSurfaceModel->findText(method2));

    result = true;
  }

  return result;
}

int GaussianProcessInputWidget::getNumberTasks()
{
    return trainingNumSamples->text().toInt();
}



