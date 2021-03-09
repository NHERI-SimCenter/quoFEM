#include <iostream>
#include <fstream>
#include <jansson.h> 
#include <string.h>
#include <string>
#include <sstream>
#include <list>
#include <vector>


struct normalRV {
  std::string name;
  double mean;
  double stdDev;
};

struct lognormalRV {
  std::string name;
  double mean;
  double stdDev;
};

struct constantRV {
  std::string name;
  double value;
};

struct uniformRV {
  std::string name;
  double lowerBound;
  double upperBound;
};

struct continuousDesignRV {
  std::string name;
  double lowerBound;
  double upperBound;
  double initialPoint;
};

struct weibullRV {
  std::string name;
  double scaleParam;
  double shapeParam;
};

struct gammaRV {
  std::string name;
  double alphas;
  double betas;
};

struct gumbellRV {
  std::string name;
  double alphas;
  double betas;
};

struct betaRV {
  std::string name;
  double alphas;
  double betas;
  double lowerBound;
  double upperBound;
};

struct discreteDesignSetRV {
  std::string name;
  std::list<std::string> elements;
};
// Additional, Aug 31, 2020

struct exponentialRV {
  std::string name;
  double lambda;
};

struct discreteRV {
  std::string name;
  double values;
  double weights;
};

struct chisquaredRV {
  std::string name;
  double k;
};

struct truncatedExponentialRV {
  std::string name;
  double lambda;
  double lowerBound;
  double upperBound;
};

struct randomVariables {
  int numRandomVariables;
  std::list<struct normalRV> normalRVs;
  std::list<struct lognormalRV> lognormalRVs;
  std::list<struct constantRV> constantRVs;
  std::list<struct uniformRV> uniformRVs;
  std::list<struct continuousDesignRV> continuousDesignRVs;
  std::list<struct weibullRV> weibullRVs;
  std::list<struct gammaRV> gammaRVs;
  std::list<struct gumbellRV> gumbellRVs;
  std::list<struct betaRV> betaRVs;
  std::list<struct discreteDesignSetRV> discreteDesignSetRVs;

  std::list<struct exponentialRV> exponentialRVs;
  std::list<struct discreteRV> discreteRVs;
  std::list<struct chisquaredRV> chisquaredRVs;
  std::list<struct truncatedExponentialRV> truncatedExponentialRVs;
};
  

// parses JSON for random variables & returns number found
  
