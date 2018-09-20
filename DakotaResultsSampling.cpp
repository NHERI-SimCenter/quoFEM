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
// added and modified: padhye

#include "DakotaResultsSampling.h"
#include "InputWidgetFEM.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>

#include <QFileDialog>
#include <QTabWidget>
#include <QTextEdit>
#include <MyTableWidget.h>
#include <QDebug>
#include <QHBoxLayout>
#include <QColor>
#include <QMenuBar>
#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QProcess>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include <InputWidgetFEM.h>
#include <InputWidgetUQ.h>
#include <MainWindow.h>


#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
using namespace QtCharts;
#include <math.h>
#include <QValueAxis>

#include <QXYSeries>

#define NUM_DIVISIONS 10



QLabel *best_fit_label_text;


DakotaResultsSampling::DakotaResultsSampling(QWidget *parent)
    : DakotaResults(parent)
{
    // title & add button
    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget,1);
    mLeft = true;
    col1 = 0;
    col2 = 0;
}

DakotaResultsSampling::~DakotaResultsSampling()
{

}


void DakotaResultsSampling::clear(void)
{
    QWidget *res=tabWidget->widget(0);
    QWidget *gen=tabWidget->widget(0);
    QWidget *dat=tabWidget->widget(0);

    tabWidget->clear();
    delete dat;
    delete gen;
    delete res;
}



static void merge_helper(double *input, int left, int right, double *scratch)
{
    // if one element: done  else: recursive call and then merge
    if(right == left + 1) {
        return;
    } else {
        int length = right - left;
        int midpoint_distance = length/2;
        /* l and r are to the positions in the left and right subarrays */
        int l = left, r = left + midpoint_distance;

        // sort each subarray
        merge_helper(input, left, left + midpoint_distance, scratch);
        merge_helper(input, left + midpoint_distance, right, scratch);

        // merge the arrays together using scratch for temporary storage
        for(int i = 0; i < length; i++) {
            /* Check to see if any elements remain in the left array; if so,
            * we check if there are any elements left in the right array; if
            * so, we compare them.  Otherwise, we know that the merge must
            * use take the element from the left array */
            if(l < left + midpoint_distance &&
                    (r == right || fmin(input[l], input[r]) == input[l])) {
                scratch[i] = input[l];
                l++;
            } else {
                scratch[i] = input[r];
                r++;
            }
        }
        // Copy the sorted subarray back to the input
        for(int i = left; i < right; i++) {
            input[i] = scratch[i - left];
        }
    }
}

static int mergesort(double *input, int size)
{
    double *scratch = new double[size];
    if(scratch != NULL) {
        merge_helper(input, 0, size, scratch);
        delete [] scratch;
        return 1;
    } else {
        return 0;
    }
}

// if sobelov indices are selected then we would need to do some processing outselves

