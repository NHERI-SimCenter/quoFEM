#ifndef AGAVE_CURL_H
#define AGAVE_CURL_H

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

/**
 *  @author  fmckenna
 *  @date    2/2017
 *  @version 1.0
 *
 *  @section DESCRIPTION
 *
 *  This is a concrete implementation of the AgaveInterface. It implements the
 *  interface by interacting with the agave interface using curl calls. These
 *  are synchronnous, so the UI will appear dead while calls ongoing!
 */

//#include <QObject>
//#include "AgaveInterface.h"
#include <RemoteService.h>
#include <curl/curl.h>

class QProcess;
class QWidget;
class QDialog;
class QLineEdit;
#include <QJsonObject>

//class QFutureWatcher;

//typedef void (*errorFunc)(const QString &message);

class AgaveCurl: public RemoteService
{
        Q_OBJECT

public:

  explicit AgaveCurl(QString &tenant, QString &storage, QString *appDirName = nullptr, QObject *parent = nullptr);
    ~AgaveCurl();

    bool login(QString login, QString password);
    bool logout();
    bool isLoggedIn();

    // methods needed atomic file/dir operations
    // methods needed for file/dir operations
    QJsonObject remoteLS(const QString &remotePath);
    bool mkdir(const QString &remoteName, const     QString &remotePath);
    bool uploadFile(const QString &local, const QString &remote);
    bool downloadFile(const QString &remote, const QString &local);
    bool uploadDirectory(const QString &local, const QString &remote);
  //  bool downloaDirectory(const QString &remote, const QString &local);
    bool removeDirectory(const QString &remote);

    virtual QString getHomeDirPath(void);

    // methods needed to start a job
    QString startJob(const QString &jobDescription);
    QString startJob(const QJsonObject &theJob);

    // methods neeed for dealing with remote jobs
    QJsonObject getJobList(const QString &matchingName);
    QJsonObject getJobDetails(const QString &jobID);
    QString getJobStatus(const QString &jobID);
    bool deleteJob(const QString &jobID, const QStringList &dirToRemove);

public slots:
    // login
    void loginCall(QString login, QString password);
    void logoutCall();

   // void loginSubmitButtonClicked(void);

    // file system
    void mkdirCall(const QString &remoteName, const QString &remotePath);
    void uploadFileCall(const QString &local, const QString &remote);
    void downloadFilesCall(const QStringList &remote, const QStringList &local);
    void uploadDirectoryCall(const QString &local, const QString &remote);
    // void downloaDirectoryCall(const QString &remote, const QString &local);
    void removeDirectoryCall(const QString &remote);
    void getHomeDirPathCall(void);

    // jobs
    void startJobCall(const QJsonObject &theJob);
    void getJobListCall(const QString &matchingName);
    void getJobDetailsCall(const QString &jobID);
    void getJobStatusCall(const QString &jobID);
    void deleteJobCall(const QString &jobID, const QStringList &dirToRemove);

private:
    // private methods
    bool invokeCurl(void);

    // variable thet are Agave specific
    QString tenant;
    QString storage;

    // need two unique file names, use these for temp storage while running
    QString uniqueFileName1;
    QString uniqueFileName2;

    // variables for login & login widget
    bool loggedInFlag;

    CURL *hnd;
    struct curl_slist *slist1;
    struct curl_slist *slist2;

    QString tenantURL;
    QString appClient;

    QString username;
    QString password;
    QString appDirName;
    QString bearer;

    bool slotNeededLocally;
    bool slotResultBool;

    // variables for login & login widget
    QDialog *loginWindow;
    QLineEdit *nameLineEdit;
    QLineEdit *passwordLineEdit;
    int numTries;

   // QFutureWatcher *fileUploadWatcher;

};

#endif // AGAVE_CURL_H
