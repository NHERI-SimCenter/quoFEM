#!/bin/bash

echo In folder $PWD
   
# Clone the examples
git clone --branch master https://github.com/NHERI-SimCenter/quoFEM.git

# Create the working directoy


# Read JSON from file
json_file="$PWD/quoFEM/Examples/Examples.json"

# Install jq
sudo apt-get install -y jq

# Iterate over array elements
jq -c '.Examples[]' "$json_file" | while read -r example; do
  name=$(echo "$example" | jq -r '.name')
  description=$(echo "$example" | jq -r '.description')
  inputfile=$(echo "$example" | jq -r '.inputFile')
  inputfile="$PWD/quoFEM/Examples/$inputfile"
  srcDir="$(dirname $inputfile)"

  echo "Example Name: $name"
  echo "Example Description: $description"
  echo "Input File: $inputfile"
  echo "srcDir: $srcDir"
  
  rm -rf tmp.SimCenter
  mkdir tmp.SimCenter
  mkdir tmp.SimCenter/templatedir
  
  cp -a $srcDir/. $PWD/tmp.SimCenter/templatedir/

  echo "Adding json params to input file..."
  echo $(cat $inputfile | jq '. + { "runDir": "'"$PWD/tmp.SimCenter"'" }') > $inputfile
  echo $(cat $inputfile | jq '. + { "localAppDir": "'"$PWD/SimCenterBackendApplications"'" }') > $inputfile
  echo $(cat $inputfile | jq '. + { "remoteAppDir": "'"$PWD/SimCenterBackendApplications"'" }') > $inputfile
  echo $(cat $inputfile | jq '. + { "runType": "runningLocal" }') > $inputfile
  
  echo "Input file contents:"
  python3.9 -m json.tool $inputfile
    
  echo "Template dir contents"
  ls $PWD/tmp.SimCenter/templatedir
  

  # Run the example in the backend
  echo "Running python"
  echo $PWD
  ls
  echo "============================="
  ls ..
  python3.9 $PWD/SimCenterBackendApplications/applications/Workflow/qWHALE.py "runningLocal" $inputfile $PWD/SimCenterBackendApplications/applications/Workflow/WorkflowApplications.json
done

