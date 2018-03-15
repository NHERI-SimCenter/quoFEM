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


#include "RemoteJobManagerWidget.h"
#include <AgaveInterface.h>
#include <QJsonDocument>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QJsonArray>
#include <QTableWidget>
#include <MainWindow.h>
#include <QTemporaryFile>

#include <QMenu>

#include  <QDebug>


RemoteJobManagerWidget::RemoteJobManagerWidget(AgaveInterface *theInt, MainWindow *theMain, QWidget *parent)
    : QWidget(parent), theInterface(theInt), theMainWindow(theMain), triggeredRow(-1)
{
    jobsTable = 0;
    htmlInputDirectory = QString("agave://designsafe.storage.default/");

   // QJsonDocument doc2(jobs);
   // qDebug() << doc2.toJson();
}

bool
RemoteJobManagerWidget::updateJobTable(void){

    jobs = theInterface->getJobList(QString(""));

    if (jobsTable == 0) {
        headers << "Name" << "STATUS" << "ID" << "Date Created";
        jobsTable=new QTableWidget(this);
        jobsTable->setColumnCount(4);
        jobsTable->setHorizontalHeaderLabels(headers);
    } else
        jobsTable->setRowCount(0);

    QJsonArray jobData=jobs["jobs"].toArray();
    int numJobs = jobData.count();
    jobsTable->setRowCount(numJobs);

    for (int i=0; i<numJobs; i++) {
        QJsonObject job=jobData.at(i).toObject();
        QString jobID = job["id"].toString();
        QString jobName = job["name"].toString();
        QString jobStatus = job["status"].toString();
        QString jobDate = job["created"].toString();
        jobsTable->setItem(i, 2, new QTableWidgetItem(jobID));
        jobsTable->setItem(i, 1, new QTableWidgetItem(jobStatus));
        jobsTable->setItem(i, 3, new QTableWidgetItem(jobDate));
        jobsTable->setItem(i, 0, new QTableWidgetItem(jobName));
    }

    // connect(jobsTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customFileMenu(QPoint)));
    connect(jobsTable, SIGNAL(cellClicked(int,int)),this,SLOT(bringUpJobActionMenu(int,int)));
    connect(jobsTable, SIGNAL(cellPressed(int,int)),this,SLOT(bringUpJobActionMenu(int,int)));
    QLayout *layout = new QHBoxLayout();
    layout->addWidget(jobsTable);
    this->setLayout(layout);

}


void
RemoteJobManagerWidget::bringUpJobActionMenu(int row, int col){

    triggeredRow = row;
    QMenu jobMenu;

    jobMenu.addAction("Refresh Job", this, SLOT(updateJobStatus()));
    jobMenu.addAction("Retrive Data", this, SLOT(getJobData()));
    jobMenu.addSeparator();
    jobMenu.addAction("Delete Job", this, SLOT(deleteJob()));
    jobMenu.addAction("Delete Job And Data", this, SLOT(deleteJobAndData()));

    jobMenu.exec(QCursor::pos());
}


void
RemoteJobManagerWidget::updateJobStatus(void)
{
    if (triggeredRow != -1) {
        QTableWidgetItem *itemID=jobsTable->item(triggeredRow,2);
        QString jobID = itemID->text();
        QString status=theInterface->getJobStatus(jobID);
        QTableWidgetItem *itemStatus=jobsTable->item(triggeredRow,1);
        itemStatus->setText(status);
    }
    triggeredRow = -1;
}

void
RemoteJobManagerWidget::deleteJob(void){

    if (triggeredRow != -1) {
        QTableWidgetItem *itemID=jobsTable->item(triggeredRow,2);
        QString jobID = itemID->text();
        bool result = theInterface->deleteJob(jobID);
        if (result == true)
            jobsTable->removeRow(triggeredRow);
    }
    triggeredRow = -1;
}

void
RemoteJobManagerWidget::deleteJobAndData(void){
    bool result = false;

    if (triggeredRow != -1) {

        //
        // get jobID, from interface obtain information about where the input and archived data is located
        // & then remove the directories
        //   NOTE SHOULD probably check job status not RUNNING befre doing this
        //

        QTableWidgetItem *itemID=jobsTable->item(triggeredRow,2);
        QString jobID = itemID->text();
        QJsonObject job=theInterface->getJobDetails(jobID);
        QJsonValue archivePath = job["archivePath"];
        if (archivePath.isString()) {
            QString archiveDir = archivePath.toString();
            theInterface->removeDirectory(archiveDir);
            // qDebug() << "deleteJobAndData: archiveDir: " << archiveDir;
        }
        QJsonValue inputs = job["inputs"];

        if (inputs.isObject()) {

             QJsonObject inputObject = inputs.toObject();
             QJsonValue inputPath = inputObject["inputDirectory"];
             if (inputPath.isArray()) {
                 QString inputDir = inputPath.toArray().at(0).toString();
                 inputDir.remove(htmlInputDirectory);
                 theInterface->removeDirectory(inputDir);
                 //qDebug() << "deleteJobAndData: inputDir: " << inputDir;
             }
        }

        bool result = theInterface->deleteJob(jobID);
        if (result == true)
            jobsTable->removeRow(triggeredRow);
    }
    triggeredRow = -1;
}

void
RemoteJobManagerWidget::getJobData(void) {

    bool result = false;

    if (triggeredRow != -1) {

        QString archiveDir = "";
        QString inputDir = "";

        //
        // get jobID, from interface obtain information about where the output directory will be
        //

        QTableWidgetItem *itemID=jobsTable->item(triggeredRow,2);
        QString jobID = itemID->text();
        QJsonObject job=theInterface->getJobDetails(jobID);
        QJsonValue archivePath = job["archivePath"];
        if (archivePath.isString()) {
            archiveDir = archivePath.toString();
        }
        QJsonValue inputs = job["inputs"];
        if (inputs.isObject()) {

             QJsonObject inputObject = inputs.toObject();
             QJsonValue inputPath = inputObject["inputDirectory"];
             if (inputPath.isArray()) {
                 inputDir = inputPath.toArray().at(0).toString();
                 inputDir.remove(htmlInputDirectory);
             }
        }

        archiveDir = archiveDir + QString("/") + inputDir.remove(QRegExp(".*\/")); // regex to remove up till last /

        //
        // create 2 temp file names neede to store remote data files locally
        //

        QTemporaryFile tmpFile1;
        QString name1, name2;
        if (tmpFile1.open()) {
            name1 = tmpFile1.fileName();
            tmpFile1.close();
        } else {
             // will have to overwrite any local dakota.in
             name1 = "dakota.in";
        }

        QTemporaryFile tmpFile2;
        if (tmpFile2.open()) {
            name2 = tmpFile2.fileName();
            tmpFile2.close();
        } else {
            // will have to overwrite any local dakotaTab.out;
            name2 = "dakotaTab.out";
        }

        //
        // download data to temp files & then process them as normal
        //

        QString dakotaJSON = archiveDir + QString("/templatedir/dakota.json");
        QString dakotaOUT = archiveDir + QString("/dakota.out");
        QString dakotaTAB = archiveDir + QString("/dakotaTab.out");

        // first download the input data & load it
        theInterface->downloadFile(dakotaJSON, name1);
        theMainWindow->loadFile(name1);

         // now download the output data & process it
        theInterface->downloadFile(dakotaOUT, name1);
        theInterface->downloadFile(dakotaTAB, name2);
        theMainWindow->processResults(name1, name2);
    }

    triggeredRow = -1;
    this->close();
}