int
parseForRV(json_t *root, struct randomVariables &theRandomVariables){ 

  int numberRVs = 0;

  json_t *fileRandomVariables =  json_object_get(root, "randomVariables");
  if (fileRandomVariables == NULL) {
    return 0; // no random variables is allowed
  }
  
  int numRVs = int(json_array_size(fileRandomVariables));
  for (int i=0; i<numRVs; i++) {
    json_t *fileRandomVariable = json_array_get(fileRandomVariables,i);
    const char *variableType = json_string_value(json_object_get(fileRandomVariable,"distribution"));
    
    if ((strcmp(variableType, "Normal") == 0) || (strcmp(variableType, "normal")==0)) {
      
      struct normalRV theRV;
      
      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      theRV.mean = json_number_value(json_object_get(fileRandomVariable,"mean"));
      theRV.stdDev = json_number_value(json_object_get(fileRandomVariable,"stdDev"));
      
      theRandomVariables.normalRVs.push_back(theRV);
      theRandomVariables.numRandomVariables += 1;
      numberRVs++;

    }

    else if ((strcmp(variableType, "Lognormal") == 0) || (strcmp(variableType, "lognormal") == 0)) {

      struct lognormalRV theRV;

      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      theRV.mean = json_number_value(json_object_get(fileRandomVariable,"mean"));
      theRV.stdDev = json_number_value(json_object_get(fileRandomVariable,"stdDev"));

      theRandomVariables.lognormalRVs.push_back(theRV);
      theRandomVariables.numRandomVariables += 1;
      numberRVs++;

    }

    else if (strcmp(variableType, "Constant") == 0) {

      struct constantRV theRV;

      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      theRV.value = json_number_value(json_object_get(fileRandomVariable,"value"));

      theRandomVariables.constantRVs.push_back(theRV);
      theRandomVariables.numRandomVariables += 1;
      numberRVs++;

    }

    else if (strcmp(variableType, "Uniform") == 0) {

      struct uniformRV theRV;

      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      theRV.lowerBound = json_number_value(json_object_get(fileRandomVariable,"lowerbound"));
      theRV.upperBound = json_number_value(json_object_get(fileRandomVariable,"upperbound"));

      theRandomVariables.uniformRVs.push_back(theRV);
      theRandomVariables.numRandomVariables += 1;
      numberRVs++;

    }

    else if (strcmp(variableType, "ContinuousDesign") == 0) {
      struct continuousDesignRV theRV;

      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      theRV.lowerBound = json_number_value(json_object_get(fileRandomVariable,"lowerbound"));
      theRV.upperBound = json_number_value(json_object_get(fileRandomVariable,"upperbound"));
      theRV.initialPoint = json_number_value(json_object_get(fileRandomVariable,"initialpoint"));

      theRandomVariables.continuousDesignRVs.push_back(theRV);
      theRandomVariables.numRandomVariables += 1;
      numberRVs++;
    }

    else if (strcmp(variableType, "Weibull") == 0) {

      struct weibullRV theRV;

      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      theRV.shapeParam = json_number_value(json_object_get(fileRandomVariable,"shapeparam"));
      theRV.scaleParam = json_number_value(json_object_get(fileRandomVariable,"scaleparam"));

      theRandomVariables.weibullRVs.push_back(theRV);
      theRandomVariables.numRandomVariables += 1;
      numberRVs++;
    }

    else if (strcmp(variableType, "Gamma") == 0) {

      struct gammaRV theRV;

      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      theRV.alphas = json_number_value(json_object_get(fileRandomVariable,"alphas"));
      theRV.betas = json_number_value(json_object_get(fileRandomVariable,"betas"));

      theRandomVariables.gammaRVs.push_back(theRV);
      theRandomVariables.numRandomVariables += 1;
      numberRVs++;
    }

    else if (strcmp(variableType, "Gumbel") == 0) {

      struct gumbellRV theRV;

      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      theRV.alphas = json_number_value(json_object_get(fileRandomVariable,"alphaparam"));
      theRV.betas = json_number_value(json_object_get(fileRandomVariable,"betaparam"));

      theRandomVariables.gumbellRVs.push_back(theRV);
      theRandomVariables.numRandomVariables += 1;
      numberRVs++;
    }


    else if (strcmp(variableType, "Beta") == 0) {

      struct betaRV theRV;

      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      theRV.alphas = json_number_value(json_object_get(fileRandomVariable,"alphas"));
      theRV.betas = json_number_value(json_object_get(fileRandomVariable,"betas"));
      theRV.lowerBound = json_number_value(json_object_get(fileRandomVariable,"lowerbound"));
      theRV.upperBound = json_number_value(json_object_get(fileRandomVariable,"upperbound"));
      //std::cerr << theRV.name << " " << theRV.upperBound << " " << theRV.lowerBound << " " << theRV.alphas << " " << theRV.betas;
      theRandomVariables.betaRVs.push_back(theRV);
      theRandomVariables.numRandomVariables += 1;
      numberRVs++;
    }

    else if (strcmp(variableType, "discrete_design_set_string") == 0) {

      struct discreteDesignSetRV theRV;

      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      std::list<std::string> theValues;
      json_t *elementsSet =  json_object_get(fileRandomVariable, "elements");
      if (elementsSet != NULL) {
      	int numValues = int(json_array_size(elementsSet));
      	for (int j=0; j<numValues; j++) {
      	  json_t *element = json_array_get(elementsSet,j);
      	  std::string value = json_string_value(element);
      	    theValues.push_back(value);
      	}

	theRV.elements = theValues;

	theRandomVariables.discreteDesignSetRVs.push_back(theRV);
	theRandomVariables.numRandomVariables += 1;
	numberRVs++;
      }
    }

  else if (strcmp(variableType, "Exponential") == 0) {

      struct exponentialRV theRV;

      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      theRV.lambda = json_number_value(json_object_get(fileRandomVariable,"lambda"));

      theRandomVariables.exponentialRVs.push_back(theRV);
      theRandomVariables.numRandomVariables += 1;
      numberRVs++;  
  }

  else if (strcmp(variableType, "Discrete") == 0) {

      struct discreteRV theRV;

      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      theRV.weights = json_number_value(json_object_get(fileRandomVariable,"Weights"));
      theRV.values = json_number_value(json_object_get(fileRandomVariable,"Values"));

      theRandomVariables.discreteRVs.push_back(theRV);
      theRandomVariables.numRandomVariables += 1;
      numberRVs++;  
  }

  else if (strcmp(variableType, "Chisquare") == 0) {

      struct chisquaredRV theRV;

      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      theRV.k = json_number_value(json_object_get(fileRandomVariable,"k"));
      

      theRandomVariables.chisquaredRVs.push_back(theRV);
      theRandomVariables.numRandomVariables += 1;
      numberRVs++;  
  }

  else if (strcmp(variableType, "Truncated exponential") == 0) {

      struct truncatedExponentialRV theRV;

      theRV.name = json_string_value(json_object_get(fileRandomVariable,"name"));
      theRV.lambda = json_number_value(json_object_get(fileRandomVariable,"lambda"));
      theRV.lowerBound = json_number_value(json_object_get(fileRandomVariable,"a"));
      theRV.upperBound = json_number_value(json_object_get(fileRandomVariable,"b"));     
    
      theRandomVariables.truncatedExponentialRVs.push_back(theRV);
      theRandomVariables.numRandomVariables += 1;
      numberRVs++;  
  }    
    

  } // end loop over random variables

  return numRVs;
}


