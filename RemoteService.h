#ifndef REMOTE_INTERFACE_H
#define REMOTE_INTERFACE_H

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
 *  This is the RemoteService Interface. It provides the abstraction of the
 *  remote service. the slot methods must be implemented by the concrete subclasses.
 *  interface by interacting with the agave interface using curl calls. These
 *  are synchronnous, so the UI will appear dead while calls ongoing! 
 */

#include <QObject>

#include <QString>
#include <QJsonObject>

class RemoteService: public QObject
{
        Q_OBJECT
public:
    explicit RemoteService(QObject *parent = nullptr);
    virtual ~RemoteService();

    virtual bool isLoggedIn() =0;

public slots:
    // login
    virtual void loginCall(QString login, QString password) =0;
    virtual void logoutCall() =0;

    // file system
    virtual void mkdirCall(const QString &remoteName, const QString &remotePath) =0;
    virtual void uploadFileCall(const QString &local, const QString &remote) =0;
    virtual void downloadFilesCall(const QStringList &remote, const QStringList &local) =0;
    virtual void uploadDirectoryCall(const QString &local, const QString &remote) =0;
    // void downloaDirectoryCall(const QString &remote, const QString &local) =0;
    virtual void removeDirectoryCall(const QString &remote) =0;
    virtual void getHomeDirPathCall(void) =0;

    // jobs
    virtual void startJobCall(const QJsonObject &theJob) =0;
    virtual void getJobListCall(const QString &matchingName) =0;
    virtual void getJobDetailsCall(const QString &jobID) =0;
    virtual void getJobStatusCall(const QString &jobID) =0;
    virtual void deleteJobCall(const QString &jobID, const QStringList &dirToRemove) =0;

signals:
    void statusMessage(QString) ;
    void errorMessage(QString) ;
    void fatalMessage(QString) ;

    // login
    void loginReturn(bool ok) ;
    void logoutReturn(bool ok) ;

    // filesystem
    void mkdirReturn(bool) ;
    void uploadFileReturn(bool) ;
    void downloadFilesReturn(bool) ;
    void uploadDirectoryReturn(bool) ;
    void downloaDirectoryReturn(bool) ;
    void removeDirectoryReturn(bool) ;
    void getHomeDirPathReturn(QString) ;

    // jobs
    void startJobReturn(QString) ;
    void getJobListReturn(QJsonObject) ;
    void getJobDetailsReturn(QJsonObject) ;
    void getJobStatusReturn(QString) ;
    void deleteJobReturn(bool) ;

private:

};

#endif // REMOTE_INTERFACE_H
