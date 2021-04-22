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

#include "FEM.h"
#include <QGridLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <sectiontitle.h>
#include <QFileInfo>
#include <QVectorIterator>
#include <QRadioButton>
#include <QGroupBox>
#include <OpenSeesPyParser.h>
#include <OpenSeesParser.h>
#include <FEAPpvParser.h>

#include <InputWidgetParameters.h>
#include <InputWidgetEDP.h>

#include <qjsondocument.h>

FEM::FEM(InputWidgetParameters *param, InputWidgetEDP *edpwidget, int tagId, QString modelName, QWidget *parent)
  : SimCenterWidget(parent), theParameters(param), theEdpWidget(edpwidget), numInputs(1)
{
    this->tag = tagId;
    femSpecific = 0;
    isGP = false;

    layout = new QVBoxLayout();        
    QVBoxLayout *name= new QVBoxLayout;

    // text and add button at top
    QHBoxLayout *titleLayout = new QHBoxLayout();
    button = new QRadioButton();
    QLabel *textFEM=new QLabel(" Model " + QString::number(tag) + " " + modelName);
    textFEM->setStyleSheet("font-weight: bold");
    QSpacerItem *spacer = new QSpacerItem(50,10);

    femSelection = new QComboBox();
    femSelection->setToolTip(tr("Remote Application to Run"));
    femSelection->addItem(tr("OpenSees"));
    femSelection->addItem(tr("FEAPpv"));
    femSelection->addItem(tr("OpenSeesPy"));
    femSelection->addItem(tr("Custom"));
    femSelection->addItem(tr("SurrogateGP"));
    connect(femSelection, SIGNAL(currentIndexChanged(QString)), this, SLOT(femProgramChanged(QString)));

    titleLayout->addWidget(button);
    titleLayout->addWidget(textFEM);
    titleLayout->addItem(spacer);
    titleLayout->addWidget(femSelection);
    titleLayout->addStretch();
    titleLayout->setSpacing(0);
    titleLayout->setMargin(0);
    titleWidget = new QWidget();
    titleWidget->setLayout(titleLayout);

    layout->addWidget(titleWidget);
    layout->setSpacing(10);
    //layout->setMargin(10);
    // name->addStretch();

    layout->addLayout(name);
    this->femProgramChanged(tr("OpenSees"));

    // layout->addStretch();
    // layout->setSpacing(10);
    //layout->setMargin(10);
    layout->addStretch();

    this->setLayout(layout);

    if (tag==0) {
        titleWidget->setVisible(false);
    }

}

void FEM::hideHeader(bool tog) {
    titleWidget->setVisible(!tog);
}


FEM::~FEM()
{
}

bool
FEM::outputToJSON(QJsonObject &jsonObject)
{
    QJsonArray apps;
    jsonObject["program"]=femSelection->currentText();
    //jsonObject["numInputs"]=numInputs;
    jsonObject["modelTag"]=tag;

    if (femSelection->currentText() == "Custom") {
       // Add driver script and post-processing script to JSON file
       QString driverScript      = inputFilenames.at(0)->text();
       QString postProcessScript = postprocessFilenames.at(0)->text();
       jsonObject["inputFile"]          = driverScript;
       jsonObject["postprocessScript"]  = postProcessScript;

       QFileInfo driverInfo(driverScript);

       jsonObject["mainInput"] = driverInfo.fileName();
       QString path     = driverInfo.absolutePath();
       jsonObject["dir"]       = path;

       QFileInfo postProcessInfo(postProcessScript);
       jsonObject["mainPostprocessScript"] = postProcessInfo.fileName();

       // Add all additional input files to JSON
       for (auto const val : customInputFiles) {
            apps.append(val->text());
       }
       jsonObject["fileInfo"] = apps;
       
    //} else if (numInputs == 1) {
    } else {
        QString fileName1=inputFilenames.at(0)->text();
        QString fileName2=postprocessFilenames.at(0)->text();

        jsonObject["inputFile"]=fileName1;
        jsonObject["postprocessScript"]=fileName2;

        QFileInfo fileInfo(fileName1);

        jsonObject["mainInput"]=fileInfo.fileName();
        QString path = fileInfo.absolutePath();
        jsonObject["dir"]=path;

        QFileInfo fileInfo2(fileName2);
        jsonObject["mainPostprocessScript"]=fileInfo2.fileName();

        if (femSelection->currentText() == "OpenSeesPy") {

             QString fileName3=parametersFilenames.at(0)->text();
             QFileInfo pFileInfo(fileName3);
             jsonObject["parametersFile"]=pFileInfo.fileName();
             jsonObject["parametersScript"]=fileName3;
        } else if (femSelection->currentText() == "SurrogateGP") {
            jsonObject["varThres"]=thresVal->text().toDouble();
            jsonObject["femOption"]=femOpt;
        }

    }
    //else{
//        for (int i=0; i<numInputs; i++) {
//            QJsonObject obj;
//            QString fileName1=inputFilenames.at(i)->text();
//            QString fileName2=postprocessFilenames.at(i)->text();

//            obj["inputFile"]=fileName1;
//            obj["postprocessScript"]=fileName2;

//            QFileInfo fileInfo(fileName1);

//            obj["mainInput"]=fileInfo.fileName();
//            QString path = fileInfo.absolutePath();
//            obj["dir"]=path;

//            QFileInfo fileInfo2(fileName2);
//            obj["mainPostprocessScript"]=fileInfo2.fileName();
//            apps.append(obj);


//        }
//        jsonObject["fileInfo"] = apps;
//    }

//    jsonObject["FEM"]=fem;

    return true;
}


