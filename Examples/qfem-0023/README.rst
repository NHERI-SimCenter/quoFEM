.. _qfem-0023:

Global Sensitivity Analysis for Field QoI
=================================================

+---------------+----------------------------------------------+
| Problem files | :github:`Github <Examples/qfem-0023/>`       |
+---------------+----------------------------------------------+

Outline
-------
In this example, we investigate how much each parameter affects multiple outputs of a model. In particular, influence of different martial model parameters to the resulting stress time history under cyclic load setting will be compared via global sensitivity analysis. To gain efficiency dealing with such high-dimensional outputs, 'principal component analysis and probability model-based global sensitivity analysis (PCA-PSA)' method is used in the analysis.


Video Resources 
------------------
Global Senstivity Analysis for Multiple outputs?  (from 7:24 to 20:15)

.. raw:: html

   <div style="text-align: center;">
      <iframe src="https://www.youtube.com/embed/Z0BqspO7yuE?start=444" width="560" height="315" frameborder="5" allowfullscreen="allowfullscreen"></iframe>
   </div>

 
.. raw:: html

   <p><br>Click to replay the video from <a href="javascript:window.location.reload(true);">7:24</a>. Please note there were minor changes in the user interface since it is recorded.</p>


Problem description
-------------------

.. _figSteel02:

.. figure:: figures/qf-0023-Steel02.png
   :align: center
   :alt: A graph displaying stress versus strain for materials, with stress in ksi on the vertical axis and strain in inches per inch on the horizontal axis. Several curves are plotted, showing different materials' responses, typically starting from the origin and progressing through different slopes (elastic moduli), yielding points, and ultimate strengths before failure.
   :width: 400
   :figclass: align-center
   
   Steel02 Material -- Hysteretic Behavior of Model with Isotropic Hardening in Compression

Consider STEEL02 material model in OpenSees that describes cyclic stress-strain response of the steel reinforcing bar in finite element simulations. The STEEL02 material model in OpenSees can take up to 11 parameters as input, as described in the `documentation <https://opensees.berkeley.edu/wiki/index.php/Steel02_Material_--_Giuffré-Menegotto-Pinto_Model_with_Isotropic_Strain_Hardening>`_. Of these 11 parameters, 7 parameters shown in `Table 1`_ will be considered.

.. _Table 1:

Table 1: Parameters of the STEEL02 material model

==========================================================  =========== ===========
Variable                                                    lower bound upper bound
==========================================================  =========== ===========
Yield strength :math:`f_y`                                  300		    700
Initial elastic tangent :math:`E`                           150000	    250000
Strain hardening ratio :math:`b`                            0	        0.2
Elastic-plastic transition parameter 1 :math:`cR_1`    	    0	        1
Elastic-plastic transition parameter 2 :math:`cR_2`         0	    	0.2
Isotropic hardening parameter for compression :math:`a_1`   0	    	0.1
Isotropic hardening parameter for tension :math:`a_3`       0		   	0.1
==========================================================  =========== ===========
	 

The other four parameters are kept fixed value at:

==========================================================  =========== 
Variable                                                    Value
==========================================================  =========== 
Elastic-plastic transition parameter :math:`R_0`            20
Isotropic hardening parameter for compression :math:`a_2`   1
Isotropic hardening parameter for tension :math:`a_4`       1
Initial stress value :math:`\sigma_{init}`                        0
==========================================================  =========== 
 
Files required
--------------
The exercise requires one main script file and one supplementary file. The user should download these files and place them in a **NEW** folder. 

.. warning::
   Do NOT place the files in your root, downloads, or desktop folder as when the application runs it will copy the contents on the directories and subdirectories containing these files multiple times. If you are like us, your root, Downloads or Documents folders contains a lot of files.

1. :qfem-0023:`matTestAllParamsReadStrain.tcl <src/matTestAllParamsReadStrain.tcl>` - This is an OpenSees script written in tcl which simulates a material test and writes the stress response (in a file called ``results.out``) when subjected to the chosen strain history, for a given value of the parameters of the material model. Note that the random variables are using ``pset`` command while the other deterministic variables are defined by ``set`` command. 

2. :qfem-0023:`stress.1.coords <src/stress.1.coords>` - This file contains the cyclic strain history that is used as input during the finite element simulation of the material response. The strain values stored in this file are read in by the tcl script performing the OpenSees analysis. **Place this file to the same directory as** :qfem-0023:`matTestAllParamsReadStrain.tcl <src/matTestAllParamsReadStrain.tcl>` **such that this file is automatically copied to the working directory of quoFEM.** Since quoFEM copies all the files in the directory, it does not require to explicitly specify the supplementary files (other than the main analysis script) individually.

