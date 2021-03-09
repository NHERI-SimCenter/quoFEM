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

#include <ZipUtils.h>
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
#include <SimCenterPreferences.h>
#include <QSettings>
#include <Utils/dialogabout.h>
#include "SidebarWidgetSelection.h"

#include <InputWidgetEDP.h>
#include <InputWidgetFEM.h>
#include <UQ_EngineSelection.h>
#include <UQ_Results.h>
#include <InputWidgetParameters.h>
#include <RandomVariablesContainer.h>
#include <GoogleAnalytics.h>

#include <DakotaResultsSampling.h>
#include <SimCenterPreferences.h>

#include <QVBoxLayout>
#include <HeaderWidget.h>
#include <FooterWidget.h>
#include <QPushButton>
#include <InputWidgetFEM.h>
#include <UQ_EngineSelection.h>
//#include <InputWidgetEDP.h>
#include <QFileInfo>
#include <QProcess>
#include <QDesktopWidget>
#include <QUuid>
#include <QLabel>
#include <QLineEdit>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QHostInfo>

#include <SimCenterPreferences.h>
//#include <AgaveCLI.h>
#include <AgaveCurl.h>
#include <RemoteJobCreator.h>
#include <RemoteJobManager.h>
#include <QThread>
#include <QSettings>
#include <QDesktopServices>
#include <Utils/RelativePathResolver.h>



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
    //
    // user settings
    //


    /*
    QSettings settings("SimCenter", "uqFEM");
    QVariant savedValue = settings.value("uuid");
    QUuid uuid;
    if (savedValue.isNull()) {
        uuid = QUuid::createUuid();
        settings.setValue("uuid",uuid);
    } else
        uuid =savedValue.toUuid();
    */

    //
    // create the interface, jobCreator and jobManager
    //
    QString tenant("designsafe");
    QString storage("agave://designsafe.storage.default/");
    QString dirName("quoFEM");

    //theRemoteInterface = new AgaveCLI(tenant, storage, this);
    theRemoteInterface =  new AgaveCurl(tenant, storage, &dirName);
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
    int width = 0.8*rec.width();

    this->resize(width, height);


    //
    // add SimCenter Header
    //

    HeaderWidget *header = new HeaderWidget();
    header->setHeadingText(tr("quoFEM Application"));
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

    random = new InputWidgetParameters();
    fem = new InputWidgetFEM(random);
    edp = new InputWidgetEDP(random);
    uq = new UQ_EngineSelection(edp);
    random->setParametersWidget(uq->getParameters());


    connect(uq, SIGNAL(onNumModelsChanged(int)), fem, SLOT(numModelsChanged(int)));

    // create selection widget & add the input widgets
    results = new UQ_Results();

    inputWidget = new SidebarWidgetSelection();

    inputWidget->addInputWidget(tr("UQ"), tr("Uncertainty Quantification Methods"), uq);
    inputWidget->addInputWidget(tr("FEM"), tr("Finite Element Method Application"), fem);
    inputWidget->addInputWidget(tr("RV"), tr("Random Variables"), random);
    inputWidget->addInputWidget(tr("QoI"), tr("Quantities of Interest"), edp);
    inputWidget->addInputWidget(tr("RES"), tr("Results"), results);

    //inputWidget->setFont(QFont( "lucida", 20, QFont::Bold, TRUE ) );

    // let ubput widget know end of ptions, then set initial input to fem
    inputWidget->buildTreee();
    inputWidget->setSelection(tr("UQ"));
    //inputWidget->setMinimumWidth(600);
    // add selection widget to the central layout previosuly created
    layout->addWidget(inputWidget);

    //
    // add run, run-DesignSafe and exit buttons into a new widget for buttons
    //

    // create the buttons widget and a layout for it
    QHBoxLayout *pushButtonLayout = new QHBoxLayout();
    pushButtonLayout->setSpacing(10);

    QPushButton *runButton = new QPushButton();
    runButton->setText(tr("RUN"));
    pushButtonLayout->addWidget(runButton);

    runDesignSafeButton = new QPushButton();
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
    SectionTitle *info=new SectionTitle();
    info->setText(tr("DesignSafe User Account Info:"));

    QLabel *nameLabel = new QLabel();
    nameLabel->setText("Username:");
    QLabel *passwordLabel = new QLabel();
    passwordLabel->setText("Password:");
    nameLineEdit = new QLineEdit();
    passwordLineEdit = new QLineEdit();
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    loginSubmitButton = new QPushButton();
    loginSubmitButton->setText("Login");
    loginLayout->addWidget(info,0,0,2,2,Qt::AlignBottom);
    loginLayout->addWidget(nameLabel,2,0);
    loginLayout->addWidget(nameLineEdit,2,1);
    loginLayout->addWidget(passwordLabel,3,0);
    loginLayout->addWidget(passwordLineEdit,3,1);
    loginLayout->addWidget(loginSubmitButton,4,2);
    loginWindow->setLayout(loginLayout);

    //
    // connect signals & slots
    //

    // error & status messages
    connect(theRemoteInterface,SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));
    connect(theRemoteInterface,SIGNAL(statusMessage(QString)), this, SLOT(errorMessage(QString)));
    connect(theRemoteInterface,SIGNAL(fatalMessage(QString)), this, SLOT(fatalMessage(QString)));

    connect(fem,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(fem,SIGNAL(sendStatusMessage(QString)),this,SLOT(statusMessage(QString)));
    connect(fem,SIGNAL(sendStatusMessage(QString)),this,SLOT(fatalMessage(QString)));

    connect(random,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(random,SIGNAL(sendStatusMessage(QString)),this,SLOT(statusMessage(QString)));
    connect(random,SIGNAL(sendFatalMessage(QString)),this,SLOT(fatalMessage(QString)));

    connect(results,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(results,SIGNAL(sendStatusMessage(QString)),this,SLOT(statusMessage(QString)));
    connect(results,SIGNAL(sendFatalMessage(QString)),this,SLOT(fatalMessage(QString)));

    connect(uq,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));

    connect(uq,SIGNAL(sendStatusMessage(QString)),this,SLOT(statusMessage(QString)));
    connect(uq,SIGNAL(sendFatalMessage(QString)),this,SLOT(fatalMessage(QString)));

    connect(edp,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(edp,SIGNAL(sendStatusMessage(QString)),this,SLOT(statusMessage(QString)));
    connect(edp,SIGNAL(sendFatalMessage(QString)),this,SLOT(fatalMessage(QString)));

    connect(jobManager,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(jobManager,SIGNAL(statusMessage(QString)),this,SLOT(statusMessage(QString)));
    connect(jobManager,SIGNAL(fatalMessage(QString)),this,SLOT(fatalMessage(QString)));

    connect(jobCreator,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(jobCreator,SIGNAL(statusMessage(QString)),this,SLOT(statusMessage(QString)));
    connect(jobCreator,SIGNAL(fatalMessage(QString)),this,SLOT(fatalMessage(QString)));

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

    // change the UQ engine
    connect(uq,SIGNAL(onUQ_EngineChanged(bool)), this,SLOT(onUQ_EngineChanged(bool)));

    // add button widget to layout
    //layout->addWidget(buttonWidget);
    layout->addLayout(pushButtonLayout);

    //
    // add SimCenter footer
    //

    //FooterWidget *footer = new FooterWidget();
    //layout->addWidget(footer);

    layout->setSpacing(0);

    this->setCentralWidget(centralWidget);

    this->createActions();

    //
    // create QThread in which the Interface will work, move interface to it,
    // connect slots to thread to quit and invoke interface destructor & start running
    //

    thread = new QThread();
    theRemoteInterface->moveToThread(thread);

    connect(thread, SIGNAL(finished()), theRemoteInterface, SLOT(deleteLater()));//adding back
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();


    // adding back ---
    //
    // at startup make some URL calls to cleect tool stats
    //

    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    // send get to my simple counter
    manager->get(QNetworkRequest(QUrl("http://opensees.berkeley.edu/OpenSees/developer/uqFEM/use.php")));

    //QDir workingDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

    workingDirectory = SimCenterPreferences::getInstance()->getRemoteWorkDir();

    QDir dirWorkRemote(workingDirectory);
    if (!dirWorkRemote.exists())
        if (!dirWorkRemote.mkpath(workingDirectory)) {
            emit errorMessage(QString("Could not create Working Dir: ") + workingDirectory + QString(" . Try using an existing directory or make sure you have permission to create the working directory."));
            return;
        }

    workingDirectory = SimCenterPreferences::getInstance()->getLocalWorkDir();

    QDir dirWork(workingDirectory);
    if (!dirWork.exists())
        if (!dirWork.mkpath(workingDirectory)) {
            emit errorMessage(QString("Could not create Working Dir: ") + workingDirectory + QString(" . Try using an existing directory or make sure you have permission to create the working directory."));
            return;
        }

    QSettings settings("SimCenter", "Common");
    QVariant  loginName = settings.value("loginAgave");
    QVariant  loginPassword = settings.value("passwordAgave");
    if (loginName.isValid()) {
        nameLineEdit->setText(loginName.toString());
    }
    if (loginPassword.isValid()) {
        passwordLineEdit->setText(loginPassword.toString());
    }
}

MainWindow::~MainWindow()
{
    //
    // call quit on thread and deleteLater on interface
    // -interface destructor called here
    thread->quit();
    theRemoteInterface->deleteLater();

    // destroy objects we created
    delete jobCreator;
    delete jobManager;
    delete manager;// adding back
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

    foreach (QString directoryName,
             originDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        if (!directoryName.startsWith(QString("tmp.SimCenter"))) {
            QString destinationPath = destinationDir + "/" + directoryName;
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

static int count = 0;

int
MainWindow::runApplication(QString program, QStringList args) {

    QProcess *proc = new QProcess();

    proc->setProcessChannelMode(QProcess::SeparateChannels);
    auto procEnv = QProcessEnvironment::systemEnvironment();
    QString pathEnv = procEnv.value("PATH");
    QString pythonPathEnv = procEnv.value("PYTHONPATH");

    QString python = QString("python");
    QString exportPath("export PATH=");
    bool colonYes = false;

    SimCenterPreferences *preferences = SimCenterPreferences::getInstance();
    python = preferences->getPython();

    QFileInfo pythonFile(python);
    if (pythonFile.exists()) {
        QString pythonPath = pythonFile.absolutePath();
        colonYes=true;
        exportPath += pythonPath;
        pathEnv = pythonPath + ';' + pathEnv;
    } else {
        emit errorMessage("NO VALID PYTHON - Read the Manual & Check your Preferences");
        return false;
    }

    QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());
    QVariant  openseesPathVariant = settingsApplication.value("openseesPath");
    if (openseesPathVariant.isValid()) {
        QFileInfo openseesFile(openseesPathVariant.toString());
        if (openseesFile.exists()) {
            QString openseesPath = openseesFile.absolutePath();
#ifdef Q_OS_WIN
            pathEnv = openseesPath + ';' + pathEnv;
#else
            pathEnv = openseesPath + ':' + pathEnv;
#endif
            if (colonYes == false) {
                colonYes = true;
            } else {
#ifdef Q_OS_WIN
                exportPath += ";";
#else
                exportPath += ":";
#endif
            }
            exportPath += openseesPath;
        }
    }

    QVariant  dakotaPathVariant = settingsApplication.value("dakotaPath");
    if (dakotaPathVariant.isValid()) {
        QFileInfo dakotaFile(dakotaPathVariant.toString());
        if (dakotaFile.exists()) {
            QString dakotaPath = dakotaFile.absolutePath();
            if (colonYes == false) {
                colonYes = true;
            } else {
#ifdef Q_OS_WIN
                exportPath += ";";
#else
                exportPath += ":";
#endif
            }

#ifdef Q_OS_WIN
            pathEnv = dakotaPath + ';' + pathEnv;
#else
            pathEnv = dakotaPath + ':' + pathEnv;
#endif
            exportPath += dakotaPath;
            QString dakotaPathPath = QFileInfo(dakotaPath).absolutePath() + QDir::separator() +
                    "share" + QDir::separator() + "Dakota" + QDir::separator() + "Python";
            pythonPathEnv = dakotaPathPath + ";" + pythonPathEnv;

        }
    }

    if (colonYes == true) {
#ifdef Q_OS_WIN
        exportPath += ";";
#else
        exportPath += ":";
#endif
    }

    exportPath += "$PATH";

    procEnv.insert("PATH", pathEnv);
    procEnv.insert("PYTHONPATH", pythonPathEnv);
    proc->setProcessEnvironment(procEnv);    


    /*************** OLD CODE .. NEW .. ABOVE IS IN LOCALAPPLICATION .. BEGIN SWAP OUT
    SimCenterPreferences *preferences = SimCenterPreferences::getInstance();
    QString python=preferences->getPython();

    QString exportPath("export PATH=$PATH");

    QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());
    QVariant  openseesPathVariant = settingsApplication.value("openseesPath");
    if (openseesPathVariant.isValid()) {
        QFileInfo openseesFile(openseesPathVariant.toString());
        if (openseesFile.exists()) {
            QString openseesPath = openseesFile.absolutePath();
            pathEnv = openseesPath + ';' + pathEnv;
            exportPath += ":" + openseesPath;
        }
    }


    QVariant  dakotaPathVariant = settingsApplication.value("dakotaPath");
    if (dakotaPathVariant.isValid()) {
        QFileInfo dakotaFile(dakotaPathVariant.toString());
        if (dakotaFile.exists()) {
            QString dakotaPath = dakotaFile.absolutePath();
            QString dakotaPythonPath = QFileInfo(dakotaPath).absolutePath() + QDir::separator() +
                    "share" + QDir::separator() + "Dakota" + QDir::separator() + "Python";
            pathEnv = dakotaPath + ';' + pathEnv;
            exportPath += ":" + dakotaPath;
            pythonPathEnv = dakotaPythonPath + ";" + pythonPathEnv;
        }
    }

    procEnv.insert("PATH", pathEnv);
    procEnv.insert("PYTHONPATH", pythonPathEnv);
    proc->setProcessEnvironment(procEnv);
    *************************************** OLD CODE SWAP OUT   ***********************/
    
#ifdef Q_OS_WIN

    //python = QString('\"') + python + QString('\"');

    proc->start(program,args);

    bool failed = false;
    if (!proc->waitForStarted(-1))
    {
        qDebug() << "Failed to start the workflow!!! exit code returned: " << proc->exitCode();
        qDebug() << proc->errorString().split('\n');
        emit errorMessage("Failed to start the workflow!!!");
        failed = true;
    }

    if(!proc->waitForFinished(-1))
    {
        qDebug() << "Failed to finish running the workflow!!! exit code returned: " << proc->exitCode();
        qDebug() << proc->errorString();
        emit errorMessage("Failed to finish running the workflow!!!");
        return -1;
    }


    if(0 != proc->exitCode())
    {
        qDebug() << "Failed to run the workflow!!! exit code returned: " << proc->exitCode();
        qDebug() << proc->errorString();
        emit errorMessage("Failed to run the workflow!!!");
        return proc->exitCode();
    }

    if(failed)
    {
        qDebug().noquote() << proc->readAllStandardOutput();
        qDebug().noquote() << proc->readAllStandardError();
        return -1;
    }

#else

    QDir homeDir(QDir::homePath());
    QString sourceBash("\"");
    if (homeDir.exists(".bash_profile")) {
      sourceBash = QString("source $HOME/.bash_profile; ");
    } else if (homeDir.exists(".bashrc")) {
      sourceBash = QString("source $HOME/.bashrc; ");
    } else if (homeDir.exists(".zprofile")) {
      sourceBash = QString("source $HOME/.zprofile; ");
    } else if (homeDir.exists(".zshrc")) {
      sourceBash = QString("source $HOME/.zshrc; ");
    } else
      emit errorMessage( "No .bash_profile, .bashrc or .zshrc file found. This may not find Dakota or OpenSees");

    // note the above not working under linux because bash_profile not being called so no env variables!!
    QString command = sourceBash + exportPath + "; \"" + program + QString("\"  ");
    for ( const auto& arg : args  )
    {
        command += "\"" + arg + "\" ";
    }

    command += "> $HOME/Documents/quoFEM/fem.out." + QString::number(count) + " file 2>&1";
    qDebug() << "COMMAND" << command;

    proc->execute("bash", QStringList() << "-c" <<  command);
    proc->waitForStarted();

#endif

   return 0;
}

void MainWindow::onRunButtonClicked() {

    GoogleAnalytics::ReportLocalRun();

    //
    // get program & input file from fem widget
    //

    QString application = fem->getApplicationName();
    QString mainInput = fem->getMainInput();

    // first, delete the tmp.SimCenter directory if it already exists
    workingDirectory = SimCenterPreferences::getInstance()->getLocalWorkDir();

    QString tmpSimCenterDirectoryName = workingDirectory + QDir::separator() + QString("tmp.SimCenter");
    QDir tmpSimCenterDirectory(tmpSimCenterDirectoryName);
    if(tmpSimCenterDirectory.exists()) {
        tmpSimCenterDirectory.removeRecursively();
    }

    // now mkpath to tmp.SimnCenter/templatedir
    QString templateDirectory = workingDirectory + QDir::separator() + QString("tmp.SimCenter") + QDir::separator() + QString("templatedir");
    tmpSimCenterDirectory.mkpath(tmpSimCenterDirectoryName);

    // if a standard fem app, copy files to template dir instead of aving backend do it
    //   .. on second thoughts maybe the create driver function should be doing this .. would mean specialCopy redone though
    if (mainInput != "") {

        QFileInfo fileInfo(mainInput);
        QDir fileDir = fileInfo.absolutePath();

        QString fileName =fileInfo.fileName();
        QString path = fileDir.absolutePath();// + QDir::separator();

        qDebug() << "workdir set to " << fileDir;

        if (! fileDir.exists()) {
            errorMessage(QString("Directory ") + path + QString(" specified does not exist!"));
            return;
        }

        //
        // given path to input file we are going to create temporary directory below it
        // and copy all files from this input file directory to the new subdirectory
        //

        qDebug() << "creating the temp directory and copying files there... " << templateDirectory;
        copyPath(path, templateDirectory, true);

        // special copy the of the main script to set up lines containg parameters for  UQ engine

        //    QString mainScriptTmp = workingDirectory + QDir::separator() + fileName;
        QString mainScriptTmp = templateDirectory + QDir::separator() + fileName;
        qDebug() << "creating a special copy of the main FE model script... " << mainScriptTmp;
        fem->specialCopyMainInput(mainScriptTmp, random->getParametereNames());

        if (application == "Custom")
        {
           auto filesToCopy = fem->getCustomInputs();

           foreach (QString filePath, filesToCopy)
           {
	     QFileInfo fileInfo(filePath);
	     QString destination(templateDirectory + QDir::separator() + fileInfo.fileName());
	     QFile::copy(filePath, destination);
           }
        }
    } else {

        // create template dir for other type of fem app to put stuff
        tmpSimCenterDirectory.mkpath(templateDirectory);
    }

    //
    // in new templatedir dir save the UI data into dakota.json file (same result as using saveAs)
    //

    QString inputFilename = templateDirectory + QDir::separator() + tr("dakota.json");

    qDebug() << "creating dakota input at " << inputFilename;
    QFile file(inputFilename);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(inputFilename),
                                  file.errorString()));
        return;
    }


    QJsonObject json;
    if  (this->outputToJSON(json) == false) {
        file.close();
        return;
    }

    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();

    //
    // Get applications to run for FEM & UQ
    //   1. get uq to outputApp to a JSON object & from object get app name
    //   2. from workflowapplications file get program name
    //

    QString homeDIR = QDir::homePath();
    QString appDIR = SimCenterPreferences::getInstance()->getAppDir();

    QString programToExe;
    QString programName;
    QString femProgramToExe;

    //
    // get UQ app
    //

    QJsonObject appData;
    uq->outputAppDataToJSON(appData);
    QJsonObject uqDataObj = appData["UQ"].toObject();
    QString appName = uqDataObj["Application"].toString();

    // open workflowapplications
    QString workflowApplications = appDIR + QDir::separator() + QString("applications") + QDir::separator() + QString("WorkflowApplications.json");
    QFile workflowApplicationsFile(workflowApplications);
    if (!workflowApplicationsFile.open(QFile::ReadOnly | QFile::Text)) {
        QString message = QString("Error: could not open file") + workflowApplications;
        this->errorMessage(message);
        return;
    }

    // find UQ program to execute given UQ appname
    QString val = workflowApplicationsFile.readAll();
    QJsonDocument docWA = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObj = docWA.object();
    workflowApplicationsFile.close();
    QJsonObject uqApplications = jsonObj["UQApplications"].toObject();
    QJsonArray uqApplicationsArray = uqApplications["Applications"].toArray();

    bool found = false;
    for (int i=0; i<uqApplicationsArray.size(); i++) {
        QJsonObject entry = uqApplicationsArray[i].toObject();
        programName = entry["Name"].toString();
        qDebug() << "programName: " << programName;
        if (programName == appName) {
            programToExe = entry["ExecutablePath"].toString();
            found = true;
            break;
        }
    }

    if (found == false) {
        QString message = QString("Error: could not find UQ application ") + appName;
        this->errorMessage(message);
        return;
    }


    //
    // repeat for FEM application
    //

    QJsonObject femApplications = jsonObj["femApplications"].toObject();
    QJsonArray femApplicationsArray = femApplications["Applications"].toArray();

    found = false;
    for (int i=0; i<femApplicationsArray.size(); i++) {
        QJsonObject entry = femApplicationsArray[i].toObject();
        QString programName = entry["Name"].toString();
        qDebug() << "programName: " << programName;
        if (programName == application) {
            femProgramToExe = entry["ExecutablePath"].toString();
            found = true;
            break;
        }
    }

    if (found == false) {
        QString message = QString("Error: could not find FEM application ") + application;
        this->errorMessage(message);
        return;
    }

    //
    // now we have applications to run, get full path & check exists
    //

    QString uqApp = appDIR +  QDir::separator() + programToExe;
    QString femApp = appDIR +  QDir::separator() + femProgramToExe;

    // if windows add a .exe to non py applications
    QString os("Linux");
#ifdef _WIN32
   if (!femApp.contains(".py"))
       femApp += ".exe";
   if (!uqApp.contains(".py"))
       uqApp += ".exe";
   os = "Windows";
#endif

   // QString tDirectory = workingDirectory + QDir::separator() + QString("tmp.SimCenter");

    // check the Apps exist
    QFile uqAppFile(uqApp);
    if (! uqAppFile.exists()) {
      errorMessage("UQ application does not exist, try to reset settings, if fails download application again");
      return;
    }

    QFile femAppFile(femApp); 
    if (! femAppFile.exists()) {
      qDebug() << "FEM application: " << femApp;

      errorMessage("FEM Application does not exist, try to reset settings, if fails download application again");
      return;
    }

    //
    // invoke the fem aplication to create workflow driver
    //

    QString python("python");
    QSettings settings("SimCenter", "Common");
    QVariant  pythonLocationVariant = settings.value("pythonExePath");
    if (pythonLocationVariant.isValid())
      python = pythonLocationVariant.toString();

    count = 1;

    // Only create workflow driver when not custom fem app, since workflow
    // driver provided as input in that case

    #ifdef Q_OS_WIN
     QString OperatingSystem="Windows";
    #else
     QString OperatingSystem="Mac";
    #endif

    if (application != "Custom")
    {
       if (femApp.contains(".py"))
       {

          QStringList args{ femApp, inputFilename, "runningLocal", OperatingSystem };

          this->runApplication(python, args);
       }
       else
       {

          QStringList args{ inputFilename, "runningLocal", OperatingSystem };

          this->runApplication(femApp, args);
       }
    }

    //
    // now invoke uq app
    //

    count = 2;
    if (uqApp.contains(".py")) {
        QStringList args{uqApp, tmpSimCenterDirectoryName, templateDirectory, "runningLocal"};
        this->runApplication(python, args);
    } else {
        QStringList args{tmpSimCenterDirectoryName, templateDirectory, "runningLocal"};
        this->runApplication(uqApp, args);
    }

    // Get the input json data from the dakota.json file
    QFile inputFile(inputFilename);
    inputFile.open(QFile::ReadOnly | QFile::Text);
    val = inputFile.readAll();
    doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject inputData = doc.object();
    inputFile.close();

    QString problemType = inputData["UQ_Method"].toObject()["uqType"].toString();
    
    qDebug() << problemType;

    QString filenameOUT = tmpSimCenterDirectoryName + QDir::separator() + tr("dakota.out");
    QString filenameTAB;
    if (problemType == "Inverse Problem") {
        filenameTAB = tmpSimCenterDirectoryName + QDir::separator() + tr("dakota_mcmc_tabular.dat");	
    } else if (programName == "Other-UQ") {
        filenameTAB = tmpSimCenterDirectoryName + QDir::separator() + tr("tabularResults.out");
    } else {
        filenameTAB = tmpSimCenterDirectoryName + QDir::separator() + tr("dakotaTab.out");
    }

    this->processResults(filenameOUT, filenameTAB);
}


void MainWindow::onRemoteRunButtonClicked(){

    // check logged in
    if (loggedIn == false) {
          errorMessage("ERROR - You Need to Login");
          return;
    }

    GoogleAnalytics::ReportDesignSafeRun();

    //
    // get program & input file from fem widget
    //

    QString application = fem->getApplicationName();
    QString mainInput = fem->getMainInput();

    // first, delete the tmp.SimCenter directory if it already exists
    workingDirectory = SimCenterPreferences::getInstance()->getRemoteWorkDir();

    //
    // given path to input file we are going to create temporary directory below it
    // and copy all files from this input file directory to the new subdirectory
    //

    QUuid uniqueName = QUuid::createUuid();
    QString strUnique = uniqueName.toString();
    strUnique = strUnique.mid(1,36);


    QString tmpSimCenterDirectoryName = workingDirectory + QDir::separator() + QString("tmp.SimCenter" + strUnique);
    QDir tmpSimCenterDirectory(tmpSimCenterDirectoryName);

    if(tmpSimCenterDirectory.exists()) {
        tmpSimCenterDirectory.removeRecursively();
    }
    tmpSimCenterDirectory.mkpath(tmpSimCenterDirectoryName);

    QString tmpDirectory = tmpSimCenterDirectoryName + QDir::separator() + QString("templatedir");

    if (mainInput != "") {

        QFileInfo fileInfo(mainInput);
        QDir fileDir = fileInfo.absolutePath();

        QString fileName =fileInfo.fileName();
        QString path = fileDir.absolutePath();// + QDir::separator();

        qDebug() << "workdir set to " << fileDir;

        if (! fileDir.exists()) {
            errorMessage(QString("Directory ") + path + QString(" specified does not exist!"));
            return;
        }
        qDebug() << "workdir exists ";

        //
        // given path to input file we are going to create temporary directory below it
        // and copy all files from this input file directory to the new subdirectory
        //

        qDebug() << "creating the temp directory and copying files there... " << tmpDirectory;
        copyPath(path, tmpDirectory, true);

        // special copy the of the main script to set up lines containg parameters for  UQ engine

        //    QString mainScriptTmp = workingDirectory + QDir::separator() + fileName;
        QString mainScriptTmp = tmpDirectory + QDir::separator() + fileName;
        qDebug() << "creating a special copy of the main FE model script... " << mainScriptTmp;
        fem->specialCopyMainInput(mainScriptTmp, random->getParametereNames());
    } else {

        // create template dir for other type of fem app to put stuff
        tmpSimCenterDirectory.mkpath(tmpDirectory);
    }


    //
    // in new templatedir dir save the UI data into dakota.json file (same result as using saveAs)
    //

    QString inputFilename = tmpDirectory + QDir::separator() + tr("dakota.json");

    QFile file(inputFilename);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(inputFilename),
                                  file.errorString()));
        return;
    }


    // remove current results widget
    results->setResultWidget(0);


    QJsonObject json;
    if (this->outputToJSON(json) == false) {
        file.close();
        return;
    }

    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();

    //
    // Get applications to run for FEM & UQ
    //   1. get uq to outputApp to a JSON object & from object get app name
    //   2. from workflowapplications file get program name
    //

    QString homeDIR = QDir::homePath();
    QString appDIR = SimCenterPreferences::getInstance()->getAppDir();

    QString programToExe;
    QString femProgramToExe;

    //
    // get UQ app
    //

    QJsonObject appData;
    uq->outputAppDataToJSON(appData);
    QJsonObject uqDataObj = appData["UQ"].toObject();
    QString appName = uqDataObj["Application"].toString();

    // open workflowapplications
    QString workflowApplications = appDIR + QDir::separator() + QString("applications") + QDir::separator() + QString("WorkflowApplications.json");
    QFile workflowApplicationsFile(workflowApplications);
    if (!workflowApplicationsFile.open(QFile::ReadOnly | QFile::Text)) {
        QString message = QString("Error: could not open file") + workflowApplications;
        this->errorMessage(message);
        return;
    }

    // find UQ program to execute given UQ appname
    QString val = workflowApplicationsFile.readAll();
    QJsonDocument docWA = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObj = docWA.object();
    workflowApplicationsFile.close();
    QJsonObject uqApplications = jsonObj["UQApplications"].toObject();
    QJsonArray uqApplicationsArray = uqApplications["Applications"].toArray();

    bool found = false;
    for (int i=0; i<uqApplicationsArray.size(); i++) {
        QJsonObject entry = uqApplicationsArray[i].toObject();
        QString programName = entry["Name"].toString();
        qDebug() << "programName: " << programName;
        if (programName == appName) {
            programToExe = entry["ExecutablePath"].toString();
            found = true;
            break;
        }
    }

    if (found == false) {
        QString message = QString("Error: could not find UQ application ") + appName;
        this->errorMessage(message);
        return;
    }

    //
    // repeat for FEM application
    //

    QJsonObject femApplications = jsonObj["femApplications"].toObject();
    QJsonArray femApplicationsArray = femApplications["Applications"].toArray();

    found = false;
    for (int i=0; i<femApplicationsArray.size(); i++) {
        QJsonObject entry = femApplicationsArray[i].toObject();
        QString programName = entry["Name"].toString();
        qDebug() << "programName: " << programName;
        if (programName == application) {
            femProgramToExe = entry["ExecutablePath"].toString();
            found = true;
            break;
        }
    }

    if (found == false) {
        QString message = QString("Error: could not find FEM application ") + application;
        this->errorMessage(message);
        return;
    }

    QString uqApp = appDIR +  QDir::separator() + programToExe;
    QString femApp = appDIR +  QDir::separator() + femProgramToExe;

    //
    // invoke the fem aplication to create workflow driver
    //

    QString python("python");
    QSettings settings("SimCenter", "Common");
    QVariant  pythonLocationVariant = settings.value("pythonExePath");
    if (pythonLocationVariant.isValid())
      python = pythonLocationVariant.toString();

    count = 1;
    if (femApp.contains(".py")) {
        QStringList args{femApp, inputFilename, "runningRemote", "Mac"};
        this->runApplication(python, args);
    } else {
        QStringList args{inputFilename, "runningRemote", "Mac"};
        this->runApplication(femApp, args);
    }

    //
    // invoke UQ engine with runningRemote flag to set up remote job
    //


    //
    // now invoke uq app
    //

    count = 2;
    if (uqApp.contains(".py")) {
        QStringList args{uqApp, tmpSimCenterDirectoryName, tmpDirectory, "runningRemote"};
        this->runApplication(python, args);
    } else {
        QStringList args{tmpSimCenterDirectoryName, tmpDirectory, "runningRemote"};
        this->runApplication(uqApp, args);
    }


    //
    // want to first remove old dakota files from the current directory
    //

    QString sourceDir = workingDirectory + QDir::separator() + QString("tmp.SimCenter") + strUnique + QDir::separator();
    QString destinationDir = workingDirectory + QDir::separator();

    QStringList files;
    files << "dakota.in" << "dakota.out" << "dakotaTab.out" << "dakota.err";

    for (int i = 0; i < files.size(); i++) {
        QString copy = files.at(i);
        QFile file(destinationDir + copy);
        file.remove();
    }

    //
    // in tmpDirectory we will zip up current template dir and then remove before sending (doone to reduce number of sends)
    //

    QString tDirectory2 = workingDirectory + QDir::separator() + QString("tmp.SimCenter") + strUnique;
    QString templateDIR(tDirectory2 + QDir::separator() + QString("templatedir"));
    QString zipFile(tDirectory2 + QDir::separator() + QString("templatedir.zip"));
    ZipUtils::ZipFolder(QDir(templateDIR), zipFile);
    qDebug() << "ZIP DIR: " << templateDIR;
    qDebug() << "ZIP FILE: " << zipFile;
    QDir dirToRemove(templateDIR);
    dirToRemove.removeRecursively();

    //
    // when setup is complete, pop open the jobCreateor Widget which will allow user
    // to set some needed info before running at DesignSafe
    //

    jobCreator->hide();
    jobManager->hide();
    jobCreator->setInputDirectory(tmpSimCenterDirectoryName);
    jobCreator->setMaxNumParallelProcesses(uq->getNumParallelTasks());
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

	QSettings settings("SimCenter", "Common");
	settings.setValue("loginAgave", nameLineEdit->text());
	settings.setValue("passwordAgave", passwordLineEdit->text());

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

  //RandomVariablesContainer *theParameters = uq->getParameters();
    QApplication::quit();
}