int DakotaResultsSampling::processResults(QString &filenameResults, QString &filenameTab)
{

    //
    // get a Qwidget ready to place summary data, the EDP name, mean, stdDev into
    //
    QWidget *summary = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summary->setLayout(summaryLayout);

    // //
    // in  to a QTextEdit we will place contents of Dakota more detailed output
    // //

    dakotaText = new QTextEdit();
    dakotaText->setReadOnly(true); // make it so user cannot edit the contents
    dakotaText->setText("\n");
    //
    // open Dakota output file
    //
    //qDebug()<<"\n The filenameResults is        "<<filenameResults<<"\n\n\n";
    //qDebug()<<"\n The filenameTab is        "<<filenameTab<<"\n\n\n";
    //padhye 8/25/2018

    //note when Dakota is being run then the name of filenameResults is dakota.out
    // and filenameTab is dakotaTab.out and has the tabular data.

    // Later if you add any other engine other than Dakota, then you will need to
    // write a postprocessor that can take and/or process files generated by that engine
    // More code would have to be written.

    std::ifstream fileResults(filenameResults.toStdString().c_str());
    if (!fileResults.is_open()) {
        qDebug() << "Could not open file: " << filenameResults;
        return -1;
    }

    // now ignore every line until Kurtosis found

    const std::string needle = "Kurtosis";
    std::string haystack;

    int found_flag=0;

        while (std::getline(fileResults, haystack))
        {
            qDebug()<<"\n   the value of haystack.find(needle)  "<<haystack.find(needle);
            //if (haystack.find(needle)==true){found_flag=1;}
            if (haystack.find(needle) != std::string::npos)
            {
                 found_flag=1;
                 break;
            }
        }
      //  qDebug()<<"\n the value of found_flag is    "<<found_flag;

     // if found then do the standard processing.
    if(found_flag==1)
    {
    //found_flag=0 means kurtosis not found, and you have to computer them.

    // the idea is that once you have found the  keyword Kurtosis
    // then from that line until end of the dakota.out you copy
    // everything and flash it into the uqFEM.

    //
    // now copy line and every subsequent line into text editor
    //  - also if still collecting sumary data, add each EDP's sumary info
    //
    dakotaText->append(haystack.c_str());
    bool isSummaryDone = false;


    // This is where you go until the end of the file to get all the data

      while (std::getline(fileResults, haystack)) {
        dakotaText->append(haystack.c_str());
        if (isSummaryDone == false) {

            if ( strlen(haystack.c_str()) == 0) {
                isSummaryDone = true;
            } else {
                //
                // add sumary info
                //

    // printing the haystack and seeing its effect
    //   qDebug()<<"\n\n    the value of the haystack is  \n    \n  "<<haystack.c_str();
            // The above string is simply the line after the the Kurtosis.

        // qDebug()<<"\n\n    I just dumped the value of the haystack above  \n    \n  ";
            std::istringstream iss(haystack);
            std::string subs;
            iss >> subs;
            QString  nameString(QString::fromStdString(subs));  // name of the output
            iss >> subs;
            QString meanText(QString::fromStdString(subs));
            double mean = meanText.toDouble();  // this is the mean value
            iss >> subs;
            QString stdDevText(QString::fromStdString(subs));
            double stdDev = stdDevText.toDouble();  // this is std. deviation
            QWidget *theWidget = this->createResultEDPWidget(nameString, mean, stdDev);
            summaryLayout->addWidget(theWidget);
            // end of summary widget
            }
        }
    }

    }else if(found_flag==0)
            {
                fileResults.close();
                std::ifstream fileResults(filenameResults.toStdString().c_str());

                // reset the file counter
              //  fileResults.seekg(0, fileResults.beg);
                //now look for "Global sensitivity"
                const std::string needle2 = "sensitivity";
                while (std::getline(fileResults, haystack))
                {
                   // qDebug()<<"\n the value is  \n"<<haystack.c_str();
                    //qDebug()<<"\n the haystack is   "<<haystack;
                    //qDebug()<<"\n";
       //             qDebug()<<"\n   the value of haystack.find(needle2)  "<<haystack.find(needle2);
                    //if (haystack.find(needle)==true){found_flag=1;}
                    if (haystack.find(needle2) != std::string::npos)
                    {
                         break;
                    }
                }

                //dakotaText contains the information for the General Widget
                dakotaText->append(haystack.c_str());
                bool isSummaryDone = false;

                // This is where you go until the end of the file to get all the data

                  while (std::getline(fileResults, haystack))
                  {
                    dakotaText->append(haystack.c_str());
                            //if (isSummaryDone == false)
                            {

                        //if (strlen(haystack.c_str()) == 0)
                            {
                          //          isSummaryDone = true;
                            } //else
                        {

                                 // std::istringstream iss(haystack);
                                 // std::string subs;
                                 // iss >> subs;
                                  //QString  nameString(QString::fromStdString(subs));  // name of the output
                                 // iss >> subs;
                                  //QString meanText(QString::fromStdString(subs));
                                  //double mean = meanText.toDouble();  // this is the mean value
                                  //iss >> subs;
                                  //QString stdDevText(QString::fromStdString(subs));
                                  //double stdDev = stdDevText.toDouble();  // this is std. deviation
                                   //QWidget *theWidget = this->createResultEDPWidget(nameString, mean, stdDev);
                                   //summaryLayout->addWidget(theWidget);
                        // end of summary widget


                                }
                            }
                        }

            }

    // close input file
    fileResults.close();

    //
    // now into a QTableWidget copy the random variable and edp's of each black box run
    //

    spreadsheet = new MyTableWidget();

    // open file containing tab data
    std::ifstream tabResults(filenameTab.toStdString().c_str());
    if (!tabResults.is_open()) {
        qDebug() << "Could not open file";
        return -1;
    }

    //
    // read first line and set headings (ignoring second column for now)
    //

    std::string inputLine;
    std::getline(tabResults, inputLine);
    std::istringstream iss(inputLine);
    int colCount = 0;
    theHeadings << "Run #";
    do
    {
        std::string subs;
        iss >> subs;
        if (colCount > 1) {
            if (subs != " ") {
                theHeadings << subs.c_str();
        //qDebug()<<"\n the value of subs.c_str() is      "<<subs.c_str()<<"\n";
            }
        }
        colCount++;
    } while (iss);

    colCount = colCount-2;
    spreadsheet->setColumnCount(colCount);
    spreadsheet->setHorizontalHeaderLabels(theHeadings);
    // now until end of file, read lines and place data into spreadsheet

    int rowCount = 0;
    while (std::getline(tabResults, inputLine)) {
        std::istringstream is(inputLine);
        int col=0;
        //qDebug()<<"\n the inputLine is        "<<inputLine.c_str();

        spreadsheet->insertRow(rowCount);
        for (int i=0; i<colCount+2; i++) {
            std::string data;
            is >> data;
            if (i != 1) {
                QModelIndex index = spreadsheet->model()->index(rowCount, col);
                spreadsheet->model()->setData(index, data.c_str());
                col++;
            }
        }
        rowCount++;
    }
    tabResults.close();
    // the above data is being read from dakotaTAB.out
    //qDebug()<<"\n  just finished printing    inputLine ";
    // rowCount;

    //if we did not compute mean and std. dev., due to sobelov indices so far, we will do so now

    QHBoxLayout *edpLayout = new QHBoxLayout();

    QList<QLineEdit*> qle_list;


    if(found_flag==0)
    {


    //QString  nameString(QString::fromStdString(subs));  // name of the output
    // iss >> subs;
    //QString meanText(QString::fromStdString(subs));
    //double mean = meanText.toDouble();  // this is the mean value
    //iss >> subs;
    //QString stdDevText(QString::fromStdString(subs));
    //double stdDev = stdDevText.toDouble();  // this is std. deviation
     //QWidget *theWidget = this->createResultEDPWidget(nameString, mean, stdDev);
     //summaryLayout->addWidget(theWidget);

        std::ifstream fileResults("dakota.in");

        //now look for "response_functions"
        const std::string needle3 = "response_functions";

                 while (std::getline(fileResults, haystack))
                {
                       //qDebug()<<"\n the value is  \n"<<haystack.c_str();
                       //qDebug()<<"\n the haystack is   "<<haystack;
                       //qDebug()<<"\n";
                       //qDebug()<<"\n   the value of haystack.find(needle2)  "<<haystack.find(needle2);
                       //if (haystack.find(needle)==true){found_flag=1;}
                       if (haystack.find(needle3) != std::string::npos)
                        {
                            break;
                        }
                 }

                 std::istringstream iss2(haystack);
                 std::string word1;
                 iss2 >> word1;
                 QString string_number_of_responses(QString::fromStdString(word1));

                 std::string word2;
                 iss2 >> word2;
                 QString string_equal_to(QString::fromStdString(word2));

                 std::string word3;
                 iss2 >> word3;
                 QString value_number_of_responses(QString::fromStdString(word3));

                 int value_no_of_variables = value_number_of_responses.toDouble();
                 //qDebug()<<"\n the value of no variables is     "<<value_no_of_variables;
                 //while(iss2 >> word) {
                 //QString string_numberofvariables(QString::fromStdString(word));
             //   qDebug()<<"\n The value of number of response functions    is   "<<value_no_of_variables;
                // }
            //    qDebug()<<"\n the value of rowCount is      "<<rowCount;

                 for(int counter_col=0;counter_col<value_no_of_variables;++counter_col)
                 {
                    // compute the mean
                    double sum_value=0;
                    for(int number_of_rows=0;number_of_rows<rowCount;++number_of_rows)
                        {
                       //   qDebug()<<"\n The value of colCount     "<<colCount;
                       //   qDebug()<<"\n The value of counter_col  "<<counter_col;
                          QTableWidgetItem *item_index = spreadsheet->item(number_of_rows,colCount-(counter_col+1));
                          double value_item = item_index->text().toDouble();
                          sum_value=sum_value+value_item;
                        }

                    double mean_value=sum_value/rowCount;

                  //  qDebug()<<"\n The   mean_value is   "<<mean_value;

                    double sd_value=0;
                    for(int number_of_rows=0;number_of_rows<rowCount;++number_of_rows)
                        {
                         QTableWidgetItem *item_index = spreadsheet->item(number_of_rows,colCount-(counter_col+1));
                          double value_item = item_index->text().toDouble();
                          sd_value = (mean_value-value_item)*(mean_value-value_item);
                        }
                            sd_value = sd_value/rowCount;
                            sd_value=sqrt(sd_value);
                 // QString  nameString(QString::fromStdString(word1));
                  //  theWidget = this->createResultEDPWidget(nameString,sd_value, sd_value);
                //  qDebug()<<"\n the value of standard deviatio is     "<<sd_value;
                  QString variable_name = theHeadings.at(colCount-(counter_col+1));
                  variable_name.append("::");
                  variable_name.append("\t");
                  variable_name.append("Mean");
                  variable_name.append("  ");
                  variable_name.append(QString::number(mean_value));

                  variable_name.append("  ");

                  variable_name.append("Std. Dev.");


                  variable_name.append("\t");

                  variable_name.append(QString::number(sd_value));

                  QLineEdit *variablenameLineEdit = new QLineEdit;

                  variablenameLineEdit->setPlaceholderText(variable_name);
                  variablenameLineEdit->setReadOnly(true);
                  variablenameLineEdit->setAlignment(Qt::AlignTop);
//                  QWidget *variablenameWidget = addLabeledLineEdit(QString("Name"), &variablenameLineEdit);

  //                variablenameLineEdit->setText("Nodal Disp.");
  //                variablenameLineEdit->setDisabled(true);
                   //         theNames.append(name);

                    qle_list.append(new QLineEdit());
  //  qle_list.at(counter_col)->setObjectName(QString("lineEdit%1").arg(counter_col)); // It's easier to recognize an object by name

                    qle_list.at(counter_col)->setText(variable_name);
                    qle_list.at(counter_col)->setDisabled(true);

                    delete variablenameLineEdit;
                    edpLayout->addWidget(qle_list.at(counter_col));


                 }
                 //edpLayout->setAlignment(Qt::AlignCenter);

                    summaryLayout->addLayout(edpLayout);
        }


  summaryLayout->addStretch();

  //  exit(1);

    // this is where we are connecting edit triggers
    spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));

    //
    // create a chart, setting data points from first and last col of spreadsheet
    //
    chart = new QChart();
    chart->setAnimationOptions(QChart::AllAnimations);

    // by default the constructor is called and it plots the graph of the last column on Y-axis w.r.t first column on the
    // X-axis
    this->onSpreadsheetCellClicked(0,colCount-1);

    // to control the properties, how your graph looks you must click and study the onSpreadsheetCellClicked

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->chart()->legend()->hide();
    //
    // into QWidget place chart and spreadsheet
    //
    best_fit_label_text = new QLabel();


    QVBoxLayout *plotting_instructions_layout = new QVBoxLayout;



    QLabel *label = new QLabel();

    label->setStyleSheet("QLabel { background-color : white; color : gray; }");

    //label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setText("PLOTTING INSTRUCTIONS:\n \n 1. First left click on a column cell plots \n "
                   "the corresponding column on Y-axis without \n changing the X-axis.\n\n "
                   "2. A subsequent right click on the same column plots its \n cumulative "
                   "probability distribution, and a subsequent\nleft click plots a histogram with best fit distribution."
                   );
    QFont f( "Helvetica [Cronyx]", 10, QFont::Normal);
    label->setFont(f);
    label->setAlignment(Qt::AlignTop);

    QLineEdit *plotting_instructions1 = new QLineEdit();
    QLineEdit *plotting_instructions2 = new QLineEdit();

    plotting_instructions1->setText("Plotting Instructions:\n");

    plotting_instructions1->setText("First left click on a column");