.. note::
   Since the tcl script creates a ``results.out`` file when it runs, no postprocessing script is needed. 


.. _figExperimentalDataSteelCouponStrain:

.. figure:: figures/qf-0023-StrainHistory.png
   :align: center
   :alt: The image is a line graph displaying a fluctuating signal over time. The horizontal axis is labeled 'Timestep' and ranges from 0 to 300. The vertical axis is labeled 'Strain' and ranges from -0.05 to 0.02. The graph shows a blue oscillating line with sharp peaks and valleys, suggesting periodic behavior with some irregularities or noise. The pattern repeats approximately every 100 timesteps.
   :width: 400
   :figclass: align-center
   
   Strain history in 'stress.1.coords'.



UQ workflow
-----------
.. note::
	Selecting the ``Global Sensitivity Analysis for Field QoI`` example in the quoFEM Examples menu will autopopulate all the input fields required to run this example. 
	The procedure outlined below demonstrates how to manually set up this problem in quoFEM.

The steps involved are as follows:

1. Start the application and the **UQ** panel will be highlighted. In the **UQ Engine** drop down menu, select the **SimCenterUQ** engine. In the **Method** select **Sensitivity Analysis** option. Enter the values in this panel as shown in the figure below. 

.. figure:: figures/qf-0023-UQ.png
   :align: center
   :alt: Screenshot of a software interface for uncertainty quantification (UQ) with a vertical menu to the left showing categories FEM, RV, EDP, and RES highlighted in teal. The main area is titled "UQ Method" with drop-down menus and fields including "Sensitivity Analysis", "UQ Engine" set to "SimCenterUO", and "Method" set to "Monte Carlo". Additional fields indicate "# Samples" as 300 and "Seed" as 487. Options for "Resample RVs from correlated dataset" and "Advanced option for global sensitivity analysis" are shown as checkboxes, both are unchecked.
   :figclass: align-center

If the total number of QoI components exceeds 15, quoFEM will automatically run 'principal component analysis and probability model-based sensitivity analysis (PCA-PSA)' [Jung2023]_ to gain efficiency.
Because this example have 342 QoI components representing stress values of discretized time series, PCA will be performed for the QoI vector by default to gain efficiency.

2. Next select the **FEM** panel from the input panel selection. This will default to the **OpenSees** FEM engine. In the **Input Script** field, enter the path to the ``matTestAllParamsReadStrain.tcl`` file or select **Choose** and navigate to the file. 

.. figure:: figures/qf-0023-FEM.png
   :align: center
   :alt: Screenshot of a computer interface section labeled "FEM" for finite element modeling, indicating OpenSees as the selected software. There are fields for "Input Script" with a file path provided and for "Postprocess Script" which is marked as optional. There are "Choose" buttons next to each field, and tabs labeled "UQ", "FEM", "RV", "EDP", and "RES" on the left side of the window.
   :figclass: align-center


.. note::
   Since the tcl script creates a ``results.out`` file when it runs, no postprocessing script is needed. 

3. Select the **RV** tab from the input panel. This panel should be pre-populated with seven random variables. If not, press the **Add** button to create new fields to define the input random variables. Enter the same variable names, as required in the model script. 

For each variable, specify the probability distribution and its parameters, as shown in the figure below. 

.. figure:: figures/qf-0023-RV.png
   :align: center
   :alt: Screenshot of a user interface titled "Input Random Variables" with tabs for UQ, FEM, RV, EDP, RES. The main section displays a table with columns for Variable Name, Input Type, Distribution, and Parameters. There are variable names like "fy," "E," "b," "cR1," "cR2," "a1," and "a3" with input types and uniform distribution selected. Minimum and maximum values are specified for some variables. Buttons for "Add," "Clear All," "Show PDF," "Correlation Matrix," "Export," and "Import" are visible at the top and next to the parameter inputs.
   :figclass: align-center


.. note::
   The results of sensitivity analysis will depend on the the choice of distribution types and parameters. Higher uncertainty leads to higher influece on the response. 

4. In the **QoI** panel denote that the variable named ``stress`` is not a scalar response variable, but has a length of 342.

