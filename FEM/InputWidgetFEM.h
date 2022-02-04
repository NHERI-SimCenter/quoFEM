#ifndef INPUTWIDGETFEM_H
#define INPUTWIDGETFEM_H

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

#include <SimCenterAppWidget.h>

#include "FEM.h"
#include <QGroupBox>
#include <QVector>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>

#include "InputWidgetParameters.h"

class InputWidgetFEM : public SimCenterAppWidget
{
    Q_OBJECT

public:
    explicit InputWidgetFEM(InputWidgetParameters *theParams, InputWidgetEDP *edpwidget, QWidget *parent = 0);
    ~InputWidgetFEM();

    bool outputAppDataToJSON(QJsonObject &jsonObject);
    bool inputAppDataFromJSON(QJsonObject &jsonObject);
  
    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    bool copyFiles(QString &destName);

  
    int processResults(double *data);
    int getNumFEM(void);
    void setFemNames(QStringList femNames);
    int setFEMforGP(QString option);
    void specialCopyMainInput(QString fileName, QStringList varNames);
    QVector< QString > getCustomInputs() const;
    QString getApplicationName(void);
    QString getMainInput(void);
    void parseAllInputfiles(void);

signals:

public slots:
   void addFEM(int i=0);
   //void removeFEM(void);
   void clear(void);
   void femProgramChanged(const QString& arg1);
private:
    void makeFEM(void);
    int numInputs;

    QCheckBox *theCheckButton;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *femLayout;
    QFrame *fem;
    QLineEdit *theGroupEdit;
    QVector<FEM *>theFEMs;
    QComboBox *femSelection;

    InputWidgetEDP *theEdpWidget;
    InputWidgetParameters *theParameters;
    void setContentsVisible(bool tog);
    //

    QVBoxLayout *layout;
    QWidget     *femSpecific;
    QSpinBox * theCustomInputNumber;
    QVBoxLayout* theCustomLayout;
    QVBoxLayout* theCustomFileInputs;
    QWidget* theCustomFileInputWidget;

    //    QLineEdit *file1;
    // QLineEdit *file2;

    QStringList varNamesAndValues;

    QVector<QLineEdit *>inputFilenames;
    QVector<QLineEdit *>postprocessFilenames;
    QVector<QLineEdit *>parametersFilenames;
    QVector<QLineEdit *>customInputFiles;
    // for GP
    double interpolateForGP(QVector<double> X, QVector<double> Y, double Xval);
    double thres;
    QStringList parseGPInputs(QString file1);
    QVector<double> percVals, thrsVals;
    QLineEdit *thresVal;
    QString femOpt;
    bool isData;
    QRadioButton * option1Button,* option2Button,* option3Button;
    QGroupBox *groupBox;
    QGridLayout *optionsLayout ;
    QString GPoption;

    //flags
    bool isGP, useMultiModels;


};

#endif // INPUTWIDGETFEM_H
