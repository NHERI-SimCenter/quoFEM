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

#include <MultiFidelityMonteCarlo.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>

MultiFidelityMonteCarlo::MultiFidelityMonteCarlo(QWidget *parent) 
: UQ_MethodInputWidget(parent)
{
    auto layout = new QGridLayout();

    // create layout label and entry for # samples
    numSamples = new QLineEdit();
    numSamples->setText(tr("500"));
    numSamples->setValidator(new QIntValidator);
    numSamples->setToolTip("Specify the number of samples");

    layout->addWidget(new QLabel("# Samples"), 0, 0);
    layout->addWidget(numSamples, 0, 1);

    numLevels = new QLineEdit();
    numLevels->setText(tr("1"));
    numLevels->setValidator(new QIntValidator);
    numLevels->setToolTip("Specify the number of fidelity levels of models");
    numberLevels = 1;

    layout->addWidget(new QLabel("# Fidelity Levels"), 1, 0);
    layout->addWidget(numLevels, 1, 1);

    // create label and entry for seed to layout
    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Set the seed");

    layout->addWidget(new QLabel("Seed"), 2, 0);
    layout->addWidget(randomSeed, 2, 1);

    //connect(numLevels, SIGNAL(textChanged(QString)), this, SLOT(numLevelsChanged(QString)));
    connect(numLevels, SIGNAL(editingFinished()), this, SLOT(numLevelsChanged()));
    layout->setRowStretch(3, 1);
    layout->setColumnStretch(2, 1);
    this->setLayout(layout);
}

MultiFidelityMonteCarlo::~MultiFidelityMonteCarlo()
{

}

bool
MultiFidelityMonteCarlo::outputToJSON(QJsonObject &jsonObj){

    bool result = true;
    jsonObj["samples"]=numSamples->text().toInt();
    jsonObj["seed"]=randomSeed->text().toInt();
    jsonObj["levels"]=numLevels->text().toInt();
    return result;    
}

bool
MultiFidelityMonteCarlo::inputFromJSON(QJsonObject &jsonObject){

  bool result = false;
  if (jsonObject.contains("samples") && jsonObject.contains("seed")) {
    int samples=jsonObject["samples"].toInt();
    int seed=jsonObject["seed"].toInt();
    int levels=jsonObject["levels"].toInt();
    numSamples->setText(QString::number(samples));
    randomSeed->setText(QString::number(seed));
    numLevels->setText(QString::number(levels));
    result = true;
    numberLevels = levels;
  }

  return result;
}

void
MultiFidelityMonteCarlo::clear(void)
{

}


int
MultiFidelityMonteCarlo::getNumberTasks()
{
  return numSamples->text().toInt();
}

void
MultiFidelityMonteCarlo::numLevelsChanged(){
    int newNumLevels = numLevels->text().toInt();
    if (numberLevels != newNumLevels) {
        numberLevels = newNumLevels;
        emit onNumModelsChanged(newNumLevels);

    }
}
