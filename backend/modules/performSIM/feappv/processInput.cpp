#include <string>
#include <fstream>
#include <jansson.h> 
#include <string>
#include <vector>


//
// function to return name of RVs in the file
//   returns number of variable names and rvList (a vector of the RV names)

int
parseForRV(json_t *rootRV, std::vector<std::string> &rvList) {

  int numRVs = json_array_size(rootRV);

  for (int i=0; i<numRVs; i++) {
    json_t *randomVariable = json_array_get(rootRV,i);
    const char *theRV = json_string_value(json_object_get(randomVariable,"name"));
    std::string newRV(theRV);
    rvList.push_back(newRV);
  }
  return numRVs;
}

int
getEDP(json_t *rootEDP, std::vector<std::string> &edpList) {

  int numResponses = json_array_size(rootEDP);
  
  for (int j=0; j<numResponses; j++) {
    json_t *theEDP_Item = json_array_get(rootEDP,j);
    const char *theEDP = json_string_value(json_object_get(theEDP_Item,"name"));
    std::string newEDP(theEDP);
    edpList.push_back(newEDP);
  }

  return numResponses;
}
