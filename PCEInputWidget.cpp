
#include "PCEInputWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>

PCEInputWidget::PCEInputWidget(QWidget *parent) : UQ_MethodInputWidget(parent)
{
    auto layout = new QGridLayout();

    // create layout label and entry for # samples
    numSamples = new QLineEdit();
    numSamples->setText(tr("10"));
    numSamples->setValidator(new QIntValidator);
    numSamples->setToolTip("Specify the number of samples");
    layout->addWidget(new QLabel("Data"), 0, 0);
    layout->addWidget(numSamples, 0, 1);

    // create label and entry for seed to layout
    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Set the seed");
    layout->addWidget(new QLabel("Seed"), 1, 0);
    layout->addWidget(randomSeed, 1, 1);

    // specify method to generate training data
    layout->addWidget(new QLabel("Method for Data Generation"), 2, 0);
    dataMethod = new QComboBox();
    dataMethod->addItem("LHS");
    dataMethod->addItem("Monte Carlo");
    dataMethod->addItem("Sparse Grid Quadrature");
    dataMethod->addItem("Other");
    layout->addWidget(dataMethod);
    layout->setRowStretch(3, 1);
    layout->setColumnStretch(2, 1);

    this->setLayout(layout);
}


bool PCEInputWidget::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    jsonObject["samples"]=numSamples->text().toInt();
    jsonObject["seed"]=randomSeed->text().toDouble();
    jsonObject["dataMethod"]=dataMethod->currentText();
    return result;
}

bool PCEInputWidget::inputFromJSON(QJsonObject &jsonObject)
{
}

int PCEInputWidget::getNumberTasks()
{
}