//    plotting_instructions->setText(instructions_text);


    //variablenameLineEdit->(variable_name);
    //plotting_instructions->setAlignment(Qt::AlignTrailing);


    //\n\n"
   //    "\n\n");
    //label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    //txt->setStandardButtons(QMessageBox->Save);
    //txt->icon(0);
    //txt->removeButton(0);
    //txt->setText("Left click on the column of the spreadsheet provides the distribution.\n \n Single right click provides"
     //"the cumulative distribution. \n\n Two consecutive provide the distribution histogram and best fit ditribution. ");
    QWidget *widget = new QWidget();
    //  QVBoxLayout *layout = new QVBoxLayout(widget);    // this was originally done by frank
    //padhye changed to grid layout
    QGridLayout *layout = new QGridLayout(widget);
    QPushButton* save_spreadsheet = new QPushButton();
    save_spreadsheet->setText("Save Data");
    save_spreadsheet->setToolTip(tr("Save data into file in a CSV format"));
    save_spreadsheet->resize(30,30);
    connect(save_spreadsheet,SIGNAL(clicked()),this,SLOT(onSaveSpreadsheetClicked()));
    //QMenu *fileMenu = new QMenu(spreadsheet);
    //fileMenu->addMenu("File");
    layout->addWidget(chartView, 0,0,1,1);
    layout->addWidget(save_spreadsheet,1,0,Qt::AlignLeft);
    layout->addWidget(spreadsheet,2,0,1,1);
    layout->addWidget(best_fit_label_text,2,1,1,1,Qt::AlignTop);
    layout->addWidget(label,0,1,1,Qt::AlignTop);
 //   layout->addWidget(plotting_instructions1,0,1,1,Qt::AlignTop);
  //  layout->addWidget(plotting_instructions2,0,1,1,Qt::AlignTop);

    //layout->setColumnMinimumWidth(1,250);

   // QLabel *best_fit_instructions=new QLabel(this);

   // layout->addWidget(best_fit_instructions,1,1,Qt::AlignLeft);

    //layout->addWidget(label,0,1,1,1,Qt::AlignLeft);



    //plotting_instructions_layout->addWidget(label);
    //plotting_instructions_layout->addWidget(plotting_instructions);
    //label->setMaximumWidth(200);
    //plotting_instructions->setMaximumWidth(200);

    //plotting_instructions_layout->setSizeConstraint(QLayout::SetDefaultConstraint);//SetMaximumSize(QSize(300,400));
    //plotting_instructions_layout->maximumSize(100);
    //layout->addLayout(plotting_instructions_layout,0,1,1,1,Qt::AlignLeft);



    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //

    tabWidget->addTab(summary,tr("Summmary"));
    tabWidget->addTab(dakotaText, tr("General"));
    tabWidget->addTab(widget, tr("Data Values"));

    tabWidget->adjustSize();

    return 0;
}

