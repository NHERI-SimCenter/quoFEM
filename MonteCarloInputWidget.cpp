/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna

#include <MonteCarloInputWidget.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>

MonteCarloInputWidget::MonteCarloInputWidget(QWidget *parent) 
: UQ_MethodInputWidget(parent)
{

  QVBoxLayout *mLayout = new QVBoxLayout();

  // create layout label and entry for # samples
  QVBoxLayout *samplesLayout= new QVBoxLayout;
  QLabel *label2 = new QLabel();
  label2->setText(QString("# Samples"));
  numSamples = new QLineEdit();
  numSamples->setText(tr("1000"));
  numSamples->setMaximumWidth(100);
  numSamples->setMinimumWidth(100);
  numSamples->setValidator(new QIntValidator);  
  numSamples->setToolTip("Some text about num samples");

  samplesLayout->addWidget(label2);
  samplesLayout->addWidget(numSamples);



  mLayout->addLayout(samplesLayout);
  mLayout->addStretch(1);
  
  // create label and entry for seed to layout
  QVBoxLayout *seedLayout= new QVBoxLayout;
  QLabel *label3 = new QLabel();
  label3->setText(QString("Seed"));
  srand(time(NULL));
  int randomNumber = rand() % 1000 + 1;
  randomSeed = new QLineEdit();
  randomSeed->setText(QString::number(randomNumber));
  randomSeed->setMaximumWidth(100);
  randomSeed->setMinimumWidth(100);
  randomSeed->setValidator(new QIntValidator);    
  randomSeed->setToolTip("Some text about seed");

  seedLayout->addWidget(label3);
  seedLayout->addWidget(randomSeed);



  mLayout->addLayout(seedLayout);
  mLayout->addStretch(1);

  this->setLayout(mLayout);
}

MonteCarloInputWidget::~MonteCarloInputWidget()
{

}

bool
MonteCarloInputWidget::outputToJSON(QJsonObject &jsonObj){

    bool result = true;
    jsonObj["samples"]=numSamples->text().toInt();
    jsonObj["seed"]=randomSeed->text().toDouble();
    return result;    
}

bool
MonteCarloInputWidget::inputFromJSON(QJsonObject &jsonObject){

  bool result = false;
  if (jsonObject.contains("samples") && jsonObject.contains("seed")) {
    int samples=jsonObject["samples"].toInt();
    double seed=jsonObject["seed"].toDouble();
    numSamples->setText(QString::number(samples));
    randomSeed->setText(QString::number(seed));
    result = true;
  }

  return result;
}

void
MonteCarloInputWidget::clear(void)
{

}



int
MonteCarloInputWidget::getNumberTasks()
{
  return numSamples->text().toInt();
}
