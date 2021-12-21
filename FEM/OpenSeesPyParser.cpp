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

// Purpose: to present a widget for OpenSeesPy,
// 1) to open feap input files, to parse for any parameters that have been set WITH THE PSET command
// and then to return the variablename and values in a string. These are used to init the random variable widget.
// 2) for dakota to again open and parse the input file, this time replacing any input parameters
// with the needed dakota input format: pset varName {varName}

#include <OpenSeesPyParser.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <iterator>
#include <string>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

using namespace std;

OpenSeesPyParser::OpenSeesPyParser()
{

}

OpenSeesPyParser::~OpenSeesPyParser()
{

}

QStringList
OpenSeesPyParser::getVariables(QString inFilename)
{
  QStringList result;

  ifstream inFile(inFilename.toStdString());

  // read lines of input searching for pset using regular expression 
  string line;
  while (getline(inFile, line)) {

    string equalTo("=");
    string comment("#");

    std::size_t found = line.find(equalTo);
    std::size_t foundComm = line.find(comment);

    if ((found != std::string::npos) && (foundComm>found)){

      std::string varName = line.substr(0, found);
      varName.erase(std::remove_if(varName.begin(), varName.end(), ::isspace), varName.end());
      std::string value = line.substr(found+1,foundComm-found-1);
      value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());

      // strip possible ; from end of value (possible if comment) line
      regex delim(";");
      value = regex_replace(value,delim,"");
      
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
OpenSeesPyParser::writeFile(QString inFilename, QString outFilename, QStringList varToChange)
{
    qDebug() << "in: " << inFilename << " " << outFilename;
    ifstream inFile(inFilename.toStdString());
    ofstream outFile(outFilename.toStdString());

    QFileInfo outFileInfo(outFilename);
    QString  outFileDir = outFileInfo.absolutePath();

    // read lines of input searching for pset using regular expression
    // read lines of input searching for pset using regular expression
    string line;
    while (getline(inFile, line)) {

      string equalTo("=");
      std::size_t found = line.find(equalTo);
      if (found != std::string::npos) {

        std::string varNameWithSpaces = line.substr(0, found);
        std::string varName(varNameWithSpaces); // need an extra to ensure indentation correct for python
        varName.erase(std::remove_if(varName.begin(), varName.end(), ::isspace), varName.end());


            // if varName in input sting list, modify line otherwise write current line
            QString val1(QString::fromStdString(varName));

            if (varToChange.contains(val1)) {
                outFile << varNameWithSpaces << " = \"RV." << varName << "\"\n";
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
