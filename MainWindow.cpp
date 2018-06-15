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

#include <QTreeView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QDebug>
#include "MainWindow.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMenuBar>
#include <QAction>
#include <QMenu>
#include <QApplication>

#include "SidebarWidgetSelection.h"

#include <InputWidgetEDP.h>
#include <InputWidgetFEM.h>
#include <InputWidgetUQ.h>
#include <DakotaResults.h>
#include <InputWidgetParameters.h>
#include <RandomVariableInputWidget.h>

#include <DakotaResultsSampling.h>

#include <QVBoxLayout>
#include <HeaderWidget.h>
#include <FooterWidget.h>
#include <QPushButton>
#include <InputWidgetFEM.h>
#include <InputWidgetUQ.h>
//#include <InputWidgetEDP.h>
#include <QFileInfo>
#include <QProcess>
#include <QDesktopWidget>
#include <QUuid>
#include <QLabel>
#include <QLineEdit>


//#include <AgaveCLI.h>
#include <AgaveCurl.h>
#include <RemoteJobCreator.h>
#include <RemoteJobManager.h>
#include <QThread>

#include <QDesktopServices>

/*
static
MainWindow::MainWindow *theOneStaticMainWindow = 0;

void
MainWindow::errorMessage(const QString msg){
    //qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    theOneStaticMainWindow->errorLabel->setText(msg);
    qDebug() << "ERROR MESSAGE" << msg;
}

void warningMessage(const QStringList &msg){

}
void updateMessage(const QStringList &msg)
{

}
*/

void
MainWindow::errorMessage(const QString msg){
    //qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    errorLabel->setText(msg);
    qDebug() << "ERROR MESSAGE" << msg;
}

