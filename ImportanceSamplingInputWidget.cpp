#include "ImportanceSamplingInputWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>

ImportanceSamplingInputWidget::ImportanceSamplingInputWidget(QWidget *parent) : UQ_MethodInputWidget(parent)
{
    auto layout = new QGridLayout();

    // create layout label and entry for # samples
    numSamples = new QLineEdit();
    numSamples->setText(tr("10"));
    numSamples->setValidator(new QIntValidator);
    numSamples->setToolTip("Specify the number of samples");

    layout->addWidget(new QLabel("# Samples"), 0, 0);
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

    layout->addWidget(new QLabel("Method"), 2, 0);
    isMethod = new QComboBox();
    isMethod->addItem("Basic Sampling", "import");
    isMethod->addItem("Adaptive Sampling", "adapt_import");
    isMethod->addItem("Multimodal Adaptive Sampling", "mm_adapt_import");
    layout->addWidget(isMethod);
    layout->setRowStretch(3, 1);
    layout->setColumnStretch(2, 1);

    this->setLayout(layout);
}


bool ImportanceSamplingInputWidget::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    jsonObject["samples"]=numSamples->text().toInt();
    jsonObject["seed"]=randomSeed->text().toDouble();
    jsonObject["ismethod"]=isMethod->currentData().toString();
    return result;
}

bool ImportanceSamplingInputWidget::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = false;
    if ( (jsonObject.contains("samples"))
         && (jsonObject.contains("seed"))
         && (jsonObject.contains("ismethod")) ) {

      int samples=jsonObject["samples"].toInt();
      double seed=jsonObject["seed"].toDouble();
      numSamples->setText(QString::number(samples));
      randomSeed->setText(QString::number(seed));

      QString method=jsonObject["ismethod"].toString();
      //isMethod->setCurrentIndex(isMethod->findText(method));
      if (method == "import") {
          isMethod->setCurrentIndex(0);
      } else if (method == "adapt_import")
          isMethod->setCurrentIndex(1);
      else
          isMethod->setCurrentIndex(2);

      result = true;
    }

    return result;
}

int ImportanceSamplingInputWidget::getNumberTasks()
{
    return 0;
}
