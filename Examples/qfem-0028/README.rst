.. _qfem-0028:

Bayesian calibration of hierarchical models
===========================================

+---------------+----------------------------------------------+
| Problem files | :github:`Github <Examples/qfem-0028/>`       |
+---------------+----------------------------------------------+

Outline
-------
In this example, ...
 
Files required
--------------
The exercise uses ...

.. warning::
   Do NOT place the files in your root, downloads, or desktop folder as when the application runs it will copy the contents on the directories and subdirectories containing these files multiple times. If you are like us, your root, Downloads or Documents folders contains a lot of files.

The following files are needed to evaluate Model 1. Evaluating Model 2 also requires a set of 5 files with the first two files in the following list replaced by the corresponding ones for Model 2.

1. :qfem-0028:`ShearBuilding_NL_1.tcl <src/model1/ShearBuilding_NL_1.tcl>` - This is a tcl script that evaluates the model and generates a file containing the time history of the element forces for element 1. The horizontal component of the element forces (the second column in the file) corresponds to the base shear experienced by this model of the structure. 

2. :qfem-0028:`create_model_1.tcl <src/model1/create_model_1.tcl>`- This file contains tcl and OpenSees commands that create the nodes, materials, and elements that make up the finite element model of the structure.

3. :qfem-0028:`run_analysis.tcl <src/model1/run_analysis.tcl>` - This file contains tcl and OpenSees commands to create the load, record the output, and perform a time history analysis of the structural response to the applied earthquake excitation.

4. :qfem-0028:`R331.dat <src/model1/R331.dat>` - This text file contains the time history of the earthquake acceleration applied to the base of the model.

5. :qfem-0028:`postprocess.tcl <src/model1/postprocess.tcl>` - This file contains tcl commands to read the time history of the base shear from the file created during the analysis, and to compute the maximum absolute value of the base shear experienced by the model.


UQ workflow
-----------

The steps involved are as follows:

1. Start the application and the **UQ** panel will be highlighted. In the **UQ Method** drop down menu, select the **Forward Propagation** option. In the **UQ Engine** dropdown menu select **UCSD-UQ** option. Enter the values in this panel as shown in the figure below. 


2. Next in the **FEM** panel , select


3. Select the **RV** tab from the input panel. This panel should be pre-populated with the names of the variables that were defined in the model scripts. If not, press the **Add** button to create a new field to define the input random variable. Enter the same variable name, as required in the model script. For this example, choose the Normal probability distribution for all the random variabels and enter the parameter values for each distribution as shown in the figure below:



4. In the **EDP** panel create the output quantities corresponding to each experiment with a descriptive name.



5. Click on the **Run** button. This will create the necessary input files to perform a Bayesian calibration of the hierarchical model, run the analysis, and display the results when the analysis is completed.


* Summary of results obtained 

* Maximum base shear vs model index:


This scatter plot shows that the maximum base shear predicted by Model 1 is higher than that predicted by Model 2.

* Histogram of maximum base shear:


If only one dataset was used:

* Summary of results:


* Histogram of maximum base shear:




We observe from the summary statistics and the histograms that ...