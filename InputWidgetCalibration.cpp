// Written: fmckenna

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

#include "InputWidgetCalibration.h"
#include <DakotaResultsCalibration.h>

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
#include <InputWidgetEDP.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <RandomVariableInputWidget.h>


//QComboBox::down-arrow{
//    image: url(C://Users//nikhil//NHERI/uqFEM//images//pulldownarrow.PNG);heigth:50px;width:100px;);
//}



InputWidgetCalibration::InputWidgetCalibration(QWidget *parent)
    : InputWidgetDakotaMethod(parent)
{
    layout = new QVBoxLayout();
    TestingLayout = new QVBoxLayout();

    QVBoxLayout *methodLayout= new QVBoxLayout;

    QLabel *label1 = new QLabel();
    label1->setText(QString("Method"));
    label1->setMaximumWidth(100);
    label1->setMinimumWidth(100);
    calibrationMethod = new QComboBox();
    calibrationMethod->addItem(tr("OPT++GaussNewton"));
    calibrationMethod->addItem(tr("NL2SOL"));
    calibrationMethod->setMaximumWidth(250);
    calibrationMethod->setMinimumWidth(100);
    calibrationMethod->setStyleSheet("QComboBox::down-arrow {image: url(C://Users//nikhil//NHERI/uqFEM//images//pulldownarrow.PNG);heigth:50px;width:100px;}");
    //calibrationMethod->setStyleSheet("QComboBox::down-arrow {image: url(:/images//pulldownarrow.PNG);heigth:50px;width:100px;}");

    //calibrationMethod->setStyleSheet("QComboBox::down-arrow {image: url(:/imagesCommon/pulldownarrow.png);heigth:105px;width:100px;}");



    calibrationMethod->addItem(tr("ColinyPattern"));
    calibrationMethod->addItem(tr("ConjugateGradient"));
    calibrationMethod->addItem(tr("Coliny_EA"));
    
    methodLayout->addWidget(label1);
    methodLayout->addWidget(calibrationMethod);
    

    // padhye, connecting this combobox method
    //connect(calibrationMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(comboboxItemChanged(QString)));

    connect(calibrationMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(comboboxItemChanged(QString)));


    QVBoxLayout *maxIterLayout= new QVBoxLayout;
    QLabel *label2 = new QLabel();
    label2->setText(QString("max # Iterations"));
    maxIterations = new QLineEdit();
    maxIterations->setText(tr("1000"));
    maxIterations->setMaximumWidth(100);
    maxIterations->setMinimumWidth(100);
    
    maxIterLayout->addWidget(label2);
    maxIterLayout->addWidget(maxIterations);
    
    QVBoxLayout *tolLayout= new QVBoxLayout;
    QLabel *label3 = new QLabel();
    label3->setText(QString("Convergence Tol"));
    convergenceTol = new QLineEdit();
    convergenceTol->setText(QString::number(1.0e-2));
    convergenceTol->setMaximumWidth(100);
    convergenceTol->setMinimumWidth(100);
    
    tolLayout->addWidget(label3);
    tolLayout->addWidget(convergenceTol);
    
    QHBoxLayout *mLayout= new QHBoxLayout;


    TestingLayout->addLayout(methodLayout);
    TestingLayout->addLayout(maxIterLayout);
    TestingLayout->addLayout(tolLayout);
    TestingLayout->addStretch();

    //layout->addLayout(mLayout);
    layout->addLayout(TestingLayout);

    theEdpWidget = new InputWidgetEDP();
    layout->addWidget(theEdpWidget,1);

    this->setLayout(layout);
}

int 
InputWidgetCalibration::getMaxNumParallelTasks(void){
  return 1;
}

bool
InputWidgetCalibration::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject uq;
    uq["method"]=calibrationMethod->currentText();
    uq["maxIterations"]=maxIterations->text().toInt();
    uq["convergenceTol"]=convergenceTol->text().toDouble();

    if(calibrationMethod->currentText()=="ColinyPattern")
    {
        uq["initialDelta"] =initial_step_size_value->text().toDouble();
        uq["thresholdDelta"] = final_step_size_value->text().toDouble() ;
        uq["contractionFactor"] = contraction_factor->text().toDouble();
        uq["maxFunEvals"] = max_function_evals_ColonyPattern->text().toInt() ;
        uq["patternMove"] = exploratory_moves->currentText();
    }
    if(calibrationMethod->currentText()=="Coliny_EA")
    {
        uq["maxFunEvals"]=max_function_evals_ColonyEA->text().toInt();
        uq["seed"]=seed_ColonyEA->text().toInt();
        uq["popSize"]=pop_sizeColonyEA->text().toInt();
        uq["fitnessType"]=fitness_typeColonyEA->currentText();
        uq["mutationType"]=mutation_typeColonyEA->currentText();
        uq["mutationRate"]=mutation_rateColonyEA->text().toDouble();
        uq["crossoverType"]=crossover_typeColonyEA->currentText();
        uq["crossoverRate"]=crossover_rateColonyEA->text().toDouble();
        uq["replacementType"]=replacement_typeColonyEA->currentText();
        uq["replacementValue"]=replacement_type_value_ColonyEA->text().toInt();
    }

    theEdpWidget->outputToJSON(uq);
    jsonObject["calibrationMethodData"]=uq;

    return result;
}