bool
FEM::inputFromJSON(QJsonObject &femObject)
{
    //this->clear();

    //if (femObject.contains("subprogram")) {
        QString program;
        if (femObject.contains("program")) {
            program=femObject["program"].toString();
        } else {
            program=femObject["program"].toString();
        }

        int index = femSelection->findText(program);
        femSelection->setCurrentIndex(index);
        this->femProgramChanged(program);

        numInputs = 1;
        if (femObject.contains("numInputs") )
            numInputs = femObject["numInputs"].toInt();

        if (numInputs == 1) {	  
            QString fileName1=femObject["inputFile"].toString();;
            QString fileName2=femObject["postprocessScript"].toString();

            inputFilenames.at(0)->setText(fileName1);
            postprocessFilenames.at(0)->setText(fileName2);

            if (femSelection->currentText() == "OpenSeesPy") {
                QString fileName3=femObject["parametersScript"].toString();
                parametersFilenames.at(0)->setText(fileName3);
                this->parseInputfilesForRV(fileName3);
            } else if (femSelection->currentText() == "SurrogateGP") {
                this->parseInputfilesForRV(fileName1);
                thresVal->setText(QString::number(femObject["varThres"].toDouble()));
                femOpt=femObject["femOption"].toString();
                if (femOpt == "giveError") {
                    option1Button ->setChecked(true);
                } else if (femOpt == "continue") {
                    option2Button ->setChecked(true);
                } else if (femOpt == "doSimulation") {
                    option3Button ->setChecked(true);
                }

            } else {
                this->parseInputfilesForRV(fileName1);
            }

            if (femSelection->currentText() == "Custom") {
              // Check how many input files
              customInputFiles.resize(femObject["fileInfo"].toArray().count());

              int count = 0;
              for (auto const& val : femObject["fileInfo"].toArray()) {
                customInputFiles.at(count)->setText(val.toString());
                count++;
              }
              this->parseInputfilesForRV(fileName1);
            }

        }
        // call setFilename1 so parser works on input file

        if (isGP) {
            if (GPoption=="GPdata")
                femSpecific -> setVisible(false);
        }

    //} else {
    //    emit sendErrorMessage("ERROR: FEM Input - no fem section in input file");
    //    return false;
    //}
    return true;
}