.. figure:: figures/qf-0023-QOI.png
   :align: center
   :alt: Screenshot of a user interface with a focus on "Quantities of Interest." It shows a section where a variable named "stress" with a length of "342" is listed. On the left side, there are tabs labeled "UQ," "FEM," "RV," "EDP," and "RES." The "UQ" tab appears to be the active one. Two buttons labeled "Add" and "Clear all" are visible at the top right corner of the Quantities of Interest section.
   :figclass: align-center

**Note that the aggregated sensitivity indices will also be provided for the field QoIs. The aggregated sensitivity indices is obtained by weighted sum of the component sensitivity indices, where the weight is proportional to the variance of each QoI component. If multiple of field QoIs are defined, aggregated sensitivity indices will be provided for each of them.**


5. Click on the **Run** button. This will cause the backend application to launch the **SimCenterUQ** engine, which performs Global Sensitivity Analysis. When done, the **RES** tab will be selected and the results will be displayed as shown in the figure below.


.. figure:: figures/qf-0023-RES1.png
   :align: center
   :alt: Screenshot of a software interface displaying a table and a bar graph titled "[aggregated]stress Sobol' Indices." The table lists random variables, with corresponding "Main" and "Total" values for Sobol' indices, such as "fy" (0.162), "E" (0.092), "b" (0.632), among others. The bar graph visually represents these indices, with "b" showing the most significant value. Additional information indicates an elapsed time of 28.6 seconds and a note mentioning PCA is performed for QoI, explaining 99.1808% of the total variance with 5 principal components. There is a dropdown menu, tabs for "Summary" and "Data Values," and a "Save Results" button.
   :figclass: align-center


The results show the sensitivity indices (Sobol indices) for each QoIs. The results tab first shows the aggregated sensitivity indices for the fields QoIs. Using the drop down menu, the user can also inspect individual sensitivity indices. See `here <../../../../technical_manual/desktop/SimCenterUQTechnical.html>`_ to learn about the difference between the main and total indices. The analysis took around 28.5 seconds and 5 principal components are used to reduce the dimension during the analysis.

If the user selects the **Data Values** tab in the results panel, they will be presented with both a graphical plot and a tabular listing of the data.

.. figure:: figures/qf-0023-RES2.png
   :align: center
   :alt: Screenshot of software displaying a scatter plot and a data table. The scatter plot, labeled "Samples" and "Run #1," shows numerous blue data points spread vertically across a horizontal axis labeled "Run #," which ranges from 0 to 301.0. The vertical axis values range from 261.8 to 738.4. To the right, a data table lists variables, including Run #, fy, E, b, cR1, and cR2, with data corresponding to 12 runs. Above the table are options to 'Save Table', 'Save Columns Separately', 'Save RVs', and 'Save QoIs'. The interface has a navigation sidebar on the left with categories such as UQ, FEM, RV, EDP, and RES highlighted in blue.
   :figclass: align-center


Comparison with the results without PCA
--------------------------------------------------
Using the same configuration but without PCA (with ``No`` option selected for 'Perform PCA with QoI' in the UQ panel), the analysis took 1198 seconds on the same computer used to run the case above with PCA. The below figure shows the aggregated sensitivity indices which appears to be similar to the previous results.

.. figure:: figures/qf-0023-RES3.png
   :align: center
   :alt: Screenshot of a software interface with an expandable menu titled "Advanced option for global sensitivity analysis." It contains two configurable settings: one for entering a group of variable names for Sobol indices, with an example format provided, and another dropdown menu for selecting whether to perform PCA (Principal Component Analysis) with the options "No" currently selected.
   :figclass: align-center

.. figure:: figures/qf-0023-RES4.png
   :align: center
   :alt: Screenshot of a software interface displaying "[aggregated] stress Sobol' Indices". There is a table on the left with columns labeled "Random Variable", "Main", and "Total". The variables listed are fy, E, b, cR1, cR2, a1, and a3 with corresponding numerical values for each under the Main and Total columns. To the right, there's a bar graph plotting these variables against values between 0.00 and 1.00. Each variable has a pair of bars representing Main and Total, with 'b' having the highest bars. At the bottom left, it reads "Elapsed time: 1198.0s". A "Save Results" button is visible at the bottom right.
   :figclass: align-center


.. [Jung2023]
   Jung, W.H., & Taflanidis, A.A. (2023). Efficient global sensitivity analysis for high-dimensional outputs combining data-driven probability models and dimensionality reduction. Reliability Engineering & System Safety, Volume 231, 108805.