void
InputWidgetCalibration::comboboxItemChanged(QString value)
{

qDebug()<<"\n\n I am inside the combochage and the string value is      "<<value;


if (value=="ColinyPattern" && colony_pattern_flag!=1)
{

    //padhye just for ant colony
    ColonyPatternLayout = new QVBoxLayout;
    ColonyPatternLayout->setAlignment(Qt::AlignBaseline);

    label2_initial_step_size = new QLabel();
    initial_step_size_value = new QLineEdit();


    final_step_size_value = new QLineEdit();
    contraction_factor = new QLineEdit();
    max_function_evals_ColonyPattern = new QLineEdit();

    exploratory_moves = new QComboBox;

    exploratory_moves->setStyleSheet("QComboBox::down-arrow {image: url(C://Users//nikhil//NHERI/uqFEM//images//pulldownarrow.PNG);heigth:50px;width:100px;}");
    label2_initial_step_size = new QLabel();
    label_final_step_size_value = new QLabel();
    label_contraction_factor = new QLabel();
    label_max_function_evals_ColonyPattern = new QLabel();
    label_exploratory_moves = new QLabel();




    TestingLayout->addLayout(ColonyPatternLayout);// add it for the first time

    label2_initial_step_size->setText(QString("Initial Delta"));
    initial_step_size_value->setText(tr("0.01"));
    initial_step_size_value->setMaximumWidth(100);
    initial_step_size_value->setMinimumWidth(100);

    ColonyPatternLayout->addWidget(label2_initial_step_size);
    ColonyPatternLayout->addWidget(initial_step_size_value);

    label_final_step_size_value->setText(QString("Threshold Delta"));
    final_step_size_value->setText(tr("0.00001"));
    final_step_size_value->setMaximumWidth(100);
    final_step_size_value->setMinimumWidth(100);

    ColonyPatternLayout->addWidget(label_final_step_size_value);
    ColonyPatternLayout->addWidget(final_step_size_value);

    label_contraction_factor->setText(QString("Contraction Factor"));
    contraction_factor->setText(QString("0.006"));
    contraction_factor->setMaximumWidth(100);
    contraction_factor->setMinimumWidth(100);
    ColonyPatternLayout->addWidget(label_contraction_factor);
    ColonyPatternLayout->addWidget(contraction_factor);

    label_max_function_evals_ColonyPattern->setText(QString("Max. Fun. Evals."));
    max_function_evals_ColonyPattern->setText(tr("100"));
    max_function_evals_ColonyPattern->setMaximumWidth(100);
    max_function_evals_ColonyPattern->setMinimumWidth(100);
    ColonyPatternLayout->addWidget(label_max_function_evals_ColonyPattern);
    ColonyPatternLayout->addWidget(max_function_evals_ColonyPattern);

    label_exploratory_moves->setText(QString("Exploratory Move"));
    exploratory_moves->addItem(tr("multi_step"));
    exploratory_moves->addItem(tr("adaptive_pattern"));
    exploratory_moves->addItem(tr("basic_pattern"));

    ColonyPatternLayout->addWidget(label_exploratory_moves);
    ColonyPatternLayout->addWidget(exploratory_moves);

    colony_pattern_flag=1;
}else
    {

         if(colony_pattern_flag==1)
            {

                    ColonyPatternLayout->removeWidget(label2_initial_step_size);
                    ColonyPatternLayout->removeWidget(initial_step_size_value);

                    ColonyPatternLayout->removeWidget(final_step_size_value);
                    ColonyPatternLayout->removeWidget(contraction_factor);
                    ColonyPatternLayout->removeWidget(max_function_evals_ColonyPattern);
                    ColonyPatternLayout->removeWidget(exploratory_moves);
                    ColonyPatternLayout->removeWidget(label_final_step_size_value);
                    ColonyPatternLayout->removeWidget(label_contraction_factor);
                    ColonyPatternLayout->removeWidget(label_max_function_evals_ColonyPattern);
                    ColonyPatternLayout->removeWidget(label_exploratory_moves);


                    TestingLayout->removeItem(ColonyPatternLayout);
                    delete ColonyPatternLayout;
                    delete initial_step_size_value;
                    delete label2_initial_step_size;
                    delete final_step_size_value;
                    delete contraction_factor;
                    delete max_function_evals_ColonyPattern;
                    delete exploratory_moves;
                    delete label_final_step_size_value;
                    delete label_contraction_factor;
                    delete label_max_function_evals_ColonyPattern;
                    delete label_exploratory_moves;


                    colony_pattern_flag=0;
            }

    }


if (value=="Coliny_EA" && colony_EA_flag!=1)
{


     ColonyEALayout = new QGridLayout();//padhye
     ColonyEALayout->setAlignment(Qt::AlignBaseline);

     TestingLayout->addLayout(ColonyEALayout);// add it for the first time

     max_function_evals_ColonyEA = new QLineEdit();
     max_function_evals_ColonyEA->setText(tr("200"));

     seed_ColonyEA = new QLineEdit();
     seed_ColonyEA->setText(tr("123"));

     pop_sizeColonyEA = new QLineEdit();
     pop_sizeColonyEA->setText(tr("100"));

     fitness_typeColonyEA = new QComboBox;
     fitness_typeColonyEA->setStyleSheet("QComboBox::down-arrow {image: url(C://Users//nikhil//NHERI/uqFEM//images//pulldownarrow.PNG);heigth:50px;width:100px;}");
     fitness_typeColonyEA->addItem(tr("linear_rank"));
     fitness_typeColonyEA->addItem(tr("merit_function"));

     mutation_typeColonyEA = new QComboBox;
     mutation_typeColonyEA->setStyleSheet("QComboBox::down-arrow {image: url(C://Users//nikhil//NHERI/uqFEM//images//pulldownarrow.PNG);heigth:50px;width:100px;}");
     mutation_typeColonyEA->addItem(tr("replace_uniform"));
     mutation_typeColonyEA->addItem(tr("offset_normal"));
     mutation_typeColonyEA->addItem(tr("offset_cauchy"));
     mutation_typeColonyEA->addItem(tr("offset_uniform"));


     mutation_rateColonyEA = new QLineEdit();
     mutation_rateColonyEA->setText(tr("0.9"));

     crossover_typeColonyEA = new QComboBox;
     crossover_typeColonyEA->setStyleSheet("QComboBox::down-arrow {image: url(C://Users//nikhil//NHERI/uqFEM//images//pulldownarrow.PNG);heigth:50px;width:100px;}");
     crossover_typeColonyEA->addItem(tr("two_point"));
     crossover_typeColonyEA->addItem(tr("blend"));
     crossover_typeColonyEA->addItem(tr("uniform"));

     crossover_rateColonyEA = new QLineEdit();
     crossover_rateColonyEA->setText(tr("0.9"));

     replacement_typeColonyEA = new QComboBox;
     replacement_typeColonyEA->setStyleSheet("QComboBox::down-arrow {image: url(C://Users//nikhil//NHERI/uqFEM//images//pulldownarrow.PNG);heigth:50px;width:100px;}");
     replacement_typeColonyEA->addItem(tr("elitist"));
     replacement_typeColonyEA->addItem(tr("roulette_wheel"));
     replacement_typeColonyEA->addItem(tr("unique_roulette_wheel"));
     replacement_typeColonyEA->addItem(tr("below_limit"));

     replacement_type_value_ColonyEA = new QLineEdit();
     replacement_type_value_ColonyEA->setText(tr("20"));

    label_max_function_evals_ColonyEA = new QLabel();
    label_max_function_evals_ColonyEA->setText(QString("Fun. Evals."));


    label_seed_ColonyEA = new QLabel();
    label_seed_ColonyEA->setText("Seed");


    label_pop_sizeColonyEA = new QLabel();
    label_pop_sizeColonyEA->setText("Pop Size");

    label_fitness_typeColonyEA = new QLabel();
    label_fitness_typeColonyEA->setText("Fitness Type");

    label_mutation_typeColonyEA = new QLabel();
    label_mutation_typeColonyEA->setText("Mutation Type");

    label_mutation_rateColonyEA = new QLabel();
    label_mutation_rateColonyEA->setText("Mutation Rate");

    label_crossover_typeColonyEA = new QLabel();
    label_crossover_typeColonyEA->setText("Crossover Type");


    label_crossover_rateColonyEA = new QLabel();
    label_crossover_rateColonyEA->setText("Crossover Rate");

    label_replacement_typeColonyEA = new QLabel();
    label_replacement_typeColonyEA->setText("Replacement Type");

    label_replacement_type_value_ColonyEA = new QLabel();
    label_replacement_type_value_ColonyEA->setText("Replacement Value");



    ColonyEALayout->addWidget(max_function_evals_ColonyEA,2,0);
    ColonyEALayout->addWidget(label_max_function_evals_ColonyEA,1,0);

    max_function_evals_ColonyEA->setMaximumWidth(100);

    ColonyEALayout->addWidget(label_seed_ColonyEA,1,1);
    ColonyEALayout->addWidget(seed_ColonyEA,2,1);

    seed_ColonyEA->setMaximumWidth(150);

    ColonyEALayout->addWidget(label_pop_sizeColonyEA,1,2);
    ColonyEALayout->addWidget(pop_sizeColonyEA,2,2);
    pop_sizeColonyEA->setMaximumWidth(150);

    ColonyEALayout->addWidget(label_fitness_typeColonyEA,1,3);
    ColonyEALayout->addWidget(fitness_typeColonyEA,2,3);
    fitness_typeColonyEA->setMaximumWidth(150);

    ColonyEALayout->addWidget(label_mutation_typeColonyEA,1,4);
    ColonyEALayout->addWidget(mutation_typeColonyEA,2,4);
    mutation_typeColonyEA->setMaximumWidth(150);


    ColonyEALayout->addWidget(label_mutation_rateColonyEA,1,5);
    ColonyEALayout->addWidget(mutation_rateColonyEA,2,5);
    mutation_rateColonyEA->setMaximumWidth(150);


    ColonyEALayout->addWidget(label_crossover_typeColonyEA,3,0);
    ColonyEALayout->addWidget(crossover_typeColonyEA,4,0);
    crossover_typeColonyEA->setMaximumWidth(150);



    ColonyEALayout->addWidget(label_crossover_rateColonyEA,3,1);
    ColonyEALayout->addWidget(crossover_rateColonyEA,4,1);
    crossover_rateColonyEA->setMaximumWidth(150);


    ColonyEALayout->addWidget(label_replacement_typeColonyEA,3,2);
    ColonyEALayout->addWidget(replacement_typeColonyEA,4,2);
    replacement_typeColonyEA->setMaximumWidth(150);


    ColonyEALayout->addWidget(label_replacement_type_value_ColonyEA,3,3);
    ColonyEALayout->addWidget(replacement_type_value_ColonyEA,4,3);
    replacement_type_value_ColonyEA->setMaximumWidth(150);


    colony_EA_flag =1;

}else
{

    if(colony_EA_flag==1)
    {
        ColonyEALayout->removeWidget(max_function_evals_ColonyEA);
        ColonyEALayout->removeWidget(label_max_function_evals_ColonyEA);
        ColonyEALayout->removeWidget(label_seed_ColonyEA);
        ColonyEALayout->removeWidget(seed_ColonyEA);
        ColonyEALayout->removeWidget(label_pop_sizeColonyEA);
        ColonyEALayout->removeWidget(pop_sizeColonyEA);
        ColonyEALayout->removeWidget(label_fitness_typeColonyEA);
        ColonyEALayout->removeWidget(fitness_typeColonyEA);
        ColonyEALayout->removeWidget(label_mutation_typeColonyEA);
        ColonyEALayout->removeWidget(mutation_typeColonyEA);
        ColonyEALayout->removeWidget(label_mutation_rateColonyEA);
        ColonyEALayout->removeWidget(mutation_rateColonyEA);
        ColonyEALayout->removeWidget(label_crossover_typeColonyEA);
        ColonyEALayout->removeWidget(crossover_typeColonyEA);
        ColonyEALayout->removeWidget(label_crossover_rateColonyEA);
        ColonyEALayout->removeWidget(crossover_rateColonyEA);
        ColonyEALayout->removeWidget(label_replacement_typeColonyEA);
        ColonyEALayout->removeWidget(replacement_typeColonyEA);
        ColonyEALayout->removeWidget(label_replacement_type_value_ColonyEA);
        ColonyEALayout->removeWidget(replacement_type_value_ColonyEA);
        TestingLayout->removeItem(ColonyEALayout);


        delete max_function_evals_ColonyEA;
        delete label_max_function_evals_ColonyEA;
        delete label_seed_ColonyEA;
        delete seed_ColonyEA;
        delete label_pop_sizeColonyEA;
        delete pop_sizeColonyEA;
        delete label_fitness_typeColonyEA;
        delete fitness_typeColonyEA;
        delete label_mutation_typeColonyEA;
        delete mutation_typeColonyEA;
        delete label_mutation_rateColonyEA;
        delete mutation_rateColonyEA;
        delete label_crossover_typeColonyEA;
        delete crossover_typeColonyEA;
        delete label_crossover_rateColonyEA;
        delete crossover_rateColonyEA;
        delete label_replacement_typeColonyEA;
        delete replacement_typeColonyEA;
        delete label_replacement_type_value_ColonyEA;
        delete replacement_type_value_ColonyEA;
        delete ColonyEALayout;

        colony_EA_flag=0;
    }

}

return;

}



