#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

#include <QMainWindow>
#include <QItemSelection>
#include <QTreeView>
#include <QStandardItemModel>

class SidebarWidgetSelection;
class SimCenterWidget;
class InputWidgetFEM;
class UQ_EngineSelection;
class InputWidgetEDP;
class InputWidgetParameters;
class UQ_Results;

class AgaveCurl;
class RemoteJobCreator;
class RemoteJobManager;
class QPushButton;
class QLabel;
class QThread;
class QNetworkAccessManager;
class QNetworkReply;
class SimCenterPreferences;

class MainWindow : public QMainWindow
{
  Q_OBJECT
    
    public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  QLabel *errorLabel;


signals:
    void attemptLogin(QString, QString);
    void logout();

  public slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();

    void onRunButtonClicked();
    void onRemoteRunButtonClicked();
    void onJobsManagerButtonClicked();
    void onExitButtonClicked();

    void onLoginButtonClicked();
    void onLoginSubmitButtonClicked();

    void attemptLoginReturn(bool);
    void logoutReturn(bool);

    void onUQ_EngineChanged(bool);

    void errorMessage(QString message);
    void fatalMessage(QString message);

    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);
    void processResults(QString &filename1, QString & filename2);

    void about();
    void manual();
    void submitFeedback();
    void version();
    void preferences();
    void copyright();
    void cite();

 private:
    void setCurrentFile(const QString &fileName);
    int runApplication(QString app, QStringList args);

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);

    void createActions();

    //Ui::MainWindow *ui;

    QString currentFile;
    SidebarWidgetSelection *inputWidget;

    InputWidgetFEM *fem;
    UQ_EngineSelection *uq;
    InputWidgetParameters *random;
    UQ_Results *results;
    InputWidgetEDP *edp;

    AgaveCurl *theRemoteInterface;
    RemoteJobCreator *jobCreator;
    RemoteJobManager *jobManager;

    bool loggedIn;
    QWidget *loginWindow;            // popup window for when login clicked
    QLineEdit *nameLineEdit;         // username line edit
    QLineEdit *passwordLineEdit;     // password line edit
    int numTries;
    QPushButton *loginButton;         // login button on main screen
    QPushButton *loginSubmitButton;   // submit button on login screen
    QPushButton *runDesignSafeButton;
    QThread *thread;
    QNetworkAccessManager *manager;

    SimCenterPreferences *thePreferences;
    QString workingDirectory;
};

#endif // MAINWINDOW_H
