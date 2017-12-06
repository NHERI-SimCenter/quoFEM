
// Written: fmckenna
// Purpose: to test the INputWidgetSheetBM widget

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


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
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
    header->setHeadingText(tr("DAKOTA-FEM Uncertainty Quantification Application"));
    layout->addWidget(header);

    //
    // create main input widget
    //

    random = new RandomVariableInputWidget();
    fem = new InputWidgetFEM();
    uq = new InputWidgetUQ();
    results = new DakotaResults();

    inputWidget = new SidebarWidgetSelection();
    inputWidget->addInputWidget(tr("Input Random Variable"), random);
    inputWidget->addInputWidget(tr("FEM Selection"), fem);
    inputWidget->addInputWidget(tr("UQ Selection"), uq);
    inputWidget->addInputWidget(tr("Results"), results);

    inputWidget->buildTreee();

    inputWidget->setMinimumWidth(800);
    layout->addWidget(inputWidget,1.0);

    //
    // add run, run-DesignSafe and exit buttons
    //

    QHBoxLayout *pushButtonLayout = new QHBoxLayout();
    QWidget *buttonWidget = new QWidget();
    buttonWidget->setLayout(pushButtonLayout);

    QPushButton *runButton = new QPushButton();
    runButton->setText(tr("RUN"));
    pushButtonLayout->addWidget(runButton);

    QPushButton *runDesignSafeButton = new QPushButton();
    runDesignSafeButton->setText(tr("RUN at DesignSafe"));
    pushButtonLayout->addWidget(runDesignSafeButton);

    QPushButton *exitButton = new QPushButton();
    exitButton->setText(tr("Exit"));
    pushButtonLayout->addWidget(exitButton);

    connect(runButton, SIGNAL(clicked(bool)),this,SLOT(onRunButtonClicked()));
    connect(runDesignSafeButton, SIGNAL(clicked(bool)),this,SLOT(onRemoteRunButtonClicked()));
    connect(exitButton, SIGNAL(clicked(bool)),this,SLOT(onExitButtonClicked()));

    layout->addWidget(buttonWidget);

    //
    // add SimCenter footer
    //

    FooterWidget *footer = new FooterWidget();
    layout->addWidget(footer);

    this->setCentralWidget(centralWidget);

    this->createActions();
}

MainWindow::~MainWindow()
{

}

bool copyPath(QString sourceDir, QString destinationDir, bool overWriteDirectory)
{
 //   qDebug() << "\n SOURCE: " << sourceDir << "\n DEST: " << destinationDir;

    QDir originDirectory(sourceDir);
//    qDebug() << originDirectory;
    if (! originDirectory.exists())
    {
//        qDebug() << "Origin Directory Does not exist";
        return false;
    }

    qDebug() << originDirectory;

    QDir destinationDirectory(destinationDir);

 //   qDebug() << destinationDirectory;

    if(destinationDirectory.exists() && !overWriteDirectory)
    {
        return false;
    }


    else if(destinationDirectory.exists() && overWriteDirectory)
    {
        destinationDirectory.removeRecursively();
    }

    originDirectory.mkpath(destinationDir);
 //   qDebug() << "MAKE PATH desitiation";
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

    QString appDIR("/Users/fmckenna/NHERI/DakotaFEM2/localApp");
    QString pySCRIPT = appDIR + QDir::separator() + QString("parseJson2.py");

    // invoke the wrapper script
    QProcess *proc = new QProcess();
    QString pythonSTRING(tr("python"));
    QString tDirectory = path + QDir::separator() + QString("tmp.SimCenter");
   // proc->execute(pythonSTRING, QStringList() << pySCRIPT << tDirectory << tmpDirectory);

    QString localScript = appDIR + QDir::separator() + QString("wrapper2.sh");
    proc->execute(localScript, QStringList() << appDIR << path << mainInput );

    //
    // now copy results file from tmp.SimCenter directory and remove tmp directory
    //

   QString sourceDir = path + QDir::separator() + QString("tmp.SimCenter") + QDir::separator();
   QString destinationDir = path + QDir::separator();

   QStringList files;
   files << "dakota.in" << "dakota.out" << "dakotaTab.out" << "dakota.err";

   for (int i = 0; i < files.size(); i++) {
       QString copy = files.at(i);
       QFile::copy(sourceDir + copy, destinationDir + copy);
   }

   QDir dirToRemove(sourceDir);
   dirToRemove.removeRecursively();

    //
    // process the results
    //

    QString filenameOUT = destinationDir + tr("dakota.out");
    QString filenameTAB = destinationDir + tr("dakotaTab.out");

    DakotaResults *result=uq->getResults();
    result->processResults(filenameOUT, filenameTAB);
    results->setResultWidget(result);
}

void MainWindow::onRemoteRunButtonClicked(){
    qDebug() << "NOT YET IMPLEMENTED";
}
void MainWindow::onExitButtonClicked(){
  QApplication::quit();
}

bool MainWindow::save()
{
    if (currentFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(currentFile);
    }
}

bool MainWindow::saveAs()
{
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
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void MainWindow::newFile()
{
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
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    // place contents of file into json object
    QString val;
    val=file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObj = doc.object();

    // close file
    file.close();

    // given the json object, create the C++ objects
    inputWidget->inputFromJSON(jsonObj);

    setCurrentFile(fileName);
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
}