void
MainWindow::fatalMessage(const QString msg){
    //qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    errorLabel->setText(msg);
    qDebug() << "ERROR MESSAGE" << msg;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), loggedIn(false)
{
   // theOneStaticMainWindow = this;

    //
    // create the interface, jobCreator and jobManager
    //
    QString tenant("designsafe");
    QString storage("agave://designsafe.storage.default/");

    //theRemoteInterface = new AgaveCLI(tenant, storage, this);
    theRemoteInterface =  new AgaveCurl(tenant, storage);
    jobCreator = new RemoteJobCreator(theRemoteInterface);
    jobManager = new RemoteJobManager(theRemoteInterface, this);


    //
    // create a layout & widget for central area of this QMainWidget
    //  to this widget we will add a header, selection, button and footer widgets
    //

    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    centralWidget->setLayout(layout);

    QRect rec = QApplication::desktop()->screenGeometry();

    int height = 0.7*rec.height();
    int width = 0.7*rec.width();

    this->resize(width, height);


    //
    // add SimCenter Header
    //

    HeaderWidget *header = new HeaderWidget();
    header->setHeadingText(tr("uqFEM Application"));
    layout->addWidget(header);

    // place a location for messages;
    QHBoxLayout *layoutMessages = new QHBoxLayout();
    errorLabel = new QLabel();
    layoutMessages->addWidget(errorLabel);
    header->appendLayout(layoutMessages);

    // place login info
    QHBoxLayout *layoutLogin = new QHBoxLayout();
    QLabel *name = new QLabel();
    //name->setText("");
    loginButton = new QPushButton();
    loginButton->setText("Login");
    layoutLogin->addWidget(name);
    layoutLogin->addWidget(loginButton);
    layoutLogin->setAlignment(Qt::AlignLeft);
    header->appendLayout(layoutLogin);

    //
    // create a widget for selection:
    //
    // first create the main widgets for input:
    //   fem - for obtaiinging FEM info
    //   random - for the random variable definitions
    //   uq - for the UQ or optimization method
    //   results - to display the results
    //
    // then we add these to the selection widget, something that will allow users to switch between
    // these input widges & add the input widets to this selection widget
    //
    // finally we add new selection widget to layout

    //the input widgets
    uq = new InputWidgetUQ();
    random = new InputWidgetParameters();
    fem = new InputWidgetFEM(random);
    random->setParametersWidget(uq->getParameters());

    // create selection widget & add the input widgets
    results = new DakotaResults();

    inputWidget = new SidebarWidgetSelection();
    inputWidget->addInputWidget(tr("FEM Selection"), fem);
    inputWidget->addInputWidget(tr("Method Selection"), uq);
    inputWidget->addInputWidget(tr("Input Variables"), random);
    inputWidget->addInputWidget(tr("Results"), results);

    // let ubput widget know end of ptions, then set initial input to fem
    inputWidget->buildTreee();
    inputWidget->setSelection(tr("FEM Selection"));

    // add selection widget to the central layout previosuly created
    layout->addWidget(inputWidget,1.0);

    //
    // add run, run-DesignSafe and exit buttons into a new widget for buttons
    //

    // create the buttons widget and a layout for it
    QHBoxLayout *pushButtonLayout = new QHBoxLayout();
    QWidget *buttonWidget = new QWidget();
    buttonWidget->setLayout(pushButtonLayout);

    // create a bunch of buttons

    QPushButton *runButton = new QPushButton();
    runButton->setText(tr("RUN"));
    pushButtonLayout->addWidget(runButton);

    QPushButton *runDesignSafeButton = new QPushButton();
    runDesignSafeButton->setText(tr("RUN at DesignSafe"));
    pushButtonLayout->addWidget(runDesignSafeButton);

    QPushButton *getDesignSafeButton = new QPushButton();
    getDesignSafeButton->setText(tr("GET from DesignSafe"));
    pushButtonLayout->addWidget(getDesignSafeButton);

    QPushButton *exitButton = new QPushButton();
    exitButton->setText(tr("Exit"));
    pushButtonLayout->addWidget(exitButton);

    //
    // finally create login window for when loogged in clicked
    //

    loginWindow = new QWidget();
    QGridLayout *loginLayout = new QGridLayout();
    QLabel *nameLabel = new QLabel();
    nameLabel->setText("username:");
    QLabel *passwordLabel = new QLabel();
    passwordLabel->setText("password:");
    nameLineEdit = new QLineEdit();
    passwordLineEdit = new QLineEdit();
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    loginSubmitButton = new QPushButton();
    loginSubmitButton->setText("Login");
    loginLayout->addWidget(nameLabel,0,0);
    loginLayout->addWidget(nameLineEdit,0,1);
    loginLayout->addWidget(passwordLabel,1,0);
    loginLayout->addWidget(passwordLineEdit,1,1);
    loginLayout->addWidget(loginSubmitButton,2,2);
    loginWindow->setLayout(loginLayout);

    //
    // connect signals & slots
    //

    // error & status messages
    connect(theRemoteInterface,SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));
    connect(theRemoteInterface,SIGNAL(statusMessage(QString)), this, SLOT(errorMessage(QString)));
    connect(theRemoteInterface,SIGNAL(fatalMessage(QString)), this, SLOT(fatalMessage(QString)));
    connect(fem,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(random,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(results,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(uq,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));

    // login
    connect(loginButton,SIGNAL(clicked(bool)),this,SLOT(onLoginButtonClicked()));
    connect(loginSubmitButton,SIGNAL(clicked(bool)),this,SLOT(onLoginSubmitButtonClicked()));

    connect(this,SIGNAL(attemptLogin(QString, QString)),theRemoteInterface,SLOT(loginCall(QString, QString)));
    connect(theRemoteInterface,SIGNAL(loginReturn(bool)),this,SLOT(attemptLoginReturn(bool)));


    // logout
    connect(this,SIGNAL(logout()),theRemoteInterface,SLOT(logoutCall()));
    connect(theRemoteInterface,SIGNAL(logoutReturn(bool)),this,SLOT(logoutReturn(bool)));

    // connect job manager
    connect(runButton, SIGNAL(clicked(bool)),this,SLOT(onRunButtonClicked()));
    connect(runDesignSafeButton, SIGNAL(clicked(bool)),this,SLOT(onRemoteRunButtonClicked()));
    connect(getDesignSafeButton, SIGNAL(clicked(bool)),this,SLOT(onJobsManagerButtonClicked()));

    // exit
    connect(exitButton, SIGNAL(clicked(bool)),this,SLOT(onExitButtonClicked()));

    connect(uq,SIGNAL(uqWidgetChanged()), this,SLOT(onDakotaMethodChanged()));

    // add button widget to layout
    layout->addWidget(buttonWidget);

    //
    // add SimCenter footer
    //

    FooterWidget *footer = new FooterWidget();
    layout->addWidget(footer);

    this->setCentralWidget(centralWidget);

    this->createActions();

    thread = new QThread();
    theRemoteInterface->moveToThread(thread);

    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();

}

MainWindow::~MainWindow()
{
    // invoke destructor as AgaveCLI not a QObject
    //delete theRemoteInterface;
    thread->quit();
    theRemoteInterface->deleteLater();

    delete jobCreator;
    delete jobManager;
}

bool copyPath(QString sourceDir, QString destinationDir, bool overWriteDirectory)
{
    QDir originDirectory(sourceDir);

    if (! originDirectory.exists())
    {
        qDebug() << "Origin Directory: " << sourceDir << " Does not exist";
        return false;
    }

    QDir destinationDirectory(destinationDir);

    if(destinationDirectory.exists() && !overWriteDirectory)
    {
        return false;
    }


    else if(destinationDirectory.exists() && overWriteDirectory)
    {
        destinationDirectory.removeRecursively();
    }

    originDirectory.mkpath(destinationDir);

    foreach (QString directoryName, originDirectory.entryList(QDir::Dirs | \
                                                              QDir::NoDotAndDotDot))
    {
        if (directoryName != QString("tmp.SimCenter")) {
        QString destinationPath = destinationDir + "/" + directoryName;
        originDirectory.mkpath(destinationPath);
        copyPath(sourceDir + "/" + directoryName, destinationPath, overWriteDirectory);
        }
    }

    foreach (QString fileName, originDirectory.entryList(QDir::Files))
    {
        QFile::copy(sourceDir + "/" + fileName, destinationDir + "/" + fileName);
    }

    /*! Possible race-condition mitigation? */
    QDir finalDestination(destinationDir);
    finalDestination.refresh();

    if(finalDestination.exists())
    {
        return true;
    }

    return false;
}

void MainWindow::onRunButtonClicked() {

    errorMessage("");

    //
    // get program & input file from fem widget
    //

    QString application = fem->getApplicationName();
    QString mainInput = fem->getMainInput();

    QFileInfo fileInfo(mainInput);
    QDir fileDir = fileInfo.absolutePath();
    QString fileName =fileInfo.fileName();
    QString path = fileDir.absolutePath();// + QDir::separator();

    //
    // given path to input file we are going to create temporary directory below it
    // and copy all files from this input file directory to the new subdirectory
    //

    QString tmpDirectory = path + QDir::separator() + QString("tmp.SimCenter") + QDir::separator() + QString("templatedir");
    copyPath(path, tmpDirectory, false);

    // special copy the of the main script to set up lines containg parameters for dakota
    QString mainScriptTmp = tmpDirectory + QDir::separator() + fileName;
    fem->specialCopyMainInput(mainScriptTmp, random->getParametereNames());
    //
    // in new templatedir dir save the UI data into dakota.json file (same result as using saveAs)
    //

    QString filenameTMP = tmpDirectory + QDir::separator() + tr("dakota.json");

    QFile file(filenameTMP);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(filenameTMP),
                                  file.errorString()));
        return;
    }
    QJsonObject json;
    inputWidget->outputToJSON(json);
    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();

    //
    // now use the applications parseJSON file to run dakota and produce output files:
    //    dakota.in dakota.out dakotaTab.out dakota.err
    //

    QString homeDIR = QDir::homePath();
    QString appDIR = qApp->applicationDirPath();

  //   appDIR = homeDIR + QDir::separator() + QString("NHERI") + QDir::separator() + QString("uqFEM") +
  //    QDir::separator() + QString("localApp");

    //
    QString pySCRIPT = appDIR +  QDir::separator() + QString("parseJson3.py");
    QString tDirectory = path + QDir::separator() + QString("tmp.SimCenter");

    // remove current results widget

    results->setResultWidget(0);

    //
    // want to first remove old dakota files from the current directory
    //

    QString sourceDir = path + QDir::separator() + QString("tmp.SimCenter") + QDir::separator();
    QString destinationDir = path + QDir::separator();

    QStringList files;
    files << "dakota.in" << "dakota.out" << "dakotaTab.out" << "dakota.err";

    for (int i = 0; i < files.size(); i++) {
        QString copy = files.at(i);
        QFile file(destinationDir + copy);
        file.remove();
    }

    //
    // now invoke dakota, done via a python script in tool app dircetory
    //

    QProcess *proc = new QProcess();