int
writeRV(std::ostream &SimCenterUQFile, struct randomVariables &theRandomVariables, std::string idVariables, std::vector<std::string> &rvList, bool includeActiveText = true){ 


    int numContinuousDesign = int(theRandomVariables.continuousDesignRVs.size());

    if (numContinuousDesign != 0) {

      if (idVariables.empty())
	       SimCenterUQFile << "variables \n ";
      else
	       SimCenterUQFile << "variables \n id_variables =  '" << idVariables << "'\n";    

      if (numContinuousDesign > 0) {
	       SimCenterUQFile << "  continuous_design = " << numContinuousDesign << "\n    initial_point = ";
	       for (auto it = theRandomVariables.continuousDesignRVs.begin(); it != theRandomVariables.continuousDesignRVs.end(); it++)
	         SimCenterUQFile << it->initialPoint << " ";
	       SimCenterUQFile << "\n    lower_bounds = ";
	       for (auto it = theRandomVariables.continuousDesignRVs.begin(); it != theRandomVariables.continuousDesignRVs.end(); it++)
	         SimCenterUQFile << it->lowerBound << " ";
	       SimCenterUQFile << "\n    upper_bounds = ";
	       for (auto it = theRandomVariables.continuousDesignRVs.begin(); it != theRandomVariables.continuousDesignRVs.end(); it++)
	         SimCenterUQFile << it->upperBound << " ";
	       SimCenterUQFile << "\n    descriptors = ";
      	 for (auto it = theRandomVariables.continuousDesignRVs.begin(); it != theRandomVariables.continuousDesignRVs.end(); it++) {
      	   SimCenterUQFile << "\'" << it->name << "\' ";
      	   rvList.push_back(it->name);
      	 }
	       SimCenterUQFile << "\n\n";
      }
      return 0;
    }

    if (includeActiveText == true) {
      if (idVariables.empty())
	SimCenterUQFile << "variables \n active uncertain \n";
      else
	SimCenterUQFile << "variables \n id_variables =  '" << idVariables << "'\n active uncertain \n";        
    } else {
	SimCenterUQFile << "variables \n";
    }

      int numNormalUncertain = int(theRandomVariables.normalRVs.size());

    int numNormal = int(theRandomVariables.normalRVs.size());
    if (int(theRandomVariables.normalRVs.size()) > 0) {
      SimCenterUQFile << "  normal_uncertain = " << numNormal << "\n    means = ";
      // std::list<struct normalRV>::iterator it;
      for (auto it = theRandomVariables.normalRVs.begin(); it != theRandomVariables.normalRVs.end(); it++)
	SimCenterUQFile << it->mean << " ";
      SimCenterUQFile << "\n    std_deviations = ";
      for (auto it = theRandomVariables.normalRVs.begin(); it != theRandomVariables.normalRVs.end(); it++)
	SimCenterUQFile << it->stdDev << " ";
      SimCenterUQFile << "\n    descriptors = ";
      for (auto it = theRandomVariables.normalRVs.begin(); it != theRandomVariables.normalRVs.end(); it++) {
	SimCenterUQFile << "\'" << it->name << "\' ";
	rvList.push_back(it->name);
      }
      
      SimCenterUQFile << "\n";
    }

    int numLognormal = int(theRandomVariables.lognormalRVs.size());
    if (numLognormal > 0) {
      SimCenterUQFile << "  lognormal_uncertain = " << numLognormal << "\n    means = ";
      //      std::list<struct lognormalRV>::iterator it;
      for (auto it = theRandomVariables.lognormalRVs.begin(); it != theRandomVariables.lognormalRVs.end(); it++)
	SimCenterUQFile << it->mean << " ";
      SimCenterUQFile << "\n    std_deviations = ";
      for (auto it = theRandomVariables.lognormalRVs.begin(); it != theRandomVariables.lognormalRVs.end(); it++)
	SimCenterUQFile << it->stdDev << " ";
      SimCenterUQFile << "\n    descriptors = ";
      for (auto it = theRandomVariables.lognormalRVs.begin(); it != theRandomVariables.lognormalRVs.end(); it++) {
	SimCenterUQFile << "\'" << it->name << "\' ";
	rvList.push_back(it->name);
      }
      SimCenterUQFile << "\n";
    }

    int numUniform = int(theRandomVariables.uniformRVs.size());
    if (numUniform > 0) {
      SimCenterUQFile << "  uniform_uncertain = " << numUniform << "\n    lower_bounds = ";
      // std::list<struct uniformRV>::iterator it;
      for (auto it = theRandomVariables.uniformRVs.begin(); it != theRandomVariables.uniformRVs.end(); it++)
	SimCenterUQFile << it->lowerBound << " ";
      SimCenterUQFile << "\n    upper_bound = ";
      for (auto it = theRandomVariables.uniformRVs.begin(); it != theRandomVariables.uniformRVs.end(); it++)
	SimCenterUQFile << it->upperBound << " ";
      SimCenterUQFile << "\n    descriptors = ";
      for (auto it = theRandomVariables.uniformRVs.begin(); it != theRandomVariables.uniformRVs.end(); it++) {
	SimCenterUQFile << "\'" << it->name << "\' ";
	rvList.push_back(it->name);
      }
      SimCenterUQFile << "\n";
    }


    int numWeibull = int(theRandomVariables.weibullRVs.size());
    if (numWeibull > 0) {
      SimCenterUQFile << "  weibull_uncertain = " << numWeibull << "\n    alphas = ";
      // std::list<struct weibullRV>::iterator it;
      for (auto it = theRandomVariables.weibullRVs.begin(); it != theRandomVariables.weibullRVs.end(); it++)
	SimCenterUQFile << it->shapeParam << " ";
      SimCenterUQFile << "\n    betas = ";
      for (auto it = theRandomVariables.weibullRVs.begin(); it != theRandomVariables.weibullRVs.end(); it++)
	SimCenterUQFile << it->scaleParam << " ";
      SimCenterUQFile << "\n    descriptors = ";
      for (auto it = theRandomVariables.weibullRVs.begin(); it != theRandomVariables.weibullRVs.end(); it++) {
	SimCenterUQFile << "\'" << it->name << "\' ";
	rvList.push_back(it->name);
      }
      SimCenterUQFile << "\n";
    }

    int numGumbell = int(theRandomVariables.gumbellRVs.size());
    if (numGumbell > 0) {
      SimCenterUQFile << "  gumbel_uncertain = " << numGumbell << "\n    alphas = ";
      // std::list<struct gumbellRV>::iterator it;
      for (auto it = theRandomVariables.gumbellRVs.begin(); it != theRandomVariables.gumbellRVs.end(); it++)
	SimCenterUQFile << it->alphas << " ";
      SimCenterUQFile << "\n    betas = ";
      for (auto it = theRandomVariables.gumbellRVs.begin(); it != theRandomVariables.gumbellRVs.end(); it++)
	SimCenterUQFile << it->betas << " ";
      SimCenterUQFile << "\n    descriptors = ";
      for (auto it = theRandomVariables.gumbellRVs.begin(); it != theRandomVariables.gumbellRVs.end(); it++) {
	SimCenterUQFile << "\'" << it->name << "\' ";
	rvList.push_back(it->name);
      }
      SimCenterUQFile << "\n";
    }


    int numGamma = int(theRandomVariables.gammaRVs.size());
    if (numGamma > 0) {
      SimCenterUQFile << "  gamma_uncertain = " << numGamma << "\n    alphas = ";
      std::list<struct gammaRV>::iterator it;
      for (auto it = theRandomVariables.gammaRVs.begin(); it != theRandomVariables.gammaRVs.end(); it++)
	SimCenterUQFile << it->alphas << " ";
      SimCenterUQFile << "\n    betas = ";
      for (auto it = theRandomVariables.gammaRVs.begin(); it != theRandomVariables.gammaRVs.end(); it++)
	SimCenterUQFile << it->betas << " ";
      SimCenterUQFile << "\n    descriptors = ";
      for (auto it = theRandomVariables.gammaRVs.begin(); it != theRandomVariables.gammaRVs.end(); it++) {
	SimCenterUQFile << "\'" << it->name << "\' ";
	rvList.push_back(it->name);
      }
      SimCenterUQFile << "\n";
    }

    int numBeta = int(theRandomVariables.betaRVs.size());
    if (numBeta > 0) {
      SimCenterUQFile << "  beta_uncertain = " << numBeta << "\n    alphas = ";
      //std::list<struct betaRV>::iterator it;
      for (auto it = theRandomVariables.betaRVs.begin(); it != theRandomVariables.betaRVs.end(); it++)
	SimCenterUQFile << it->alphas << " ";
      SimCenterUQFile << "\n    betas = ";
      for (auto it = theRandomVariables.betaRVs.begin(); it != theRandomVariables.betaRVs.end(); it++)
	SimCenterUQFile << it->betas << " ";
      SimCenterUQFile << "\n    lower_bounds = ";
      for (auto it = theRandomVariables.betaRVs.begin(); it != theRandomVariables.betaRVs.end(); it++)
	SimCenterUQFile << it->lowerBound << " ";
      SimCenterUQFile << "\n    upper_bounds = ";
      for (auto it = theRandomVariables.betaRVs.begin(); it != theRandomVariables.betaRVs.end(); it++)
	SimCenterUQFile << it->upperBound << " ";
      SimCenterUQFile << "\n    descriptors = ";
      for (auto it = theRandomVariables.betaRVs.begin(); it != theRandomVariables.betaRVs.end(); it++) {
	SimCenterUQFile << "\'" << it->name << "\' ";
	rvList.push_back(it->name);
      }
      SimCenterUQFile << "\n";
    }

    int numConstant = theRandomVariables.constantRVs.size();
    if (numConstant > 0) {
      SimCenterUQFile << "  discrete_state_set  \n    real = " << numConstant;
      SimCenterUQFile << "\n    elements_per_variable = ";
      for (auto it = theRandomVariables.constantRVs.begin(); it != theRandomVariables.constantRVs.end(); it++)
        SimCenterUQFile << "1 ";     //std::list<struct betaRV>::iterator it;
      SimCenterUQFile << "\n    elements = ";
      for (auto it = theRandomVariables.constantRVs.begin(); it != theRandomVariables.constantRVs.end(); it++)
        SimCenterUQFile << it->value << " ";
      SimCenterUQFile << "\n    descriptors = ";      
      for (auto it = theRandomVariables.constantRVs.begin(); it != theRandomVariables.constantRVs.end(); it++) {
        SimCenterUQFile << "\'" << it->name << "\' ";
        rvList.push_back(it->name);
      }
      SimCenterUQFile << "\n";
    }
            
    int numDiscreteDesignSet = int(theRandomVariables.discreteDesignSetRVs.size());
    if (numDiscreteDesignSet > 0) {
      SimCenterUQFile << "    discrete_uncertain_set\n    string " << numDiscreteDesignSet << "\n    num_set_values = ";
      std::list<struct discreteDesignSetRV>::iterator it;
      for (it = theRandomVariables.discreteDesignSetRVs.begin(); it != theRandomVariables.discreteDesignSetRVs.end(); it++)
	SimCenterUQFile << it->elements.size() << " ";
      SimCenterUQFile << "\n    set_values ";
      for (it = theRandomVariables.discreteDesignSetRVs.begin(); it != theRandomVariables.discreteDesignSetRVs.end(); it++) {
	it->elements.sort(); // sort the elements NEEDED THOUGH NOT IN SimCenterUQ DOC!
	std::list<std::string>::iterator element;
	for (element = it->elements.begin(); element != it->elements.end(); element++) 
	  SimCenterUQFile << " \'" << *element << "\'";
      }
      SimCenterUQFile << "\n    descriptors = ";
      for (auto it = theRandomVariables.discreteDesignSetRVs.begin(); it != theRandomVariables.discreteDesignSetRVs.end(); it++) {
	SimCenterUQFile << "\'" << it->name << "\' ";
	rvList.push_back(it->name);
      }
      SimCenterUQFile << "\n";
    }

    int numExponential = int(theRandomVariables.exponentialRVs.size());
    if (numExponential > 0) {
      for (auto it = theRandomVariables.exponentialRVs.begin(); it != theRandomVariables.exponentialRVs.end(); it++) {
        rvList.push_back(it->name);
      }
    }

    int numDiscrete = int(theRandomVariables.discreteRVs.size());
    if (numDiscrete > 0) {
      for (auto it = theRandomVariables.discreteRVs.begin(); it != theRandomVariables.discreteRVs.end(); it++) {
        rvList.push_back(it->name);
      }
    }

    int numChisquared = int(theRandomVariables.chisquaredRVs.size());
    if (numChisquared > 0) {
      for (auto it = theRandomVariables.chisquaredRVs.begin(); it != theRandomVariables.chisquaredRVs.end(); it++) {
        rvList.push_back(it->name);
      }
    }

    int numTruncatedExponential = int(theRandomVariables.truncatedExponentialRVs.size());
    if (numTruncatedExponential > 0) {
      for (auto it = theRandomVariables.truncatedExponentialRVs.begin(); it != theRandomVariables.truncatedExponentialRVs.end(); it++) {
        rvList.push_back(it->name);
      }
    }

    // if no random variables .. create 1 call & call it dummy!
    int numRV = theRandomVariables.numRandomVariables;
    if (numRV == 0) {
      SimCenterUQFile << "   discrete_uncertain_set\n    string 1 \n    num_set_values = 2";      
      SimCenterUQFile << "\n    set_values  '1' '2'";
      SimCenterUQFile << "\n    descriptors = dummy\n";
      rvList.push_back(std::string("dummy"));
    }

    SimCenterUQFile << "\n\n";

    return 0;
}

