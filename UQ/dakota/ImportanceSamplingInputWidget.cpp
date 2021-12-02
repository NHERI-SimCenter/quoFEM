#include "ImportanceSamplingInputWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>

ImportanceSamplingInputWidget::ImportanceSamplingInputWidget(QWidget *parent) : UQ_MethodInputWidget(parent)
{
    auto layout = new QGridLayout();

    // create layout label and entry for # samples
    numSamples = new QLineEdit();
    numSamples->setText(tr("500"));
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


    probabilityLevel = new QLineEdit();
    probabilityLevel->setText("");

    layout->addWidget(new QLabel("Response Levels"), 3, 0);
    layout->addWidget(probabilityLevel, 3, 1);

//   // layout->addWidget(new QLabel("Probability Levels"), 3, 1);
//    layout->addWidget(levelType, 3, 0);
//    layout->addWidget(probabilityLevel, 3, 1);

    layout->setColumnStretch(2,2);
    layout->setColumnStretch(3,4);
    layout->setRowStretch(5,1);

//    for(int i = 1; i < 3; i++)
//        {
//           layout->itemAtPosition(i,1)->widget()->hide();
//           layout->itemAtPosition(i,2)->widget()->hide();
//        }

    this->setLayout(layout);
}


bool ImportanceSamplingInputWidget::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    jsonObject["samples"]=numSamples->text().toInt();
    jsonObject["seed"]=randomSeed->text().toDouble();
    jsonObject["ismethod"]=isMethod->currentData().toString();

    QJsonArray responseLevel;
    QStringList responseLevelList = QStringList(probabilityLevel->text().split(" "));
    for (int i = 0; i < responseLevelList.size(); ++i)
        responseLevel.push_back(responseLevelList.at(i).toDouble());
    jsonObject["responseLevel"]=responseLevel;

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


      probabilityLevel->setText("");

      QJsonArray probLevels;

      QJsonValue probLevelVal = jsonObject["responseLevel"];
      if (probLevelVal.isArray()) {

          QStringList levelList;
          QJsonArray levels = probLevelVal.toArray();

          for (int i=0; i<levels.count(); i++) {
              QJsonValue value = levels.at(i);
              double levelV = value.toDouble();
              levelList << QString::number(levelV);
          }
          probabilityLevel->setText(levelList.join(" "));
      }
      result = true;
    }

    return result;
}

int ImportanceSamplingInputWidget::getNumberTasks()
{
    return numSamples->text().toInt();
}
