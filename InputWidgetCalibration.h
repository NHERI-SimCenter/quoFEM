#ifndef INPUTWIDGET_CALIBRATION_H
#define INPUTWIDGET_CALIBRATION_H

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
// padhye modified/added

#include <InputWidgetDakotaMethod.h>

#include "EDP.h"
#include <QGroupBox>
#include <QVector>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QtGui>
#include <QDialog>


class DakotaCalibrationResults;
class DakotaResults;
class RandomVariableInputWidget;


class InputWidgetEDP;

class InputWidgetCalibration : public InputWidgetDakotaMethod
{
    Q_OBJECT
public:
    explicit InputWidgetCalibration(QWidget *parent = 0);
    ~InputWidgetCalibration();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);

    int processResults(QString &filenameResults, QString &filenameTab);
    DakotaResults *getResults(void);

    RandomVariableInputWidget *getParameters();

signals:

public slots:
   void clear(void);
   void methodChanged(const QString &arg1);
   void comboboxItemChanged(QString value);


private:
    QVBoxLayout *layout;
    QComboBox   *calibrationMethod;

//    QHBoxLayout *mLayout;//not using it anymore

    QVBoxLayout *TestingLayout; //padhye

    //additional widgets just for ant colony

    QVBoxLayout *ColonyPatternLayout;//padhye

    QLineEdit *initial_step_size_value;
    QLineEdit *final_step_size_value;
    QLineEdit *contraction_factor;
    QLineEdit *max_function_evals_ColonyPattern;
    QComboBox *exploratory_moves;

    QLabel *label2_initial_step_size;
    QLabel *label_final_step_size_value;
    QLabel *label_contraction_factor;
    QLabel *label_max_function_evals_ColonyPattern;
    QLabel *label_exploratory_moves;

    int colony_pattern_flag=0;

    // additional for colony_EA

    QGridLayout *ColonyEALayout;//padhye


    QLineEdit *max_function_evals_ColonyEA;
    QLineEdit *seed_ColonyEA;
    QLineEdit *pop_sizeColonyEA;
    QComboBox *fitness_typeColonyEA;
    QComboBox *mutation_typeColonyEA;
    QLineEdit *mutation_rateColonyEA;
    QComboBox *crossover_typeColonyEA;
    QLineEdit *crossover_rateColonyEA;
    QComboBox *replacement_typeColonyEA;
    QLineEdit *replacement_type_value_ColonyEA;

    QLabel *label_max_function_evals_ColonyEA;
    QLabel *label_seed_ColonyEA;
    QLabel *label_pop_sizeColonyEA;
    QLabel *label_fitness_typeColonyEA;
    QLabel *label_mutation_typeColonyEA;
    QLabel *label_mutation_rateColonyEA;
    QLabel *label_crossover_typeColonyEA;
    QLabel *label_crossover_rateColonyEA;
    QLabel *label_replacement_typeColonyEA;
    QLabel *label_replacement_type_value_ColonyEA;

    int colony_EA_flag=0;


    //common variables for all calibration methods
    QLineEdit   *maxIterations;
    QLineEdit   *convergenceTol;

    RandomVariableInputWidget *theParameters;
    InputWidgetEDP *theEdpWidget;
    DakotaCalibrationResults *results;
};


#endif // INPUTWIDGET_CALIBRATION_H
