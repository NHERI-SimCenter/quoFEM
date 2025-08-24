.. _qfem-0027:

Forward propagation of uncertainty with multiple simulation models
==================================================================

+---------------+----------------------------------------------+
| Problem files | :github:`Github <Examples/qfem-0027/>`       |
+---------------+----------------------------------------------+

Outline
-------
In this example, two models of a 6-story structure are used to predict the maximum base shear by conducting a nonlinear time history analysis of the structure subjected to seismic excitation. The beliefs represent the uncertainty about which of these two models are the best model to be used in the analysis, and in this example, equal belief is placed in the two models.

The structure is modeled as having flexible columns and stiff elastic beams. The structure is subjected to an earthquake excitation at the base which is strong enough to result in nonlinear response. 

Model 1 uses Steel01 material to represent the behavior of the columns while model 2 uses Steel02. There are three random variables in each model which correspond to three parameters in the uniaxial material models Steel01 and Steel02: the initial elastic stiffness of the columns (k), the yield strength of the columns (Fy), and the strain hardening ratio of the columns (alp), i.e., the ratio between the post-yield stiffness and the initial elastic stiffness. For Steel02, R0 is set to 15, and cR1 and cR2 are set to their recommended values of 0.925 and 0.15, respectively.


Material used in Model 1:

.. literalinclude:: create_model_1.tcl
   :language: tcl
   :lines: 24


Material used in Model 2:

.. literalinclude:: create_model_2.tcl
   :language: tcl
   :lines: 24


The output quantity of interest (QoI) is the amplitude of the maximum base shear.

 
Files required
--------------
The exercise uses two finite element models of a structure. The required files to run each model are collected into separate directories per model. In each directory, the following files are required: one main tcl script file, two supplementary tcl scripts used to build the model and run the analysis, one supplementary file that contains the ground motion, and a tcl script to postprocess the output created during the OpenSees analysis and create the quantity of interest which is the maximum absolute value of the base shear. The user should download the required directories and files and place them in a **NEW** folder. 

.. warning::
   Do NOT place the files in your root, downloads, or desktop folder as when the application runs it will copy the contents on the directories and subdirectories containing these files multiple times. If you are like us, your root, Downloads or Documents folders contains a lot of files.

The following files are needed to evaluate Model 1. Evaluating Model 2 also requires a set of 5 files with the first two files in the following list replaced by the corresponding ones for Model 2.

1. :qfem-0027:`ShearBuilding_NL_1.tcl <src/model1/ShearBuilding_NL_1.tcl>` - This is a tcl script that evaluates the model and generates a file containing the time history of the element forces for element 1. The horizontal component of the element forces (the second column in the file) corresponds to the base shear experienced by this model of the structure. 

2. :qfem-0027:`create_model_1.tcl <src/model1/create_model_1.tcl>`- This file contains tcl and OpenSees commands that create the nodes, materials, and elements that make up the finite element model of the structure.

3. :qfem-0027:`run_analysis.tcl <src/model1/run_analysis.tcl>` - This file contains tcl and OpenSees commands to create the load, record the output, and perform a time history analysis of the structural response to the applied earthquake excitation.

4. :qfem-0027:`R331.dat <src/model1/R331.dat>` - This text file contains the time history of the earthquake acceleration applied to the base of the model.

5. :qfem-0027:`postprocess.tcl <src/model1/postprocess.tcl>` - This file contains tcl commands to read the time history of the base shear from the file created during the analysis, and to compute the maximum absolute value of the base shear experienced by the model.


UQ workflow
-----------

The steps involved are as follows:

1. Start the application and the **UQ** panel will be highlighted. In the **UQ Method** drop down menu, select the **Forward Propagation** option. In the **UQ Engine** dropdown menu select **Dakota** option. Enter the values in this panel as shown in the figure below. 

.. figure:: figures/UQ.png
   :align: center
   :alt: Screenshot of a user interface related to uncertainty quantification (UQ) with a focus on settings for a method of UQ. The sidebar shows options for FEM, RV, EDP, and RES, while the main panel has options for UQ Method as 'Forward Propagation,' UQ Engine as 'Dakota,' with checked options for 'Parallel Execution' and 'Save Working dirs.' It also shows a dropdown for the Method set to 'LHS' and input fields for the number of samples as '2000' and seed as '811'.
   :width: 1200
   :figclass: align-center