//padhye 8/15/2018, this is where we are setting signals to click on the spread sheet and
//then deciding what to plot.

// if the the spreadsheet is clicked anywhere, then corresponding to the cell cliked
// we emit a signal and come here. Otherwise it won't come here.

// by the way, the code will come to this routine for the default plot.

void
DakotaResultsSampling::onSaveSpreadsheetClicked()
{

          int rowCount = spreadsheet->rowCount();
          int columnCount = spreadsheet->columnCount();

          QString fileName = QFileDialog::getSaveFileName(this,
              tr("Save Data"), "",
              tr("All Files (*)"));

          QFile file(fileName);
          if (file.open(QIODevice::ReadWrite))
          {
              QTextStream stream(&file);
              for (int j=0; j<columnCount; j++)
              {
                 stream <<theHeadings.at(j)<<",\t";
              }
              stream <<endl;
                for (int i=0; i<rowCount; i++)
                    {
                        for (int j=0; j<columnCount; j++)
                           {
                                QTableWidgetItem *item_value = spreadsheet->item(i,j);
                                double value = item_value->text().toDouble();
                                stream << value << ",\t";
                               //     qDebug()<<value;
                            }
                        stream<<endl;
                    }
          }

    //    QFileDialog::getOpenFileName( this, tr("Open Document"), QDir::currentPath(), tr("Document files (*.doc *.rtf);;All files (*.*)"), 0, QFileDialog::DontUseNativeDialog );

     //  qDebug()<<QDir::currentPath();

}

