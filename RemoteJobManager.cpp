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


#include "RemoteJobManager.h"
#include <AgaveCurl.h>
#include <QJsonDocument>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QJsonArray>
#include <QTableWidget>
#include <MainWindow.h>
#include <QTemporaryFile>
#include <QHeaderView>

#include <QMenu>

#include  <QDebug>
#include "AgaveCurl.h"
#include <MainWindow.h>

RemoteJobManager::RemoteJobManager(AgaveCurl *theRemoteInterface, MainWindow *theMain, QWidget *parent)
    : QWidget(parent), theMainWindow(theMain), triggeredRow(-1)
{
    QVBoxLayout *layout = new QVBoxLayout();

    jobsTable = 0;
    htmlInputDirectory = QString("agave://designsafe.storage.default/");
    headers << "Name" << "STATUS" << "ID" << "Date Created";
    jobsTable=new QTableWidget(this);
    jobsTable->setColumnCount(4);
    jobsTable->setHorizontalHeaderLabels(headers);
    jobsTable->setRowCount(0);

    QHeaderView* header = jobsTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(jobsTable, 1.0);
    //jobsTable->setSizePolicy(QSizePolicy::Ignored);
    this->setLayout(layout);
    //jobsTable->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    this->setMinimumHeight(200);
    this->setMinimumWidth(200);
    connect(jobsTable, SIGNAL(cellClicked(int,int)),this,SLOT(bringUpJobActionMenu(int,int)));
    connect(jobsTable, SIGNAL(cellPressed(int,int)),this,SLOT(bringUpJobActionMenu(int,int)));

    // update jobs list
    connect(this,SIGNAL(getJobsList(QString)),theRemoteInterface,SLOT(getJobListCall(QString)));
    connect(theRemoteInterface,SIGNAL(getJobListReturn(QJsonObject)), this,SLOT(jobsListReturn(QJsonObject)));

    // update job status
    connect(this,SIGNAL(getJobStatus(QString)),theRemoteInterface,SLOT(getJobStatusCall(QString)));
    connect(theRemoteInterface,SIGNAL(getJobStatusReturn(QString)),this,SLOT(jobStatusReturn(QString)));

    // getJobDetails
    connect(this,SIGNAL(getJobDetails(QString)), theRemoteInterface,SLOT(getJobDetailsCall(QString)));
    connect(theRemoteInterface,SIGNAL(getJobDetailsReturn(QJsonObject)),this,SLOT(getJobDetailsReturn(QJsonObject)));

    // delete job
    connect(this,SIGNAL(deleteJob(QString,QStringList)),theRemoteInterface,SLOT(deleteJobCall(QString,QStringList)));
    connect(theRemoteInterface,SIGNAL(deleteJobReturn(bool)), this,SLOT(deleteJobReturn(bool)));

    // download files
    connect(this,SIGNAL(downloadFiles(QStringList,QStringList)), theRemoteInterface,SLOT(downloadFilesCall(QStringList,QStringList)));
    connect(theRemoteInterface,SIGNAL(downloadFilesReturn(bool)),this,SLOT(downloadFilesReturn(bool)));
}

void
RemoteJobManager::clearTable(void){
    jobsTable->setRowCount(0);

}

void
RemoteJobManager::updateJobTable(QString appName){

    emit getJobsList(appName);
   // jobs = theInterface->getJobList(QString(""));
}

void
RemoteJobManager::jobsListReturn(QJsonObject theJobs){

    jobs = theJobs;

    if (jobs.contains("jobs")) {
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
    }
     //jobsTable->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
     this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

   // jobsTable->resizeRowsToContents();
   // jobsTable->setSizePolicy(QSizePolicy::Ignored);
    return;
}


void
RemoteJobManager::bringUpJobActionMenu(int row, int col){

    triggeredRow = row;
    QMenu jobMenu;

    jobMenu.addAction("Refresh Job", this, SLOT(updateJobStatus()));
    jobMenu.addAction("Retrieve Data", this, SLOT(getJobData()));
    jobMenu.addSeparator();
    jobMenu.addAction("Delete Job", this, SLOT(deleteJob()));
    jobMenu.addAction("Delete Job And Data", this, SLOT(deleteJobAndData()));

    jobMenu.exec(QCursor::pos());
}


void
RemoteJobManager::updateJobStatus(void)
{
    if (triggeredRow != -1) {
        QTableWidgetItem *itemID=jobsTable->item(triggeredRow,2);
        QString jobID = itemID->text();
        //QString status=theInterface->getJobStatus(jobID);
        emit getJobStatus(jobID);
    }
}

void
RemoteJobManager::jobStatusReturn(QString status) {
    if (triggeredRow != -1) {
        QTableWidgetItem *itemStatus=jobsTable->item(triggeredRow,1);
        itemStatus->setText(status);
    }
    triggeredRow = -1;
}


