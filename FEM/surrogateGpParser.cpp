
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

// Written: Sangri Yi - copied template from fmckenna

// Purpose: to present a widget for surrogateGP,
// 1) to open feap input files, to parse for any parameters that have been set WITH THE PSET command
// and then to return the variablename and values in a string. These are used to init the random variable widget.
// 2) for dakota to again open and parse the input file, this time replacing any input parameters
// with the needed dakota input format: pset varName {varName}

#include <surrogateGpParser.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <iterator>
#include <string>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
using namespace std;

surrogateGpParser::surrogateGpParser()
{

}

surrogateGpParser::~surrogateGpParser()
{

}

QStringList
surrogateGpParser::getVariables(QString inFilename)
{
    QStringList varNamesAndValues;

    QFile file(inFilename);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString val;
        val=file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject jsonSur = doc.object();
        file.close();

        auto GPidentifier = jsonSur.find("kernName"); // should be the right .json file
        if (!jsonSur.isEmpty() && GPidentifier != jsonSur.end()) {
            QJsonArray RVArray = jsonSur["randomVariables"].toArray();
            foreach (const QJsonValue & v, RVArray){
                 QJsonObject jsonRV = v.toObject();
                 varNamesAndValues.push_back(jsonRV["name"].toString());
                 varNamesAndValues.push_back(QString::number(jsonRV["value"].toDouble()));
            }
        }
    }

    return varNamesAndValues;
}

void 
surrogateGpParser::writeFile(QString inFilename, QString outFilename, QStringList varToChange)
{
    qDebug() << "surrogateGpParser::writeFile: " << inFilename << " out: " << outFilename << " args: " << varToChange;

    ifstream inFile(inFilename.toStdString());
    ofstream outFile(outFilename.toStdString());

    string line;
    while (getline(inFile, line)) {
            outFile << line << "\n";
    }

    // close file
    inFile.close();
    outFile.close();
    return;
}