#ifdef Q_OS_WIN
    QString command = QString("python ") + pySCRIPT + QString(" ") + tDirectory + QString(" ") + tmpDirectory  + QString(" runningLocal");
 qDebug() << command;
    proc->execute("cmd", QStringList() << "/C" << command);
    //   proc->start("cmd", QStringList(), QIODevice::ReadWrite);
    qDebug() << command;

    //std::cerr << command << "\n";
#else
   // QString command = QString("source $HOME/.bashrc; python ") + pySCRIPT + QString(" ") + tDirectory + QString(" ") +
   //         tmpDirectory + QString(" runningLocal");

    QString command = QString("python ") + pySCRIPT + QString(" ") + tDirectory + QString(" ") +
            tmpDirectory + QString(" runningLocal");

    proc->execute("bash", QStringList() << "-c" <<  command);
    qInfo() << command;
    // proc->start("bash", QStringList("-i"), QIODevice::ReadWrite);
#endif
    proc->waitForStarted();

    //
    // now copy results file from tmp.SimCenter directory and remove tmp directory
    //

   for (int i = 0; i < files.size(); i++) {
       QString copy = files.at(i);
       QFile::copy(sourceDir + copy, destinationDir + copy);
   }

   QDir dirToRemove(sourceDir);
   dirToRemove.removeRecursively(); // padhye 4/28/2018, this removes the temprorary directory
    //                                  // so to debug you can simply comment it

    //
    // process the results
    //

    QString filenameOUT = destinationDir + tr("dakota.out");
    QString filenameTAB = destinationDir + tr("dakotaTab.out");

    this->processResults(filenameOUT, filenameTAB);
}

