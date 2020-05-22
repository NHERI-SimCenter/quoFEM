#ifndef DAKOTA_INPUT_SENSITIVITY_H
#define DAKOTA_INPUT_SENSITIVITY_H

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

#include <UQ_Engine.h>

#include <QGroupBox>
#include <QVector>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>

class DakotaSensitivityResults;
class DakotaResults;
class QCheckBox;
class RandomVariablesContainer;
class QStackedWidget;
class UQ_MethodInputWidget;

class DakotaInputSensitivity : public UQ_Engine
{
    Q_OBJECT
public:
    explicit DakotaInputSensitivity(QWidget *parent = 0);
    ~DakotaInputSensitivity();

    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);

    int processResults(QString &filenameResults, QString &filenameTab);

    UQ_Results *getResults(void);
    RandomVariablesContainer  *getParameters();

    int getMaxNumParallelTasks(void);

    QVBoxLayout *mLayout;

signals:

public slots:
   void clear(void);
   void onMethodChanged(QString);

private:
    QVBoxLayout *layout;
    QWidget     *methodSpecific;
    QComboBox   *samplingMethod;
    QLineEdit   *numSamples;
    QLineEdit   *randomSeed;
    //    QPushButton *run;

    QComboBox   *uqSelection;
    QWidget     *uqSpecific;

    RandomVariablesContainer *theRandomVariables;
    DakotaSensitivityResults *results;

    QStackedWidget *theStackedWidget;
    UQ_MethodInputWidget *theCurrentMethod;
    UQ_MethodInputWidget *theMC;
    UQ_MethodInputWidget *theLHS;
    UQ_MethodInputWidget *theIS;
    UQ_MethodInputWidget *theGP;
    UQ_MethodInputWidget *thePCE;
};

#endif // DAKOTA_INPUT_SENSITIVITY_H
