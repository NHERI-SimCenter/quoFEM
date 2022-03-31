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

// Written: fmckenna

#include "WorkflowApp_quoFEM.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <RunLocalWidget.h>
#include <QProcess>
#include <QCoreApplication>
#include <RemoteService.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QHostInfo>
#include <QUuid>
#include <QDir>
#include <QFile>
#include <QStatusBar>

#include <SimCenterComponentSelection.h>
#include <RandomVariablesContainer.h>
#include <UQ_EngineSelection.h>
#include <FEM_Selection.h>
#include <InputWidgetEDP.h>

#include <LocalApplication.h>
#include <RemoteApplication.h>
#include <RemoteJobManager.h>
#include <RunWidget.h>

#include "CustomizedItemModel.h"

#include <QSettings>
#include <QUuid>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QHostInfo>
#include <DakotaResultsSampling.h>
#include <Utils/PythonProgressDialog.h>
#include <Utils/RelativePathResolver.h>

#include <GoogleAnalytics.h>

// static pointer for global procedure set in constructor
static WorkflowApp_quoFEM *theApp = 0;

// global procedure
int getNumParallelTasks() {
    return theApp->getMaxNumParallelTasks();
}

WorkflowApp_quoFEM::WorkflowApp_quoFEM(RemoteService *theService, QWidget *parent)
    : WorkflowAppWidget(theService, parent)
{
    // set static pointer for global procedure
    theApp = this;

    //
    // create the various widgets
    //

    theRVs = RandomVariablesContainer::getInstance();
    theFEM_Selection = new FEM_Selection(false);
    theUQ_Selection = new UQ_EngineSelection();
    theEDPs = new InputWidgetEDP();

    //theResults = new DakotaResultsSampling(theRVs);
    theResults = theUQ_Selection->getResults();

    localApp = new LocalApplication("qWHALE.py");
    remoteApp = new RemoteApplication("qWHALE.py", theService);

    //    localApp = new LocalApplication("EE-UQ workflow.py");
    //   remoteApp = new RemoteApplication("EE-UQ workflow.py", theService);

    // localApp = new LocalApplication("EE-UQ.py");
    // remoteApp = new RemoteApplication("EE-UQ.py", theService);
    theJobManager = new RemoteJobManager(theService);

    SimCenterWidget *theWidgets[1];// =0;
    theRunWidget = new RunWidget(localApp, remoteApp, theWidgets, 0);

    //
    // connect signals and slots
    //

    // error messages and signals

    connect(theResults,SIGNAL(sendStatusMessage(QString)), this,SLOT(statusMessage(QString)));
    connect(theResults, SIGNAL(sendErrorMessage(QString)), this, SLOT(errorMessage(QString)));

    connect(theRunWidget,SIGNAL(sendErrorMessage(QString)), this,SLOT(errorMessage(QString)));
    connect(theRunWidget,SIGNAL(sendStatusMessage(QString)), this,SLOT(statusMessage(QString)));
    connect(theRunWidget,SIGNAL(sendFatalMessage(QString)), this,SLOT(fatalMessage(QString)));

    connect(localApp,SIGNAL(sendErrorMessage(QString)), this,SLOT(errorMessage(QString)));
    connect(localApp,SIGNAL(sendStatusMessage(QString)), this,SLOT(statusMessage(QString)));
    connect(localApp,SIGNAL(sendFatalMessage(QString)), this,SLOT(fatalMessage(QString)));

    connect(remoteApp,SIGNAL(sendErrorMessage(QString)), this,SLOT(errorMessage(QString)));
    connect(remoteApp,SIGNAL(sendStatusMessage(QString)), this,SLOT(statusMessage(QString)));
    connect(remoteApp,SIGNAL(sendFatalMessage(QString)), this,SLOT(fatalMessage(QString)));

    connect(localApp,SIGNAL(setupForRun(QString &,QString &)), this, SLOT(setUpForApplicationRun(QString &,QString &)));
    connect(this,SIGNAL(setUpForApplicationRunDone(QString&, QString &)), theRunWidget, SLOT(setupForRunApplicationDone(QString&, QString &)));
    connect(localApp,SIGNAL(processResults(QString&)), this, SLOT(processResults(QString&)));

    connect(remoteApp,SIGNAL(setupForRun(QString &, QString &)), this, SLOT(setUpForApplicationRun(QString &,QString &)));

    connect(theJobManager,SIGNAL(processResults(QString&)), this, SLOT(processResults(QString&)));
    connect(theJobManager,SIGNAL(loadFile(QString&)), this, SLOT(loadFile(QString&)));

    connect(remoteApp,SIGNAL(successfullJobStart()), theRunWidget, SLOT(hide()));

    connect(localApp,SIGNAL(runComplete()), this, SLOT(runComplete()));
    connect(remoteApp,SIGNAL(successfullJobStart()), this, SLOT(runComplete()));
    connect(theService, SIGNAL(closeDialog()), this, SLOT(runComplete()));
    connect(theJobManager, SIGNAL(closeDialog()), this, SLOT(runComplete()));

    //connect(theRunLocalWidget, SIGNAL(runButtonPressed(QString, QString)), this, SLOT(runLocal(QString, QString)));


    //
    // create layout to hold component selectio
    //


    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    this->setLayout(horizontalLayout);
    this->setContentsMargins(0,5,0,5);
    horizontalLayout->setMargin(0);

    //
    // create the component selection & add the components to it
    //

    theComponentSelection = new SimCenterComponentSelection();
    horizontalLayout->addWidget(theComponentSelection);

    theComponentSelection->addComponent(QString("UQ"),  theUQ_Selection);
    theComponentSelection->addComponent(QString("FEM"), theFEM_Selection);
    theComponentSelection->addComponent(QString("RV"),  theRVs);
    theComponentSelection->addComponent(QString("EDP"),  theEDPs);    
    theComponentSelection->addComponent(QString("RES"), theResults);

    theComponentSelection->displayComponent("UQ");

    // access a web page which will increment the usage count for this tool
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    manager->get(QNetworkRequest(QUrl("http://opensees.berkeley.edu/OpenSees/developer/eeuq/use.php")));


    PythonProgressDialog *theDialog=PythonProgressDialog::getInstance();
    theDialog->appendInfoMessage("Welcome to quoFEM");
    //    theDialog->hideAfterElapsedTime(1);
}

