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
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

// Purpose: a widget for managing submiited jobs by uqFEM tool
//  - allow for refresh of status, deletion of submitted jobs, and download of results from finished job

#include "RemoteJobCreatorWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QJsonObject>

#include <AgaveInterface.h>
#include <QDebug>
#include <QDir>


RemoteJobCreatorWidget::RemoteJobCreatorWidget(AgaveInterface *theInt, QWidget *parent)
    : QWidget(parent), theInterface(theInt)
{
    QGridLayout *layout = new QGridLayout();
    QLabel *nameLabel = new QLabel();
    nameLabel->setText(QString("job Name:"));
    layout->addWidget(nameLabel, 0,0);

    nameLineEdit = new QLineEdit();
    layout->addWidget(nameLineEdit,0,1);

    QLabel *numCPU_Label = new QLabel();
    numCPU_Label->setText(QString("num Nodes:"));
    layout->addWidget(numCPU_Label,1,0);

    numCPU_LineEdit = new QLineEdit();
    numCPU_LineEdit->setText("1");
    layout->addWidget(numCPU_LineEdit,1,1);

    QLabel *numProcessorsLabel = new QLabel();
    numProcessorsLabel->setText(QString("num Nodes:"));
    layout->addWidget(numProcessorsLabel,2,0);

    numProcessorsLineEdit = new QLineEdit();
    numProcessorsLineEdit->setText("32");
    layout->addWidget(numProcessorsLineEdit,2,1);

    QLabel *runtimeLabel = new QLabel();
    runtimeLabel->setText(QString("max Run Time:"));
    layout->addWidget(runtimeLabel,3,0);

    runtimeLineEdit = new QLineEdit();
    runtimeLineEdit->setText("00:10:00");
    layout->addWidget(runtimeLineEdit,3,1);

    QPushButton *pushButton = new QPushButton();
    pushButton->setText("Submit");
    layout->addWidget(pushButton,4,1);

    this->setLayout(layout);

    connect(pushButton,SIGNAL(clicked()), this, SLOT(pushButtonClicked()));
}


void
RemoteJobCreatorWidget::pushButtonClicked(void)
{
    this->hide();
    QJsonObject job;

    job["name"]=QString("uqFEM:") + nameLineEdit->text();
    job["nodeCount"]=numCPU_LineEdit->text();
    job["processorsPerNode"]=numProcessorsLineEdit->text();
    job["requestedTime"]=runtimeLineEdit->text();

    // defaults (possibly from a parameters file)
    job["appId"]="dakota-6.6.0";
    job["memoryPerNode"]= "1GB";
    job["archive"]="true";
    job["archiveSystem"]="designsafe.storage.default";

    QJsonObject parameters;
    parameters["inputFile"]="dakota.in";
    parameters["driverFile"]="fem_driver";
    parameters["modules"]="petsc";
    job["parameters"]=parameters;


    QDir theDirectory(directoryName);
    QString dirName = theDirectory.dirName();

  // QString userName=theInterface->getHomeDir();
  //  QString remoteDirectory = QString("agave://designsafe.storage.default/") + userName + QString("/") + dirName;
    QString remoteDirectory = theInterface->getHomeDirPath() + QString("/") + dirName;

     QJsonObject inputs;
     inputs["inputDirectory"]=remoteDirectory;
     job["inputs"]=inputs;

    // upload directory under user & submit job
    theInterface->uploadDirectory(directoryName, theInterface->getHomeDirPath());

    theInterface->startJob(job);

    // now remove the tmp directory
    theDirectory.removeRecursively();
}


void RemoteJobCreatorWidget::setInputDirectory(const QString &name)
{
    directoryName = name;
    qDebug() << "DIR NAME: " << directoryName ;
}