void MainWindow::onUQ_EngineChanged(bool abilityToRunRemote) {

    qDebug() << "onUQENGINECHANGED " << abilityToRunRemote;
    runDesignSafeButton->setDisabled(!abilityToRunRemote);
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
    return saveFile(dialog.selectedFiles().first());
}

void MainWindow::open()
{
    errorMessage("");

    QString fileName = QFileDialog::getOpenFileName(this, "Open Simulation Model", "",  "Json files (*.json);;All files (*)");
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

bool MainWindow::outputToJSON(QJsonObject &jsonObj) {

    QJsonObject appsUQ;
    uq->outputAppDataToJSON(appsUQ);
    jsonObj["Applications"]=appsUQ;

    if (fem->outputToJSON(jsonObj) != true) {
        emit errorMessage(QString("FEM: failed to write output"));
        return false;
    }

    if (uq->outputToJSON(jsonObj) != true) {
        emit errorMessage(QString("UQ: failed to write output"));
        return false;
    }

    if (random->outputToJSON(jsonObj) != true) {
        emit errorMessage(QString("RV: failed to write output"));
        return false;
    }

    if (edp->outputToJSON(jsonObj) != true) {
        emit errorMessage(QString("EDP: failed to write output"));
        return false;
    }

    //UQ_Results *result=uq->getResults();
    //results->setResultWidget(result);
    results->outputToJSON(jsonObj);

    // output the preferences

    jsonObj["localAppDir"]=SimCenterPreferences::getInstance()->getAppDir();
    jsonObj["remoteAppDir"]=SimCenterPreferences::getInstance()->getRemoteAppDir();
    jsonObj["workingDir"]=SimCenterPreferences::getInstance()->getLocalWorkDir();
    jsonObj["python"]=SimCenterPreferences::getInstance()->getPython();

    return true;
}

bool MainWindow::inputFromJSON(QJsonObject &jsonObj){

    if (jsonObj.contains("Applications")) {

        QJsonObject theApplicationObject = jsonObj["Applications"].toObject();
        if (uq->inputAppDataFromJSON(theApplicationObject) != true) {
            emit errorMessage(QString("UQ: failed to read app data input"));
            return false;
        }
    } else {
        // possibly old code: default is Dakota
    }


    if (fem->inputFromJSON(jsonObj) != true) {
        emit errorMessage(QString("FEM: failed to read input"));
        return false;
    }

    if (uq->inputFromJSON(jsonObj) != true) {
        emit errorMessage(QString("UQ: failed to read input"));
        return false;
    }

    if (random->inputFromJSON(jsonObj) != true) {
        emit errorMessage(QString("RV: failed to read input"));
        return false;
    }

    if (edp->inputFromJSON(jsonObj) != true) {
        emit errorMessage(QString("EDP: failed to read input"));
        return false;
    }

    //UQ_Results *result=uq->getResults();
    results->setResultWidget(uq->getResults());
    results->inputFromJSON(jsonObj); // results can fail if no results when file saved

    return true;
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
    if (this->outputToJSON(json) == false) {
        qDebug() << "MainWindow - outputToJSON returned false";
        file.close();
        return false;
    }


    //Resolve relative paths before saving
    QFileInfo fileInfo(fileName);
    SCUtils::ResolveRelativePaths(json, fileInfo.dir());


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

    //Resolve absolute paths from relative ones
    QFileInfo fileInfo(fileName);
    SCUtils::ResolveAbsolutePaths(jsonObj, fileInfo.dir());

    // close file
    file.close();

    // check file contains valid object
    if (jsonObj.isEmpty()) {
        this->errorMessage("ERROR: file either empty or malformed JSON");
        return;
    }

    // given the json object, create the C++ objects
    //inputWidget->inputFromJSON(jsonObj);
    this->inputFromJSON(jsonObj);

    setCurrentFile(fileName);
}


void MainWindow::processResults(QString &dakotaIN, QString &dakotaTAB)
{
    errorMessage("Processing Results");

    UQ_Results *result=uq->getResults();
    connect(result,SIGNAL(sendErrorMessage(QString)), this, SLOT(errorMessage(QString)));
    connect(result,SIGNAL(sendStatusMessage(QString)), this, SLOT(errorMessage(QString)));

    result->processResults(dakotaIN, dakotaTAB);
    results->setResultWidget(result);
    
    inputWidget->setSelection(QString("RES"));
}

void MainWindow::createActions() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAction = new QAction(tr("&Open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing file"));
    connect(openAction, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAction);

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
    QAction *versionAct = helpMenu->addAction(tr("&Version"), this, &MainWindow::version);
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    QAction *preferencesAct = helpMenu->addAction(tr("&Preferences"), this, &MainWindow::preferences);
    //aboutAct->setStatusTip(tr("Show the application's About box"));
    QAction *manualAct = helpMenu->addAction(tr("&Manual"), this, &MainWindow::manual);
    QAction *submitAct = helpMenu->addAction(tr("&Submit Bug/Feature Request"), this, &MainWindow::submitFeedback);
    //QAction *submitFeature = helpMenu->addAction(tr("&Submit Feature Request"), this, &MainWindow::submitFeatureRequest);
    QAction *citeAct = helpMenu->addAction(tr("&How to Cite"), this, &MainWindow::cite);
    QAction *copyrightAct = helpMenu->addAction(tr("&License"), this, &MainWindow::copyright);

    //
    // Examples
    //

    auto pathToExamplesJson = QCoreApplication::applicationDirPath() + QDir::separator() +
                "Examples" + QDir::separator() + "Examples.json";

    QFile jsonFile(pathToExamplesJson);
    if (jsonFile.exists()) {
        qDebug() << "Examples Exist";
        jsonFile.open(QFile::ReadOnly);
        QJsonDocument exDoc = QJsonDocument::fromJson(jsonFile.readAll());

        QJsonObject docObj = exDoc.object();
        QJsonArray examples = docObj["Examples"].toArray();
        QMenu *exampleMenu = 0;
        if (examples.size() > 0)
            exampleMenu = menuBar()->addMenu(tr("&Examples"));

        foreach (const QJsonValue & example, examples) {
            QJsonObject exampleObj = example.toObject();
            QString name = exampleObj["name"].toString();
            QString inputFile = exampleObj["InputFile"].toString();
            auto action = exampleMenu->addAction(name, this, &MainWindow::loadExamples);
            action->setProperty("InputFile",inputFile);
        }
    } else
        qDebug() << "No Examples" << pathToExamplesJson;


    thePreferences = SimCenterPreferences::getInstance();
}