void FEM::femProgramChanged(const QString &arg1)
{
    if (arg1!="MultipleModels")
        femSelection->setCurrentText(arg1);

    //
    // remove old widget and clear file names
    //
    if (femSpecific != nullptr) {      
      layout->removeWidget(femSpecific);
    }
    
    inputFilenames.clear();
    postprocessFilenames.clear();
    parametersFilenames.clear();
    customInputFiles.clear();

    auto newFemSpecific = new QWidget();
    auto oldFemSpecific = femSpecific;

    if (arg1 == QString("Custom")) {
      theCustomInputNumber = new QSpinBox();
      theCustomLayout = new QVBoxLayout();
      theCustomFileInputs = new QVBoxLayout();
      theCustomFileInputWidget = new QWidget();      

      QLabel *customNumberLabel = new QLabel();
      customNumberLabel->setText(tr("Number of input files and executables required by driver script"));      
      QHBoxLayout *numberLayout = new QHBoxLayout();
      numberLayout->addWidget(customNumberLabel);
      numberLayout->addWidget(theCustomInputNumber);
      numberLayout->addStretch();
      
      theCustomLayout->addLayout(numberLayout);

      theCustomFileInputs = new QVBoxLayout();
      theCustomFileInputWidget = new QWidget();
      theCustomFileInputWidget->setLayout(theCustomFileInputs);
      theCustomLayout->addWidget(theCustomFileInputWidget);
           
      QVBoxLayout *customLayout = new QVBoxLayout();      

      // Add location to add driver script
      SectionTitle *textCustom=new SectionTitle();
      textCustom->setText(tr("Input driver script that runs and post-processes application"));
      QSpacerItem *customSpacer = new QSpacerItem(50,10);
      
      QLabel *driverLabel = new QLabel();
      driverLabel->setText(tr("Driver script"));
      QLineEdit *driverFile = new QLineEdit;
      QPushButton *chooseDriver = new QPushButton();
      chooseDriver->setText(tr("Choose"));
      connect(chooseDriver, &QPushButton::clicked, this, [=](){
                driverFile->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*)"));
            });      
      chooseDriver->setToolTip(tr("Push to choose a file from your file system"));
      
      QGridLayout *driverLayout = new QGridLayout();
      driverLayout->addWidget(driverLabel, 1, 0);
      driverLayout->addWidget(driverFile, 1, 1);
      driverLayout->addWidget(chooseDriver, 1, 2);
      driverLayout->setColumnStretch(3,1);
      driverLayout->setColumnStretch(1,3);

      // Add location to add post-processing script
      QLabel *postProcessingLabel = new QLabel();
      postProcessingLabel->setText("Postprocessing script");
      QLineEdit *postProcessScript = new QLineEdit;
      QPushButton *choosePostProcessing = new QPushButton();
      
      connect(choosePostProcessing, &QPushButton::clicked, this, [=](){
            postProcessScript->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*)"));
							});
      choosePostProcessing->setText(tr("Choose"));
      choosePostProcessing->setToolTip(tr("Push to choose a file from your file system"));

      QGridLayout *postProcessLayout = new QGridLayout();
      postProcessLayout->addWidget(postProcessingLabel, 1, 0);
      postProcessLayout->addWidget(postProcessScript, 1, 1);
      postProcessLayout->addWidget(choosePostProcessing, 1, 2);
      postProcessLayout->setColumnStretch(3,1);
      postProcessLayout->setColumnStretch(1,3);      

      // Add custom layout for specifying additional inputs
      customLayout->addWidget(textCustom);
      customLayout->addItem(customSpacer);
      customLayout->addLayout(driverLayout);
      customLayout->addLayout(postProcessLayout);
      customLayout->addLayout(theCustomLayout);
      customLayout->addStretch();

      connect(theCustomInputNumber, SIGNAL(valueChanged(int)), this, SLOT(customInputNumberChanged(int)));      
      theCustomInputNumber->setValue(0);                
      inputFilenames.append(driverFile);
      postprocessFilenames.append(postProcessScript);
      newFemSpecific->setLayout(customLayout);
      
    } else if (numInputs == 1) {

        //QGridLayout *femLayout = new QGridLayout(); // Changed it to local->private for surrogate - may cause problem in multiple model case
        femLayout = new QGridLayout();

        QLabel *label1 = new QLabel();
        QLineEdit *file1 = new QLineEdit;
        QPushButton *chooseFile1 = new QPushButton();
        chooseFile1->setText(tr("Choose"));
        connect(chooseFile1, &QPushButton::clicked, this, [=](){
             file1->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
             if ((arg1 == QString("FEAPpv")) || (arg1 == QString("OpenSees")) || (arg1 == QString("SurrogateGP")) )
                    this->parseInputfilesForRV(file1->text());
         });

        chooseFile1->setToolTip(tr("Push to choose a file from your file system"));

        femLayout->addWidget(label1, 0,0);
        femLayout->addWidget(file1,  0,1);
        femLayout->addWidget(chooseFile1, 0, 2);

        QLabel *label2 = new QLabel();
        label2->setText("Postprocess Script");
        QLineEdit *file2 = new QLineEdit;
        QPushButton *chooseFile2 = new QPushButton();

        connect(chooseFile2, &QPushButton::clicked, this, [=](){
            file2->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
        });
         chooseFile2->setText(tr("Choose"));
        chooseFile2->setToolTip(tr("Push to choose a file from your file system"));

        if (arg1 == QString("FEAPpv")){
            label1->setText("Input File");
            file1->setToolTip(tr("Name of FEAPpv input file"));
            file2->setToolTip(tr("Name of Python script that will process FEAPpv output file for UQ engine"));
        } else if (arg1 == "OpenSees") {
            label1->setText("Input Script");
            file1->setToolTip(tr("Name of OpenSees input script"));
            file2->setToolTip(tr("Name of Python/Tcl script that will process OpenSees output file for UQ engine"));
        } else if (arg1 == "SurrogateGP"){
            label1->setText("SurrogateGP Info (.json)");
            label2->setText("SurrogateGP Model (.pkl)");
            file1->setToolTip(tr("Name of SurrogateGP model file (.json)"));
            file2->setToolTip(tr("Name of SurrogateGP info file (.pkl)"));
            optionsLayout = 0;
        } else {
            label1->setText("Input Script");
            file1->setToolTip(tr("Name of OpenSeesPy input script"));
            file2->setToolTip(tr("Name of Python script that will process OpenSeesPy output"));
        }

        femLayout->addWidget(label2, 1,0);
        femLayout->addWidget(file2, 1,1);
        femLayout->addWidget(chooseFile2, 1,2);


        if (arg1 == "OpenSeesPy") {
            QLabel *label3 = new QLabel();
            label3->setText("Parameters File");
            QLineEdit *file3 = new QLineEdit;
            file2->setToolTip(tr("Name of Python script that contains defined parameters"));
            QPushButton *chooseFile3 = new QPushButton();

            connect(chooseFile3, &QPushButton::clicked, this, [=](){
                file3->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
                this->parseInputfilesForRV(file3->text());
            });
            chooseFile3->setText(tr("Choose"));
            chooseFile3->setToolTip(tr("Push to choose a file from your file system"));

            parametersFilenames.append(file3);

            femLayout->addWidget(label3, 2,0);
            femLayout->addWidget(file3, 2,1);
            femLayout->addWidget(chooseFile3, 2,2);

            femLayout->setColumnStretch(3,1);
            femLayout->setColumnStretch(1,3);
        }
        inputFilenames.append(file1);
        postprocessFilenames.append(file2);
        newFemSpecific->setLayout(femLayout);
        femLayout->setColumnStretch(3,1);
        femLayout->setColumnStretch(1,3);
    } else {

//        QVBoxLayout *multiLayout = new QVBoxLayout();
//        newFemSpecific->setLayout(multiLayout);

//        for (int i=0; i< numInputs; i++) {
//            //QGridLayout *femLayout = new QGridLayout(); // Changed it to local->private for surrogate - may cause problem in multiple model case
//            femLayout = new QGridLayout();

//            QLabel *modelIdLabel = new QLabel("Model " + QString::number(i+1));
//            modelIdLabel-> setStyleSheet("font-weight: bold");

//            QLabel *label1 = new QLabel();
//            QLabel *label2 = new QLabel();
//            label2->setText("Postprocess Script");

//            QLineEdit *file1 = new QLineEdit;
//            QPushButton *chooseFile1 = new QPushButton();
//            chooseFile1->setText(tr("Choose"));
//            connect(chooseFile1, &QPushButton::clicked, this, [=](){
//                file1->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
//                this->parseInputfilesForRV(file1->text());
//            });

//            if (arg1 == QString("FEAPpv")){
//                label1->setText("Input File");
//                file1->setToolTip(tr("Name of FEAPpv input file"));
//            } else {
//                label1->setText("Input Script");
//                file1->setToolTip(tr("Name of OpenSees input script"));
//            }

//            chooseFile1->setToolTip(tr("Push to choose a file from your file system"));

//            QLineEdit *file2 = new QLineEdit;
//            QPushButton *chooseFile2 = new QPushButton();

//            connect(chooseFile2, &QPushButton::clicked, this, [=](){
//                file2->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
//            });

//            file2->setToolTip(tr("Name of Python script that will process FEAPpv output file for UQ engine"));
//            chooseFile2->setToolTip(tr("Push to choose a file from your file system"));

//            chooseFile2->setText(tr("Choose"));

//            femLayout->addWidget(modelIdLabel, 0,0);
//            femLayout->addWidget(label1, 1,0);
//            femLayout->addWidget(file1,  1,1);
//            femLayout->addWidget(chooseFile1, 1, 2);
//            femLayout->addWidget(label2, 2,0);
//            femLayout->addWidget(file2, 2,1);
//            femLayout->addWidget(chooseFile2, 2,2);
//            femLayout->addWidget(new QLabel(""), 3,0);

//            femLayout->setColumnStretch(4,1);
//            femLayout->setColumnStretch(1,3);

//            inputFilenames.append(file1);
//            postprocessFilenames.append(file2);
//            multiLayout->addLayout(femLayout);

//        }
//        newFemSpecific->setLayout(multiLayout);
    }

    layout->insertWidget(1, newFemSpecific);
    femSpecific = newFemSpecific;
    oldFemSpecific->deleteLater();
}

