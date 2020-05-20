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

// Purpose: to present a widget for FEAPpv,
// 1) to open feap input files, to parse for any parameters that have been set
// and then to return the variablename and values in a string. These are used to init the random variable widget.
// 2) for dakota to again open and parse the input file, this time replacing any input parameters
// with the needed dakota input format: varName {varName}

#include <FEAPpvParser.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <iterator>
#include <string>

using namespace std;

FEAPpvParser::FEAPpvParser()
{

}

FEAPpvParser::~FEAPpvParser()
{

}

QStringList
FEAPpvParser::getVariables(QString inFilename)
{
  QStringList result;

  ifstream inFile(inFilename.toStdString());

  /*  ************** THIS MIGHT BE MORE EFFICIENT AS DON't REGEX EVERY LINE ***************************
  // read lines of input searching for pset using regular expression 
  regex paramLine("PARA");
  regex paramSet("[A-Za-z0-9]+[ ]+[=][-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?");
  regex anything("[a-zA-Z0-9]");
  string line;
  bool doParam = false;

  while (getline(inFile, line)) {

    if (doParam == true) {
        if (!regex_search(line, anything)) {
            doParam = false;
        } else {
            // if found break into cmd, varName and value (ignore the rest)
            istringstream iss(line);
            string varName, eq, value;
            iss >> varName >> eq >> value;

            // add varName and value to results
            result.append(QString::fromStdString(varName));
            result.append(QString::fromStdString(value));
        }
    }
    if (regex_search(line, paramLine)) {
      doParam = true;
    }
  } 
 ************************************************************************************/

  // read lines of input searching for param set using regular expression
  regex paramSet("[ \t]*[A-Za-z0-9]+[ \t]+[=][ \t]+[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?");
  string line;

  while (getline(inFile, line)) {
      if (regex_search(line, paramSet)) {
          // if found break into cmd, varName and value (ignore the rest)
          istringstream iss(line);
          string varName, eq, value;
          iss >> varName >> eq >> value;

          // add varName and value to results
          result.append(QString::fromStdString(varName));
          result.append(QString::fromStdString(value));
      }
  }

  // close file   
  inFile.close();
  
  return result;
}

void 
FEAPpvParser::writeFile(QString inFilename, QString outFilename, QStringList varToChange)
{
    ifstream inFile(inFilename.toStdString());
    ofstream outFile(outFilename.toStdString());

    regex paramSet("[ \t]*[A-Za-z0-9]+[ \t]+[=][ \t]+[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?");
    string line;

    while (getline(inFile, line)) {

        if (regex_search(line, paramSet)) {

            // if found break into cmd, varName and value (ignore the rest)
            istringstream iss(line);
            string varName, eq, value;
            iss >> varName >> eq >> value;

            // if varName in input sting list, modify line otherwise write current line
            QString val1(QString::fromStdString(varName));

            if (varToChange.contains(val1)) {
                // write new line format to output
               // OLD: outFile << varName << " =  \{" << varName << "\}\n";
                outFile << varName << " =  \"RV." << varName << "\"\n";
            } else {

                // not there, write current line
                outFile << line << "\n";
            }
        } else
            // just copy line to output
            outFile << line << "\n";
    }

    // close file
    inFile.close();
    outFile.close();
}