void MainWindow::onRemoteRunButtonClicked(){

    // check logged in
    if (loggedIn == false) {
          errorMessage("ERROR - You Need to Login");
          return;
    }

    //
    // get program & input file from fem widget
    //

    QString application = fem->getApplicationName();
    QString mainInput = fem->getMainInput();

    QFileInfo fileInfo(mainInput);
    QDir fileDir = fileInfo.absolutePath();
    QString fileName =fileInfo.fileName();
    QString path = fileDir.absolutePath();// + QDir::separator();

    //
    // given path to input file we are going to create temporary directory below it
    // and copy all files from this input file directory to the new subdirectory
    //

    QUuid uniqueName = QUuid::createUuid();
    QString strUnique = uniqueName.toString();
    strUnique = strUnique.mid(1,36);

    QString tmpDirectory = path + QDir::separator() + QString("tmp.SimCenter") + strUnique + QDir::separator() + QString("templatedir");
    copyPath(path, tmpDirectory, false);

    // special copy the of the main script to set up lines containg parameters for dakota
    QString mainScriptTmp = tmpDirectory + QDir::separator() + fileName;
    fem->specialCopyMainInput(mainScriptTmp, random->getParametereNames());

    //
    // in new templatedir dir save the UI data into dakota.json file (same result as using saveAs)
    //

    QString filenameTMP = tmpDirectory + QDir::separator() + tr("dakota.json");

    QFile file(filenameTMP);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(filenameTMP),
                                  file.errorString()));
        return;
    }
    QJsonObject json;
    inputWidget->outputToJSON(json);
    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();

    //
    // now use the applications parseJSON file to run dakota and produce output files:
    //    dakota.in dakota.out dakotaTab.out dakota.err
    //

    QString homeDIR = QDir::homePath();
    QString appDIR = qApp->applicationDirPath();

   // appDIR = homeDIR + QDir::separator() + QString("NHERI") + QDir::separator() + QString("uqFEM") +
   //   QDir::separator() + QString("localApp");

    //
    QString pySCRIPT = appDIR +  QDir::separator() + QString("parseJson3.py");
    QString tDirectory = path + QDir::separator() + QString("tmp.SimCenter") + strUnique;

    // remove current results widget
    results->setResultWidget(0);

    //
    // want to first remove old dakota files from the current directory
    //

    QString sourceDir = path + QDir::separator() + QString("tmp.SimCenter") + strUnique + QDir::separator();
    QString destinationDir = path + QDir::separator();

    QStringList files;
    files << "dakota.in" << "dakota.out" << "dakotaTab.out" << "dakota.err";

    for (int i = 0; i < files.size(); i++) {
        QString copy = files.at(i);
        QFile file(destinationDir + copy);
        file.remove();
    }

    //
    // now invoke dakota, done via a python script in tool app dircetory
    //

    QProcess *proc = new QProcess();