int FEM::parseInputfilesForRV(QString name1){
    QString fileName1 = name1;
    //  file1->setText(name1);

    // if OpenSees or FEAP parse the file for the variables
    QString pName = femSelection->currentText();
    if (pName == "OpenSees" || pName == "OpenSees-2") {
        OpenSeesParser theParser;
        varNamesAndValues = theParser.getVariables(fileName1);
    }  else if (pName == "FEAPpv") {
        FEAPpvParser theParser;
        varNamesAndValues = theParser.getVariables(fileName1);
    } else if (pName == "OpenSeesPy") {
        OpenSeesPyParser theParser;
        varNamesAndValues = theParser.getVariables(fileName1);
    } else if (pName == "SurrogateGP") {
        varNamesAndValues = parseGPInputs(fileName1);
    } else if (pName == "Custom") {
      // No need to do anything here for custom
    }

    if (tag>0)
        this->addTagId();
      
    // qDebug() << "VARNAMESANDVALUES: " << varNamesAndValues;
    if (pName != "Custom" && !isGP) {
      theParameters->setInitialVarNamesAndValues(varNamesAndValues);
    } else if (pName != "Custom" && isGP) {
      theParameters->setGPVarNamesAndValues(varNamesAndValues);
    }
    return 0;
}

void FEM::addTagId(void) {
    //for ( const auto& i : varNamesAndValues  )
    for (int i=0; i<varNamesAndValues.size();i++)
    {
        if (i % 2 == 0) {
            varNamesAndValues[i] = varNamesAndValues[i] + "." +QString::number(tag);
        }
    }
}