WorkflowApp_quoFEM::~WorkflowApp_quoFEM()
{
    // hack to get around a sometimes occuring seg fault
    // as some classes in destructur remove RV from the RVCOntainer
    // which may already have been destructed .. so removing so no destructor called

  //    QWidget *newUQ = new QWidget();
  //    theComponentSelection->swapComponent("RV",newUQ);
}

void WorkflowApp_quoFEM::replyFinished(QNetworkReply *pReply)
{
    return;
}


bool
WorkflowApp_quoFEM::outputToJSON(QJsonObject &jsonObjectTop) {

    //
    // get each of the main widgets to output themselves
    //
  
    bool result = true;
    QJsonObject apps;
    result = theUQ_Selection->outputAppDataToJSON(apps);
    if (result == false)
        return result;
    
    result = theFEM_Selection->outputAppDataToJSON(apps);
    if (result == false)
        return result;

    jsonObjectTop["Applications"]=apps;
    
    result = theRVs->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;

    result = theEDPs->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;    

    result = theFEM_Selection->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;

    result = theUQ_Selection->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;    

    result = theRunWidget->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;

    //theRunLocalWidget->outputToJSON(jsonObjectTop);
    
    return result;
}

void
WorkflowApp_quoFEM::processResults(QString &dirName){


  //
  // get results widget fr currently selected UQ option
  //

  theResults=theUQ_Selection->getResults();
  if (theResults == NULL) {
    this->errorMessage("FATAL - UQ option selected not returning results widget");
    return;
  }

  //
  // connect signals for results widget
  //

  connect(theResults,SIGNAL(sendStatusMessage(QString)), this,SLOT(statusMessage(QString)));
  connect(theResults,SIGNAL(sendErrorMessage(QString)), this,SLOT(errorMessage(QString)));

  //
  // swap current results with existing one in selection & disconnect signals
  //

  QWidget *oldResults = theComponentSelection->swapComponent(QString("RES"), theResults);
  if (oldResults != NULL) {
    disconnect(oldResults,SIGNAL(sendErrorMessage(QString)), this,SLOT(errorMessage(QString)));
    disconnect(oldResults,SIGNAL(sendFatalMessage(QString)), this,SLOT(fatalMessage(QString)));  
    delete oldResults;
  }

  //
  // process results
  // 

  theResults->processResults(dirName);

  // theRunWidget->hide();
  theComponentSelection->displayComponent("RES");
}

void
WorkflowApp_quoFEM::clear(void)
{
    theFEM_Selection->clear();
    theRVs->clear();
    theEDPs->clear();
    theUQ_Selection->clear();
}

bool
WorkflowApp_quoFEM::inputFromJSON(QJsonObject &jsonObject)
{
  //
  // get each of the main widgets to input themselves
  //
  
  if (jsonObject.contains("Applications")) {
    
    QJsonObject theApplicationObject = jsonObject["Applications"].toObject();
    
    if (theUQ_Selection->inputAppDataFromJSON(theApplicationObject) == false)
      emit errorMessage("quoFEM: failed to read UQ application");
    
    if (theFEM_Selection->inputAppDataFromJSON(theApplicationObject) == false)
      emit errorMessage("quoFEM: failed to read FEM application");
    
  } else {
    qDebug() << jsonObject;
    emit errorMessage("quoFEM: failed to find Applicatons section in JSON");
    return false;
  }

  if (theUQ_Selection->inputFromJSON(jsonObject) == false)
    emit errorMessage("quoFEM: failed to read UQ Method data");
  
  if (theFEM_Selection->inputFromJSON(jsonObject) == false)
    emit errorMessage("quoFEM: failed to read FEM Method data");

  theEDPs->inputFromJSON(jsonObject);
  theRVs->inputFromJSON(jsonObject);
  theRunWidget->inputFromJSON(jsonObject);
  
    return true;
}