2. Next in the **FEM** panel , select **Multi Model** FEM engine. Use the **Add** or **Remove** option as needed to have two tabs, one for each of the two models being used in this example, and enter the values shown in the following figures:

.. figure:: figures/FEM1.png
   :align: center
   :alt: Screenshot of a user interface with a navigation menu on the left side highlighting the "FEM" tab. In the main panel, there are sections for inputting and managing models, including "Multi Model" with "Add" and "Remove" buttons, tabs labeled "FEM-1" and "FEM-2", and fields showing "Belief: 1 Out of 2 (i.e., 50%)". At the bottom, there are paths to "Input Script" and "Postprocess Script" with "Choose" buttons next to file paths containing the user "aakash" and referencing a directory for examples of a finite element model (FEM). An "OpenSees" button is also present.
   :figclass: align-center
   :width: 1200

.. figure:: figures/FEM2.png
   :align: center
   :alt: Screenshot of a user interface for a simulation or modeling software, featuring a navigation panel on the left with highlighted options such as "FEM" and "UQ". On the right, there are input fields and buttons, including a "Multi Model" toggle, "Add" and "Remove" buttons, a drop-down selection for "FEM-1" and "FEM-2", as well as fields for specifying "Input Script" and "Postprocess Script" with corresponding "Choose" buttons. File paths are displayed beneath the script fields, indicating locations on the user's system.
   :figclass: align-center
   :width: 1200


3. Select the **RV** tab from the input panel. This panel should be pre-populated with the names of the variables that were defined in the model scripts. If not, press the **Add** button to create a new field to define the input random variable. Enter the same variable name, as required in the model script. For this example, choose the Uniform probability distribution for all the random variabels and enter the Min. and Max. values for each distribution as shown in the figure below:

.. figure:: figures/RV.png
   :align: center
   :alt: Screenshot of a software interface displaying a list of input random variables with fields for variable name, distribution, minimum, and maximum values. Six variables labeled as k1, alp1, Fy1, k2, alp2, and Fy2 are listed, each with a uniform distribution and their respective min and max values. Options are available to add a new variable, delete variables, show probability distribution function (PDF) for each, and there are buttons titled "Clear All" and "Correlation Matrix," as well as "Export" and "Import" at the top right corner.
   :figclass: align-center
   :width: 1200


4. In the **EDP** panel create an output quantity of length 1 with a descriptive name.

.. figure:: figures/EDP.png
   :align: center
   :alt: Screenshot of a software interface with a navigation sidebar on the left showing menu options UQ, FEM, RV, EDP highlighted in teal, and RES. The main section is titled "Quantities of Interest" with fields to input a Variable Name "MaxBaseShear" and Length "1", with "Add" and "Clear All" buttons on the top right.
   :figclass: align-center
   :width: 1200


5. Click on the **Run** button. This will create the necessary input files to perform a forward propagation analysis with Dakota, run the analysis, and display the results when the analysis is completed.

When we plot the QoI vs MultiModel-FEM, we see a systematic difference in the range of the predicted QoIs - model 2 predicts lower max base shear values than model 1. The example runs in < 1 minute on my Mac, for 100 samples.

* Summary of results obtained by using the two models:

.. figure:: figures/RES1.png
   :align: center
   :alt: Screenshot of a statistical software interface showing summary data for two variables. On the left side, a navigation bar with the options UQ, FEM, RV, EDP, and RES highlighted. The main panel is divided into two sections for different variables. The first section is labeled "MultiModel-FEM" with Mean: 1.5, StdDev: 0.500125, Skewness: 0, and Kurtosis: 0.997997. The second section is labeled "MaxBaseShear" with Mean: 21.0259, StdDev: 1.34018, Skewness: -0.0254933, and Kurtosis: 2.37179. Two tabs at the top of the main panel indicate "Summary" (selected) and "Data Values".
   :figclass: align-center
   :width: 1200

* Maximum base shear vs model index:

.. figure:: figures/RES2.png
   :align: center
   :alt: A screenshot displaying a user interface with a navigation sidebar on the left and two main sections on the right. The left sidebar shows vertical tabs labeled UQ, FEM, RV, EDP, and RES, with the RES tab highlighted. The right side is split into a chart and a data table. The chart shows a single vertical bar graph with "Max Base Shear" on the y-axis and "MultiModel-FEM" on the x-axis, including a note saying "corr coef.f = -0.32". The data table is titled "Data Values" with a row of buttons above it including "Save Table" and "Save Columns Separately". The table lists rows numbered 1 through 12 and multiple columns with headers such as "Run #", "k1", "alp1", "Fy1", "k2", "alp2", and numbers corresponding to each row. Some rows are highlighted in gray, indicating a selection.
   :figclass: align-center
   :width: 1200