void
FEM::numModelsChanged(int newNum) {
    numInputs = newNum;
    this->femProgramChanged(femSelection->currentText());
}

void FEM::customInputNumberChanged(int numCustomInputs) {
  // numInputs = numCustomInputs;
  auto tempLayout = theCustomFileInputs;

  // Delete old inputs
  QLayoutItem * item;
  while ((item = tempLayout->takeAt(0)) != nullptr) {
    if (item->layout() != nullptr) {
      auto sublayout = item->layout();
      while (sublayout->takeAt(0) != nullptr) {
  	if (sublayout->takeAt(0)->widget() != nullptr) {
  	  auto widget = sublayout->takeAt(0)->widget();
  	  sublayout->removeWidget(widget);
  	  widget->deleteLater();
  	}
      }
      tempLayout->removeItem(item);
      item->layout()->deleteLater();
    } else if (item->widget() != nullptr) {
      tempLayout->removeItem(item);
      item->widget()->deleteLater();
    }
  }

  tempLayout->deleteLater();
  tempLayout = nullptr;

  
  // Create new file inputs
  QVBoxLayout * newLayout = new QVBoxLayout();
  customInputFiles.clear();
  for (int i = 0; i < numCustomInputs; ++i) {
    auto inputLabel = new QLabel();
    auto inputFile = new QLineEdit();
    inputLabel->setText(tr("Input location"));
    auto *chooseFile = new QPushButton();
    chooseFile->setText(tr("Choose"));
    connect(chooseFile, &QPushButton::clicked, this, [=](){
                inputFile->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*)"));
            });      
    chooseFile->setToolTip(tr("Push to choose a file from your file system"));
    
    auto inputLayout = new QHBoxLayout();
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(inputFile);
    inputLayout->addWidget(chooseFile);
    newLayout->addLayout(inputLayout);
    customInputFiles.append(inputFile);    
  }
  newLayout->addStretch();  

  // Replace outdated widget with new one containing new file input layout
  theCustomFileInputs = newLayout;
  theCustomFileInputWidget->layout()->deleteLater();

  theCustomLayout->removeWidget(theCustomFileInputWidget);  
  theCustomFileInputWidget->deleteLater();

  theCustomFileInputWidget = new QWidget();
  theCustomFileInputWidget->setLayout(theCustomFileInputs);
  theCustomLayout->addWidget(theCustomFileInputWidget);
    
  newLayout = nullptr;
}


