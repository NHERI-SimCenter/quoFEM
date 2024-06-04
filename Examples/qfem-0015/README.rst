.. _qfem-0015:

Surrogate Modeling for One Story Building Earthquake Response
===============================================================

+----------------+------------------------------------------+
| Problem files  | :github:`Github <Examples/qfem-0015/>`   |
+----------------+------------------------------------------+

.. note::
   This example may run up to 20 minutes depending on the computer performance. For a quick test run, the user may want to reduce the **Max Computation Time** or **Max Number of model Runs**.

Outline
-------

This example constructs a Gaussian process-based surrogate model for the response of a building structure given a ground motion time history. We are interested in the maximum inter-story drift/acceleration response determined in 14 structural parameters. 

.. figure:: figures/SUR-Concept.PNG
   :align: center
   :alt: "An educational image displaying a process of structural analysis and reliability assessment. On the left, a colorful 3D finite element model of a building structure with a visible mesh grid, next to a series of graphical data outputs. In the center, a label with a double-arrow points to the right towards a 3D plot representing a mathematical surface in the next two images. The bottom right image shows a 3D bell-shaped probability distribution curve with a highlighted area labeled 'Failure Domain.' The top right image depicts a complex 3D surface with two different peaks and valleys, with an arrow showing the path from the peak through the failure domain. The illustration suggests a step-by-step process from structural modeling to failure probability analysis."
   :figclass: align-center
   :width: 600

   Conceptual illustration of surrogate modeling

The building model based on the work described in [Zou2020]_.

.. [Zou2020]
   Zou, J., Welch, D. P., Zsarnoczay, A., Taflanidis, A., & Deierlein, G. G. (2020). Surrogate Modeling for the Seismic Response Estimation of Residential Wood Frame Structures. In Proceedings of the 17th World Conference on Earthquake Engineering, Japan. [`link <https://www.researchgate.net/publication/344803660_Surrogate_Modeling_for_the_Seismic_Response_Estimation_of_Residential_Wood_Frame_Structures>`_]

Problem description
-------------------

The structure (:qfem-0015:`single story building with a cripple wall <src/CWH.py>`) has the following uncertain properties:

================== ============ =========
Random Variable    lower bound  upper bound
					      
================== ============ =========
Material Quality   0.2          1.0
Plan Area          1200          2400
Aspect Ratio       0.4          0.8
Damping            0.02         0.1
external Density_x 0.5          0.8
external Density_y 0.5          0.8
intternal Density  0.06         0.08
extEcc_x           0.0          0.5
extEcc_z           0.0          0.5
intEcc_x           0.0          0.5
intEcc_z           0.0          0.5
floor1 Weight      10.0         15.5
flower2 Weight     10.0         15.5
roof Weight        10.0         15.5
================== ============ =========

Below is the time history of the ground motion excitation.

.. figure:: figures/SUR-GM.PNG
   :align: center
   :alt: A line graph displaying acceleration in g-force (y-axis) over time in seconds (x-axis). The graph shows a sharp spike in acceleration occurring just before 5 seconds, followed by smaller fluctuations that gradually stabilize to a nearly flat line as time progresses toward 30 seconds. The spike reaches just above 2 g's, and most other fluctuations stay within ±1g.
   :figclass: align-center
   :width: 600


Input files
-------------
Once the user selects OpenSeesPy as FEM applications, below three fields are requested.

1. **Input Script** - ``CWH.py``: This file is the main Python script that builds the model and runs the analysis. It is supplied to the Input Script field of the FEM tab. Because this file writes directly to results.out, it obviates the need for supplying a post-process script.
2. **Postprocess Script (Optional)** - None: This optional script is not required.
3. **Parameters File** - ``params.py``: This file is a Python script that defines the problem’s random variables. The **RV tab** will be auto-populated when this file is loaded. The literal values which are assigned to variables in this file will be varied at runtime by the UQ engine.


**The other subsidiary scripts (including ground motion time history) are stored in the same directory of the main input script.**

UQ Workflow
-------------

1. Start the application, and the **UQ** Selection will be highlighted. Change the UQ Engine to **SimCenterUQ** and the Method Category to **Training GP Surrogate Model**. Since the model is provided, the Training Dataset will be obtained by **Sampling and Simulation**. Default settings are used for the advanced options.

