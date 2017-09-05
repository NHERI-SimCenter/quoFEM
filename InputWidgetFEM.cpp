// Written: fmckenna

#include "InputWidgetFEM.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>


InputWidgetFEM::InputWidgetFEM(QWidget *parent) : QWidget(parent)
{
    femSpecific = 0;

    layout = new QHBoxLayout();

    QVBoxLayout *name= new QVBoxLayout;
    QLabel *feLabel = new QLabel();
    feLabel->setText(QString("FE Program"));
    femSelection = new QComboBox();
    femSelection->setMaximumWidth(100);
    femSelection->setMinimumWidth(100);
    name->addWidget(feLabel);
    name->addWidget(femSelection);

    name->setSpacing(10);
    name->setMargin(0);
    name->addStretch();

    femSelection->addItem(tr("OpenSees"));
    femSelection->addItem(tr("FEAPpv"));

    connect(femSelection, SIGNAL(currentIndexChanged(QString)), this, SLOT(femProgramChanged(QString)));

    layout->addLayout(name);
    this->femProgramChanged(tr("OpenSees"));

   // layout->addStretch();
    this->setLayout(layout);

     QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Fixed);
     this->setSizePolicy(sp);
}

InputWidgetFEM::~InputWidgetFEM()
{

}


void InputWidgetFEM::clear(void)
{

}



void
InputWidgetFEM::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject fem;
    fem["program"]=femSelection->currentText();
    fileName1=file1->text();
    fileName2=file2->text();

    fem["inputFile"]=fileName1;
    fem["postprocessScript"]=fileName2;
    jsonObject["fem"]=fem;
}


void
InputWidgetFEM::inputFromJSON(QJsonObject &jsonObject)
{
  this->clear();
  QJsonObject fem = jsonObject["fem"].toObject();
  fileName1=fem["inputFile"].toString();
  fileName2=fem["postprocessScript"].toString();

  QString program=fem["program"].toString();
  int index = femSelection->findText(program);
  femSelection->setCurrentIndex(index);
  this->femProgramChanged(program);
  if (program == tr("OpenSees")) {
      file1->setText(fileName1);
  } else {
      file1->setText(fileName1);
      file2->setText(fileName2);
  }
}

void InputWidgetFEM::femProgramChanged(const QString &arg1)
{
    if (femSpecific != 0) {
       // layout->rem
        layout->removeWidget(femSpecific);
        delete femSpecific;
        femSpecific = 0;
    }

    femSpecific = new QWidget();
    //femLayout = new QVBoxLayout();
    if (arg1 == QString("OpenSees")) {
        QVBoxLayout *femLayout = new QVBoxLayout();
        QLabel *label1 = new QLabel();
        label1->setText("main script");

        QHBoxLayout *fileName1Layout = new QHBoxLayout();
        file1 = new QLineEdit;
        QPushButton *chooseFile1 = new QPushButton();
        chooseFile1->setText(tr("Choose"));
        connect(chooseFile1,SIGNAL(clicked()),this,SLOT(chooseFileName1()));

        fileName1Layout->addWidget(file1);
        fileName1Layout->addWidget(chooseFile1);
        fileName1Layout->addStretch();

       fileName1Layout->setSpacing(0);
       fileName1Layout->setMargin(0);

        femLayout->addWidget(label1);
        femLayout->addLayout(fileName1Layout);
        femLayout->setSpacing(10);
        femLayout->setMargin(0);
        femLayout->addStretch();
        femSpecific->setLayout(femLayout);


    } else if (arg1 == QString("FEAPpv")) {
        QVBoxLayout *femLayout = new QVBoxLayout();
        QLabel *label1 = new QLabel();
        label1->setText("main script");
        QLabel *label2 = new QLabel();
        label2->setText("postprocess script");

        QHBoxLayout *fileName1Layout = new QHBoxLayout();
        file1 = new QLineEdit;
        QPushButton *chooseFile1 = new QPushButton();
        chooseFile1->setText(tr("Choose"));
         connect(chooseFile1,SIGNAL(clicked()),this,SLOT(chooseFileName1()));

        fileName1Layout->addWidget(file1);
        fileName1Layout->addWidget(chooseFile1);
        fileName1Layout->addStretch();

        QHBoxLayout *fileName2Layout = new QHBoxLayout();
        file2 = new QLineEdit;
        QPushButton *chooseFile2 = new QPushButton();
        chooseFile2->setText(tr("Choose"));
        connect(chooseFile2,SIGNAL(clicked()),this,SLOT(chooseFileName2()));
        fileName2Layout->addWidget(file2);
        fileName2Layout->addWidget(chooseFile2);
        fileName2Layout->addStretch();


       fileName1Layout->setSpacing(10);
       fileName1Layout->setMargin(0);
       fileName2Layout->setSpacing(10);
       fileName2Layout->setMargin(0);

        femLayout->addWidget(label1);
        femLayout->addLayout(fileName1Layout);
        femLayout->addWidget(label2);
        femLayout->addLayout(fileName2Layout);
        femLayout->setSpacing(10);
        femLayout->setMargin(0);
        femLayout->addStretch();
        femSpecific->setLayout(femLayout);
    }
   // femSpecific->addStretch();

    layout->addWidget(femSpecific);
}

void InputWidgetFEM::chooseFileName1(void)
{
   fileName1=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
   file1->setText(fileName1);
}

void InputWidgetFEM::chooseFileName2(void)
{
   fileName2=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
   file2->setText(fileName2);
}