int
writeInterface(std::ostream &SimCenterUQFile, json_t *uqData, std::string &workflowDriver, std::string idInterface, int evalConcurrency) {

  SimCenterUQFile << "interface \n";
  if (!idInterface.empty())
    SimCenterUQFile << "  id_interface = '" << idInterface << "'\n";

  SimCenterUQFile << "  analysis_driver = '" << workflowDriver << "'\n";

  SimCenterUQFile << "  fork\n";  

  SimCenterUQFile << "   parameters_file = 'params.in'\n";
  SimCenterUQFile << "   results_file = 'results.out' \n";
  SimCenterUQFile << "   aprepro \n";
  SimCenterUQFile << "   work_directory\n";
  SimCenterUQFile << "     named \'workdir\' \n";
  SimCenterUQFile << "     directory_tag\n";
  SimCenterUQFile << "     directory_save\n";

  /*
    if uqData['keepSamples']:
        SimCenterUQ_input += ('        directory_save\n')    
  */

  SimCenterUQFile << "     copy_files = 'templatedir/*' \n";
  if (evalConcurrency > 0)
    SimCenterUQFile << "  asynchronous evaluation_concurrency = " << evalConcurrency << "\n\n";
  else
    SimCenterUQFile << "  asynchronous \n\n";

  /*
  if (runType == "local") {
    uqData['concurrency'] = uqData.get('concurrency', 4)
  }    
  if uqData['concurrency'] == None:
     SimCenterUQ_input += "  asynchronous\n"
  elif uqData['concurrency'] > 1:
     SimCenterUQ_input += "  asynchronous evaluation_concurrency = {}\n".format(uqData['concurrency'])
  }
  */

  return 0;
}