void DakotaResultsSampling::onSpreadsheetCellClicked(int row, int col)
{
    mLeft = spreadsheet->wasLeftKeyPressed();

  //  best_fit_instructions->clear();
    //see the file MyTableWiget.cpp in order to find the function wasLeftKeyPressed();
    //qDebug()<<"\n the value of mLeft       "<<mLeft;
    //qDebug()<<"\n I am inside the onSpreadsheetCellClicked routine  and I am exiting!!  ";
    //  exit(1);
    // create a new series
    chart->removeAllSeries();
    //chart->removeA

    QAbstractAxis *oldAxisX=chart->axisX();
    if (oldAxisX != 0)
        chart->removeAxis(oldAxisX);
    QAbstractAxis *oldAxisY=chart->axisY();
    if (oldAxisY != 0)
        chart->removeAxis(oldAxisY);

    // QScatterSeries *series;//= new QScatterSeries;

    int oldCol;
    if (mLeft == true) {
        oldCol= col2;   //
        //oldCol=0;   // padhye trying. I don't think it makes sense to use coldCol as col2, i.e., something that was
                    // previously selected?
        col2 = col; // col is the one that comes in te function, based on the click made after clicking

    //    qDebug()<<"\n the value of oldcol is  "<<oldCol;
    //    qDebug()<<"\n the value of col2 is    "<<col2;
    //    qDebug()<<"\t the value of col is     "<<col;

    } else {
        oldCol= col1;
        col1 = col;
    }

    int rowCount = spreadsheet->rowCount();

    if (col1 != col2) {
        QScatterSeries *series = new QScatterSeries;

        for (int i=0; i<rowCount; i++) {
            QTableWidgetItem *itemX = spreadsheet->item(i,col1);    //col1 goes in x-axis, col2 on y-axis
            //col1=0;
            QTableWidgetItem *itemY = spreadsheet->item(i,col2);
            QTableWidgetItem *itemOld = spreadsheet->item(i,oldCol);
            itemOld->setData(Qt::BackgroundRole, QColor(Qt::white));
            itemX->setData(Qt::BackgroundRole, QColor(Qt::lightGray));
            itemY->setData(Qt::BackgroundRole, QColor(Qt::lightGray));

            series->append(itemX->text().toDouble(), itemY->text().toDouble());
        }
        chart->addSeries(series);
        series->setName("Samples");

        QValueAxis *axisX = new QValueAxis();
        QValueAxis *axisY = new QValueAxis();

        axisX->setTitleText(theHeadings.at(col1));
        axisY->setTitleText(theHeadings.at(col2));

        // padhye adding ranges 8/25/2018
        // finding the range for X and Y axis
        // now the axes will look a bit clean.

        double minX, maxX;
        double minY, maxY;

        for (int i=0; i<rowCount; i++) {

            QTableWidgetItem *itemX = spreadsheet->item(i,col1);
            QTableWidgetItem *itemY = spreadsheet->item(i,col2);

            double value1 = itemX->text().toDouble();

            double value2 = itemY->text().toDouble();


            if (i == 0) {
                minX=value1;
                maxX=value1;
                minY=value2;
                maxY=value2;
                        }
            if(value1<minX){minX=value1;}
            if(value1>maxX){maxX=value1;}

            if(value2<minY){minY=value2;}
            if(value2>maxY){maxY=value2;}

        }

        double xRange=maxX-minX;
        double yRange=maxY-minY;

      //  qDebug()<<"\n the value of xRange is     ";
        //qDebug()<<xRange;

        //qDebug()<<"\n the value of yRange is     ";
        //qDebug()<<yRange;

        // if the column is not the run number, i.e., 0 column, then adjust the x-axis differently

        if(col1!=0)
        {
        axisX->setRange(minX - 0.01*xRange, maxX + 0.1*xRange);
        }
        else{

        axisX->setRange(int (minX - 1), int (maxX +1));
       // axisX->setTickCount(1);

        }
        // adjust y with some fine precision
        axisY->setRange(minY - 0.1*yRange, maxY + 0.1*yRange);
        chart->setAxisX(axisX, series);
        chart->setAxisY(axisY, series);

    } else {

        QLineSeries *series= new QLineSeries;

        static double NUM_DIVISIONS_FOR_DIVISION = 10.0;
        double *dataValues = new double[rowCount];
        double histogram[NUM_DIVISIONS];
        for (int i=0; i<NUM_DIVISIONS; i++)
            histogram[i] = 0;

        double min = 0;
        double max = 0;
        for (int i=0; i<rowCount; i++) {
            QTableWidgetItem *itemX = spreadsheet->item(i,col1);
            QTableWidgetItem *itemOld = spreadsheet->item(i,oldCol);
            itemOld->setData(Qt::BackgroundRole, QColor(Qt::white));
            itemX->setData(Qt::BackgroundRole, QColor(Qt::lightGray));
            double value = itemX->text().toDouble();
            dataValues[i] =  value;

            if (i == 0) {
                min = value;
                max = value;
            } else if (value < min) {
                min = value;
            } else if (value > max) {
                max = value;
            }
         }

            if (mLeft == true) {

            // frequency distribution
            double range = max-min;
            double dRange = range/NUM_DIVISIONS_FOR_DIVISION;

            for (int i=0; i<rowCount; i++) {
                // compute block belongs to, watch under and overflow due to numerics
                int block = floor((dataValues[i]-min)/dRange);
                if (block < 0) block = 0;
                if (block > NUM_DIVISIONS-1) block = NUM_DIVISIONS-1;
                histogram[block] += 1;
            }

            double maxPercent = 0;
            for (int i=0; i<NUM_DIVISIONS; i++) {
                histogram[i]/rowCount;
                if (histogram[i] > maxPercent)
                    maxPercent = histogram[i];
            }
            for (int i=0; i<NUM_DIVISIONS; i++) {
                series->append(min+i*dRange, 0);
                series->append(min+i*dRange, histogram[i]);
                series->append(min+(i+1)*dRange, histogram[i]);
                series->append(min+(i+1)*dRange, 0);
            }


            chart->addSeries(series);
            series->setName("Histogram");

            QValueAxis *axisX = new QValueAxis();
            QValueAxis *axisY = new QValueAxis();

            axisX->setRange(min-(max-min)*.1, max+(max-min)*.1);
            axisY->setRange(0, 1.1*maxPercent);
            axisY->setTitleText("Frequency %");
            axisX->setTitleText(theHeadings.at(col1));
            axisX->setTickCount(NUM_DIVISIONS+1);
            chart->setAxisX(axisX, series);
            chart->setAxisY(axisY, series);

            //calling external python script to find the best fit, generating the data and then plotting it.

            // this will be done in the application directory

            QString appDIR = qApp->applicationDirPath(); // this is where the .exe is and also the parseJson.py, and now the fit_distribution.py

           // qDebug()<<"\n the value of appDIR is    "<<appDIR;
          //  exit(1);
            QString data_input_file = appDIR +  QDir::separator() + QString("data_input.txt");
            QString pySCRIPT_dist_fit =  appDIR +  QDir::separator() + QString("fit.py");

            //QString tDirectory = appDIR + QDir::separator() + QString("tmp.distributionfit");
            // dump the data into a file


            //QDir mDir; // an object of the class
            //mDir.mkdir(tDirectory);
            //qDebug()<<"\n the value of tDirectory is  "<<pySCRIPT_dist_fit;

            QFile file(data_input_file);
            QTextStream stream(&file);

            //qDebug()<<"\n the data values are   \n";
            if (file.open(QIODevice::ReadWrite))
            {
                    for(int i=0;i<rowCount;++i)
                     {

                        stream<<dataValues[i];
                        stream<< endl;

                        //qDebug()<<"\n the dataValues is"<<dataValues[i];
                     }
            }else {qDebug()<<"\n error in opening file data file for histogram fit  ";exit(1);}


            delete [] dataValues;

            QString file_fitted_path = appDIR +  QDir::separator() + QString("Best_fit.out");

            QFile file_fitted_distribution(file_fitted_path);
            if(!file_fitted_distribution.open(QIODevice::WriteOnly)) {
                QMessageBox::information(0,"error",file.errorString());
            }else
            {

            }
            // make sure to check if Q_OS_WIN or Mac etc. else there will be a bug
            //QString command = QString("python ") + pySCRIPT_dist_fit;
            //QString command = QString(" python C:\\Users\\nikhil\\NHERI\\build-uqFEM-Desktop_Qt_5_11_0_MSVC2015_32bit-Release\\debug\\fit.py ");
           QProcess process;
          // qDebug()<<"\n the pySCRIPT is        "<<pySCRIPT_dist_fit;
           process.setWorkingDirectory(appDIR);
           process.execute("python",QStringList()<<pySCRIPT_dist_fit);
           QFile file_fitted_data("Best_fit.out");
           if(!file_fitted_data.exists())
           {
            qDebug()<<"\n The file does not exist and hence exiting";
            exit(1);

           }
           QLineSeries *series_best_fit = new QLineSeries();
            series_best_fit->setName("Best Fit");
           if(file_fitted_data.open(QIODevice::ReadOnly |QIODevice::Text ))
           {

               QTextStream txtStream(&file_fitted_data);
               while(!txtStream.atEnd())
           {
                   QString line = txtStream.readLine();
                 //  double first_value;
                //   txtStream>>first_value;

                  QStringList list2 = line.split(',', QString::SkipEmptyParts);
                   double value1= list2[0].toDouble();
                   double value2= list2[1].toDouble();

                  series_best_fit->append(value1,value2);

                  //chart->setAxisX(axisX, series_best_fit);
                  //chart->setAxisY(axisY, series_best_fit);
                //  chart->adjustSize();

                // qDebug()<<"\n the line read is      "<<line;
                // qDebug()<<"\n the value1 is         "<<value1;
                // qDebug()<<"\n the value2 is         "<<value2;
            }

           file_fitted_data.close();

           chart->addSeries(series_best_fit);
           chart->legend()->setVisible(true);
           chart->legend()->setAlignment(Qt::AlignTop);
          // chart->setTitle("The best fit plot is");
           QString best_fit_info_file = appDIR +  QDir::separator() + QString("data_fit_info.out");

           QFile info_fit_file("data_fit_info.out");
           QTextStream stream(&info_fit_file);

           //QStringList message_fitting_info;

           QString line_from_file="";
           if (info_fit_file.open(QIODevice::ReadWrite))
           {

                while(!info_fit_file.atEnd())
                {
               line_from_file=line_from_file+info_fit_file.readLine();//+"\n";
             //  qDebug()<<"\n the value of info_fit_file.readLine() is   "<<info_fit_file.readLine();
             //  qDebug()<<"\n The value of line_from_file is    "<<line_from_file;
             //   message_fitting_info<<info_fit_file.readLine();
             //   message_fitting_info<<"\n";
                }
                //line_from_file=line_from_file+info_fit_file.readLine()+"\n";
           }
           best_fit_label_text->setText(line_from_file);
           best_fit_label_text->setStyleSheet("QLabel { background-color : white; color : gray; }");

           QFont f2("Helvetica [Cronyx]", 10, QFont::Normal);
           best_fit_label_text->setFont(f2);
           //msgBox.show();
            //  msgBox.exec();

          //     best_fit_instructions->setText("I am fitting the best fit info. here");
           //qDebug()<<"\n\n\n I am about to exit from here   \n\n\n";
           // exit(1);
           }
           //std::ifstream fitted_data_file("Best_fit.out");
           //if(!fitted_data_file.exists)
          // process.execute("python C:/Users/nikhil/NHERI/build-uqFEM-Desktop_Qt_5_11_0_MSVC2015_32bit-Release/debug\\fit.py");
           // exit(1);
            //qDebug()<<"\n   the value of pySCRIPT_dist_fit   is         "<<pySCRIPT_dist_fit;
            //#ifdef Q_OS_WIN
         //   QProcess process;
         //String command = "notepad";
         //process.execute("cmd", QStringList() << "/C" << command);
         //process.start("cmd", QStringList(), QIODevice::ReadWrite);
         //std::cerr << command << "\n";

       //     process.waitForStarted();

       //     qDebug()<<"process has been completed       ";
   //         exit(1);
/*
    #else
    QString command = QString("source $HOME/.bashrc; python ") + pySCRIPT + QString(" ") + tDirectory + QString(" ") +
            tmpDirectory + QString(" runningLocal");
    //QString command = QString("python ") + pySCRIPT + QString(" ") + tDirectory + QString(" ") +
    //        tmpDirectory + QString(" runningLocal");
    proc->execute("bash", QStringList() << "-c" <<  command);
    qInfo() << command;
    // proc->start("bash", QStringList("-i"), QIODevice::ReadWrite);
    #endif
    proc->waitForStarted();
*/
    } else {
            // cumulative distribution
            mergesort(dataValues, rowCount);
            for (int i=0; i<rowCount; i++) {
                series->append(dataValues[i], 1.0*i/rowCount);
                   //qDebug()<<"\n the dataValues[i] and rowCount is     "<<dataValues[i]<<"\t"<<rowCount<<"";
            }
            delete [] dataValues;
            chart->addSeries(series);
            QValueAxis *axisX = new QValueAxis();
            QValueAxis *axisY = new QValueAxis();
            // padhye, make these consistent changes all across.
            axisX->setRange(min- (max-min)*0.1, max+(max-min)*0.1);
            axisY->setRange(0, 1);
            axisY->setTitleText("Cumulative Probability");
            axisX->setTitleText(theHeadings.at(col1));
            axisX->setTickCount(NUM_DIVISIONS+1);
            chart->setAxisX(axisX, series);
            chart->setAxisY(axisY, series);
            series->setName("Cumulative Frequency Distribution");
        }
    }
}