.. figure:: figures/SUR-UQtab1.png
   :align: center
   :alt: Screenshot of a software interface for uncertainty quantification settings. On the left sidebar, options such as UQ, FEM, RV, EDP, and RES are visible, with UQ highlighted. The main panel shows settings for the UQ Method called Surrogate Modeling, with drop-down menus and text fields for UQ Engine with 'SimCenterUQ' selected, SimCenterUQ Method Category set to 'Train GP Surrogate Model,' and Training Dataset as 'Sampling and Simulation.' Fields for Number of Samples, Max Computation Time, Target Accuracy, and Random Seed are filled with values 150, 20 minutes, 0.02, and 71 respectively. A checkbox for Parallel Execution is ticked, and there are two unchecked options for Advanced Options for Gaussian Process Model and Start with Existing Dataset.
   :figclass: align-center
   :width: 1200

2. Select the **FEM** tab from the input panel. Choose the engine to be OpenSeesPy. For the main script, copy the path to ``CWH.py`` or click choose and navigate to the file. Post-process script field can remain empty and ``params.py`` file is called in the Parameters File field.


.. figure:: figures/SUR-FEMtab.png
   :align: center
   :alt: Screenshot of a graphical user interface for a simulation software with a focus on finite element modeling (FEM). The dropdown menu indicates the software uses OpenSeesPy. Below it, there are three fields labeled "Input Script," "Postprocess Script (Optional)," and "Parameters Script," each accompanied by file paths and a "Choose" button to select files. The paths suggest the scripts are examples located in a directory structure under C:\SimCenter\quoFEM\Examples\qfem-0015\src\. The left side of the interface has a vertical menu with options such as UQ, FEM, RV, EDP, and RES highlighted in blue.
   :figclass: align-center
   :width: 1200

3. Select the **RV** tab from the input panel. This should be pre-populated with 14 random variables once ``params.py`` is imported. For each variable, distribution is fixed to be **Uniform** and only the lower and upper bounds should be specified by users. 

.. figure:: figures/SUR-RVtab.png
   :align: center
   :alt: Screenshot of a software interface with a table titled "Input Random Variables." The table includes columns for Variable Name, Input Type, Distribution, Minimum Value, and Maximum Value, with various parameters listed like materialQuality, planArea, aspectRatio, damping, and others. Each row has a "Show PDF" button. The interface has tabs labeled UQ, FEM, RV, EDP, and RES, and there are buttons for Add, Clear All, Correlation Matrix, Export, and Import.
   :figclass: align-center
   :width: 1200

.. note::
   When user need to manually specify random variables with **add** button, eg. when using a custom FEM application, the user should use drop-down menu to set the distribution as Uniform.


4. Select the **QoI** tab. Here, enter two output names as ``InterstoryDrift_x`` and ``Accel_Roof_x``. 

.. figure:: figures/SUR-QoItab.png
   :align: center
   :alt: A screenshot of a software interface with a menu on the left and a "Quantities of Interest" section on the right. The menu contains items labeled UQ, FEM, RV, EDP, and RES, highlighted in turquoise and gray. Under the "Quantities of Interest" heading, there are buttons for 'Add' and 'Clear all,' and a list with two entries showing "Variable Name" alongside their respective "Length," which are "InterstoryDrift_x" and "Accel_Roof_x," each with a length of 1. An 'x' icon is present next to each variable name for the option to remove them from the list.
   :figclass: align-center
   :width: 1200

5. Click on the **Run** button. This will cause the back-end application to run SimCenterUQ Engine.

6. When done, the **RES** tab will be selected and the results will be displayed.


* Summary of Results:

.. figure:: figures/SUR-REStab1st1.png
   :align: center
   :alt: Screenshot of a software output indicating "Surrogate Modeling Completed! - Process ended as the maximum allowable number of simulations is reached." The summary section shows 150 training samples and model simulations, with analysis time of 7.6 minutes. The Goodness-of-Fit section displays numerical values under "InterstoryDrift_x" and "Accel_Roof_x" categories, showing the normalized error (NRMSE), R2, and correlation coefficient, with InterstoryDrift_x having better goodness-of-fit scores (NRMSE: 0.009, R2: 0.998, Correlation coeff: 0.999) than Accel_Roof_x (NRMSE: 0.087, R2: 0.742, Correlation coeff: 0.902). A note at the bottom states, "Some or all of the QoIs did not converge to the target accuracy (NRMSE=0.02)."
   :figclass: align-center
   :width: 1200