void
WorkflowApp_quoFEM::onRunButtonClicked() {
    theRunWidget->hide();
    theRunWidget->setMinimumWidth(this->width()*0.5);
    theRunWidget->showLocalApplication();
    GoogleAnalytics::ReportLocalRun();
}

void
WorkflowApp_quoFEM::onRemoteRunButtonClicked(){
    emit errorMessage("");

    bool loggedIn = theRemoteService->isLoggedIn();

    if (loggedIn == true) {
        theRunWidget->hide();
        theRunWidget->setMinimumWidth(this->width()*0.5);
        theRunWidget->showRemoteApplication();

    } else {
        errorMessage("ERROR - You Need to Login");
    }

    GoogleAnalytics::ReportDesignSafeRun();
}

void
WorkflowApp_quoFEM::onRemoteGetButtonClicked(){

    emit errorMessage("");

    bool loggedIn = theRemoteService->isLoggedIn();

    if (loggedIn == true) {

        theJobManager->hide();
        theJobManager->updateJobTable("");
        theJobManager->show();

    } else {
        errorMessage("ERROR - You Need to Login");
    }
}

void
WorkflowApp_quoFEM::onExitButtonClicked(){

}

void
WorkflowApp_quoFEM::setUpForApplicationRun(QString &workingDir, QString &subDir) {

    errorMessage("");

    //
    // create temporary directory in working dir
    // and copy all files needed to this directory by invoking copyFiles() on app widgets
    //

    // designsafe will need a unique name
    /* *********************************************
    will let ParallelApplication rename dir
    QUuid uniqueName = QUuid::createUuid();
    QString strUnique = uniqueName.toString();
    strUnique = strUnique.mid(1,36);
    QString tmpDirName = QString("tmp.SimCenter") + strUnique;
    *********************************************** */

    QString tmpDirName = QString("tmp.SimCenter");
    QDir workDir(workingDir);

    QString tmpDirectory = workDir.absoluteFilePath(tmpDirName);
    QDir destinationDirectory(tmpDirectory);

    if(destinationDirectory.exists()) {
      destinationDirectory.removeRecursively();
    } else
      destinationDirectory.mkpath(tmpDirectory);

    QString templateDirectory  = destinationDirectory.absoluteFilePath(subDir);
    destinationDirectory.mkpath(templateDirectory);

    // copyPath(path, tmpDirectory, false);
    if (theFEM_Selection->copyFiles(templateDirectory) != true) {
        emit errorMessage("FEM selection failed to copy files");
        return;      
    }
    
    if (theUQ_Selection->copyFiles(templateDirectory) != true) {
        emit errorMessage("UQ selection failed to copy files");
        return;      
    }

    if (theRVs->copyFiles(templateDirectory) != true) {
        emit errorMessage("RV tab failed to copy files");
        return;
    }

    //
    // in new templatedir dir save the UI data into a file (same result as using saveAs)
    // NOTE: we append object workingDir to this which points to template dir
    //

    QString inputFile = templateDirectory + QDir::separator() + tr("scInput.json");

    QFile file(inputFile);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }
    QJsonObject json;
    if (this->outputToJSON(json) == false) {
        emit errorMessage("WorkflowApp - failed in outputToJson");
        return;
    }
    json["runDir"]=tmpDirectory;
    json["WorkflowType"]="Building Simulation";


    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();


    statusMessage("SetUp Done .. Now starting application");

    emit setUpForApplicationRunDone(tmpDirectory, inputFile);
}

int
WorkflowApp_quoFEM::loadFile(QString &fileName){

    statusMessage(QString("Loading File .. ") + fileName);

    //
    // open file
    //

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        emit errorMessage(QString("Could Not Open File: ") + fileName);
        return -1;
    }

    //
    // place contents of file into json object
    //

    QString val;
    val=file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObj = doc.object();

    //Resolve absolute paths from relative ones
    QFileInfo fileInfo(fileName);
    SCUtils::ResolveAbsolutePaths(jsonObj, fileInfo.dir());
    
    // close file
    file.close();

    //
    // clear current and input from new JSON
    //

    this->clear();
    bool result = this->inputFromJSON(jsonObj);
    if (result == true)
      return 0;
    else
      return -1;
}

int
WorkflowApp_quoFEM::getMaxNumParallelTasks() {
    return theUQ_Selection->getNumParallelTasks();
}