// padhye
// this function is called if you decide to say save the data from UI into a json object
bool
DakotaResultsSampling::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["resultType"]=QString(tr("DakotaResultsSampling"));
   // qDebug()<<"\n \n I am inside the Dakota Sampling    \n    ";
   // qDebug()<<"\n \n the value of resultType is     "<<jsonObject["resultType"]<<"\n \n";
    //qDebug()<<"\n \n  \n \n the value of jsonObject is \n\n\n    ";
    //qDebug()<<jsonObject<<"\n\n";

    //
    // add summary data
    //

    QJsonArray resultsData;
    int numEDP = theNames.count();
    for (int i=0; i<numEDP; i++) {
        QJsonObject edpData;
        edpData["name"]=theNames.at(i);
        edpData["mean"]=theMeans.at(i);
        edpData["stdDev"]=theStdDevs.at(i);
        resultsData.append(edpData);

//        qDebug()<<"\the value of edpData is     \n";
//        qDebug()<<edpData<<"\n  ";
//        qDebug()<<edpData["name"]<<edpData["mean"]<<"\n";

    }

  //  qDebug()<<"\n I am exiting here     ";

//    exit(1);


    jsonObject["summary"]=resultsData;


    // add general data
    jsonObject["general"]=dakotaText->toPlainText();

    //
    // add spreadsheet data
    //

    QJsonObject spreadsheetData;

    int numCol = spreadsheet->columnCount();
    int numRow = spreadsheet->rowCount();

    spreadsheetData["numRow"]=numRow;
    spreadsheetData["numCol"]=numCol;

    QJsonArray headingsArray;
    for (int i = 0; i <theHeadings.size(); ++i) {
        headingsArray.append(QJsonValue(theHeadings.at(i)));
    }

    spreadsheetData["headings"]=headingsArray;

    QJsonArray dataArray;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int row = 0; row < numRow; ++row) {
        for (int column = 0; column < numCol; ++column) {
            QTableWidgetItem *theItem = spreadsheet->item(row,column);
            QString textData = theItem->text();
            dataArray.append(textData.toDouble());
        }
    }
    QApplication::restoreOverrideCursor();
    spreadsheetData["data"]=dataArray;

    jsonObject["spreadsheet"] = spreadsheetData;
    return result;
}