* Leave-one-out cross-validation (LOOCV) predictions:

.. figure:: figures/SUR-REStab1st2.png
   :align: center
   :alt: Two scatter plot graphs side by side titled "Leave-One-Out Cross-Validation (LOOCV) Prediction". The left graph plots predicted responses versus exact responses, with data points closely aligned along a straight diagonal line, indicating a strong positive correlation. The right graph shows a more dispersed set of data points, still with a positive trend but with greater variability. Both graphs have markers indicating sample predictions and a shaded area representing the inter-quartile range. The bottom of each graph notes "nugget variance: 0.000".
   :figclass: align-center
   :width: 1200


Well-trained model will form a clear diagonal line while poorly trained model are more scattered around. The detailed parameter values of the trained Gaussian process model can be found by clicking **Save GP Info** button below the scatter plot.

.. figure:: figures/SUR-REStab3.png
   :align: center
   :alt: Screenshot of a software interface showing a toolbar with the label "Saving Options" and four blue buttons labeled "Save GP Model," "Save GP Info," "RV Data," and "Qol Data." The "Save GP Info" button is highlighted with a red outline, indicating it is selected or being pointed out.
   :figclass: align-center
   :width: 600

7.  **Continue Analysis**: If the users are not satisfied with the convergence status, they can continue training by saving the current sample points by clicking **RV Data** and **QoI Data** buttons. Then the user may go back to **UQ tab** and activate **Start with Existing Dataset** option. The user can directly import the sample tables generated by quoFEM.

.. figure:: figures/SUR-UQtab2.png
   :align: center
   :alt: Screenshot of a software interface with a focus on the Uncertainty Quantification (UQ) method settings. The left panel shows different tabs such as UQ, FEM, RV, EDP, and RES. The main section is titled 'UQ Method' with the 'Surrogate Modeling' option selected, and specific settings for 'SimCenterUQ', including parameters like 'Number of Samples', 'Max Computation Time', and 'Random Seed'. Two sections are outlined in red; the left one highlights 'Start with Existing Dataset' with file paths for 'Training Points (Input RV)' and 'System Responses (Output QoI)' text files with 'Choose' buttons next to them indicating paths for input files. The right section outlines the 'Saving Options' with buttons for 'Save GP Model' and 'Save GP Info', and below it, a file list showing 'X.txt' and 'Y.txt' as part of the local directory, indicating output files. There are annotations pointing to these sections, with "(from the previous UQ training results)" near the top right and "(local directory)" at the bottom right, suggesting user guidance on the interface.
   :figclass: align-center
   :width: 1200

| Click on the **Run** button, and surrogate model training will be continued. Followings are the results after 150 more simulations.

* Summary:

.. figure:: figures/SUR-REStab2nd1.png
   :align: center
   :alt: Screenshot of a summary report from a simulation software displaying completed surrogate modeling with the notice "Process ended as the maximum allowable number of simulations is reached." The report shows 299 training samples, 150 model simulations, and an analysis time of 9.1 minutes. Under "Goodness-of-Fit," two categories are listed: InterstoryDrift_x and Accel_Roof_x, with corresponding Normalized error (NRMSE) values of 0.006 and 0.052, R2 values of 0.999 and 0.807, and Correlation coeff values of 0.999 and 0.919 respectively. A note at the bottom states that some or all of the QoIs did not converge to the target accuracy (NRMSE=0.02). The tabs "Summary" and "Data Values" are visible at the top with the Summary tab active.
   :figclass: align-center
   :width: 1200

.. note::
   Note that in the second training period, 150 initial samples were provided from the data files and 150 more simulations were conducted. However, the number of total samples used to train the surrogate model is displayed as 299 since one simulation is consumed to check the consistency between the user provided model (in the **FEM tab**) and the dataset (in the **UQ tab**).

* Leave-one-out cross-validation (LOOCV) predictions:

.. figure:: figures/SUR-REStab2nd2.png
   :align: center
   :alt: The image displays two scatter plots titled "Leave-One-Out Cross-Validation (LOOCV) Prediction." The left plot shows a nearly perfect linear relationship between predicted response and exact response, indicating precise predictions. The right plot illustrates a more dispersed set of data points with a positive trend, suggesting variability in the predictions. Both graphs have an added inter-quartile range feature denoted by squares connected by lines, representing a range of predicted values. The bottom of the image notes a "nugget variance: 0.000" for both plots.
   :figclass: align-c
   :width: 1200
   
