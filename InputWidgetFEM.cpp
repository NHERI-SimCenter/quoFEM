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

#include "InputWidgetFEM.h"
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

#include <OpenSeesParser.h>
#include <FEAPpvParser.h>

#include <InputWidgetParameters.h>

InputWidgetFEM::InputWidgetFEM(InputWidgetParameters *param, QWidget *parent)
    : SimCenterWidget(parent), theParameters(param)
{
    femSpecific = 0;

    layout = new QVBoxLayout();

    QVBoxLayout *name= new QVBoxLayout;

    // text and add button at top
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *textFEM=new SectionTitle();
    textFEM->setText(tr("Finite Element Application"));
    textFEM->setMinimumWidth(250);
    QSpacerItem *spacer = new QSpacerItem(50,10);

    femSelection = new QComboBox();
    femSelection->setMaximumWidth(400);
    femSelection->setMinimumWidth(200);
    femSelection->setToolTip(tr("Remote Application to Run"));
    titleLayout->addWidget(textFEM);
    titleLayout->addItem(spacer);
    titleLayout->addWidget(femSelection);
    titleLayout->addStretch();
    titleLayout->setSpacing(0);
    titleLayout->setMargin(0);

    layout->addLayout(titleLayout);

    layout->setSpacing(10);
    layout->setMargin(0);
    // name->addStretch();

    femSelection->addItem(tr("OpenSees"));
    femSelection->addItem(tr("FEAPpv"));

    connect(femSelection, SIGNAL(currentIndexChanged(QString)), this, SLOT(femProgramChanged(QString)));

    layout->addLayout(name);
    this->femProgramChanged(tr("OpenSees"));

    // layout->addStretch();
    // layout->setSpacing(10);
    layout->setMargin(0);
    layout->addStretch();

    this->setLayout(layout);

}

InputWidgetFEM::~InputWidgetFEM()
{

}


void InputWidgetFEM::clear(void)
{

}



bool
InputWidgetFEM::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject fem;
    fem["program"]=femSelection->currentText();

    fileName1=file1->text();
    fileName2=file2->text();

    fem["inputFile"]=fileName1;
    fem["postprocessScript"]=fileName2;

    QFileInfo fileInfo(fileName1);

    fem["mainInput"]=fileInfo.fileName();
    QString path = fileInfo.absolutePath();
    fem["dir"]=path;

    QFileInfo fileInfo2(fileName2);

    fem["mainPostprocessScript"]=fileInfo2.fileName();

    jsonObject["fem"]=fem;

    return true;
}


bool
InputWidgetFEM::inputFromJSON(QJsonObject &jsonObject)
{
    this->clear();

    if (jsonObject.contains("fem")) {

        QJsonObject fem = jsonObject["fem"].toObject();

        fileName1=fem["inputFile"].toString();
        fileName2=fem["postprocessScript"].toString();
        QString program=fem["program"].toString();

        int index = femSelection->findText(program);
        femSelection->setCurrentIndex(index);
        this->femProgramChanged(program);

        file1->setText(fileName1);
        file2->setText(fileName2);

        // call setFilename1 so parser works on input file
        this->setFilename1(fileName1);
    } else {
        emit sendErrorMessage("ERROR: FEM Input - no fem section in input file");
        return false;
    }
    return true;
}

