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
#include "InputWidgetEDP.h"
#include "InputWidgetParameters.h"
#include "OpenSeesParser.h"
#include "FEAPpvParser.h"
#include "OpenSeesPyParser.h"


#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QFrame>
#include <sectiontitle.h>
#include <QSpacerItem>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QDir>

#include <QDebug>


InputWidgetFEM::InputWidgetFEM(InputWidgetParameters *param, InputWidgetEDP *edpwidget, QWidget *parent)
  : SimCenterWidget(parent), numInputs(1), theEdpWidget(edpwidget), theParameters(param)
{
    numInputs = 1;
    verticalLayout = new QVBoxLayout();
    this->setLayout(verticalLayout);
    this->makeFEM();
}

InputWidgetFEM::~InputWidgetFEM()
{

}

void
InputWidgetFEM::makeFEM(void)
{
    // title & add button

    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *textFEM=new SectionTitle();
    textFEM->setText(tr("Finite Element Method Application"));
    //textFEM->setMinimumWidth(250);
    QSpacerItem *spacer = new QSpacerItem(50,10);

    femSelection = new QComboBox();
    femSelection->setToolTip(tr("Remote Application to Run"));
    titleLayout->addWidget(textFEM);
    titleLayout->addItem(spacer);
    titleLayout->addWidget(femSelection);
    titleLayout->addStretch();
    titleLayout->setSpacing(0);
    titleLayout->setMargin(0);

    verticalLayout->addLayout(titleLayout);

    femSelection->addItem(tr("OpenSees"));
    femSelection->addItem(tr("FEAPpv"));
    femSelection->addItem(tr("OpenSeesPy"));
    femSelection->addItem(tr("Custom"));
    femSelection->addItem(tr("SurrogateGP"));
    //femSelection->addItem(tr("MultipleModels"));
    connect(femSelection, SIGNAL(currentIndexChanged(QString)), this, SLOT(femProgramChanged(QString)));

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    fem = new QFrame();
    fem->setFrameShape(QFrame::NoFrame);
    fem->setLineWidth(0);

    femLayout = new QVBoxLayout;

    //setLayout(layout);
    fem->setLayout(femLayout);
    femLayout->addStretch();
    femLayout->setDirection(QBoxLayout::TopToBottom);
    femLayout->setSpacing(0);
    this->addFEM(0);

    sa->setWidget(fem);
    verticalLayout->addWidget(sa);

    //verticalLayout->setSpacing(0);
    verticalLayout->setMargin(0);

}

//void InputWidgetFEM::setFemNames(QStringList femNames) {

//    int numInputs = 1;
//    // remove existing boxes
//    int numInputss = theFEMs.size();
//    for (int i = numInputss-1; i >= 0; i--) {
//        FEM *theFEM = theFEMs.at(i);
//        theFEM->close();
//        femLayout->removeWidget(theFEM);
//        theFEMs.remove(i);
//        //theFEM->setParent(0);
//        delete theFEM;
//    }

//    for (int i=0; i<numInputs; i++) {
//        QString varName = femNames.at(i);
//        FEM *theFEM = new FEM(theParameters, theEdpWidget);
//        theFEMs.append(theFEM);
//        femLayout->insertWidget(femLayout->count()-1, theFEM);
//    }
//}



int
InputWidgetFEM::setFEMforGP(QString option){
    GPoption = option;
    int index = femSelection->findText("MultipleModels");
    if (index>-1) {
        femSelection->removeItem(index);
    }

    if (option == "reset")
    {
        numInputs=1;
        femSelection -> setDisabled(false);
        //this->femProgramChanged("OpenSees");
        //femSelection->setCurrentIndex(1);
        //femSelection->setCurrentIndex(0);
        this -> setContentsVisible(true);
        theFEMs.at(0)->setAsGP(false);
        this->parseAllInputfiles();
        theParameters->setCorrelationDisabled(false);
    } else if (option == "GPdata") {
        numInputs=1;
        femSelection -> setDisabled(true);
        //femSelection->setCurrentIndex(1);
        //femSelection->setCurrentIndex(0);
        femProgramChanged("OpenSees");
        this -> setContentsVisible(false);
        theFEMs.at(0)->setAsGP(true);
        this->parseAllInputfiles();
        theParameters->setCorrelationDisabled(true);
    } else if (option == "GPmodel")
    {
        numInputs=1;
        femSelection -> setDisabled(false);
        //femSelection->setCurrentIndex(1);
        //femSelection->setCurrentIndex(0);
        //femProgramChanged("OpenSees");
        this -> setContentsVisible(true);
        theFEMs.at(0)->setAsGP(true);
        this->parseAllInputfiles();
        theParameters->setCorrelationDisabled(true);
    } else if (option == "GPMFmodel")
    {
        numInputs=2;
        femSelection->addItem(tr("MultipleModels"));
        int index = femSelection->findText("MultipleModels");
        femSelection->setCurrentIndex(index);
        femSelection->setDisabled(true);
        this -> setContentsVisible(true);
        theFEMs.at(0)->setAsGP(true);
        theFEMs.at(1)->setAsGP(true);
        theParameters->setCorrelationDisabled(true);
    }

    return 0;
}

void
InputWidgetFEM::femProgramChanged(const QString& arg1) {

    // remove existing boxes
    this->clear();
    if (arg1 == "MultipleModels")
    {
        for (int i =0 ; i<numInputs; i++) {
            this->addFEM(i+1);
        }
        //verticalLayout->addStretch();
    } else {
        this->addFEM(0);
        theFEMs.at(0)->femProgramChanged(arg1);
    }
//        int numFem = theFEMs.size();
//        for (int i = numFem-1; i >= 0; i--) {
//            FEM *theFEM = theFEMs.at(i);
//            theFEM->femProgramChanged(arg1);
}