int
writeResponse(std::ostream &SimCenterUQFile, json_t *rootEDP,  std::string idResponse, bool numericalGradients, bool numericalHessians,
	      std::vector<std::string> &edpList) {

  int numResponses = 0;

  SimCenterUQFile << "responses\n";

  if (!idResponse.empty() && (idResponse.compare("calibration") != 0))
    SimCenterUQFile << "  id_responses = '" << idResponse << "'\n";
    
  //
  // look in file for EngineeringDemandParameters 
  // .. if there parse edp for each event
  //

  json_t *EDPs = json_object_get(rootEDP,"EngineeringDemandParameters");

  if (EDPs != NULL) {

    numResponses = int(json_integer_value(json_object_get(rootEDP,"total_number_edp")));
    SimCenterUQFile << " response_functions = " << numResponses << "\n response_descriptors = ";

    // for each event write the edps
    int numEvents = int(json_array_size(EDPs));
    
    // loop over all events
    for (int i=0; i<numEvents; i++) {
      
      json_t *event = json_array_get(EDPs,i);
      json_t *eventEDPs = json_object_get(event,"responses");
      int numResponses = int(json_array_size(eventEDPs));  
      
      // loop over all edp for the event
      for (int j=0; j<numResponses; j++) {
	
	json_t *eventEDP = json_array_get(eventEDPs,j);
	const char *eventType = json_string_value(json_object_get(eventEDP,"type"));
	bool known = false;
	std::string edpAcronym("");
	const char *floor = NULL;
	std::cerr << "writeResponse: type: " << eventType;
	// based on edp do something 
	if (strcmp(eventType,"max_abs_acceleration") == 0) {
	  edpAcronym = "PFA";
	  floor = json_string_value(json_object_get(eventEDP,"floor"));
	  known = true;
	} else if	(strcmp(eventType,"max_drift") == 0) {
	  edpAcronym = "PID";
	  floor = json_string_value(json_object_get(eventEDP,"floor2"));
	  known = true;
	} else if	(strcmp(eventType,"residual_disp") == 0) {
	  edpAcronym = "RD";
	  floor = json_string_value(json_object_get(eventEDP,"floor"));
	  known = true;
	} else if (strcmp(eventType,"max_pressure") == 0) {
	  edpAcronym = "PSP";
	  floor = json_string_value(json_object_get(eventEDP,"floor2"));
	  known = true;
	} else if (strcmp(eventType,"max_rel_disp") == 0) {
	  edpAcronym = "PFD";
	  floor = json_string_value(json_object_get(eventEDP,"floor"));
	  known = true;
	} else if (strcmp(eventType,"peak_wind_gust_speed") == 0) {
	  edpAcronym = "PWS";
	  floor = json_string_value(json_object_get(eventEDP,"floor"));
	  known = true;
	} else {
	  SimCenterUQFile << "'" << eventType << "' ";
	  std::string newEDP(eventType);
	  edpList.push_back(newEDP);
	}
	
	if (known == true) {
	  json_t *dofs = json_object_get(eventEDP,"dofs");
	  int numDOF = int(json_array_size(dofs));
	  
	  // loop over all edp for the event
	  for (int k=0; k<numDOF; k++) {
	    int dof = int(json_integer_value(json_array_get(dofs,k)));
	    SimCenterUQFile << "'" << i+1 << "-" << edpAcronym << "-" << floor << "-" << dof << "' ";
	    std::string newEDP = std::string(std::to_string(i+1)) + std::string("-")
	      + edpAcronym 
	      + std::string("-") 
	      + std::string(floor) +
	      std::string("-") + std::string(std::to_string(dof));
	    edpList.push_back(newEDP);
	  }
	}
      }
    }
  } else {

    //
    // quoFEM .. just a list of straight EDP
    //

    numResponses = int(json_array_size(rootEDP));

    if (idResponse.compare("calibration") != 0)
      SimCenterUQFile << " response_functions = " << numResponses << "\n response_descriptors = ";
    else
      SimCenterUQFile << " calibration_terms = " << numResponses << "\n response_descriptors = ";
    
    for (int j=0; j<numResponses; j++) {
      json_t *theEDP_Item = json_array_get(rootEDP,j);
      const char *theEDP = json_string_value(json_object_get(theEDP_Item,"name"));
      SimCenterUQFile << "'" << theEDP << "' ";
      std::string newEDP(theEDP);
      edpList.push_back(newEDP);
    }
  }

  if (numericalGradients == true) 
    SimCenterUQFile << "\n numerical_gradients";
  else
    SimCenterUQFile << "\n no_gradients";

  if (numericalHessians == true) 
    SimCenterUQFile << "\n numerical_hessians\n\n";
  else
    SimCenterUQFile << "\n no_hessians\n\n";

  return numResponses;
}


