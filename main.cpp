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

#include <QApplication>
#include <QFile>
#include <QTime>
#include <QThread>
#include <QTextStream>
#include <QDir>
#include <QStandardPaths>
#include <QStatusBar>
#include <QProcessEnvironment>

#include <MainWindowWorkflowApp.h>
#include <WorkflowApp_quoFEM.h>
#include <WorkflowCLI.h>
#include <GoogleAnalytics.h>
#include <AgaveCurl.h>
#include <QWebEngineView>

 // customMessgaeOutput code from web:
 // https://stackoverflow.com/questions/4954140/how-to-redirect-qdebug-qwarning-qcritical-etc-output

static QString logFilePath;
static bool logToFile = false;


void customMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QHash<QtMsgType, QString> msgLevelHash({{QtDebugMsg, "Debug"}, {QtInfoMsg, "Info"}, {QtWarningMsg, "Warning"}, {QtCriticalMsg, "Critical"}, {QtFatalMsg, "Fatal"}});
    QByteArray localMsg = msg.toLocal8Bit();
    QTime time = QTime::currentTime();
    QString formattedTime = time.toString("hh:mm:ss.zzz");
    QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
    QString logLevelName = msgLevelHash[type];
    QByteArray logLevelMsg = logLevelName.toLocal8Bit();

    if (logToFile) {
        QString txt = QString("%1 %2: %3 (%4)").arg(formattedTime, logLevelName, msg,  context.file);
        QFile outFile(logFilePath);
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt << "\n";
        outFile.close();
    } else {
        fprintf(stderr, "%s %s: %s (%s:%u, %s)\n", formattedTimeMsg.constData(), logLevelMsg.constData(), localMsg.constData(), context.file, context.line, context.function);
        fflush(stderr);
    }

    if (type == QtFatalMsg)
        abort();
}


int main(int argc, char *argv[])
{

#ifdef Q_OS_WIN
    QApplication::setAttribute(Qt::AA_UseOpenGLES);
#else
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
#endif
    
    //Setting Core Application Name, Organization, and Version
    QCoreApplication::setApplicationName("quoFEM");
    QCoreApplication::setOrganizationName("SimCenter");
    QCoreApplication::setApplicationVersion("3.5.2");

    //
    // set up logging of output messages for user debugging
    //

    logFilePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
      + QDir::separator() + QCoreApplication::applicationName();


    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();    
    QString workDir = env.value("SIMCENTER_WORKDIR","None");
    if (workDir != "None") {
      logFilePath = workDir;
    }

    // make sure tool dir exists in Documentss folder
    QDir dirWork(logFilePath);
    if (!dirWork.exists())
      if (!dirWork.mkpath(logFilePath)) {
	qDebug() << QString("Could not create Working Dir: ") << logFilePath;
      }

    // full path to debug.log file
    logFilePath = logFilePath + QDir::separator() + QString("debug.log");    
    
    // remove old log file
    QFile debugFile(logFilePath);
    debugFile.remove();

    QByteArray envVar = qgetenv("QTDIR");       //  check if the app is run in Qt Creator

    if (envVar.isEmpty())
        logToFile = true;

    qInstallMessageHandler(customMessageOutput);

    qDebug() << "LogFILE: " << logFilePath;

  //
  // windows scaling - Qt HighDPI scaling is problematic (llok at QtCreator on high res laptop
  //    - this constitutes my best effort to make it look better on window laptop

  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  //
  // start Qt mainwindow per normal
  //

  QApplication app(argc, argv);


    //
    // create a remote interface
    //

    QString tenant("designsafe");
    QString storage("agave://designsafe.storage.default/");
    QString dirName("quoFEM");

    AgaveCurl *theRemoteService = new AgaveCurl(tenant, storage, &dirName);


    //
    // create the main window
    //

    WorkflowAppWidget *theInputApp = new WorkflowApp_quoFEM(theRemoteService);
    
    MainWindowWorkflowApp w(QString("quoFEM: Quantified Uncertainty with Optimization for the Finite Element Method"), theInputApp, theRemoteService);
    
    QString aboutTitle = "About the SimCenter quoFEM Application"; // this is the title displayed in the on About dialog
    QString aboutSource = ":/images/aboutQUOFEM.html";  // this is an HTML file stored under resources

    w.setAbout(aboutTitle, aboutSource);

    QString version = QString("Version ") + QCoreApplication::applicationVersion();
    w.setVersion(version);
    QString citeText = QString("1) Frank McKenna, Sang-ri Yi, Aakash Bangalore Satish, Adam Zsarnoczay, Michael Gardner, & Wael Elhaddad. (2023). NHERI-SimCenter/quoFEM: Version 3.5.0 (v3.5.0). Zenodo. https://doi.org/10.5281/zenodo.10443180 \n\n2) Gregory G. Deierlein, Frank McKenna, Adam Zsarnóczay, Tracy Kijewski-Correa, Ahsan Kareem, Wael Elhaddad, Laura Lowes, Matthew J. Schoettler, and Sanjay Govindjee (2020) A Cloud-Enabled Application Framework for Simulating Regional-Scale Impacts of Natural Hazards on the Built Environment. Frontiers in the Built Environment. 6:558706. doi: 10.3389/fbuil.2020.558706");
  
    w.setCite(citeText);

    QString manualURL("https://nheri-simcenter.github.io/quoFEM-Documentation/");
    w.setDocumentationURL(manualURL);

    QString messageBoardURL("https://simcenter-messageboard.designsafe-ci.org/smf/index.php?board=4.0");
    w.setFeedbackURL(messageBoardURL);


    
    if (argc > 1) {
      logToFile = true;
      WorkflowCLI cli(&w, theInputApp);
      int cli_complete;
      // if ((cli_complete = cli.parseAndRun(argc, argv)) <= 0)
      //     return cli_complete;
      cli.parseAndRun(argc, argv);
      return 0;
    }

    //
    // move remote interface to a thread
    //

    QThread *thread = new QThread();
    theRemoteService->moveToThread(thread);

    QWidget::connect(thread, SIGNAL(finished()), theRemoteService, SLOT(deleteLater()));
    QWidget::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();

    //
    // show the main window, set styles & start the event loop
    //

    w.show();
    w.statusBar()->showMessage("Ready", 5000);

    // load style sheet

#ifdef Q_OS_WIN
    QFile file(":/styleCommon/stylesheetWIN.qss");
#endif

#ifdef Q_OS_MACOS
    QFile file(":/styleCommon/stylesheetMAC.qss");
#endif

#ifdef Q_OS_LINUX
    QFile file(":/styleCommon/stylesheetMAC.qss");
#endif

  if(file.open(QFile::ReadOnly)) {
      app.setStyleSheet(file.readAll());
      file.close();
  } else {
      qDebug() << "could not open stylesheet";
  }

  //Setting Google Analytics Tracking Information
  /* ****************************************************************/
  GoogleAnalytics::SetMeasurementId("G-7P3PV7SM6J");
  GoogleAnalytics::SetAPISecret("UxuZgMQaS7aoqpQskrcG9w");
  GoogleAnalytics::CreateSessionId();
  GoogleAnalytics::StartSession();

  // Opening a QWebEngineView and using github to get app geographic usage
  QWebEngineView view;
  view.setUrl(QUrl("https://nheri-simcenter.github.io/quoFEM/GA4.html"));
  view.resize(1024, 750);
  view.show();
  view.hide();
  /******************************************************************** */
  
  //
  // exe application event-loop
  //

  int res = app.exec();
  GoogleAnalytics::EndSession();
  return res;
}
