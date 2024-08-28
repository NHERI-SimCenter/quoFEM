
echo In folder $PWD
   
# Clone the examples
git clone --branch master --depth 1 https://github.com/NHERI-SimCenter/quoFEM.git


echo In folder $PWD

# Add the current dir in the example file
sed -i "s|{Current_Dir}|$(pwd)|g" $PWD//Examples/qfem-001/src/input.json

# Create the working directoy
mkdir tmp.SimCenter

# Copy over the input data
cp -R $PWD/Examples/qfem-001/src/input_data .

# Run the example in the backend
python $PWD/SimCenterBackendApplications/applications/Workflow/qWHALE.py "runningLocal" $PWD/Examples/qfem-001/src/input.json $PWD/SimCenterBackendApplications/applications/Workflow/WorkflowApplications.json

