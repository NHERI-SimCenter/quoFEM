
#include "GaussianProcessInputWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>
#include <QGroupBox>

GaussianProcessInputWidget::GaussianProcessInputWidget(QWidget *parent) : UQ_MethodInputWidget(parent)
{
    //auto layout = new QGridLayout();
    QVBoxLayout *layout = new QVBoxLayout();

    // create layout label and entry for # samples
    QGroupBox *trainingDataGroup = new QGroupBox("Training Data");
    QGridLayout * trainingDataLayout = new QGridLayout();
    numSamples = new QLineEdit();
    numSamples->setText(tr("10"));
    numSamples->setValidator(new QIntValidator);
    numSamples->setToolTip("Specify the number of samples");
    trainingDataLayout->addWidget(new QLabel("Data"), 0, 0);
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
    layout->addStretch();

    //layout->setRowStretch(3, 1);
    //layout->setColumnStretch(2, 1);

    this->setLayout(layout);
}


bool GaussianProcessInputWidget::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    jsonObject["samples"]=numSamples->text().toInt();
    jsonObject["seed"]=randomSeed->text().toDouble();
    jsonObject["dataMethod"]=dataMethod->currentText();
    return result;
}

bool GaussianProcessInputWidget::inputFromJSON(QJsonObject &jsonObject)
{
}

int GaussianProcessInputWidget::getNumberTasks()
{
}