qDebug() << "HELLO";
#ifdef Q_OS_WIN
    QString command = QString("python ") + pySCRIPT + QString(" ") + tDirectory + QString(" ") + tmpDirectory + QString(" runningRemote");
    qDebug() << command;
    proc->execute("cmd", QStringList() << "/C" << command);
    //   proc->start("cmd", QStringList(), QIODevice::ReadWrite);

#else
    QString command = QString("source $HOME/.bashrc; python ") + pySCRIPT + QString(" ") + tDirectory + QString(" ") + tmpDirectory + QString(" runningRemote");
    proc->execute("bash", QStringList() << "-c" <<  command);
    qDebug() << command;
    // proc->start("bash", QStringList("-i"), QIODevice::ReadWrite);
#endif
    proc->waitForStarted();

    //
    // when setup is complete, pop open the jobCreateor Widget which will allow user
    // to set some needed info before running at DesignSafe
    //

    jobCreator->hide();
    jobManager->hide();
    jobCreator->setInputDirectory(tDirectory);
    jobCreator->show();
}


void MainWindow::onJobsManagerButtonClicked(){

    errorMessage("");

    if (loggedIn == true) {
        jobManager->hide();
        jobManager->updateJobTable("");
        jobManager->show();
    } else {
        errorMessage("ERROR - You Need to Login");
    }
}


void MainWindow::onLoginButtonClicked() {

    if (loggedIn == false) {
        numTries = 0;
        loginWindow->show();
    } else {
        loggedIn = false;
        emit logout();
    }
}

void MainWindow::onLoginSubmitButtonClicked() {

    int maxNumTries = 3;

    if (loggedIn == false && numTries < maxNumTries) {
        // obtain login info
        QString login = nameLineEdit->text();
        QString password = passwordLineEdit->text();
        if (login.size() == 0) {
            login = "no_username";
        }
        if (password.size() == 0)
            password = "no_password";

        emit attemptLogin(login, password);
        return;
    }
}


void
MainWindow::attemptLoginReturn(bool ok){

    int maxNumTries = 3;

    if (ok == true) {
  //      emit updateJobTable("");
        loginWindow->hide();
        loggedIn = true;
        loginButton->setText("Logout");
        //this->enableButtons();

        //theJobManager->up
    } else {
        loggedIn = false;

        numTries++;

        if (numTries >= maxNumTries) {
            loginWindow->hide();
            nameLineEdit->setText("");
            passwordLineEdit->setText("");
            this->errorMessage("ERROR: Max Login Attempts Exceeded .. Contact DesignSafe for password help");
        }
    }
}


