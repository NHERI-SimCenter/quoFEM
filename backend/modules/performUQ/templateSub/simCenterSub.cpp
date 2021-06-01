#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iterator>

using std::string;
using std::vector;

/* ***************** params.in example ******
2
mag  6.903245983350862e+00
test 1.90 
*********************************************/

int main(int argc, char **argv)
{
  //
  // open params, in & out files
  //

  std::ifstream params(argv[1]);
  std::ifstream in(argv[2]);
  std::ofstream out(argv[3]);

  if (!params.is_open()) {
    std::cerr << "ERROR: simCenterDprepro could not open: " << argv[1] << "\n";
    exit(-1);
  }
  if (!in.is_open()) {
    std::cerr << "ERROR: simCenterDprepro could not open: " << argv[2] << "\n";
    exit(-1);
  }
  if (!out.is_open()) {
    std::cerr << "ERROR: simCenterDprepro could not open: " << argv[3] << "\n";
    exit(-1);
  }

  //
  // vectors that will contain strings to search for & their replacement
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
      numRVs = std::stoi(tokens.at(0));
      // std::cerr << "numRV: " << numRVs << "\n";

    } else {

      // subsequent lines contain RV and value .. add to string vectors
      string rvName = "\"RV." + tokens.at(0) + "\"";  // add SimCenter delimiters begin="RV. & end="
      string rvValue = tokens.at(1);
      original.push_back(rvName);
      replace.push_back(rvValue);
      originalLength.push_back(rvName.length());
      replacementLength.push_back(rvValue.length());
      // std::cerr << rvName << " " << rvValue << "\n";
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


