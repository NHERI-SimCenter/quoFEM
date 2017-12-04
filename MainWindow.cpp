
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


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    centralWidget->setLayout(layout);

    HeaderWidget *header = new HeaderWidget();
    header->setHeadingText(tr("DAKOTA-FEM Uncertainty Quantification Application"));

    layout->addWidget(header);

    random = new RandomVariableInputWidget();
    // edp = new InputWidgetEDP();
    fem = new InputWidgetFEM();
    uq = new InputWidgetUQ();
    results = new DakotaResults();

    inputWidget = new SidebarWidgetSelection();
    inputWidget->addInputWidget(tr("Input Random Variable"), random);
    inputWidget->addInputWidget(tr("FEM Selection"), fem);
    // inputWidget->addInputWidget(tr("Output Paramaters"), edp);
    inputWidget->addInputWidget(tr("UQ Selection"), uq);
    inputWidget->addInputWidget(tr("Results"), results);

    inputWidget->buildTreee();

    inputWidget->setMinimumWidth(800);
    layout->addWidget(inputWidget,1.0);

    QPushButton *run = new QPushButton();
    run->setText(tr("RUN"));
    layout->addWidget(run);
    //connect(removeEDP,SIGNAL(clicked()),this,SLOT(removeEDP()));
    connect(run, SIGNAL(clicked(bool)),this,SLOT(onRunButtonClicked()));
    FooterWidget *footer = new FooterWidget();
    layout->addWidget(footer);

    this->setCentralWidget(centralWidget);

    this->createActions();
}

MainWindow::~MainWindow()
{

}

void MainWindow::onRunButtonClicked() {

    // get program & input file from fem
    QString application = fem->getApplicationName();
    QString mainInput = fem->getMainInput();

    QFileInfo fileInfo(mainInput);
    QDir fileDir = fileInfo.absolutePath();

    QString fileName =fileInfo.fileName();

    // QString path = fileInfo.path();
    QString path = fileDir.absolutePath() + QDir::separator();

    // in inputfile dir, crate a file of the data and copy the dakota python script
    QString filenameTMP = path + tr("dakota.json");
    qDebug() << filenameTMP;

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


    QString appDIR("/Users/fmckenna/NHERI/DakotaFEM2/localApp");
    QString localScript = appDIR + QDir::separator() + QString("wrapper.sh");

    qDebug() << localScript;
    QStringList baby; baby<< appDIR << path << mainInput ;
    qDebug() << baby;

    // invoke the wrapper script
    QProcess *proc = new QProcess();
    //  proc->execute(localScript, QStringList() << appDIR << path << mainInput );

    // read the results
    QString filenameOUT = path + tr("dakota.out");
    QString filenameTAB = path + tr("dakotaTab.out");

    //DakotaResults *result = uq->getResults();
    DakotaResults *result=uq->getResults();
    result->processResults(filenameOUT, filenameTAB);
    results->setResultWidget(result);

    /*
    results = uq->processResults(filenameOUT, filenameTAB);

   // if (results == 0) {
          results = new DakotaResultsSampling(this);
          inputWidget->addInputWidget(tr("Results"), results);
          results->processResults(filenameOUT, filenameTAB);
    }
    inputWidget->setSelection("Results");
    */
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