// if you already have a json data file then you can populate the UI with the entries from json.

bool
DakotaResultsSampling::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;

    this->clear();

    //
    // create a summary widget in which place basic output (name, mean, stdDev)
    //

    QWidget *summary = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summary->setLayout(summaryLayout);

    QJsonArray edpArray = jsonObject["summary"].toArray();
    foreach (const QJsonValue &edpValue, edpArray) {
        QString name;
        double mean, stdDev;
        QJsonObject edpObject = edpValue.toObject();
        QJsonValue theNameValue = edpObject["name"];
        name = theNameValue.toString();

        QJsonValue theMeanValue = edpObject["mean"];
        mean = theMeanValue.toDouble();

        QJsonValue theStdDevValue = edpObject["stdDev"];
        stdDev = theStdDevValue.toDouble();

        QWidget *theWidget = this->createResultEDPWidget(name, mean, stdDev);
        summaryLayout->addWidget(theWidget);
    }
    summaryLayout->addStretch();

    //
    // into a QTextEdit place more detailed Dakota text
    //

    dakotaText = new QTextEdit();
    dakotaText->setReadOnly(true); // make it so user cannot edit the contents
    QJsonValue theValue = jsonObject["general"];
    dakotaText->setText(theValue.toString());

    //
    // into a spreadsheet place all the data returned
    //

    spreadsheet = new MyTableWidget();
    QJsonObject spreadsheetData = jsonObject["spreadsheet"].toObject();
    int numRow = spreadsheetData["numRow"].toInt();
    int numCol = spreadsheetData["numCol"].toInt();
    spreadsheet->setColumnCount(numCol);
    spreadsheet->setRowCount(numRow);

    QJsonArray headingData= spreadsheetData["headings"].toArray();
    for (int i=0; i<numCol; i++) {
        theHeadings << headingData.at(i).toString();
    }

    spreadsheet->setHorizontalHeaderLabels(theHeadings);

    QJsonArray dataData= spreadsheetData["data"].toArray();
    int dataCount =0;
    for (int row =0; row<numRow; row++) {
        for (int col=0; col<numCol; col++) {
            QModelIndex index = spreadsheet->model()->index(row, col);
            spreadsheet->model()->setData(index, dataData.at(dataCount).toDouble());
            dataCount++;
        }
    }
    spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));

    //
    // create a chart, setting data points from first and last col of spreadsheet
    //

    chart = new QChart();
    chart->setAnimationOptions(QChart::AllAnimations);
    QScatterSeries *series = new QScatterSeries;
    col1 = 0;           // col1 is initialied as the first column in spread sheet
    col2 = numCol-1;    // col2 is initialized as the second column in spread sheet
    mLeft = true;       // left click

    this->onSpreadsheetCellClicked(0,numCol-1);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->chart()->legend()->hide();


    //
    // create a widget into which we place the chart and the spreadsheet
    //

    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget(chartView, 1);
    layout->addWidget(spreadsheet, 1);

    //layout->addWidget(analysis_message,1);
    // add 3 Widgets to TabWidget
    //

    tabWidget->addTab(summary,tr("Summmary"));
    tabWidget->addTab(dakotaText, tr("General"));
    tabWidget->addTab(widget, tr("Data Values"));

    tabWidget->adjustSize();

    //qDebug()<<"\n debugging the values: result is  \n"<<result<<"\n";

    return result;
}