QString InputWidgetFEM::getApplicationName(void) {

    return femSelection->currentText();
}


QString InputWidgetFEM::getMainInput(void) {
    FEM *theFEM = theFEMs.at(0);
    return theFEM->getMainInput();
}

QVector< QString > InputWidgetFEM::getCustomInputs() const {
    FEM *theFEM = theFEMs.at(0);
    return theFEM->getCustomInputs();
}

void
InputWidgetFEM::specialCopyMainInput(QString fileName, QStringList varNames) {

    FEM *theFEM = theFEMs.at(0);
    return theFEM->specialCopyMainInput(fileName, varNames);
}

void InputWidgetFEM::addFEM(int i)
{

    FEM *theFEM = new FEM(theParameters, theEdpWidget, i);
    theFEMs.append(theFEM);
    femLayout->insertWidget(femLayout->count()-1, theFEM);

    if (theFEMs.size() == 1)
        theFEMs.at(0)->hideHeader(true);
    else
        theFEMs.at(0)->hideHeader(false);

    //theFEM->setAsGP(isGP);

}

//void InputWidgetFEM::addFEM(QString name)
//{
//    FEM *theFEM = new FEM(theParameters, theEdpWidget, name);
//    theFEMs.append(theFEM);
//    femLayout->insertWidget(femLayout->count()-1, theFEM);
//}


//int InputWidgetFEM::getnumInputs(void)
//{
//    return theFEMs.length();
//}

void InputWidgetFEM::clear(void)
{

    // loop over random variables, removing from layout & deleting
    for (int i = 0; i <theFEMs.size(); ++i) {
        FEM *theFEM = theFEMs.at(i);
        femLayout->removeWidget(theFEM);
        delete theFEM;
    }

    theFEMs.clear();
}

void InputWidgetFEM::setContentsVisible(bool tog)
{
    // loop over random variables, removing from layout & deleting
    for (int i = 0; i <theFEMs.size(); ++i) {
        FEM *theFEM = theFEMs.at(i);
        theFEM->setVisible(tog);
    }
}


//void InputWidgetFEM::removeFEM(void)
//{
//    // find the ones selected & remove them
//    int numInputss = theFEMs.size();
//    for (int i = numInputss-1; i >= 0; i--) {
//        FEM *theFEM = theFEMs.at(i);
//        if (theFEM->isSelectedForRemoval()) {
//            theFEM->close();
//            FEMLayout->removeWidget(theFEM);
//            theFEMs.remove(i);
//            //theFEM->setParent(0);
//            delete theFEM;
//        }
//    }
//}


bool
InputWidgetFEM::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject fem;
    fem["program"]=femSelection->currentText();
    fem["numInputs"]=numInputs;

    // create an JSON array and add each one that succesfully writes itself

    if (femSelection->currentText() == "MultipleModels") {
        // creat json array for multple models
        QJsonArray femArray;
        for (int i = 0; i <theFEMs.size(); ++i) {
            QJsonObject fem_comp;
            if (theFEMs.at(i)->outputToJSON(fem_comp) == true)
                femArray.append(fem_comp);
            else
                result = false;
        }
        fem["submodels"] = femArray;
        jsonObject["fem"]=fem;
    } else {
        // merge json
        QJsonObject fem_comp;
        if (theFEMs.at(0)->outputToJSON(fem_comp) == true) {
            for (auto it = fem_comp.constBegin(); it != fem_comp.constEnd(); it++) {
                fem.insert(it.key(), it.value());
            }
            jsonObject["fem"]=fem;
        }
        else
            result = false;
    }

    return result;
}


bool
InputWidgetFEM::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;

    if (jsonObject.contains("fem")) {
        QJsonObject femObject = jsonObject["fem"].toObject();

        // clear existing

        // go get the array, and for each component create one, get it to read & then add
        QString program=femObject["program"].toString();
        if (program=="MultipleModels") {
            this->setFEMforGP("GPMFmodel");
        }
        int index = femSelection->findText(program);
        femSelection->setCurrentIndex(index);
        //this->femProgramChanged(program);
        this->clear();

        if (femSelection->currentText() == "MultipleModels") {
            QJsonArray femArray = femObject["submodels"].toArray();
            for (int i=0; i<femArray.size(); i++) {
                QJsonObject femObject_comp = femArray[i].toObject();
                this->addFEM(i+1);
                if (theFEMs[i]->inputFromJSON(femObject_comp) == true)
                {
                    femLayout->insertWidget(femLayout->count()-2, theFEMs[i]);
                }
                else
                    result = false;

            }
        } else {
            this->addFEM(0);
            if (theFEMs[0]->inputFromJSON(femObject) == true)
                femLayout->insertWidget(femLayout->count()-2, theFEMs[0]);
            else
                result = false;
        }
    } else {
        emit sendErrorMessage("ERROR: FEM Input - no fem section in input file");
        return false;
    }
    return result;
}

void InputWidgetFEM::parseAllInputfiles(void){
    int numFem = theFEMs.size();
    for (int i = numFem-1; i >= 0; i--) {
        FEM *theFEM = theFEMs.at(i);
        theFEM->parseAllInputfiles();
    }
}

//int
//InputWidgetFEM::processResults(double *data) {
//    for (int i = 0; i <theFEMs.size(); ++i) {
//        theFEMs.at(i)->setResults(&data[i*4]);
//    }
//    return 0;
//}

