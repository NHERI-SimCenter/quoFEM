#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iterator>
#include <stdlib.h>
#include <stdio.h>

using std::string;
using std::vector;

/* application to replicate in c code what is done in dprepro
   which is to take input file and replace strings with RV.name
   with values in a params file. Output into a new file
*/

/* ***************** params.in example ******
{ DAKOTA_VARS     =                      2 }
{ mag             =  6.903245983350862e+00 }
{ test             =  1.90                 }
{ DAKOTA_FNS      =                      5 }
{ ASV_1:1-PFA-0-1 =                      1 }
{ ASV_2:1-PFD-0-1 =                      1 }
{ ASV_3:1-PFA-1-1 =                      1 }
{ ASV_4:1-PFD-1-1 =                      1 }
{ ASV_5:1-PID-1-1 =                      1 }
{ DAKOTA_DER_VARS =                      1 }
{ DVV_1:mag       =                      1 }
{ DAKOTA_AN_COMPS =                      0 }
{ DAKOTA_EVAL_ID  =                      1 }
******************************************** */

int main(int argc, char **argv)
{

    if (argc < 4) {
    std::cerr << "insufficient args: simCenterSub params.in inputFile outputFile\n";
    exit(-1);
  }
  
  //
  // open params, in & out files
  //

  std::ifstream params(argv[1]);
  std::ifstream in(argv[2]);
  std::ofstream out(argv[3]);
  const char *fixedFormat = NULL;
  if (argc == 5)
    fixedFormat = argv[4];

  if (!params.is_open()) {
    std::cerr << "ERROR: simCenterDprepro could not open: " << argv[1] << "\n";
    exit(-1);
  }
  if (!in.is_open()) {
    std::cerr << "ERROR: simCenterDprepro could not open: " << argv[1] << "\n";
    exit(-1);
  }
  if (!out.is_open()) {
    std::cerr << "ERROR: simCenterDprepro could not open: " << argv[1] << "\n";
    exit(-1);
  }

  //
  // vectors that will contain strings to search for (original) & their replacement (replace)
  //

  vector<string> original; 
  vector<string> replace; 
  vector<int> originalLength; 
  vector<int> replacementLength; 

  //
  // from params file, 1) read # of RV and 2) then read RV names and values
  //   

  int lineCount = 0;
  int numRVs = 0;
  string line;
  while (getline(params, line)) {
    std::istringstream buf(line);
    std::istream_iterator<std::string> beg(buf), end;
    vector<std::string> tokens(beg, end); // done!
    if (lineCount == 0) {

      // first line contains #RV
      numRVs = std::stoi(tokens.at(3));

    } else {

      // subsequent lines contain RV and value .. add to string vectors
      string rvName = "\"RV." + tokens.at(1) + "\"";  // add SimCenter delimiters begin="RV. & end="
      string rvValue = tokens.at(3);

      if (fixedFormat == NULL) {
	replace.push_back(rvValue);
      } else {
	try {
	  double val = std::stod(rvValue);
	  char valA[45];
	  std::sprintf(valA,"%30.15f",val);
	  std::string valS(valA);
	  replace.push_back(valS);	
	} catch(...) {
	  replace.push_back(rvValue);
	}
      }

      original.push_back(rvName);
      originalLength.push_back(rvName.length());
      replacementLength.push_back(rvValue.length());
    }

    lineCount++;

    if (lineCount > numRVs)
      break; // don't need to do anything with additional giberish in the file
  }

  //
  // read input file line by line
  //  - search each line for RV names, if found replace with RV value, send to output
  //

  // for each input line
  while (getline(in, line)) {

    // for each RV
    for (int i = 0; i < original.size(); i++) {

      string &oldString = original.at(i);
      string &newString = replace.at(i);
      int oldSize = originalLength.at(i);
      int newSize = replacementLength.at(i);

      // search for RV in string till end of string
      while (true) {
	  size_t pos = line.find(oldString);

	  // if found .. replace
	  if (pos != string::npos) 

	    if( oldSize == newSize ) {

	      // if they're same size, use std::string::replace
	      line.replace( pos, oldSize, newString );
	    } else {

	      // if not same size, replace by erasing and inserting (costly)
	      line.erase(pos, oldSize );
	      line.insert(pos, newString );
	    }

	  // end of string .. break .. onto next RV
	  else 
	    break;
        }
    }

    // send line to output
    out << line << '\n';
  }

  //
  // close the files
  //

  params.close();
  in.close();
  out.close();

  //
  // exit
  //

  exit(0);
}


