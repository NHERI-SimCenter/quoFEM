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

#include "RemoteJobCreator.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QJsonObject>

//#include <AgaveInterface.h>
#include <QDebug>
#include <QDir>


RemoteJobCreator::RemoteJobCreator(AgaveCurl *theInt, QWidget *parent)
    : QWidget(parent), theInterface(theInt),maxParallel(1)
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
    numProcessorsLabel->setText(QString("num Processors:"));
    layout->addWidget(numProcessorsLabel,2,0);

    numProcessorsLineEdit = new QLineEdit();
    numProcessorsLineEdit->setText("32");
    layout->addWidget(numProcessorsLineEdit,2,1);

    QLabel *runtimeLabel = new QLabel();
    runtimeLabel->setText(QString("max Run Time:"));
    layout->addWidget(runtimeLabel,3,0);

    runtimeLineEdit = new QLineEdit();
    runtimeLineEdit->setText("00:20:00");
    layout->addWidget(runtimeLineEdit,3,1);

    QLabel *appNameLabel = new QLabel();
    appNameLabel->setText("App Name");
    layout->addWidget(appNameLabel,4,0);

    appLineEdit = new QLineEdit();
    appLineEdit->setText("simcenter-dakota-1.0.0u1");
    //appLineEdit->setText("Dakota-6.6.0.0u1");
    layout->addWidget(appLineEdit,4,1);

    pushButton = new QPushButton();
    pushButton->setText("Submit");
    layout->addWidget(pushButton,5,1);

    this->setLayout(layout);

    //
    // set up connections
    //

    connect(pushButton,SIGNAL(clicked()), this, SLOT(pushButtonClicked()));

    // on login from interface to set up homeDirPath
    connect(theInterface,SIGNAL(loginReturn(bool)),this,SLOT(attemptLoginReturn(bool)));
    connect(this,SIGNAL(getHomeDirCall()),theInterface,SLOT(getHomeDirPathCall()));
    connect(theInterface,SIGNAL(getHomeDirPathReturn(QString)), this, SLOT(getHomeDirReturned(QString)));

    // to start job need to connect uploadDir and start job
    connect(this,SIGNAL(uploadDirCall(const QString &,const QString &)), theInterface, SLOT(uploadDirectoryCall(const QString &,const QString &)));
    connect(theInterface, SIGNAL(uploadDirectoryReturn(bool)), this, SLOT(uploadDirReturn(bool)));
    connect(this,SIGNAL(startJobCall(QJsonObject)),theInterface,SLOT(startJobCall(QJsonObject)));
    connect(theInterface,SIGNAL(startJobReturn(QString)), this, SLOT(startJobReturn(QString)));
}

void 
RemoteJobCreator::setMaxNumParallelProcesses(int max) {
  maxParallel = max;
  qDebug() << "MAX SET: " << max;
  numProcessorsLineEdit->setText("32");
  if (numProcessorsLineEdit->text().toInt() > maxParallel)
    numProcessorsLineEdit->setText(QString::number(max));
}

void
RemoteJobCreator::pushButtonClicked(void)
{
  int numProcesses =numCPU_LineEdit->text().toInt() * numProcessorsLineEdit->text().toInt();
  if (numProcesses > maxParallel) {
    QString errorMsg = QString("ERROR - Too many Parallel Tasks Specified - max allowed from UQ Method is: " )
            + QString::number(maxParallel);
    emit errorMessage(errorMsg);
    return;
  }
 // qDebug() << "processors set (cpu * numProcesses): " << numProcesses << " max tasks: " << maxParallel;

    QDir theDirectory(directoryName);
    QString dirName = theDirectory.dirName();

    QString remoteDirectory = remoteHomeDirPath + QString("/") + dirName;

    // upload directory under user & submit job
    //  NOTE: the job is actually submitted when the uploadDirectory returns
    pushButton->setEnabled(false);

    directoryName.replace("\"","");
    emit uploadDirCall(directoryName, remoteHomeDirPath);
}

// this slot is invoked on return from uploadDirectory signal in pushButtonClicked slot
void
RemoteJobCreator::uploadDirReturn(bool result)
{
    if (result == true) {

        // this->hide();
        QJsonObject job;

        pushButton->setDisabled(true);

        job["name"]=QString("uqFEM:") + nameLineEdit->text();
        job["nodeCount"]=numCPU_LineEdit->text();
        job["processorsPerNode"]=numProcessorsLineEdit->text();
        job["requestedTime"]=runtimeLineEdit->text();

        // defaults (possibly from a parameters file)
        //Dakota-6.6.0.0u1
        //job["appId"]="dakota-6.6.0";
        //job["appId"]="Dakota-6.6.0.0u1";

        job["appId"]=appLineEdit->text();
        job["memoryPerNode"]= "1GB";
        job["archive"]="true";
        job["archiveSystem"]="designsafe.storage.default";

        QJsonObject parameters;
        parameters["inputFile"]="dakota.in";
        parameters["outputFile"]="dakota.out";
        parameters["errorFile"]="dakota.err";
        parameters["driverFile"]="fem_driver";
        parameters["modules"]="petsc";
        job["parameters"]=parameters;

        QDir theDirectory(directoryName);
        QString dirName = theDirectory.dirName();

        QString remoteDirectory = remoteHomeDirPath + QString("/") + dirName;

        QJsonObject inputs;

        inputs["inputDirectory"]=remoteDirectory;
        job["inputs"]=inputs;

        // disable the button while the job is being uploaded and started
        pushButton->setEnabled(false);

        emit startJobCall(job);

        // now remove the tmp directory
        theDirectory.removeRecursively();
    }
}


void RemoteJobCreator::setInputDirectory(const QString &name)
{
    directoryName = name;
}

void
RemoteJobCreator::attemptLoginReturn(bool ok) {
    if (ok == true) {
        emit getHomeDirCall();
    }
}


void
RemoteJobCreator::getHomeDirReturned(QString path){
    remoteHomeDirPath = path;
}

void
RemoteJobCreator::startJobReturn(QString result) {
   pushButton->setEnabled(true);
   this->hide();
}
