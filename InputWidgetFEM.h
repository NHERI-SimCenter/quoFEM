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

#include <SimCenterWidget.h>

#include "EDP.h"
#include <QVector>
#include <QSpinBox>
class QGridLayout;
class QVBoxLayout;
class QGroupBox;
class QComboBox;
class QGroupBox;
class QSpinBox;

class InputWidgetParameters;
class InputWidgetEDP;

class InputWidgetFEM : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit InputWidgetFEM(InputWidgetParameters *theParams, InputWidgetEDP *edpwidget, QWidget *parent = 0);
    ~InputWidgetFEM();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);

    QString getApplicationName();
    QString getMainInput();
    QVector< QString > getCustomInputs() const;  

     // copy main file to new filename ONLY if varNamesAndValues not empy
    void specialCopyMainInput(QString fileName, QStringList varNamesAndValues);
    //int parseInputfilesForRV(QString filnema1);
    int parseInputfilesForRV(QString name1);
    int parseInputfilesForGP(QString filnema1);
    void setFemGP(bool on);
    int setFEMforGP(QString option);

  signals:

  public slots:
    void clear(void);
    void femProgramChanged(const QString& arg1);
    void numModelsChanged(int newNum);
    void customInputNumberChanged(int numCustomInputs);
    // void chooseFileName1(void);
    // void chooseFileName2(void);

  private:

    QVBoxLayout *layout;
    QWidget     *femSpecific;
    QComboBox   *femSelection;
    QSpinBox * theCustomInputNumber;
    QVBoxLayout* theCustomLayout;
    QVBoxLayout* theCustomFileInputs;
    QWidget* theCustomFileInputWidget;

    //    QLineEdit *file1;
    // QLineEdit *file2;

    InputWidgetParameters *theParameters;
    InputWidgetEDP *theEdpWidget;
    QStringList varNamesAndValues;

    int numInputs;
    QVector<QLineEdit *>inputFilenames;
    QVector<QLineEdit *>postprocessFilenames;
    QVector<QLineEdit *>parametersFilenames;
    QVector<QLineEdit *>customInputFiles;
    QGridLayout *femLayout;
    // for GP
    double interpolateForGP(QVector<double> X, QVector<double> Y, double Xval);
    double thres;
    QStringList parseGPInputs(QString file1);
    bool isGP;
    QVector<double> percVals, thrsVals;
    QLineEdit *thresVal;
    QString femOpt;
    bool isData;
    QRadioButton * option1Button,* option2Button,* option3Button;

};

#endif // INPUTWIDGETFEM_H