Users may want to perform additional simulations in a similar way.




Verification of the Surrogate model
-----------------------------------

Once surrogate model is constructed, it can be used for various UQ/optimization applications. Here we perform a simple forward analysis to verify the constructed surrogate model. 

1. The constructed surrogate model can be saved by **Save GP Model** button. Two files and a folder will be saved which are **SurroateGP Info File** (default name: ``SimGpModel.json``), **SurroateGP model file** (default name: ``SimGpModel.pkl``) and **Simulation template directory** which contains the simulation model information (``templatedir_SIM``).

.. figure:: figures/SUR-VER1.png
   :align: center
   :alt: A screenshot of a computer interface with a focus on a section titled "Saving Options" containing buttons like "Save GP Model," "Save GP Info," "RV Data," and "Qol Data." A file dialog window is open, showing a selected file named "SimGpModel.json" inside a folder named "templatedir_SIM." Red outlines highlight the "Save GP Model" button and the selected json file, indicating an action where the SimGpModel.json file is to be saved using the Save GP Model option.
   :figclass: align-center
   :width: 1200

.. note::
   * Do not change the name of ``templatedir_SIM``. **SurrogateGP Info and model** file names may be changed.
   * When location of the files are changed, ``templatedir_SIM`` should be always located in the directory same to the **SurroateGP Info file**.
   
.. warning::

   Do not place above surrogate model files in your root, downloads, or desktop folder as when the application runs it will copy the contents on the directories and subdirectories containing these files multiple times. If you are like us, your root, Downloads or Documents folders contains and awful lot of files and when the backend workflow runs you will slowly find you will run out of disk space!

2. Restart the quoFEM (or press **UQ tab**) and select Dakota Forward Propagation method.

.. figure:: figures/SUR-VER2.png
   :align: center
   :alt: Screenshot of a software interface with various input fields and dropdown menus for uncertainty quantification settings. The interface is labeled with the sections "UQ," "FEM," "RV," "EDP," and "RES," and shows settings such as "UQ Engine" set to "Dakota," "Dakota Method Category" set to "Forward Propagation," "Method" dropdown with "LHS" selected, "# Samples" field with "50" entered, "Seed" field with "954" entered, and a checkbox for "Parallel Execution" which is unchecked.
   :figclass: align-center
   :width: 1200

3. Select the **FEM tab**  from the input panel and choose **SurrogateGP** application. For the **SurrogateGP Info** field, copy the path to the ``SimGpModel.json`` or click choose and navigate to the file. Similarly, the  **SurroateGP Model field** calls ``SimGpModel.pkl`` file. Once the first file is imported, additional options will be visible. Here, the user can specify the Maximum Allowable Normalized Variance level. The exceedance percentage is provided to help users decision along with the pre-informed accuracy of the surrogate model obtained right after the training session. In this example, we would like to compare two cases: **(CASE 1)** (Ignore the precision tolerance and) **Continue** analysis, **(CASE 2)** **Run Exact FEM simulation** for those samples prediction variance is high. The Maximum Allowable Normalized Variance level is set as 0.15.


.. figure:: figures/SUR-VER3.png
   :align: center
   :alt: Screenshot of a software interface showing a tab labeled "FEM" with various input fields and options related to a surrogate model configuration. Fields include paths to "SurrogateGP Info (.json)" and "SurrogateGP Model (.pkl)," while options include "Maximum Allowable Normalized Variance" set to 0.15 and radiobuttons for handling imprecise predictions, with "Ignore and Continue" currently selected. There are annotations with red arrows pointing to two cases, labeled "CASE1" and "CASE2." The interface also shows settings for the "Quality of Interest (QoI) list" with "InterstoryDrift_x, Accel_Roof_x" entered and a "GP output" dropdown set to "Median (representative) prediction." The sidebar includes additional tabs labeled "UQ," "RV," "EDP," and "RES."
   :figclass: align-center
   :width: 1200


4. Once the SurrogateGP Info field in the **FEM** tab is entered, the **RV tab** is automatically populated. The user can select the distribution and its parameters. This example applied the following conditions.


