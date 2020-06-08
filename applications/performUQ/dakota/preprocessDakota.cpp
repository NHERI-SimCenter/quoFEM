#include <iostream>
#include <fstream>
#include <jansson.h> 
#include <string.h>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <thread>

#include "dakotaProcedures.h"

int main(int argc, const char **argv) {

  const char *inputFile = argv[1];
  const char *workflow = argv[2];
  const char *runType = argv[3];
  const char *osType = argv[4];
  
  struct randomVariables theRandomVariables;
  theRandomVariables.numRandomVariables = 0;

  std::vector<std::string> edpList;
  std::vector<std::string> rvList;

  //
  // open files & parse for RV, if RV found rename file, and put text in workflow driver for dprepro
  // 

  json_error_t error;
  json_t *rootINPUT = json_load_file(inputFile, 0, &error);
  if (rootINPUT == NULL) {
    std::cerr << "parseFileForRV:: could not open BIM file with name: " << inputFile << "\n";
    exit(801); // no random variables is allowed
  } 

  int numRV = parseForRV(rootINPUT, theRandomVariables);

  //
  // open empty dakota input file & write file
  //

  std::ofstream dakotaFile("dakota.in");

  if (!dakotaFile.is_open()) {
    std::cerr << "parseFileForRV:: could not create dakota input file: dakota.in\n";
    exit(802); // no random variables is allowed
  }

  json_t *uqData =  json_object_get(rootINPUT, "UQ_Method");
  if (uqData == NULL) {
    std::cerr << "preprocessJSON - no UQ Data in inputfile\n";
    exit(-1); // no random variables is allowed
  }

  json_t *rootEDP =  json_object_get(rootINPUT, "EDP");
  if (rootEDP == NULL) {
    return 0; // no random variables is allowed
  }

  int evalConcurrency = 0;
  if (strcmp(runType,"runningLocal") == 0) {
    evalConcurrency = (int)OVERSUBSCRIBE_CORE_MULTIPLIER * std::thread::hardware_concurrency();
    std::cerr << "EVAL: " << evalConcurrency << "\n";
  }

  std::string workflowDriver = "workflow_driver";
  if ((strcmp(osType,"Windows") == 0) && (strcmp(runType,"runningLocal") == 0))
    workflowDriver = "workflow_driver.bat";

  int errorWrite = writeDakotaInputFile(dakotaFile, uqData, rootEDP, theRandomVariables, workflowDriver, rvList, edpList, evalConcurrency);

  dakotaFile.close();

  //
  // open workflow_driver 
  //

  std::ofstream workflowDriverFile(workflowDriver);

  if (!workflowDriverFile.is_open()) {
    std::cerr << "parseFileForRV:: could not create workflow driver file: " << workflowDriver << "\n";
    exit(802); // no random variables is allowed
  }

  std::string dpreproCommand;
  std::string openSeesCommand;
  std::string pythonCommand;
  std::string feapCommand;
  std::string moveCommand;

  const char *localDir = json_string_value(json_object_get(rootINPUT,"localAppDir"));
  const char *remoteDir = json_string_value(json_object_get(rootINPUT,"remoteAppDir"));

  if (strcmp(runType, "runningLocal") == 0) {
    dpreproCommand = std::string("\"") + localDir + std::string("/applications/performUQ/dakota/simCenterDprepro\"");
    openSeesCommand = std::string("OpenSees");
    pythonCommand = std::string("\"") + json_string_value(json_object_get(rootINPUT,"python")) + std::string("\"");
    if ((strcmp(osType,"Windows") == 0)) {
      feapCommand = std::string("Feappv41.exe");
      moveCommand = std::string("move /y ");
    }
    else {
      feapCommand = std::string("feappv");
      moveCommand = std::string("mv ");
    }
  } else {
    dpreproCommand = remoteDir + std::string("/applications/performUQ/dakota/simCenterDprepro");
    openSeesCommand = std::string("OpenSees");
    pythonCommand = std::string("python");
    feapCommand = std::string("/home1/00477/tg457427/bin/feappv");
    moveCommand = std::string("mv ");
  }


  //
  // based on fem program we do things
  //

  json_t *fem =  json_object_get(rootINPUT, "fem");
  if (fem == NULL) {
    return 0; // no random variables is allowed
  }

  json_t *programType =  json_object_get(fem, "program");
  const char *program = json_string_value(programType);    

  std::cerr << "PROGRAM: " << program;

  if (strcmp(program,"OpenSees") == 0) {

    const char *mainInput =  json_string_value(json_object_get(fem, "mainInput"));
    const char *postprocessScript =  json_string_value(json_object_get(fem, "mainPostprocessScript"));
    
    int scriptType = 0;
    if (strstr(postprocessScript,".py") != NULL) 
      scriptType = 1;
    else if (strstr(postprocessScript,".tcl") != NULL) 
      scriptType = 2;

    std::ofstream templateFile("SimCenterInput.RV");
    for(std::vector<std::string>::iterator itRV = rvList.begin(); itRV != rvList.end(); ++itRV) {
      templateFile << "pset " << *itRV << " \"RV." << *itRV << "\"\n";
    }

    templateFile << "\n set listQoI \"";
    for(std::vector<std::string>::iterator itEDP = edpList.begin(); itEDP != edpList.end(); ++itEDP) {
      templateFile << *itEDP << " ";
    }

    templateFile << "\"\n\n\n source " << mainInput << "\n";

    workflowDriverFile << dpreproCommand << "  params.in SimCenterInput.RV SimCenterInput.tcl\n";
    workflowDriverFile << openSeesCommand << "  SimCenterInput.tcl >> ops.out\n";

    // depending on script type do something
    if (scriptType == 1) { // python script
      workflowDriverFile << "\n" << pythonCommand << " " << postprocessScript;
      for(std::vector<std::string>::iterator itEDP = edpList.begin(); itEDP != edpList.end(); ++itEDP) {
	workflowDriverFile << " " << *itEDP;
      }
    } 
    else if (scriptType == 2) { // tcl script
      templateFile << " source " << postprocessScript << "\n";      
    }

    templateFile.close();
  }    

  else  if (strcmp(program,"FEAPpv") == 0) {

    const char *mainInput =  json_string_value(json_object_get(fem, "mainInput"));
    const char *postprocessScript =  json_string_value(json_object_get(fem, "mainPostprocessScript"));

    // write run file
    std::ofstream feapFile("feapname");
    feapFile << "SimCenterIn.txt   \n";
    feapFile << "SimCenterOut.txt   \n";
    feapFile << "SimCenterR.txt   \n";
    feapFile << "SimCenterR.txt   \n";
    feapFile << "NONE   \n";
    feapFile << "\n";
    feapFile.close();

    // write driiver file
    workflowDriverFile << dpreproCommand << "  params.in " << mainInput  << " SimCenterIn.txt --formatFixed\n";
    workflowDriverFile << "echo y | " << feapCommand << "\n";
    workflowDriverFile << pythonCommand << " " << postprocessScript;
    for(std::vector<std::string>::iterator itEDP = edpList.begin(); itEDP != edpList.end(); ++itEDP) {
      workflowDriverFile << " " << *itEDP;
    }
    workflowDriverFile << "\n";
  }


  else if (strcmp(program,"OpenSeesPy") == 0) {

    const char *mainScript =  json_string_value(json_object_get(fem, "mainInput"));
    const char *postprocessScript =  json_string_value(json_object_get(fem, "mainPostprocessScript"));
    const char *parametersScript =  json_string_value(json_object_get(fem, "parametersFile"));

    if (strcmp(parametersScript,"") == 0) {	
      // workflowDriverFile << moveCommand << mainScript << " tmpSimCenter.script \n";
      workflowDriverFile << dpreproCommand << "  params.in tmpSimCenter.script " << mainScript << "\n";
    } else {
      // workflowDriverFile << moveCommand << parametersScript << " tmpSimCenter.params \n";
      workflowDriverFile << dpreproCommand << "  params.in  tmpSimCenter.params " << " " << parametersScript << "\n";
    }

    workflowDriverFile << pythonCommand << " " << mainScript;

    if (strcmp(postprocessScript,"") != 0) {
      workflowDriverFile << "\n" << pythonCommand << " " << postprocessScript;
      for(std::vector<std::string>::iterator itEDP = edpList.begin(); itEDP != edpList.end(); ++itEDP) {
	workflowDriverFile << " " << *itEDP;
      }
    } else {
      for(std::vector<std::string>::iterator itEDP = edpList.begin(); itEDP != edpList.end(); ++itEDP) {
	workflowDriverFile << " " << *itEDP;
      }
    }
  }

  workflowDriverFile.close();

  //
  // done
  //

  exit(errorWrite);
}

