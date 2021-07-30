#ifndef SURROGATE_NO_DOE_INPUT_WIDGET_H
#define SURROGATE_NO_DOE_INPUT_WIDGET_H

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

class SurrogateNoDoEInputWidget : public UQ_MethodInputWidget
{
    Q_OBJECT
public:
    explicit SurrogateNoDoEInputWidget(InputWidgetParameters *param,InputWidgetFEM *femwidget,InputWidgetEDP *edpwidget, QWidget *parent = 0);
    ~SurrogateNoDoEInputWidget();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    void clear(void);

    int getNumberTasks(void);
    int parseInputDataForRV(QString name1);
    int parseOutputDataForQoI(QString name1);
    int numSamples;

public slots:
    void setOutputDir(bool tog);
    void doAdvancedGP(bool tog);
    void showNuggetBox(int idx);
private:
    QLineEdit *randomSeed;
    QLineEdit *inpFileDir;
    QLineEdit *outFileDir;
    QCheckBox *theCheckButton;
    QPushButton *chooseOutFile;
    QLineEdit *initialDoE;
    QComboBox *gpKernel;
    QCheckBox *theLinearCheckBox;
    QCheckBox *theAdvancedCheckBox;
    QCheckBox *theLogtCheckBox;

    QLabel * theAdvancedTitle;
    QLabel * theKernelLabel;
    QLabel * theLinearLabel;
    QLabel * theLogtLabel;
    QLabel * theLogtLabel2;
    QLabel * theInitialLabel;
    QLabel * errMSG;

    QComboBox * theNuggetSelection;
    QCheckBox * theNugCheckBox;
    QLineEdit * theNuggetVals;
    QLabel * theNuggetLabel;
    QLabel * theNuggetMsg;
    InputWidgetParameters *theParameters;
    InputWidgetEDP *theEdpWidget;
    InputWidgetFEM *theFemWidget;

    QFrame * lineA;

    int countColumn(QString name1);
};

#endif // SURROGATE_NO_DOE_INPUT_WIDGET_H