extern QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit);

QWidget *
DakotaResultsSampling::createResultEDPWidget(QString &name, double mean, double stdDev) {
    QWidget *edp = new QWidget;
    QHBoxLayout *edpLayout = new QHBoxLayout();

    edp->setLayout(edpLayout);

    QLineEdit *nameLineEdit;
    QWidget *nameWidget = addLabeledLineEdit(QString("Name"), &nameLineEdit);
    nameLineEdit->setText(name);
    nameLineEdit->setDisabled(true);
    theNames.append(name);
    edpLayout->addWidget(nameWidget);

    QLineEdit *meanLineEdit;
    QWidget *meanWidget = addLabeledLineEdit(QString("Mean"), &meanLineEdit);
    meanLineEdit->setText(QString::number(mean));
    meanLineEdit->setDisabled(true);
    theMeans.append(mean);
    edpLayout->addWidget(meanWidget);

    QLineEdit *stdDevLineEdit;
    QWidget *stdDevWidget = addLabeledLineEdit(QString("StdDev"), &stdDevLineEdit);
    stdDevLineEdit->setText(QString::number(stdDev));
    stdDevLineEdit->setDisabled(true);
    theStdDevs.append(stdDev);
    edpLayout->addWidget(stdDevWidget);

    edpLayout->addStretch();

    return edp;
}