int
writeSimCenterUQInputFile(std::ostream &SimCenterUQFile, 
		     json_t *uqData, 
		     json_t *rootEDP, 
		     struct randomVariables &theRandomVariables, 
		     std::string &workflowDriver,
		     std::vector<std::string> &rvList,
		     std::vector<std::string> &edpList,
		     int evalConcurrency) { 

  const char *type = json_string_value(json_object_get(uqData, "uqType"));
  
  bool sensitivityAnalysis = false;
  if (strcmp(type, "Sensitivity Analysis") == 0)
    sensitivityAnalysis = true;

  json_t *EDPs = json_object_get(rootEDP,"EngineeringDemandParameters");
  int numResponses = 0;
  if (EDPs != NULL) {
    numResponses = int(json_integer_value(json_object_get(rootEDP,"total_number_edp")));
  } else {
    numResponses = int(json_array_size(rootEDP));
  }

  //
  // based on method do stuff
  // 

  if ((strcmp(type, "Forward Propagation") == 0) || sensitivityAnalysis == true) {

    json_t *samplingMethodData = json_object_get(uqData,"samplingMethodData");

    const char *method = json_string_value(json_object_get(samplingMethodData,"method"));

    if (strcmp(method,"Monte Carlo")==0) {
      int numSamples = int(json_integer_value(json_object_get(samplingMethodData,"samples")));
      int seed = int(json_integer_value(json_object_get(samplingMethodData,"seed")));

      SimCenterUQFile << "environment \n tabular_data \n tabular_data_file = 'SimCenterUQTab.out' \n\n";
      SimCenterUQFile << "method, \n sampling \n sample_type = random \n samples = " << numSamples << " \n seed = " << seed << "\n\n";

      if (sensitivityAnalysis == true)
	SimCenterUQFile << "variance_based_decomp \n\n";

      std::string emptyString;
      writeRV(SimCenterUQFile, theRandomVariables, emptyString, rvList, true);
      writeInterface(SimCenterUQFile, uqData, workflowDriver, emptyString, evalConcurrency);
      writeResponse(SimCenterUQFile, rootEDP, emptyString, false, false, edpList);
    }

    else if (strcmp(method,"LHS")==0) {

      int numSamples = int(json_integer_value(json_object_get(samplingMethodData,"samples")));
      int seed = int(json_integer_value(json_object_get(samplingMethodData,"seed")));

      std::cerr << numSamples << " " << seed;

      SimCenterUQFile << "environment \n tabular_data \n tabular_data_file = 'SimCenterUQTab.out' \n\n";
      SimCenterUQFile << "method,\n sampling\n sample_type = lhs \n samples = " << numSamples << " \n seed = " << seed << "\n\n";

      if (sensitivityAnalysis == true)
	SimCenterUQFile << "variance_based_decomp \n\n";

      std::string emptyString;
      writeRV(SimCenterUQFile, theRandomVariables, emptyString, rvList);
      writeInterface(SimCenterUQFile, uqData, workflowDriver, emptyString, evalConcurrency);
      writeResponse(SimCenterUQFile, rootEDP, emptyString, false, false, edpList);
    }

    else if (strcmp(method,"Importance Sampling")==0) {

      const char *isMethod = json_string_value(json_object_get(samplingMethodData,"ismethod"));
      int numSamples = int(json_integer_value(json_object_get(samplingMethodData,"samples")));
      int seed = int(json_integer_value(json_object_get(samplingMethodData,"seed")));

      SimCenterUQFile << "environment \n tabular_data \n tabular_data_file = 'SimCenterUQTab.out' \n\n";
      SimCenterUQFile << "method, \n importance_sampling \n " << isMethod << " \n samples = " << numSamples << "\n seed = " << seed << "\n\n";

      std::string emptyString;
      writeRV(SimCenterUQFile, theRandomVariables, emptyString, rvList);
      writeInterface(SimCenterUQFile, uqData, workflowDriver, emptyString, evalConcurrency);
      writeResponse(SimCenterUQFile, rootEDP, emptyString, false, false, edpList);
    }

    else if (strcmp(method,"Gaussian Process Regression")==0) {

      int trainingSamples = int(json_integer_value(json_object_get(samplingMethodData,"trainingSamples")));
      int trainingSeed = int(json_integer_value(json_object_get(samplingMethodData,"trainingSeed")));
      const char *trainMethod = json_string_value(json_object_get(samplingMethodData,"trainingMethod"));    
      int samplingSamples = int(json_integer_value(json_object_get(samplingMethodData,"samplingSamples")));
      int samplingSeed = int(json_integer_value(json_object_get(samplingMethodData,"samplingSeed")));
      const char *sampleMethod = json_string_value(json_object_get(samplingMethodData,"samplingMethod"));

      const char *surrogateMethod = json_string_value(json_object_get(samplingMethodData,"surrogateSurfaceMethod"));

      std::string trainingMethod(trainMethod);
      std::string samplingMethod(sampleMethod);
      if (strcmp(trainMethod,"Monte Carlo") == 0)
	trainingMethod = "random";
      if (strcmp(sampleMethod,"Monte Carlo") == 0) 
	samplingMethod = "random";


      SimCenterUQFile << "environment \n method_pointer = 'SurrogateMethod' \n tabular_data \n tabular_data_file = 'SimCenterUQTab.out'\n";
      SimCenterUQFile << "custom_annotated header eval_id \n\n";

      SimCenterUQFile << "method \n id_method = 'SurrogateMethod' \n model_pointer = 'SurrogateModel'\n";
      SimCenterUQFile << " sampling \n samples = " << samplingSamples << "\n seed = " << samplingSeed << "\n sample_type = "
		 << samplingMethod << "\n\n";

      SimCenterUQFile << "model \n id_model = 'SurrogateModel' \n surrogate global \n dace_method_pointer = 'TrainingMethod'\n "
		 << surrogateMethod << "\n\n";

      SimCenterUQFile << "method \n id_method = 'TrainingMethod' \n model_pointer = 'TrainingModel'\n";
      SimCenterUQFile << " sampling \n samples = " << trainingSamples << "\n seed = " << trainingSeed << "\n sample_type = "
		 << trainingMethod << "\n\n";

      SimCenterUQFile << "model \n id_model = 'TrainingModel' \n single \n interface_pointer = 'SimulationInterface'";

      std::string emptyString;
      std::string interfaceString("SimulationInterface");
      writeRV(SimCenterUQFile, theRandomVariables, emptyString, rvList);
      writeInterface(SimCenterUQFile, uqData, workflowDriver, interfaceString, evalConcurrency);
      writeResponse(SimCenterUQFile, rootEDP, emptyString, false, false, edpList);

    }

    else if (strcmp(method,"Polynomial Chaos Expansion")==0) {

      const char *dataMethod = json_string_value(json_object_get(samplingMethodData,"dataMethod"));    
      int intValue = int(json_integer_value(json_object_get(samplingMethodData,"level")));
      int samplingSeed = int(json_integer_value(json_object_get(samplingMethodData,"samplingSeed")));
      int samplingSamples = int(json_integer_value(json_object_get(samplingMethodData,"samplingSamples")));
      const char *sampleMethod = json_string_value(json_object_get(samplingMethodData,"samplingMethod"));

      std::string pceMethod;
      if (strcmp(dataMethod,"Quadrature") == 0)
	pceMethod = "quadrature_order = ";
      else if (strcmp(dataMethod,"Smolyak Sparse_Grid") == 0)
	pceMethod = "sparse_grid_level = ";
      else if (strcmp(dataMethod,"Stroud Curbature") == 0)
	pceMethod = "cubature_integrand = ";
      else if (strcmp(dataMethod,"Orthogonal Least_Interpolation") == 0)
	pceMethod = "orthogonal_least_squares collocation_points = ";
      else
	pceMethod = "quadrature_order = ";

      std::string samplingMethod(sampleMethod);
      if (strcmp(sampleMethod,"Monte Carlo") == 0) 
	samplingMethod = "random";

      SimCenterUQFile << "environment \n  tabular_data \n tabular_data_file = 'a.out'\n\n"; // a.out for trial data

      std::string emptyString;
      std::string interfaceString("SimulationInterface");
      writeRV(SimCenterUQFile, theRandomVariables, emptyString, rvList);
      writeInterface(SimCenterUQFile, uqData, workflowDriver, interfaceString, evalConcurrency);
      int numResponse = writeResponse(SimCenterUQFile, rootEDP, emptyString, false, false, edpList);

      SimCenterUQFile << "method \n polynomial_chaos \n " << pceMethod << intValue;
      SimCenterUQFile << "\n samples_on_emulator = " << samplingSamples << "\n seed = " << samplingSeed << "\n sample_type = "
		 << samplingMethod << "\n";
      SimCenterUQFile << " probability_levels = ";
      for (int i=0; i<numResponse; i++)
	SimCenterUQFile << " .1 .5 .9 ";
      SimCenterUQFile << "\n export_approx_points_file = 'SimCenterUQTab.out'\n\n"; // SimCenterUQTab.out for surrogate evaluations
    }

  } else if ((strcmp(type, "Reliability Analysis") == 0)) {

    json_t *reliabilityMethodData = json_object_get(uqData,"reliabilityMethodData");

    const char *method = json_string_value(json_object_get(reliabilityMethodData,"method"));

    if (strcmp(method,"Local Reliability")==0) {

      const char *localMethod = json_string_value(json_object_get(reliabilityMethodData,"localMethod"));    
      const char *mppMethod = json_string_value(json_object_get(reliabilityMethodData,"mpp_Method"));    
      const char *levelType = json_string_value(json_object_get(reliabilityMethodData,"levelType"));    
      const char *integrationMethod = json_string_value(json_object_get(reliabilityMethodData,"integrationMethod"));    

      std::string intMethod;
      if (strcmp(integrationMethod,"First Order") == 0)
	intMethod = "first_order";
      else
	intMethod = "second_order";

      SimCenterUQFile << "environment \n tabular_data \n tabular_data_file = 'SimCenterUQTab.out' \n\n";
      if (strcmp(localMethod,"Mean Value") == 0) {
	SimCenterUQFile << "method, \n local_reliability \n";	  
      } else {
	SimCenterUQFile << "method, \n local_reliability \n mpp_search " << mppMethod 
		   << " \n integration " << intMethod << " \n";
      }

      json_t *levels =  json_object_get(reliabilityMethodData, "probabilityLevel");
      if (levels == NULL) {
	return 0; 
      }

      int numLevels = int(json_array_size(levels));
      if (strcmp(levelType, "Probability Levels") == 0) 
	SimCenterUQFile << " \n num_probability_levels = ";
      else 
	SimCenterUQFile << " \n num_response_levels = ";

      for (int i=0; i<numResponses; i++) 
	SimCenterUQFile << numLevels << " ";

      if (strcmp(levelType, "Probability Levels") == 0) 	
	SimCenterUQFile << " \n probability_levels = " ;
      else
	SimCenterUQFile << " \n response_levels = " ;

      for (int j=0; j<numResponses; j++) {
	for (int i=0; i<numLevels; i++) {
	    json_t *responseLevel = json_array_get(levels,i);
	    double val = json_number_value(responseLevel);
	    SimCenterUQFile << val << " ";
	  }
	SimCenterUQFile << "\n\t";
      }
      SimCenterUQFile << "\n\n";

      std::string emptyString;
      writeRV(SimCenterUQFile, theRandomVariables, emptyString, rvList);
      writeInterface(SimCenterUQFile, uqData, workflowDriver, emptyString, evalConcurrency);
      writeResponse(SimCenterUQFile, rootEDP, emptyString, true, true, edpList);
    }

    else if (strcmp(method,"Global Reliability")==0) {

      const char *gp = json_string_value(json_object_get(reliabilityMethodData,"gpApproximation"));    
      std::string gpMethod;
      if (strcmp(gp,"x-space") == 0)
	gpMethod = "x_gaussian_process";
      else
	gpMethod = "u_gaussian_process";


      json_t *levels =  json_object_get(reliabilityMethodData, "responseLevel");
      if (levels == NULL) {
	return 0; 
      }
      int numLevels = int(json_array_size(levels));

      SimCenterUQFile << "environment \n tabular_data \n tabular_data_file = 'SimCenterUQTab.out' \n\n";
      SimCenterUQFile << "method, \n global_reliability " << gpMethod << " \n"; // seed " << seed;

      SimCenterUQFile << " \n num_response_levels = ";
      for (int i=0; i<numResponses; i++) 
	SimCenterUQFile << numLevels << " ";

      SimCenterUQFile << " \n response_levels = " ;
      for (int j=0; j<numResponses; j++) {
	for (int i=0; i<numLevels; i++) {
	  json_t *responseLevel = json_array_get(levels,i);
	  double val = json_number_value(responseLevel);
	  SimCenterUQFile << val << " ";
	}
	SimCenterUQFile << "\n\t";
      }
      SimCenterUQFile << "\n\n";

      std::string emptyString;
      writeRV(SimCenterUQFile, theRandomVariables, emptyString, rvList);
      writeInterface(SimCenterUQFile, uqData, workflowDriver, emptyString, evalConcurrency);
      writeResponse(SimCenterUQFile, rootEDP, emptyString, true, false, edpList);
    }
  
  } else if ((strcmp(type, "Parameters Estimation") == 0)) {

    json_t *methodData = json_object_get(uqData,"calibrationMethodData");

    const char *method = json_string_value(json_object_get(methodData,"method"));

    std::string methodString("nl2sol");
    if (strcmp(method,"OPT++GaussNewton")==0) 
      methodString = "optpp_g_newton";

    int maxIterations = int(json_integer_value(json_object_get(methodData,"maxIterations")));    
    double tol = json_number_value(json_object_get(methodData,"convergenceTol"));    
    const char *factors = json_string_value(json_object_get(methodData,"factors"));    
    
    SimCenterUQFile << "environment \n tabular_data \n tabular_data_file = 'SimCenterUQTab.out' \n\n";

    SimCenterUQFile << "method, \n " << methodString << "\n  convergence_tolerance = " << tol 
	       << " \n   max_iterations = " << maxIterations;

    if (strcmp(factors,"") != 0) 
      SimCenterUQFile << "\n  scaling\n";

    SimCenterUQFile << "\n\n";
      
    std::string calibrationString("calibration");
    std::string emptyString;
    writeRV(SimCenterUQFile, theRandomVariables, emptyString, rvList);
    writeInterface(SimCenterUQFile, uqData, workflowDriver, emptyString, evalConcurrency);
    writeResponse(SimCenterUQFile, rootEDP, calibrationString, true, false, edpList);

    if (strcmp(factors,"") != 0) {      
      SimCenterUQFile << "\n  primary_scale_types = \"value\" \n  primary_scales = ";
      std::string factorString(factors);
      std::stringstream factors_stream(factorString);
      std::string tmp;
      while (factors_stream >> tmp) {
	// maybe some checks, i.e. ,
	SimCenterUQFile << tmp << " ";
      }
      SimCenterUQFile << "\n";
    }

  } else if ((strcmp(type, "Inverse Problem") == 0)) {

    json_t *methodData = json_object_get(uqData,"bayesianCalibrationMethodData");

    const char *method = json_string_value(json_object_get(methodData,"method"));
    
    /*
    const char *emulator = json_string_value(json_object_get(methodData,"emulator"));
    std::string emulatorString("gaussian_process");
    if (strcmp(emulator,"Polynomial Chaos")==0) 
      emulatorString = "pce";
    else if (strcmp(emulator,"Multilevel Polynomial Chaos")==0) 
      emulatorString = "ml_pce";
    else if (strcmp(emulator,"Multifidelity Polynomial Chaos")==0) 
      emulatorString = "mf_pce";
    else if (strcmp(emulator,"Stochastic Collocation")==0) 
      emulatorString = "sc";
    */

    int chainSamples = int(json_integer_value(json_object_get(methodData,"chainSamples")));    
    int seed = int(json_integer_value(json_object_get(methodData,"seed")));    
    int burnInSamples = int(json_integer_value(json_object_get(methodData,"burnInSamples")));    
    int jumpStep = int(json_integer_value(json_object_get(methodData,"jumpStep")));    
    //    int maxIterations = json_integer_value(json_object_get(methodData,"maxIter"));    
    //    double tol = json_number_value(json_object_get(methodData,"tol"));    

    if (strcmp(method,"DREAM")==0) {

      int chains = int(json_integer_value(json_object_get(methodData,"chains")));    

      SimCenterUQFile << "environment \n tabular_data \n tabular_data_file = 'SimCenterUQTab.out' \n\n";
      SimCenterUQFile << "method \n bayes_calibration dream "
		 << "\n  chain_samples = " << chainSamples
		 << "\n  chains = " << chains
		 << "\n  jump_step = " << jumpStep
		 << "\n  burn_in_samples = " << burnInSamples << "\n\n";

    } else {

      const char *mcmc = json_string_value(json_object_get(methodData,"mcmcMethod"));
      std::string mcmcString("dram");
      if (strcmp(mcmc,"Delayed Rejection")==0)       
	mcmcString = "delayed_rejection";
      else if (strcmp(mcmc,"Adaptive Metropolis")==0)
	mcmcString = "adaptive_metropolis";
      else if (strcmp(mcmc,"Metropolis Hastings")==0)
	mcmcString = "metropolis_hastings";
      else if (strcmp(mcmc,"Multilevek")==0)
	mcmcString = "multilevel";

      SimCenterUQFile << "environment \n tabular_data \n tabular_data_file = 'SimCenterUQTab.out' \n\n";
      SimCenterUQFile << "method \n bayes_calibration queso\n  " << mcmc
		 << "\n  chain_samples = " << chainSamples
		 << "\n  burn_in_samples = " << burnInSamples << "\n\n";
    }

    std::string calibrationString("calibration");
    std::string emptyString;
    writeRV(SimCenterUQFile, theRandomVariables, emptyString, rvList, false);
    writeInterface(SimCenterUQFile, uqData, workflowDriver, emptyString, evalConcurrency);
    writeResponse(SimCenterUQFile, rootEDP, calibrationString, false, false, edpList);

  } else {
    std::cerr << "uqType: NOT KNOWN\n";
    return -1;
  }
  return 0;
}




