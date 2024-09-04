#!/bin/bash

echo In folder $PWD
   
# Clone the examples
git clone --branch master --depth 1 https://github.com/NHERI-SimCenter/quoFEM.git

# Create the working directoy
mkdir tmp.SimCenter

sudo apt-get install jq

inputfile=$PWD/Examples/qfem-0001/src/input.json
echo $inputfile

cat $inputfile

# echo "$(<$PWD/Examples/qfem-0001/src/input.json)"  | jq '. + { "runDir": "'"$PWD"'" }' > $PWD/Examples/qfem-0001/src/input.json

echo "doing jq ================="
echo $(cat $PWD/Examples/qfem-0001/src/input.json | jq '. + { "runDir": "'"$PWD/tmp.SimCenter"'" }') > $PWD/Examples/qfem-0001/src/input.json
echo "did jq ==================="

# # Add the current dir in the example file
# sed -i "s|{Current_Dir}|$(pwd)|g" $PWD/Examples/qfem-0001/src/input.json

# # Add the current dir in the example file
# sed -i "s|{runDir}|$(pwd)|g" $PWD/Examples/qfem-0001/src/input.json

echo "catting the thing now ==================="
cat $PWD/Examples/qfem-0001/src/input.json 



# # Copy over the input data
# cp -R $PWD/Examples/qfem-0001/src/input_data .

# Run the example in the backend
python $PWD/SimCenterBackendApplications/applications/Workflow/qWHALE.py "runningLocal" $PWD/Examples/qfem-0001/src/input.json $PWD/SimCenterBackendApplications/applications/Workflow/WorkflowApplications.json

