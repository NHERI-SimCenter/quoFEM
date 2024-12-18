#!/bin/bash

echo In folder $PWD
   
# Clone the examples
git clone --depth 1 https://github.com/NHERI-SimCenter/quoFEM.git

# make sure all packages are installed
python3 -m pip install --upgrade pip
python3 -m pip install nheri-simcenter --upgrade
python3 -m pip install GPy==1.13.2

# Read JSON from file
json_file="$PWD/quoFEM/Examples/Examples.json"

# Install jq
sudo apt-get install -y jq
rm -rf cache
mkdir cache
# Iterate over array elements
jq -c '.Examples[]' "$json_file" | while read -r example; do

  name=$(echo "$example" | jq -r '.name')
  description=$(echo "$example" | jq -r '.description')
  inputfile=$(echo "$example" | jq -r '.inputFile')
  
  inputfile="$PWD/quoFEM/Examples/$inputfile"
  srcDir="$(dirname $inputfile)"
  examplenumber="$(dirname $srcDir)"
  examplenumber="$(basename $examplenumber)"

  echo "==========================================="
  echo "Example Number: $examplenumber"
  echo "Example Name: $name"
  echo "Example Description: $description"
  echo "Input File: $inputfile"
  echo "srcDir: $srcDir"
  echo "---------------------------"
  echo "Adding json params to input file..."
  echo $(cat $inputfile | jq '. + { "runDir": "'"$PWD/tmp.SimCenter"'" }') > $inputfile
  echo $(cat $inputfile | jq '. + { "localAppDir": "'"$PWD/SimCenterBackendApplications"'" }') > $inputfile
  echo $(cat $inputfile | jq '. + { "remoteAppDir": "'"$PWD/SimCenterBackendApplications"'" }') > $inputfile
  echo $(cat $inputfile | jq '. + { "runType": "runningLocal" }') > $inputfile
  
  echo "copying files"
  rm -rf tmp.SimCenter
  mkdir tmp.SimCenter
  mkdir tmp.SimCenter/templatedir
  
  cp -a $srcDir/. $PWD/tmp.SimCenter/templatedir/

  if [[ "$examplenumber" == "qfem-0027" ]]; then
    # do some stuff
    cp -a $srcDir/model1/. $PWD/tmp.SimCenter/templatedir/
    cp -a $srcDir/model2/. $PWD/tmp.SimCenter/templatedir/
  fi
  if [[ "$examplenumber" == "qfem-0028" ]]; then
    # do some stuff
    echo "CAUGHT EXAMPLE 28"
    echo "$srcDir"
    cp -a "$srcDir/CouponCyclicTestData_every_20th_point/." "$PWD/tmp.SimCenter/templatedir/"
  fi

  
  #echo "Input file contents:"
  #python3 -m json.tool $inputfile
    
  #echo "Template dir contents"
  #ls $PWD/tmp.SimCenter/templatedir
  

  # Run the example in the backend
  echo "Running python:"
  echo "==============="
  python3 $PWD/SimCenterBackendApplications/applications/Workflow/qWHALE.py "runningLocal" $inputfile $PWD/SimCenterBackendApplications/applications/Workflow/WorkflowApplications.json
  mkdir "cache/tmp.SimCenter.$examplenumber/"
  cp -r tmp.SimCenter/. "cache/tmp.SimCenter.$examplenumber/"
done

# Check with archives have a dakotaTab.out file
echo ""
echo ""
echo "======================================"
for dir in $PWD/cache/*; do
  if [ -f "$dir/dakotaTab.out" ]; then
    echo "$dir: PASS"
    # Do something if the file exists
  else
    echo "$dir: FAIL"
    # Do something else if the file does not exist
  fi
done