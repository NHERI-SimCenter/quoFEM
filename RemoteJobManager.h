#ifndef REMOTEJOBMANAGER_H
#define REMOTEJOBMANAGER_H

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

#include <QWidget>
#include <QString>
#include <QJsonObject>
#include <QStringList>
#include <QString>
class MainWindow;

//#include <AgaveCurl.h>

class MainWindow;
class QTableWidget;
class QPushButton;
class AgaveCurl;

class RemoteJobManager : public QWidget
{
    Q_OBJECT
public:

    explicit RemoteJobManager(AgaveCurl *theInterface, MainWindow *, QWidget *parent = nullptr);
    bool addJob(QString &jobID);
    void clearTable(void);

signals:
    void statusMessage(QString);
    void errorMessage(QString);

    void getJobsList(QString);
    void getJobStatus(QString);
    void getJobDetails(QString);
    void deleteJob(QString, QStringList);
    void downloadFiles(QStringList, QStringList);

  //  void deleteDirectory(QString);
  //  void downloadFile(QString);

public slots:
    void updateJobTable(QString);
    void jobsListReturn(QJsonObject);
    void jobStatusReturn(QString);
    void deleteJobReturn(bool);
    void getJobDetailsReturn(QJsonObject);


    // void deleteDirectoryReturn(bool);
    // void jobDetailsReturn(QJsonObject);
    void downloadFilesReturn(bool);

public slots:
    void bringUpJobActionMenu(int row, int col);
    void updateJobStatus(void);
    void deleteJob(void);
    void deleteJobAndData(void);
    void getJobData(void);

private:
    QString inputDirectory;
   // AgaveCurl *theInterface;
    QJsonObject jobs;

    QTableWidget *jobsTable;
    QStringList  headers;
    int triggeredRow;
    QString htmlInputDirectory;

    QPushButton *pushButton;

    QString jobIDRequest;
    int getJobDetailsRequest;

    QString name1;
    QString name2;
    QString name3;
    QString name4;
    QString name5;
    MainWindow *theMainWindow;
};

#endif // REMOTEJOBMANAGER_H