void
FEM::specialCopyMainInput(QString fileName, QStringList varNames) {
    // if OpenSees or FEAP parse the file for the variables
    if (femSelection->currentText() != "Custom")
    {
       if (varNames.size() > 0)
       {
          QString pName = femSelection->currentText();
          if (pName == "OpenSees" || pName == "OpenSees-2")
          {
             OpenSeesParser theParser;
             QVectorIterator< QLineEdit* > i(inputFilenames);
             while (i.hasNext())
                theParser.writeFile(i.next()->text(), fileName, varNames);
          }
          else if (pName == "FEAPpv")
          {
             FEAPpvParser theParser;
             QVectorIterator< QLineEdit* > i(inputFilenames);
             while (i.hasNext())
                theParser.writeFile(i.next()->text(), fileName, varNames);
          }
          else if (pName == "OpenSeesPy")
          {
             // do things a little different!
             QFileInfo file(fileName);
             QString fileDir = file.absolutePath();
             // qDebug() << "FILENAME: " << fileName << " path: " << fileDir;
             // qDebug() << "LENGTHS:" << inputFilenames.count() << parametersFilenames.count() <<
             // parametersFilenames.length();
             OpenSeesPyParser theParser;
             bool hasParams = false;
             QVectorIterator< QLineEdit* > i(parametersFilenames);
             QString newName = fileDir + QDir::separator() + "tmpSimCenter.params";
             while (i.hasNext())
             {
                QString fileName = i.next()->text();
                if (fileName.length() != 0)
                {
                   theParser.writeFile(fileName, newName, varNames);
                   hasParams = true;
                }
             }

             if (hasParams == false)
             {
                QString newName = fileDir + QDir::separator() + "tmpSimCenter.script";
                QVectorIterator< QLineEdit* > i(inputFilenames);
                while (i.hasNext())
                {
                   QFile::copy(i.next()->text(), newName);
                }
             }
          }
       }
    }
}

QString FEM::getApplicationName() {
    return femSelection->currentText();
}

QString FEM::getMainInput() {
  if (inputFilenames.length() != 0) {
    return inputFilenames.at(0)->text();    
  } else {
    return(QString(""));    
  }
}

QVector< QString > FEM::getCustomInputs() const {
   QVector< QString > stringOutput(customInputFiles.size());

   unsigned int count = 0;
   for (auto const& val : customInputFiles)
   {
      stringOutput[count] = val->text();
      count++;
   }

   return stringOutput;
}


// for surrogate. It makes RVs to be uniform
void FEM::setAsGP(bool tog){
    isGP = tog;
}


// Simple interpolation to show the percentage value.
double FEM::interpolateForGP(QVector<double> X, QVector<double> Y, double Xval){
    int N = X.count();

    if (X.size()==1) {
        return 0; // make it %
    }

    double estY=0;
    if (X[0]<X[1]) {
        for (int np=0; np<N; np++) {
            if(X[np] > Xval) {
                if (np==0)
                    estY = X[0];
                else
                    estY = Y[np-1]+(Y[np]-Y[np-1])*(Xval-X[np-1])/(X[np]-X[np-1]);
                break;
            }
        }
    } else {
        for (int np=N-1; np>-1; np--) {
            if(X[np] > Xval) {
                if (np==0)
                    estY = X[0];
                else
                    estY = Y[np-1]+(Y[np]-Y[np-1])*(Xval-X[np-1])/(X[np]-X[np-1]);
                break;
            }
        }
    }

    return std::round(estY*1000)/10; // make it %
}