void
RemoteJobManager::deleteJob(void){

    if (triggeredRow != -1) {
        QStringList noDirToRemove;
        QTableWidgetItem *itemID=jobsTable->item(triggeredRow,2);
        QString jobID = itemID->text();
//        bool result = theInterface->deleteJob(jobID);
        emit deleteJob(jobID, noDirToRemove);
    }
}

void
RemoteJobManager::deleteJobReturn(bool result) {
    if (result == true)
        jobsTable->removeRow(triggeredRow);
    triggeredRow = -1;
}


void
RemoteJobManager::deleteJobAndData(void){
    bool result = false;

    if (triggeredRow != -1) {

        //
        // get jobID, from interface obtain information about where the input and archived data is located
        // & then remove the directories
        //   NOTE SHOULD probably check job status not RUNNING befre doing this
        //



        QTableWidgetItem *itemID=jobsTable->item(triggeredRow,2);

        jobIDRequest = itemID->text();
        getJobDetailsRequest = 1;
        emit getJobDetails(jobIDRequest);
    }
}

void
RemoteJobManager::getJobDetailsReturn(QJsonObject job)  {


    if (getJobDetailsRequest == 1) {
        //
        // the request was from deleteJobAndData
        //   - request the deletion of archive and input directories along with the job
        //
        QStringList dirToRemove;
        QJsonValue archivePath = job["archivePath"];
        if (archivePath.isString()) {
            QString archiveDir = archivePath.toString();
            dirToRemove << archiveDir;
        }
        QJsonValue inputs = job["inputs"];

        if (inputs.isObject()) {

            QJsonObject inputObject = inputs.toObject();
            QJsonValue inputPath = inputObject["inputDirectory"];
            if (inputPath.isArray()) {
                QString inputDir = inputPath.toArray().at(0).toString();
                inputDir.remove(htmlInputDirectory);
                dirToRemove << inputDir;
            }
        }
        emit deleteJob(jobIDRequest, dirToRemove);
    }

     if (getJobDetailsRequest == 2) {

         QString archiveDir;
         QString inputDir;
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
       // QString name1, name2, name3;

        if (tmpFile1.open()) {
            name1 = tmpFile1.fileName();
            tmpFile1.close();
        } else {
             // will have to overwrite any local dakota.in
             name1 = "dakota.json";
        }

        QTemporaryFile tmpFile2;
        if (tmpFile2.open()) {
            name2 = tmpFile2.fileName();
            tmpFile2.close();
        } else {
            // will have to overwrite any local dakotaTab.out;
            name2 = "dakota.out";
        }

        QTemporaryFile tmpFile3;
        if (tmpFile3.open()) {
            name3 = tmpFile3.fileName();
            tmpFile3.close();
        } else {
            // will have to overwrite any local dakotaTab.out;
            name3 = "dakotaTab.out";
        }
        QStringList localFiles;
        localFiles.append(name1);
        localFiles.append(name2);
        localFiles.append(name3);

        //
        // download data to temp files & then process them as normal
        //

        QString dakotaJSON = archiveDir + QString("/templatedir/dakota.json");
        QString dakotaOUT = archiveDir + QString("/dakota.out");
        QString dakotaTAB = archiveDir + QString("/dakotaTab.out");

        // first download the input data & load it
        QStringList filesToDownload;
        filesToDownload.append(dakotaJSON);
        filesToDownload.append(dakotaOUT);
        filesToDownload.append(dakotaTAB);

        emit downloadFiles(filesToDownload, localFiles);

        //theInterface->downloadFile(dakotaJSON, name1);
        //theMainWindow->loadFile(name1);

         // now download the output data & process it
        //theInterface->downloadFile(dakotaOUT, name1);
       // theInterface->downloadFile(dakotaTAB, name2);
       // theMainWindow->processResults(name1, name2);
     }
}

void
RemoteJobManager::downloadFilesReturn(bool result)
{
    qDebug() << " name1: " << name1 << "name2: " << name2 << " name3: " << name3;
    theMainWindow->loadFile(name1);
    theMainWindow->processResults(name2, name3);
    this->hide();
}

void
RemoteJobManager::getJobData(void) {

    bool result = false;

    if (triggeredRow != -1) {

        QString archiveDir = "";
        QString inputDir = "";

        //
        // get jobID, from interface obtain information about where the output directory will be
        //

        QTableWidgetItem *itemID=jobsTable->item(triggeredRow,2);
        jobIDRequest = itemID->text();
        getJobDetailsRequest = 2;
        emit getJobDetails(jobIDRequest);
    }
    triggeredRow = -1;
}




