
echo In folder $PWD
   
# Clone the examples
git clone --branch master --depth 1 https://github.com/NHERI-SimCenter/quoFEM.git


# Add the current dir in the example file
sed -i "s|{Current_Dir}|$(pwd)|g" $PWD/quoFEM/Examples/E1BasicHAZUS/input.json

# Create the working directoy
mkdir tmp.SimCenter

# Copy over the input data
cp -R $PWD/quoFEM/Examples/E1BasicHAZUS/input_data .

# Run the example in the backend
python $PWD/SimCenterBackendApplications/applications/Workflow/qWHALE.py "runningLocal" $PWD/quoFEM/Examples/qfem-001/srcr/input.json $PWD/SimCenterBackendApplications/applications/Workflow/WorkflowApplications.json

