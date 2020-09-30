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
  std::string workflow1(argv[3]);
  const char *runType = argv[4];
  const char *osType = argv[5];
  
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

  std::ofstream dakotaFile("dakota.in", std::ios::binary);

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

  int errorWrite = writeDakotaInputFile(dakotaFile, uqData, rootEDP, theRandomVariables, workflow1, rvList, edpList, evalConcurrency);

  dakotaFile.close();

  //
  // open workflow_driver 
  //

  std::ofstream workflowDriverFile(workflow1, std::ios::binary);

  if (!workflowDriverFile.is_open()) {
    std::cerr << "parseFileForRV:: could not create workflow driver file: " << workflow1 << "\n";
    exit(802); // no random variables is allowed
  }

  workflowDriverFile << "python3 writeParam.py paramsDakota.in params.in\n";
  workflowDriverFile << "source " << workflow << "\n";

  workflowDriverFile.close();
  
  std::string moveCommand;
  
  std::ofstream writeParamsFile("writeParam.py", std::ios::binary);

  if (!writeParamsFile.is_open()) {
    std::cerr << "parseFileForRV:: could not create writeParam file\n";
    exit(802); // no random variables is allowed
  }  

  writeParamsFile << "import sys\n";
  writeParamsFile << "import os\n";
  writeParamsFile << "from subprocess import Popen, PIPE\n";
  writeParamsFile << "import subprocess\n\n";
  writeParamsFile << "def main():\n";
  writeParamsFile << "    paramsIn = sys.argv[1]\n";
  writeParamsFile << "    paramsOut = sys.argv[2]\n\n";
  writeParamsFile << "    if not os.path.isfile(paramsIn):\n";
  writeParamsFile << "        print('Input param file {} does not exist. Exiting...'.format(paramsIn))\n        sys.exit()\n\n";
  writeParamsFile << "    outFILE = open(paramsOut, 'w')\n\n";
  writeParamsFile << "    with open(paramsIn) as inFILE:\n\n";
  writeParamsFile << "        line = inFILE.readline()\n";
  writeParamsFile << "        splitLine = line.split()\n";
  writeParamsFile << "        numRV = int(splitLine[3])\n";
  writeParamsFile << "        print(numRV, file=outFILE)\n\n";
  writeParamsFile << "        for i in range(numRV):\n";
  writeParamsFile << "            line = inFILE.readline()\n";
  writeParamsFile << "            splitLine = line.split()\n";
  writeParamsFile << "            nameRV = splitLine[1]\n";
  writeParamsFile << "            valueRV = splitLine[3]\n";
  writeParamsFile << "            print('{} {}'.format(nameRV, valueRV), file=outFILE)\n\n";
  writeParamsFile << "    outFILE.close \n";
  writeParamsFile << "    inFILE.close\n\n";
  writeParamsFile << "if __name__ == '__main__':\n";
  writeParamsFile << "   main()\n";
				     
  writeParamsFile.close();

  //
  // done
  //

  exit(errorWrite);
}