void InputWidgetFEM::femProgramChanged(const QString &arg1)
{
    if (femSpecific != 0) {
        // layout->rem
        layout->removeWidget(femSpecific);
        delete femSpecific;
        femSpecific = 0;
    }

    femSpecific = new QWidget();
    //femLayout = new QVBoxLayout();

    /************ single script OpenSees *****************
    if (arg1 == QString("OpenSees")) {
        QVBoxLayout *femLayout = new QVBoxLayout();
        QLabel *label1 = new QLabel();
        label1->setText("Main Script");

        QHBoxLayout *fileName1Layout = new QHBoxLayout();
        file1 = new QLineEdit;
        file2 = new QLineEdit;

        QPushButton *chooseFile1 = new QPushButton();
        chooseFile1->setText(tr("Choose"));
        connect(chooseFile1,SIGNAL(clicked()),this,SLOT(chooseFileName1()));

        fileName1Layout->addWidget(file1);
        fileName1Layout->addWidget(chooseFile1);
        fileName1Layout->addStretch();

        fileName1Layout->setSpacing(0);
        fileName1Layout->setMargin(0);

        femLayout->addWidget(label1);
        femLayout->addLayout(fileName1Layout);
        femLayout->setSpacing(10);
        femLayout->setMargin(0);

        femLayout->addStretch();
        femSpecific->setLayout(femLayout);

    } else
    * *********************************************************/

    if (arg1 == QString("FEAPpv")) {
        QVBoxLayout *femLayout = new QVBoxLayout();
        QLabel *label1 = new QLabel();
        label1->setText("Input File");
        QLabel *label2 = new QLabel();
        label2->setText("Postprocess Script");

        QHBoxLayout *fileName1Layout = new QHBoxLayout();
        file1 = new QLineEdit;
        QPushButton *chooseFile1 = new QPushButton();
        chooseFile1->setText(tr("Choose"));
        connect(chooseFile1,SIGNAL(clicked()),this,SLOT(chooseFileName1()));

        fileName1Layout->addWidget(file1);
        fileName1Layout->addWidget(chooseFile1);
        fileName1Layout->addStretch();

        file1->setToolTip(tr("Name of FEAPpv input file"));
        chooseFile1->setToolTip(tr("Push to choose a file from your file system"));

        QHBoxLayout *fileName2Layout = new QHBoxLayout();
        file2 = new QLineEdit;
        QPushButton *chooseFile2 = new QPushButton();

        file2->setToolTip(tr("Name of Python script that will process FEAPpv output file for UQ engine"));
        chooseFile2->setToolTip(tr("Push to choose a file from your file system"));

        chooseFile2->setText(tr("Choose"));
        connect(chooseFile2,SIGNAL(clicked()),this,SLOT(chooseFileName2()));
        fileName2Layout->addWidget(file2);
        fileName2Layout->addWidget(chooseFile2);
        fileName2Layout->addStretch();


        fileName1Layout->setSpacing(10);
        fileName1Layout->setMargin(0);
        fileName2Layout->setSpacing(10);
        fileName2Layout->setMargin(0);

        femLayout->addWidget(label1);
        femLayout->addLayout(fileName1Layout);
        femLayout->addWidget(label2);
        femLayout->addLayout(fileName2Layout);
        femLayout->setSpacing(10);
        femLayout->setMargin(0);
        femLayout->addStretch();
        femSpecific->setLayout(femLayout);

    } else if (arg1 == QString("OpenSees")) {

        QVBoxLayout *femLayout = new QVBoxLayout();
        QLabel *label1 = new QLabel();
        label1->setText("Input Script");
        QLabel *label2 = new QLabel();
        label2->setText("Postprocess Script");

        QHBoxLayout *fileName1Layout = new QHBoxLayout();
        file1 = new QLineEdit;
        QPushButton *chooseFile1 = new QPushButton();
        chooseFile1->setText(tr("Choose"));
        connect(chooseFile1,SIGNAL(clicked()),this,SLOT(chooseFileName1()));

        file1->setToolTip(tr("Name of OpenSees main input script. Note all scripts OpenSees uses must be in the directory or directories below this file"));
        chooseFile1->setToolTip(tr("Push to choose a file from your file system"));

        fileName1Layout->addWidget(file1);
        fileName1Layout->addWidget(chooseFile1);
        fileName1Layout->addStretch();

        QHBoxLayout *fileName2Layout = new QHBoxLayout();
        file2 = new QLineEdit;
        QPushButton *chooseFile2 = new QPushButton();

        file2->setToolTip(tr("Name of Python script that will process OpenSees output files for UQ engine"));
        chooseFile2->setToolTip(tr("Push to choose a file from your file system"));

        chooseFile2->setText(tr("Choose"));
        connect(chooseFile2,SIGNAL(clicked()),this,SLOT(chooseFileName2()));
        fileName2Layout->addWidget(file2);
        fileName2Layout->addWidget(chooseFile2);
        fileName2Layout->addStretch();

        fileName1Layout->setSpacing(10);
        fileName1Layout->setMargin(0);
        fileName2Layout->setSpacing(10);
        fileName2Layout->setMargin(0);

        femLayout->addWidget(label1);
        femLayout->addLayout(fileName1Layout);
        femLayout->addWidget(label2);
        femLayout->addLayout(fileName2Layout);
        femLayout->setSpacing(10);
        femLayout->setMargin(0);
        femLayout->addStretch();
        femSpecific->setLayout(femLayout);
    }
    // femSpecific->addStretch();

    //layout->addWidget(femSpecific);
    layout->insertWidget(1, femSpecific);

}

int InputWidgetFEM::setFilename1(QString name1){
    fileName1 = name1;
    file1->setText(name1);

    // if OpenSees or FEAP parse the file for the variables
    QString pName = femSelection->currentText();
    if (pName == "OpenSees" || pName == "OpenSees-2") {
        OpenSeesParser theParser;
        varNamesAndValues = theParser.getVariables(fileName1);
    }  else if (pName == "FEAPpv") {
        FEAPpvParser theParser;
        varNamesAndValues = theParser.getVariables(fileName1);
    }
    // qDebug() << "VARNAMESANDVALUES: " << varNamesAndValues;
    theParameters->setInitialVarNamesAndValues(varNamesAndValues);

    return 0;
}

void InputWidgetFEM::chooseFileName1(void)
{
    fileName1=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
    int ok = this->setFilename1(fileName1);
}

void
InputWidgetFEM::specialCopyMainInput(QString fileName, QStringList varNames) {
    // if OpenSees or FEAP parse the file for the variables
    if (varNames.size() > 0) {
        QString pName = femSelection->currentText();
        if (pName == "OpenSees" || pName == "OpenSees-2") {
            OpenSeesParser theParser;
            theParser.writeFile(file1->text(), fileName,varNames);
        }  else if (pName == "FEAPpv") {
            FEAPpvParser theParser;
            theParser.writeFile(file1->text(), fileName,varNames);
        }
    }
}

void InputWidgetFEM::chooseFileName2(void)
{
    fileName2=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
    file2->setText(fileName2);
}

QString InputWidgetFEM::getApplicationName() {
    return femSelection->currentText();
}

QString InputWidgetFEM::getMainInput() {
    return fileName1;
}
