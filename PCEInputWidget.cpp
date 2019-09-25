
#include "PCEInputWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>

PCEInputWidget::PCEInputWidget(QWidget *parent) : UQ_MethodInputWidget(parent)
{
    layout = new QGridLayout();

    // specify method to generate training data
    layout->addWidget(new QLabel("Method for Data Generation"), 0, 0);
    dataMethod = new QComboBox();
    dataMethod->addItem("   ");
    dataMethod->addItem("Quadrature");
    dataMethod->addItem("Sparse Grid Quadrature");
    connect(dataMethod,SIGNAL(currentIndexChanged(int)),this,SLOT(dataMethodChanged(int)));
    layout->addWidget(dataMethod, 0, 1);

    // create layout label and entry for # samples
    quadOd = new QLineEdit();
    quadOd->setText(tr("6"));
    quadOd->setValidator(new QIntValidator);
    quadOd->setToolTip("Specify the order of the quadrature method");
    layout->addWidget(new QLabel("Quadrature Order"), 1, 0);
    layout->addWidget(quadOd, 1, 1);


    // create label and entry for seed to layout
    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Set the seed");
    layout->addWidget(new QLabel("Seed"), 2, 0);
    layout->addWidget(randomSeed, 2, 1);

    // create layout label and entry for level
    level = new QLineEdit();
    level->setText(tr("5"));
    level->setValidator(new QIntValidator);
    level->setToolTip("Specify the quadrature levels");
    layout->addWidget(new QLabel("Quadrature Level"), 3, 0);
    layout->addWidget(level, 3, 1);

    layout->itemAtPosition(3,0)->widget()->hide();
    layout->itemAtPosition(3,1)->widget()->hide();

    layout->setRowStretch(4, 1);
    layout->setColumnStretch(2, 1);

    this->setLayout(layout);
}


bool PCEInputWidget::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    jsonObject["order"]=quadOd->text().toInt();
    jsonObject["seed"]=randomSeed->text().toDouble();
    jsonObject["dataMethod"]=dataMethod->currentText();
    jsonObject["level"]=level->text().toInt();
    return result;
}

bool PCEInputWidget::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = false;
    if ( (jsonObject.contains("order"))
         && (jsonObject.contains("seed"))
         && (jsonObject.contains("dataMethod")) ) {

        int order=jsonObject["order"].toInt();
        double seed=jsonObject["seed"].toDouble();
        quadOd->setText(QString::number(order));
        randomSeed->setText(QString::number(seed));

        QString method=jsonObject["dataMethod"].toString();
        dataMethod->setCurrentIndex(dataMethod->findText(method));
        return true;

    }
    return result;
}

int PCEInputWidget::getNumberTasks()
{
    return 0;
}

void PCEInputWidget::dataMethodChanged(int method) {

    if (method == 0 || method == 1) {
        layout->itemAtPosition(1,0)->widget()->show();
        layout->itemAtPosition(1,1)->widget()->show();
        layout->itemAtPosition(2,0)->widget()->show();
        layout->itemAtPosition(2,1)->widget()->show();
        layout->itemAtPosition(3,0)->widget()->hide();
        layout->itemAtPosition(3,1)->widget()->hide();
    } else {
        layout->itemAtPosition(1,0)->widget()->hide();
        layout->itemAtPosition(1,1)->widget()->hide();
        layout->itemAtPosition(2,0)->widget()->hide();
        layout->itemAtPosition(2,1)->widget()->hide();
        layout->itemAtPosition(3,0)->widget()->show();
        layout->itemAtPosition(3,1)->widget()->show();
    }
}