void
MainWindow::logoutReturn(bool ok){

    if (ok == true) {
        loggedIn = false;
        jobManager->clearTable();
        loginButton->setText("Login");
        jobManager->hide();
        jobCreator->hide();
        //this->disableButtons();

        // bring up login button
      //  this->onLoginButtonClicked();
    }
}


void MainWindow::onExitButtonClicked(){
    RandomVariableInputWidget *theParameters = uq->getParameters();
    QApplication::quit();
}

void MainWindow::onDakotaMethodChanged(void) {
    random->setParametersWidget(uq->getParameters());
}


bool MainWindow::save()
{
    errorMessage("");

    if (currentFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(currentFile);
    }
}

bool MainWindow::saveAs()
{
    errorMessage("");

    //
    // get filename
    //

    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;

    // and save the file
    return saveFile(dialog.selectedFiles().first());
}

void MainWindow::open()
{
    errorMessage("");

    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void MainWindow::newFile()
{
    errorMessage("");

    // clear old
    inputWidget->clear();

    // set currentFile blank
    setCurrentFile(QString());
}


void MainWindow::setCurrentFile(const QString &fileName)
{
    currentFile = fileName;
    //  setWindowModified(false);

    QString shownName = currentFile;
    if (currentFile.isEmpty())
        shownName = "untitled.json";

    setWindowFilePath(shownName);
}

bool MainWindow::saveFile(const QString &fileName)
{   
    //
    // open file
    //

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }


    //
    // create a json object, fill it in & then use a QJsonDocument
    // to write the contents of the object to the file in JSON format
    //

    QJsonObject json;
    inputWidget->outputToJSON(json);
    QJsonDocument doc(json);
    file.write(doc.toJson());

    // close file
    file.close();

    // set current file
    setCurrentFile(fileName);

    return true;
}

void MainWindow::loadFile(const QString &fileName)
{    
    //
    // open file
    //

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QString message = QString("Error: could not open file") + fileName;
        this->errorMessage(message);
        return;
    }

    // place contents of file into json object
    QString val;
    val=file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObj = doc.object();

    // close file
    file.close();

    // check file contains valid object
    if (jsonObj.isEmpty()) {
        this->errorMessage("ERROR: file either empty or malformed JSON");
        return;
    }


    // given the json object, create the C++ objects
    //inputWidget->inputFromJSON(jsonObj);
    if (fem->inputFromJSON(jsonObj) != true)
        return;
    if (uq->inputFromJSON(jsonObj) != true)
        return;
    if (random->inputFromJSON(jsonObj) != true)
        return;
    if (results->inputFromJSON(jsonObj) != true)
        return;

    setCurrentFile(fileName);
}


void MainWindow::processResults(QString &dakotaIN, QString &dakotaTAB)
{
    errorMessage("Processing Results");

    DakotaResults *result=uq->getResults();
    result->processResults(dakotaIN, dakotaTAB);
    results->setResultWidget(result);
    inputWidget->setSelection(QString("Results"));
}

void MainWindow::createActions() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    //const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    //const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));

    //QToolBar *fileToolBar = addToolBar(tr("File"));

    QAction *newAction = new QAction(tr("&New"), this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new file"));
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAction);
    //fileToolBar->addAction(newAction);

    QAction *openAction = new QAction(tr("&Open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing file"));
    connect(openAction, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAction);
    //fileToolBar->addAction(openAction);


    QAction *saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the document to disk"));
    connect(saveAction, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAction);


    QAction *saveAsAction = new QAction(tr("&Save As"), this);
    saveAction->setStatusTip(tr("Save the document with new filename to disk"));
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAction);

    // strangely, this does not appear in menu (at least on a mac)!! ..
    // does Qt not allow as in tool menu by default?
    // check for yourself by changing Quit to drivel and it works
    QAction *exitAction = new QAction(tr("&Quit"), this);
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    // exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAction);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *infoAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    QAction *submitAct = helpMenu->addAction(tr("&Provide Feedback"), this, &MainWindow::submitFeedback);
    //aboutAct->setStatusTip(tr("Show the application's About box"));
    QAction *aboutAct = helpMenu->addAction(tr("&Version"), this, &MainWindow::version);
    //aboutAct->setStatusTip(tr("Show the application's About box"));
    QAction *copyrightAct = helpMenu->addAction(tr("&License"), this, &MainWindow::copyright);
    //aboutAct->setStatusTip(tr("Show the application's About box"));

}



