#include <iostream>
#include <fstream>
#include <jansson.h> 
#include <string.h>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <thread>
#include <filesystem>
#include <algorithm>


int getEDP(json_t *edp, std::vector<std::string> &edpList);
int getRV(json_t *edp, std::vector<std::string> &rvList);

void eraseAllSubstring(std::string & mainStr, const std::string & toErase)
{
    size_t pos = std::string::npos;
    // Search for the substring in string in a loop untill nothing is found
    while ((pos  = mainStr.find(toErase) )!= std::string::npos)
    {
        // If found then erase it from string
        mainStr.erase(pos, toErase.length());
    }
}

int main(int argc, const char **argv) {

  if (argc < 4) {
    std::cerr << "createOpenSeesDriver:: expecting 3 inputs\n";
    exit(-1);
  }

  std::string thisProgram(argv[0]);
  std::string inputFile(argv[1]);
  std::string runType(argv[2]);
  std::string osType(argv[3]);
  
  eraseAllSubstring(thisProgram,"\"");
  eraseAllSubstring(runType,"\"");
  eraseAllSubstring(osType,"\"");

  std::cerr << "runType: " << runType << '\n';
  std::cerr << "osType: " << osType << '\n';
  
  if (!std::filesystem::exists(inputFile)) {
    std::cerr << "createOpenSeesDriver:: input file: " << inputFile << " does not exist\n";
    exit(801);
  }
  
  //current_path(currentPath);
  auto path = std::filesystem::current_path(); //getting path
  std::string fileName = std::filesystem::path(inputFile).filename();
  std::string fullPath = std::filesystem::path(inputFile).remove_filename();

  std::filesystem::current_path(fullPath); //getting path
  path = std::filesystem::current_path(); //getting path
  std::cerr << "PATH: " << path << '\n';
  
  //
  // open input file to get RV's and EDP names
  //

  std::vector<std::string> rvList;  
  std::vector<std::string> edpList;
  
  json_error_t error;
  json_t *rootInput = json_load_file(inputFile.c_str(), 0, &error);
  if (rootInput == NULL) {
    std::cerr << "createOpenSeesDriver:: input file " << inputFile << " is not valid JSON\n";
    exit(801); 
  } 

  json_t *rootRV =  json_object_get(rootInput, "randomVariables");
  if (rootRV == NULL) {
    std::cerr << "createOpenSeesDriver:: no randomVariables found\n";
    return 0; // no random variables is allowed
  }
  
  json_t *rootEDP =  json_object_get(rootInput, "EDP");
  if (rootEDP == NULL) {
    std::cerr << "createOpenSeesDriver:: no EDP found\n";    
    return 0; // no random variables is allowed
  }

  int numRV = getEDP(rootRV, rvList);  
  int numEDP = getEDP(rootEDP, edpList);
  
  //
  // open workflow_driver 
  //
  
  std::string workflowDriver = "workflow_driver"; 
  if ((osType.compare("Windows") == 0) && (runType.compare("runningLocal") == 0))
    workflowDriver = "workflow_driver.bat";
  
  std::ofstream workflowDriverFile(workflowDriver, std::ios::binary);
  
  if (!workflowDriverFile.is_open()) {
    std::cerr << "createOpenSeesDriver:: could not create workflow driver file: " << workflowDriver << "\n";
    exit(802); // no random variables is allowed
  }

  std::string dpreproCommand;
  std::string openSeesCommand;
  std::string pythonCommand;
  std::string feapCommand;
  std::string moveCommand;

  const char *localDir = json_string_value(json_object_get(rootInput,"localAppDir"));
  const char *remoteDir = json_string_value(json_object_get(rootInput,"remoteAppDir"));

  if (runType.compare("runningLocal") == 0) {

    dpreproCommand = std::string("\"") + localDir + std::string("/applications/performUQ/templateSub/simCenterSub\"");
    openSeesCommand = std::string("OpenSees");
    pythonCommand = std::string("\"") + json_string_value(json_object_get(rootInput,"python")) + std::string("\"");

  } else {

    dpreproCommand = remoteDir + std::string("/applications/performUQ/templateSub/simCenterSub");
    openSeesCommand = std::string("/home1/00477/tg457427/bin/OpenSees");
    pythonCommand = std::string("python");
  }



  json_t *fem =  json_object_get(rootInput, "fem");
  if (fem == NULL) {
    return 0; // no random variables is allowed
  }
  
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
  
  workflowDriverFile << dpreproCommand << " params.in SimCenterInput.RV SimCenterInput.tcl\n";
  workflowDriverFile << openSeesCommand << " SimCenterInput.tcl 1> ops.out 2>&1\n";


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

  workflowDriverFile.close();

  //
  // done
  //

  exit(0);
}