.. figure:: figures/SUR-VER4.png
   :align: center
   :alt: Screenshot of a user interface for inputting random variables, with tabs labeled UQ, FEM, RV, EDP, and RES. The interface shows a list of variable names such as "materialQuality," "planArea," and "damping," each with specified distribution types like "Uniform," and minimum and maximum values. There are buttons for adding variables, clearing all inputs, and accessing the correlation matrix, as well as links for 'Show PDF', 'Export', and 'Import'.
   :figclass: align-center
   :width: 1200


5. Users need to manually fill in the **QoI tab**. Users do not need to include here all the QoIs used for the training, but the users may not add new QoIs or modify the names of existing QoIs. [Tip] List of the trained QoI names can be found and copied in the **FEM tab**.


.. figure:: figures/SUR-VER5.png
   :align: center
   :alt: An interface screenshot showing a section labeled "Quantities of Interest" with an "Add" button and a "Clear all" button. Below, there are two entries listed under "Variable Name" and "Length": the first entry is "InterstoryDrift_x" with a length of 1, and the second is "Accel_Roof_x" also with a length of 1. To the left of the interface, there is a vertical navigation menu with the acronyms "UQ", "FEM", "RV", "EDP", and "RES" highlighted in blue, suggesting different sections of the application.
   :figclass: align-center
   :width: 1200

5. Click on the **Run** button. This will cause the back-end application to launch dakota.

6. When done, the **RES** tab will be selected and the results will be displayed. Below tables compare the results from the two cases.

* InterstoryDrift_x

================== ============================== ======================== ======================================
Prob. Moments      Reference (only simulation)    CASE1 (only surrogate)   CASE2 (surrogate and simulation)					     
================== ============================== ======================== ======================================
Mean               4.54e-4                        4.53e-4                  4.54e-4
StdDev             6.01e-5                        5.99e-5                  6.01e-5
Skewness           2.59                           2.53                     2.58
Kurtosis           13.49                          13.16                    13.49
================== ============================== ======================== ======================================

* Accel_Roof_x

================== ============================== ======================== ======================================
Prob. Moments      Reference (only simulation)    CASE1 (only surrogate)   CASE2 (surrogate and simulation)		
================== ============================== ======================== ======================================
Mean               35.84                          36.2                     35.79
StdDev             3.20                           3.63                     3.10
Skewness           0.46                           1.36                     0.31
Kurtosis           4.86                           9.76                     5.38
================== ============================== ======================== ======================================


* CASE1:

.. figure:: figures/SUR-VER6.png
   :align: center
   :alt: The image shows two scatter plots side by side. The left plot is labeled "drift" on the y-axis in scientific notation scale and "sample id" on the x-axis, with points ranging from 0 to 50. The right plot is labeled "acceleration" on the y-axis with values ranging from 20 to 55 and "sample id" on the x-axis with identical range. Both plots feature two types of markers: red circles labeled "surrogate model prediction" and black crosses labeled "exact simulation." The markers are distributed across the range of sample ids, illustrating a comparison between the surrogate model predictions and exact simulation results.
   :figclass: align-center
   :width: 1200


* CASE2: 

.. figure:: figures/SUR-VER7.png
   :align: center
   :alt: Two side-by-side scatter plots with numerical labels along the horizontal and vertical axes. The left plot is labeled 'drift' on the y-axis, ranging from approximately 3 to 7 times 10^-4, with 'sample id' on the x-axis ranging from 0 to 50. The right plot is labeled 'acceleration' on the y-axis, ranging from about 20 to 55, with the same 'sample id' range on the x-axis. Both plots feature red solid circles representing 'exact simulation' data points, black crosses indicating 'reference' values, and open red circles labeled as 'surrogate model prediction.' The data points are dispersed throughout the plots with no clear pattern.
   :figclass: align-center
   :width: 1200

The peak drifts are well-predicted with the surrogate model (CASE1), while the acceleration predictions are improved by alternating between the surrogate and simulation model (CASE2).

.. note::
   If the user wants to inspect the simulation status or check error/warning messages related to the surrogate model, they can refer to the messages written at: ``{Local Jobs Directory}/tmp.SimCenter/surrogateLog.err``. (Note: ``{Local Jobs Directory}`` is specified from the file-preference in the menu bar.)