void MainWindow::copyright()
{
    QString textCopyright = "\
        <p>\
        The source code is licensed under a BSD 2-Clause License:<p>\
        \"Copyright (c) 2017-2018, The Regents of the University of California (Regents).\"\
        All rights reserved.<p>\
        <p>\
        Redistribution and use in source and binary forms, with or without \
        modification, are permitted provided that the following conditions are met:\
        <p>\
         1. Redistributions of source code must retain the above copyright notice, this\
         list of conditions and the following disclaimer.\
         \
         \
         2. Redistributions in binary form must reproduce the above copyright notice,\
         this list of conditions and the following disclaimer in the documentation\
         and/or other materials provided with the distribution.\
         <p>\
         THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND\
         ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\
         WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\
         DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR\
         ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\
         (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\
         LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\
            ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\
            (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\
            SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\
            <p>\
            The views and conclusions contained in the software and documentation are those\
            of the authors and should not be interpreted as representing official policies,\
            either expressed or implied, of the FreeBSD Project.\
            <p>\
            REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, \
            THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.\
            THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS \
            PROVIDED \"AS IS\". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,\
            UPDATES, ENHANCEMENTS, OR MODIFICATIONS.\
            <p>\
            ------------------------------------------------------------------------------------\
            <p>\
            The compiled binary form of this application is licensed under a GPL Version 3 license.\
            The licenses are as published by the Free Software Foundation and appearing in the LICENSE file\
            included in the packaging of this application. \
            <p>\
            ------------------------------------------------------------------------------------\
            <p>\
            This software makes use of the QT packages (unmodified): core, gui, widgets and network\
                                                                     <p>\
                                                                     QT is copyright \"The Qt Company Ltd&quot; and licensed under the GNU Lesser General \
                                                                     Public License (version 3) which references the GNU General Public License (version 3)\
      <p>\
      The licenses are as published by the Free Software Foundation and appearing in the LICENSE file\
      included in the packaging of this application. \
      <p>\
      ------------------------------------------------------------------------------------\
      ";


         QMessageBox msgBox;
    QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(textCopyright);
    QGridLayout *layout = (QGridLayout*)msgBox.layout();
    layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
    msgBox.exec();

}


void MainWindow::version()
{
    QMessageBox::about(this, tr("Version"),
                       tr("Version 1.0.0 "));
}

void MainWindow::about()
{
    QString textAbout = "\
              This is the open-source uqFEM tool. It is an application intended to augment finite element applications with\
              sampling and optimization methods. These methods will allow users to provide for example uncertainty\
             quantification in the responses generated and parameter estimation in the input variables in calibration studies.\
             <p>\
             Version 1 of this till utilizes the Dakota software to provide the UQ and optimization methods. Each of these\
             methods will repeatedly invoke the Finite Element application on either locally on the users dekstp machine or remotely\
             on HPC resources at the Texas Advanced Computing Center throgh the NHERI DesignSafe cyberinfrastructure.\
             <p>\
             The tool does not limit the capabilities provided by these applications, and as such, does not stop the user from doing\
             stupid things.<p>\
            ";

            QMessageBox msgBox;
    QSpacerItem *theSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(textAbout);
    QGridLayout *layout = (QGridLayout*)msgBox.layout();
    layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
    msgBox.exec();
}

void MainWindow::submitFeedback()
{
   // QDesktopServices::openUrl(QUrl("https://github.com/NHERI-SimCenter/MDOF/issues", QUrl::TolerantMode));
 QDesktopServices::openUrl(QUrl("https://www.designsafe-ci.org/help/new-ticket/", QUrl::TolerantMode));
}