This scatter plot shows that the maximum base shear predicted by Model 1 is higher than that predicted by Model 2.

* Histogram of maximum base shear:

.. figure:: figures/RES3.png
   :align: center
   :alt: Screenshot of a software interface featuring a histogram on the left and a data table on the right. The histogram displays a bell-shaped distribution of frequencies against 'MaxBaseShear' values. The data table has columns labeled 'alp1', 'Fy1', 'k2', 'alp2', 'Fy2', 'MultiModel-FEM', and 'MaxBaseShear', with rows of numerical data. Some tabs and buttons are present, including 'Summary', 'Data Values', 'Save Table', 'Save Columns Separately', 'Save RVS', and 'Save QoIs'. There is a navigation panel on the left with the options 'UQ', 'FEM', 'RV', 'EDP', and 'RES' highlighted.
   :figclass: align-center
   :width: 1200

If only Model 1 was used:

* Summary of results:

.. figure:: figures/RES4.png
   :align: center
   :alt: The image shows a screenshot of a software interface with statistical data displayed. On the left is a vertical navigation menu with the options "UQ," "FEM," "RV," "EDP," and "RES" highlighted in blue. In the main panel, there is a section labeled "Summary," and below it, the following statistical measures for an item named "MaxBaseShear" are shown: "Mean" with a value of 21.4233, "StdDev" (standard deviation) with a value of 1.27395, "Skewness" with a value of 0.0799317, and "Kurtosis" with a value of 2.24344. A tab with the label "Data Values" is present next to the "Summary" label, suggesting that additional information can be displayed.
   :figclass: align-center
   :width: 1200

* Histogram of maximum base shear:

.. figure:: figures/RES5.png
   :align: center
   :alt: Screenshot of a computer interface showing a histogram and accompanying data table. The histogram displays a distribution of values labeled "MaxBaseShear" on the x-axis with frequency percentages on the y-axis. The table to the right lists numerical data with columns labeled k1, apl1, Fy1, k2, apl2, Fy2, and MaxBaseShear, filled with various numerical entries. Additional buttons for saving table data are present above the table.
   :figclass: align-center
   :width: 1200

If only Model 2 was used:

* Summary of results:

.. figure:: figures/RES6.png
   :align: center
   :alt: A screenshot of a statistics interface displaying summary data for an item named 'MaxBaseShear'. The data includes a mean value of 20.9323, standard deviation of 1.1546, skewness of -0.0622959, and kurtosis of 2.42761. The interface has menu options on the left side labeled 'UQ', 'FEM', 'RV', 'EDP', and highlighted 'RES', with two tabs at the top right corner named 'Summary' and 'Data Values'.
   :figclass: align-center
   :width: 1200

* Histogram of maximum base shear:

.. figure:: figures/RES7.png
   :align: center
   :alt: A split screen image showing a histogram on the left and a spreadsheet on the right. The histogram appears to have a bin range of approximately 15.5 to 23.5 on the x-axis, labeled "MaxBaseShear," and a frequency percentage on the y-axis that ranges up to approximately 0.13. The histogram indicates a bell-shaped distribution of data with the highest frequency just above 20 on the x-axis. On the right side, the spreadsheet contains numerical data with columns labeled "Run #," "k2," "alp1," "Fy1," "alp2," "Fy2," and "MaxBaseShear," with 23 numbered rows of data. The table entries for "Run #10" are highlighted, with values including 385.722 for k2, 0.0946541 for alp1, 19.8504 for Fy1, 0.0511268 for alp2, 21.4821 for Fy2, and 21.9684 for MaxBaseShear. There are buttons above the spreadsheet for "Save Table," "Save Columns Separately," "Save RVs," and "Save Qols."
   :figclass: align-center
   :width: 1200


We observe from the summary statistics and the histograms that if it is not known which of the two models considered is the best model to be used to represent the behavior of the structure, the predicted range of the maximum base shear is larger (i.e., the standard deviation of the maximum base shear is higher when using the two models than in the case when either of the two models are used on their own). The mean value of the base shear predicted in this case will be the mean of the maximum base shear predicted by the two models when used on their own (here we numerically estimate the mean by sampling values of the inputs from the specified distribution, hence there is sampling variablity in the estimate of the mean). 