InputWidgetCalibration::~InputWidgetCalibration()
{

}


void InputWidgetCalibration::clear(void)
{

}

bool
InputWidgetCalibration::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;
    this->clear();

    QJsonObject uq = jsonObject["calibrationMethodData"].toObject();
    QString method =uq["method"].toString();
    int maxIter=uq["maxIterations"].toInt();
    double convTol=uq["convergenceTol"].toDouble();

    if(method=="ColinyPattern")
    {
       double initialDelta = uq["initialDelta"].toDouble();
       double thresholdDelta = uq["thresholdDelta"].toDouble();
       double contractionFactor = uq["contractionFactor"].toDouble();
       int maxFunEvals = uq["maxFunEvals"].toInt();
       QString patternMove = uq["patternMove"].toString();

       initial_step_size_value->setText(QString::number(initialDelta));
       final_step_size_value->setText(QString::number(thresholdDelta));
       contraction_factor->setText(QString::number(contractionFactor));
       max_function_evals_ColonyPattern->setText(QString::number(maxFunEvals));
       int index_for_pattern_move = exploratory_moves->findText(patternMove);
       exploratory_moves->setCurrentIndex(index_for_pattern_move);
    }

    if(method=="Coliny_EA")
    {

       int seedEa = uq["seed"].toInt();
       int maxFunEvalsEa= uq["maxFunEvals"].toInt();
       int popSizeEa = uq["PopSize"].toInt();
       QString fitness_typeEA = uq["FitnessType"].toString();
       QString mutation_typeEA =  uq["MutationType"].toString();
       double mutation_rate =  uq["MutationRate"].toDouble();
       QString crossover_typeEA =  uq["CrossoverType"].toString();
       double crossover_rate =  uq["CrossoverRate"].toDouble();
       QString replacement_typeEA =  uq["ReplacementType"].toString();
       int replacement_rate =  uq["ReplacementValue"].toInt();

       max_function_evals_ColonyEA->setText(QString::number(maxFunEvalsEa));
       seed_ColonyEA->setText(QString::number(seedEa));
       pop_sizeColonyEA->setText(QString::number(popSizeEa));
       int index = fitness_typeColonyEA->findText(fitness_typeEA);
       fitness_typeColonyEA->setCurrentIndex(index);
       index = mutation_typeColonyEA->findText(mutation_typeEA);
       mutation_typeColonyEA->setCurrentIndex(index);
       mutation_rateColonyEA->setText(QString::number(mutation_rate));
       index =crossover_typeColonyEA->findText(crossover_typeEA);
       crossover_typeColonyEA->setCurrentIndex(index);
       crossover_rateColonyEA->setText(QString::number(crossover_rate));
       index = replacement_typeColonyEA->findText(replacement_typeEA);
       replacement_typeColonyEA->setCurrentIndex(index);
       replacement_type_value_ColonyEA->setText(QString::number(replacement_rate));


    }



    maxIterations->setText(QString::number(maxIter));
    convergenceTol->setText(QString::number(convTol));
    int index = calibrationMethod->findText(method);
    calibrationMethod->setCurrentIndex(index);

    result = theEdpWidget->inputFromJSON(uq);
    return result;

}

void 
InputWidgetCalibration::methodChanged(const QString &arg1)
{

}

int 
InputWidgetCalibration::processResults(QString &filenameResults, QString &filenameTab) 
{
    return 0;
}

DakotaResults *
InputWidgetCalibration::getResults(void) {
  return new DakotaResultsCalibration();
}

RandomVariableInputWidget *
InputWidgetCalibration::getParameters(void) {
  QString classType("Design");
  return new RandomVariableInputWidget(classType);
}
