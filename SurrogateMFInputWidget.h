#ifndef SURROGATE_MF_INPUT_WIDGET_H
#define SURROGATE_MF_INPUT_WIDGET_H

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

#include <UQ_MethodInputWidget.h>
class QLineEdit;
class QCheckBox;
class QPushButton;
class QComboBox;
class QLabel;
class QFrame;
class InputWidgetParameters;
class InputWidgetEDP;
class InputWidgetFEM;
class QGroupBox;

class SurrogateMFInputWidget : public UQ_MethodInputWidget
{
    Q_OBJECT
public:
    explicit SurrogateMFInputWidget(InputWidgetParameters *param,InputWidgetFEM *femwidget,InputWidgetEDP *edpwidget, QWidget *parent = 0);
    ~SurrogateMFInputWidget();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    void clear(void);

    int getNumberTasks(void);
    int parseInputDataForRV(QString name1);
    int parseOutputDataForQoI(QString name1);

public slots:
    void setLowSim(bool tog);
    void setHighSim(bool tog);

    void doAdvancedGP(bool tog);
    void doExistingLF(bool tog);
    void doExistingHF(bool tog);

private:
    QLineEdit *numSamples_HF, *initialDoE_HF, *randomSeed,  *accuracyMeasure, *timeMeasure;
    QLineEdit *numSamples_LF, *initialDoE_LF;
    QLineEdit *inpFileDir_HF, *inpFileDir_LF, *outFileDir_HF, *outFileDir_LF;
    QComboBox *gpKernel;
    QCheckBox *theAdvancedCheckBox,*theExistingCheckBox_HF,*theExistingCheckBox_LF, *theDoECheckBox;
    QCheckBox *theLinearCheckBox,*theLogtCheckBox;
    QCheckBox *parallelCheckBox;

    QLabel * theAdvancedTitle,* theKernelLabel,* theLinearLabel,* theLogtLabel,* theLogtLabel2,* theInitialLabel;
    QLabel * modelMSG, * errMSG, *theExistingLabel;

    InputWidgetParameters *theParameters;
    InputWidgetEDP *theEdpWidget;
    InputWidgetFEM *theFemWidget;
    QPushButton *chooseInpFile_LF, *chooseInpFile_HF, *chooseOutFile_LF, *chooseOutFile_HF;
    QFrame * line0,* line1, * lineA;
    int countColumn(QString name1);

    QLabel *theLowDataLabel,*theLowSimLabel,* theHighSimLabel,* theHighDataLabel;
    QCheckBox* theLowDataButton, *theLowSimButton, * theHighDataButton, *theHighSimButton;
    QWidget *theLowSimOptions, *theHighSimOptions;
    QGroupBox * theSimBox;


};

#endif // SURROGATE_MF_INPUT_WIDGET_H
