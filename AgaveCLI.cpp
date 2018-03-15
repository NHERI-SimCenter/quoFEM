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
#include <AgaveCLI.h>
#include <QApplication>
#include <QDebug>
#include <QTemporaryFile>

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <iterator>
#include <string>


using namespace std;

AgaveCLI::AgaveCLI()
{

}

AgaveCLI::~AgaveCLI()
{

}

bool 
AgaveCLI::login(QString &login, QString &password){

}

void 
AgaveCLI::uploadDirectory(QString &remote, QString &local){

}

void AgaveCLI::downloadFile(QString &remote, QString &local){

}


QString AgaveCLI::startJob(QString &jobDescription){
  QString fileName;
  QTemporary tmpFile;
  if (tmpFile.open()) {
    fileName = tmpFile.fileName();
    qDebug() << fileName;
    tmpFile.close();
  }

#ifdef Q_OS_WIN
  //  QString command = QString("python ") + pySCRIPT + QString(" ") + tDirectory + QString(" ") + tmpDirectory;
  //  proc->execute("cmd", QStringList() << "/C" << command);
#else
  QString command = QString("jobs-submit -v -F ") + jobDescription + QString(" > ") + tmpFile;
  proc->execute("bash", QStringList() << "-c" <<  command);
  qDebug() << command;
#endif
  proc->waitForStarted();
  QFile resultsFile;
  resultsFile.open();
}

const AgaveCLI::QString &getJobStatus(in jobID){

}

void AgaveCLI::deleteJob(QString &jobID){

}
