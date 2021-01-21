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

#include <UCSD_TMMC.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QPushButton>
#include <QFileDialog>


UCSD_TMMC::UCSD_TMMC(QWidget *parent) 
: UQ_MethodInputWidget(parent)
{
    auto layout = new QGridLayout();

    // create layout label and entry for # samples
    numParticles = new QLineEdit();
    numParticles->setText(tr("500"));
    numParticles->setValidator(new QIntValidator);
    numParticles->setToolTip("Specify the number of samples");

    layout->addWidget(new QLabel("# Samples"), 1, 0);
    layout->addWidget(numParticles, 1, 1);

    // create label and entry for seed to layout
    randomSeed = new QLineEdit();
    randomSeed->setText(tr("0"));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Specify the random seed value");

    layout->addWidget(new QLabel("Seed"), 2, 0);
    layout->addWidget(randomSeed, 2, 1);


    logLikelihoodScript = new QLineEdit();

    layout->addWidget(new QLabel("Log Likelihood Script"), 0, 0);
    layout->addWidget(logLikelihoodScript, 0, 1, 1, 2);

    QPushButton *chooseFile = new QPushButton("Choose");

    connect(chooseFile, &QPushButton::clicked, this, [=](){
        logLikelihoodScript->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.py)"));
    });
    layout->addWidget(chooseFile,0,3);


    layout->setRowStretch(3, 1);
    layout->setColumnStretch(4, 1);
    layout->setColumnStretch(2, 1);
    this->setLayout(layout);
}

UCSD_TMMC::~UCSD_TMMC()
{

}

bool
UCSD_TMMC::outputToJSON(QJsonObject &jsonObj){

    bool result = true;
    jsonObj["numParticles"]=numParticles->text().toInt();
    jsonObj["seed"]=randomSeed->text().toInt();

    QString logLike = logLikelihoodScript->text();
    QFileInfo fileInfo(logLike);

    jsonObj["logLikelihoodFile"]=fileInfo.fileName();
    QString path = fileInfo.absolutePath();
    jsonObj["logLikelihoodPath"]=path;

    return result;    
}

bool
UCSD_TMMC::inputFromJSON(QJsonObject &jsonObject){

  bool result = true;
  this->clear();

  if (jsonObject.contains("numParticles") && jsonObject.contains("logLikelihoodFile") && jsonObject.contains("logLikelihoodPath") && jsonObject.contains("seed")) {
    int particles=jsonObject["numParticles"].toInt();
    numParticles->setText(QString::number(particles));

    int seed=jsonObject["seed"].toInt();
    randomSeed->setText(QString::number(seed));

    QString file = jsonObject["logLikelihoodFile"].toString();
    QString path = jsonObject["logLikelihoodPath"].toString();
    logLikelihoodScript->setText(file + QDir::separator() + path);
  }
  else {
          emit sendErrorMessage("ERROR: TMCMC input - not all data specified");
          result = false;
      }

  return result;
}

void
UCSD_TMMC::clear(void)
{

}


int
UCSD_TMMC::getNumberTasks()
{
  return numParticles->text().toInt();
}

/*
int
UCSD_TMMC::getNumberTasks() {
    QFileDialog dialog(this, "Save Simulation Model");
    //dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList filters;
    filters << "Json files (*.json)"
            << "All files (*)";
    dialog.setNameFilters(filters);


    if (dialog.exec() != QDialog::Accepted)
        return false;

    // and save the file
    logLikelihoodScript->setText(dialog.selectedFiles().first());
}
*/
