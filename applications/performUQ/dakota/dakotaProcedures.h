
#define OVERSUBSCRIBE_CORE_MULTIPLIER 1

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
};

int parseForRV(json_t *root, 
	       struct randomVariables &theRandomVariables);

int writeRV(std::ostream &dakotaFile, 
	    struct randomVariables &theRandomVariables, 
	    std::string idVariables,
	    std::vector<std::string> &rvList);

int writeInterface(std::ostream &dakotaFile, 
		   json_t *uqData, 
		   std::string &workflowDriver, 
		   std::string idInterface,
		   int evalConcurrency);

int writeResponse(std::ostream &dakotaFile, 
		  json_t *rootEDP,  
		  std::string idResponse, 
		  bool numericalGradients, 
		  bool numericalHessians,
		  std::vector<std::string> &edpList);

int writeDakotaInputFile(std::ostream &dakotaFile, 
			 json_t *uqData, 
			 json_t *rootEDP, 
			 struct randomVariables &theRandomVariables, 
			 std::string &workflowDriver,
			 std::vector<std::string> &rvList,
			 std::vector<std::string> &edpList,
			 int evalConcurrency);