void MainWindow::loadExamples()
{
    auto pathToExample = QCoreApplication::applicationDirPath() + QDir::separator() + "Examples" + QDir::separator();
    pathToExample += QObject::sender()->property("InputFile").toString();

    if(pathToExample.isNull())
    {
        qDebug()<<"Error loading examples";
        return;
    }

    this->loadFile(pathToExample);
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
                       tr("Version 2.2.2 "));
}

void MainWindow::preferences()
{
  thePreferences->show();
}

void MainWindow::about()
{
  /*
    QString textAbout = "\
              This is the open-source quoFEM tool. It is an application intended to augment finite element applications with\
              sampling and optimization methods. These methods will allow users to provide, for example, uncertainty\
             quantification in the structural responses and parameter estimation of input variables in calibration studies.\
             <p>\
             Version 2.2.2 of this tool utilizes the Dakota software to provide the UQ and optimization methods. Dakota\
             will repeatedly invoke the finite element application either locally on the users dekstop machine or remotely\
             on high performance computing resources at the Texas Advanced Computing Center through the NHERI DesignSafe cyberinfrastructure.\
             <p>\
             This work is based on material supported by the National Science Foundation under grant 1612843<p>\
            ";

            QMessageBox msgBox;
    QSpacerItem *theSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(textAbout);
    QGridLayout *layout = (QGridLayout*)msgBox.layout();
    layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
    msgBox.exec();
  */


    QString aboutTitle = "About the SimCenter quoFEM Application"; // this is the title displayed in the on About dialog
    QString aboutSource = ":/images/aboutQUOFEM.html";  // this is an HTML file stored under resources

    DialogAbout *dlg = new DialogAbout();
    dlg->setTitle(aboutTitle);
    dlg->setTextSource(aboutSource);

    //
    // adjust size of application window to the available display
    //
    QRect rec = QApplication::desktop()->screenGeometry();
    int height = 0.50*rec.height();
    int width  = 0.50*rec.width();

    dlg->resize(width, height);
    dlg->exec();
    delete dlg;
}

void MainWindow::submitFeedback()
{
    QString messageBoardURL("https://simcenter-messageboard.designsafe-ci.org/smf/index.php?board=4.0");
    QDesktopServices::openUrl(QUrl(messageBoardURL, QUrl::TolerantMode));
}

void MainWindow::manual()
{
  QString featureRequestURL = QString("https://nheri-simcenter.github.io/quoFEM-Documentation/");
    QDesktopServices::openUrl(QUrl(featureRequestURL, QUrl::TolerantMode));
}




void MainWindow::cite()
{
  QString citeText = QString("Frank McKenna, Nikhil Padhye, & Adam Zsarnoczay. (2019, September 30). NHERI-SimCenter/quoFEM: Vesion 2.0.0 (Version v2.0.0). Zenodo. http://doi.org/10.5281/zenodo.3466061");
    QMessageBox msgBox;
    QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(citeText);
    QGridLayout *layout = (QGridLayout*)msgBox.layout();
    layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
    msgBox.exec();
}