QStringList FEM::parseGPInputs(QString file1){

    //
    // want to make a GP option box
    //

    // completely delete layout and sublayouts
    if (optionsLayout != nullptr) {

        QLayoutItem * item;
        QLayout * sublayout;
        QWidget * widget;
        while ((item = optionsLayout->takeAt(0))) {
            if ((sublayout = item->layout()) != 0) {/* do the same for sublayout?*/}
            else if ((widget = item->widget()) != 0) {widget->hide(); delete widget;}
            else {delete item;}
        }

    }


    QFile file(file1);
    QString appName, mainScriptDir,postScriptDir;
    QStringList qoiNames;
    thresVal = new QLineEdit;

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString val;
        val=file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject jsonSur = doc.object();
        file.close();

        auto GPidentifier = jsonSur.find("kernName"); // should be the right .json file
        if (!jsonSur.isEmpty() && GPidentifier != jsonSur.end()) {
            bool from_data=!jsonSur["doSimulation"].toBool();
            QJsonArray RVArray = jsonSur["randomVariables"].toArray();
            QJsonArray QoIArray = jsonSur["ylabels"].toArray();
            foreach (const QJsonValue & v, RVArray){
                 QJsonObject jsonRV = v.toObject();
                 varNamesAndValues.push_back(jsonRV["name"].toString());
                 varNamesAndValues.push_back(QString::number(jsonRV["value"].toDouble()));
            }

            foreach (const QJsonValue & v, QoIArray){
                 qoiNames.push_back(v.toString());
            }

            if (from_data) {
                isData = true;
                appName = "data";
            } else {
                isData = false;
                QJsonObject jsonPred = jsonSur["predError"].toObject();
                QJsonArray precArray = jsonPred["percent"].toArray();
                QJsonArray valsArray = jsonPred["value"].toArray();
                QJsonObject jsonFEM = jsonSur["fem"].toObject();
                // interpolate
                QVector<double> percVal_tmp, thrsVal_tmp;
                foreach (const QJsonValue & v, precArray)
                     percVal_tmp.push_back(v.toDouble());
                foreach (const QJsonValue & v, valsArray)
                     thrsVal_tmp.push_back(v.toDouble());
                percVals=percVal_tmp;
                thrsVals=thrsVal_tmp;
                thres = this->interpolateForGP(percVal_tmp,thrsVal_tmp,0.5);
                // save names
                appName = jsonFEM["program"].toString();
                mainScriptDir = jsonFEM["inputFile"].toString();
                postScriptDir = jsonFEM["postprocessScript"].toString();
            }
        } else {
            appName = "NA";
            thresVal->setText("0");
            return {};
        }
    } else {
        appName = "NA";
        thresVal->setText("0");
        return {};
    }

    groupBox = new QGroupBox("Options");
    optionsLayout = new QGridLayout();
    groupBox->setLayout(optionsLayout);

    femLayout->addWidget(new QLabel(""),2,0);
    femLayout->addWidget(groupBox,3,0,1,3);

    QLabel *labelVarThres = new QLabel("Maximum Allowable Normalized Variance  ");
    QLabel *optionsLabel = new QLabel("When surroagate model gives imprecise prediction at certain sample locations");

    option1Button = new QRadioButton();
    QLabel *option1Label = new QLabel("     Stop Analysis");
    option1Button->setChecked(true);
    option2Button = new QRadioButton();
    QLabel *option2Label = new QLabel("     Continue (not recommended)");
    option3Button = new QRadioButton();
    QLabel *option3Label = new QLabel("     Run Exact FEM Simulation");
    QLabel *labelThresMsg = new QLabel(" ");
    labelThresMsg->setStyleSheet("color: red");

    thresVal->setMaximumWidth(100);
    optionsLayout->addWidget(labelVarThres, 0,0);
    optionsLayout->addWidget(thresVal,0,1, Qt::AlignVCenter);
    optionsLayout->addWidget(labelThresMsg,1,0,1,-1);
    optionsLayout->addWidget(optionsLabel, 2,0,1,-1);
    optionsLayout->addWidget(option1Label, 3,0,1,-1);
    optionsLayout->addWidget(option1Button, 3,0);
    optionsLayout->addWidget(option2Label, 4,0,1,-1);
    optionsLayout->addWidget(option2Button, 4,0);
    optionsLayout->addWidget(option3Label, 5,0,1,-1);
    optionsLayout->addWidget(option3Button, 5,0);
    option1Button -> setDisabled(true);
    option2Button -> setDisabled(true);
    option3Button -> setDisabled(true);
    labelThresMsg->setVisible(false);
    QLabel *labelProgName = new QLabel();
    QLabel *labelProgDir1 = new QLabel();
    QLabel *labelProgDir2 = new QLabel();
    optionsLayout->addWidget(labelProgName, 6,0,1,-1);
    optionsLayout->addWidget(labelProgDir1, 7,0,1,-1);
    optionsLayout->addWidget(labelProgDir2, 8,0,1,-1);
    labelProgName->setVisible(false);
    labelProgDir1->setVisible(false);
    labelProgDir2->setVisible(false);
    optionsLayout->setColumnStretch(3, 1);

    if (appName == "NA") {
        option1Button -> setDisabled(true);
        option2Button -> setDisabled(true);
        option3Button -> setDisabled(true);
        labelThresMsg -> setVisible(false);
        option3Label->setText("     Run Exact FEM simulation");
        thresVal->setDisabled(true);
    } else if (appName == "data") {
        option1Button -> setDisabled(false);
        option2Button -> setDisabled(false);
        option3Button -> setDisabled(true);
        option1Button -> setChecked(true);
        labelThresMsg->setVisible(false);
        option3Label->setText("     Run Exact FEM simulation (not supported for data-based surrogate model)");
        option3Label->setStyleSheet("color: grey");
        percVals={0};
        thrsVals={0};
        thresVal->setText("0.02");
        thresVal->setDisabled(false);
    } else {
        // interpolate
        option1Button -> setDisabled(false);
        option1Button -> setChecked(true);
        option2Button -> setDisabled(false);
        option3Button -> setDisabled(false);
        labelThresMsg -> setVisible(true);
        option3Label->setText("     Run Exact FEM simulation");
        option3Label->setStyleSheet("color: black");
        labelProgName->setText("      • Application Name: " + appName);
        labelProgDir1->setText("      • Main Script: "+ mainScriptDir);
        labelProgDir2->setText("      • Postprocess Script: "+ postScriptDir );
        labelProgName->setVisible(false);
        labelProgDir1->setVisible(false);
        labelProgDir2->setVisible(false);
        thresVal->setDisabled(false);
    }

    //
    // For option 3
    //

    connect(thresVal, &QLineEdit::textChanged, this, [=](QString val) mutable {
        double thres=val.toDouble();
        double percEst = this->interpolateForGP(thrsVals,percVals,thres);
        if (thres>thrsVals[thrsVals.size()-1]) {
            percEst=std::round(percVals[percVals.size()-1]*1000)/10;
        } else if (thres<thrsVals[0]) {
            percEst=std::round(percVals[0]*1000)/10;
        }
        if (!isData) {
            labelThresMsg->setText("Note: around " + QString::number(percEst) + "% of new samples in training range will exceed the tolerance limit.");
        }
    });

    femOpt = "giveError";
    connect(option1Button, &QCheckBox::toggled, this, [=](bool tog){
        if (tog==true)
        {
            femOpt = "giveError";
            option2Button->setChecked(false);
            option3Button->setChecked(false);
        }
    });

    connect(option2Button, &QCheckBox::toggled, this, [=](bool tog){
        if (tog==true)
        {
            femOpt = "continue";
            option1Button->setChecked(false);
            option3Button->setChecked(false);
        }
    });

    connect(option3Button, &QCheckBox::toggled, this, [=](bool tog){
        if (tog==true)
        {
            femOpt = "doSimulation";
            option2Button->setChecked(false);
            option1Button->setChecked(false);
            labelProgName->setVisible(true);
            labelProgDir1->setVisible(true);
            labelProgDir2->setVisible(true);
        } else {
            labelProgName->setVisible(false);
            labelProgDir1->setVisible(false);
            labelProgDir2->setVisible(false);
        }
    });
    thresVal->setText(QString::number(thres/100));
    //theParameters->setInitialVarNamesAndValues(varNamesAndValues);
    theEdpWidget->setGPQoINames(qoiNames);
    option1Button->setChecked(false);
    return varNamesAndValues;
}
