.. _qfem-0016:

Surrogate Modeling to Predict Mean and Variance of Earthquake Response
======================================================================

+----------------+------------------------------------------+
| Problem files  | :github:`Github <Examples/qfem-0016/>`   |
+----------------+------------------------------------------+

Outline
-------

This example constructs a Gaussian process-based surrogate model for mean and standard deviation of a building structure given ten ground motion set. 


Problem description
-------------------

The structure (`three story nonlinear building stick model <https://github.com/NHERI-SimCenter/quoFEM/blob/master/Examples/qfem-0016/src/ShearBuilding_NL.tcl>`_) has the following uncertain properties:

============================= ============ =========
Random Variable               lower bound  upper bound			      
============================= ============ =========
weight (w)                    50           150
roof weight (wR)              40           80
initial stiffness (k)         240          360
post-yield stiff. ratio (alp) 0.05         0.15
yield strength (Fy)           18           22
============================= ============ =========

The goal is to make a surrogate model that predicts mean and standard deviation of the peak displacement at node 1.

.. figure:: figures/SUR2-sturcture.PNG
   :align: center
   :alt: The image displays a scientific dashboard containing two graphs. On the top right, there's a graph labeled "Ground motion" showing a time-series plot with oscillations. Below it on the bottom right is a "Hysteresis curve," which depicts a looped curve pattern. To the left, a vertical sidebar shows numbers from 1 to 6, each paired with a grey dot and icon, with the number 1 highlighted in green.
   :figclass: align-center
   :width: 600


Video resources 
---------------
Note that even though we use the exact same structure, the parameters, their distributions and the quantity of interest (QoI) in the video is different from those in this example. The files for the corresponding example can be found at :github:`Github <Examples/qfem-0016/LET>`. 

.. raw:: html

   <div style="text-align: center;">
      <iframe src="https://www.youtube.com/embed/Ze_K61TTuvg?start=1602" width="560" height="315" frameborder="5" allowfullscreen="allowfullscreen"></iframe>
   </div>

 
.. raw:: html

   <p><br>Click to replay the video from <a href="javascript:window.location.reload(true);">26:42</a>. Please note there were minor changes in the user interface since it is recorded.</p>


Input files
-------------
Once the user selects OpenSeesPy as FEM applications, the below two fields are requested.

1. Input Script - ``ShearBuilding_NL.tcl``: This file is the main OpenSees input script that implements builds the model, reads ground motion time histories, and runs the analysis repeatedly. It is supplied to the Input Script field of the FEM tab. 

2. Postprocess Script (Optional) - ``postprocess.tcl``: This file is a postprocess script that connects the QoI name to the output value. According to this postprocess file, the QoI should be entered as either in the format of ``Node_i_Disp_j_Mean`` or ``Node_i_Disp_j_Std``, where i and j respectively denote the node number and degree of freedom.

**The other subsidiary scripts (including ground motion time histories) are stored in the same directory of the main input script.**


UQ Workflow
-------------

1. Since the model is provided, Training Dataset can be obtained by **Sampling and Simulation**. Since it is known that the mean and variance of peak drift are always positive, log-transform is introduced. Since a trend is expected, a linear trend function is introduced. 


.. figure:: figures/SUR2-UQtab.png
   :align: center
   :alt: Screenshot of a software interface for uncertainty quantification with a focus on surrogate modeling. The interface includes options to select an uncertainty quantification (UQ) method, with 'SimCenterUQ' chosen as the UQ Engine, and 'Train GP Surrogate Model' selected in the SimCenterUQ Method Category dropdown menu. Various parameters such as 'Number of Samples', 'Max Computation Time', 'Target Accuracy', 'Random Seed', and 'Parallel Execution' can be configured. Advanced options for the Gaussian Process Model are visible, including selections for 'Kernel Function', checkboxes for adding a linear trend function and log-space transform, 'DoE Options', and 'Nugget Variances'. There is also an option to 'Start with Existing Dataset' at the bottom. The sidebar contains tabs titled 'FEM', 'RV', 'EDP', and 'RES', highlighted on 'UQ'.
   :figclass: align-center
   :width: 1200

2. Select the **FEM** tab from the input panel. Choose the engine to be OpenSeesPy. For the main script copy the path name to ``ShearBuilding_NL.tcl`` or click choose and navigate to the file. For the postprocess script field, repeat the same procedure for the ``postprocess.tcl`` script.


.. figure:: figures/SUR2-FEMtab.png
   :align: center
   :alt: This image displays a section of a graphical user interface with a focus on the "Finite Element Method Application" settings. It presents a dropdown menu with the option "OpenSees" selected. Below this, there are two fields: "Input Script" containing a file path labeled "C:/SimCenter/quoFEM/Examples/qfem-0016/src/ShearBuilding_NL.tcl," and "Postprocess Script" with a file path "C:/SimCenter/quoFEM/Examples/qfem-0016/src/postprocess.tcl." Each field has an associated "Choose" button to presumably change or confirm the file paths. On the left-hand side, vertically aligned tabs labeled "UQ," "FEM," "RV," "QoI," and "RES" are visible, suggesting different sections or functionalities within the application.
   :figclass: align-center
   :width: 1200

3. Select the **RV** tab from the input panel. This should be pre-populated with 5 random variables by detecting ``pset`` command in ``ShearBuilding_NL.tcl``. For each variable, the distribution option is fixed to be **Uniform**, and only the lower and upper bounds need to be specified as given in the table.

.. figure:: figures/SUR2-RVtab.png
   :align: center
   :alt: Screenshot of a user interface titled "Input Random Variables," which includes a list of variables with specified distributions and range parameters. Each variable row contains fields for "Variable Name," "Distribution" type (selected as "Uniform" for all), and numerical "Min." and "Max." values. Buttons for actions such as "Add," "Clear All," "Correlation Matrix," "Export," and "Import" are available above the list, and a "Show PDF" link is adjacent to each variable entry. The listed variables are "w" with a range of 50 to 150, "wR" from 40 to 80, "k" from 240 to 360, "alp" from 0.05 to 0.15, and "Fy" from 18 to 22.
   :figclass: align-center
   :width: 1200

.. note::
When the user needs to manually add random variables with **add** button, eg. when using a custom FEM application, the user should set the distribution to be **Uniform** using the drop-down menu.


4. Select the **QoI** tab. Here enter two output names as ``Node_2_Disp_1_Mean`` and ``Node_2_Disp_1_Std``. Note that Node_2_Disp_1 means x-direction displacement of second story floor. These QoI names are processed in the ``postprocess.tcl`` provided at the **FEM** tab.

.. figure:: figures/SUR2-QoItab.png
   :align: center
   :alt: Screenshot of a user interface segment titled "Quantities of Interest" with two listed items. Each item has a "Variable Name" and a "Length" with values "Node_2_Disp_1_Mean" and "Node_2_Disp_1_Std," both with a length of 1. To the right, there are "Add" and "Clear all" buttons. On the left sidebar, tabs are labeled "UQ," "FEM," "RV," "EDP," and "RES," with "RV" currently selected.
   :figclass: align-center
   :width: 1200

5. Click on the **Run** button. This will cause the backend application to run SimCenterUQ Engine.

6. When done, the **RES** tab will be selected and the results will be displayed.

* Summary of Results:

.. figure:: figures/SUR2-REStab1.png
   :align: center
   :alt: Screenshot of a software interface displaying the summary results of a surrogate modeling process with a notification that reads "Surrogate Modeling Completed! - Process ended as the maximum allowable number of simulations is reached." The results include the number of training samples and model simulations, both listed as 50, and the analysis time of 1.4 minutes. There are also statistics for goodness-of-fit, including Normalized error (NRMSE), R2, and Correlation coeff for 'Node_2_Disp_1_Mean' and 'Node_2_Disp_1_Std,' with values ranging from 0.016 to 0.999. A footnote indicates that some quality of life indices did not converge to the target accuracy.
   :figclass: align-center
   :width: 1200

* Leave-one-out cross-validation (LOOCV) predictions:

.. figure:: figures/SUR2-REStab2.png
   :align: center
   :alt: The image displays two graphs side by side, both titled "Leave-One-Out Cross-Validation (LOOCV) Prediction". Each graph plots data points comparing 'Exact response' on the x-axis with 'Predicted response (LOOCV)' on the y-axis, suggesting a statistical analysis or model evaluation. The left graph shows a tight correlation between the predicted and exact responses, indicated by points closely following a diagonal line. The right graph has a similar distribution of points but also includes shaded areas representing an inter-quartile range around the line of predicted responses, which is not present in the left graph. Both graphs include a note on the bottom indicating "nugget variance (log-transformed space)" with numerical values, the left being 0.000 and the right being 0.0008011.
   :figclass: align-center
   :width: 1200

7. Save the surrogate model by clicking ``Save GP Surrogate``


Sensitivity analysis using the Surrogate model
-----------------------------------------------

Once the surrogate model is trained, it can be used for various UQ/optimization applications. Here we perform a sensitivity analysis and compare it with the results from simulation model.

1. The constructed surrogate model can be saved by **Save GP Model** button. Two files and a folder will be saved which are **SurroateGP Info File** (default name: ``SimGpModel.json``), **SurroateGP model file** (default name: ``SimGpModel.pkl``) and **Simulation template directory** which contains the simulation model information (``templatedir_SIM``).

.. figure:: figures/SUR2-VER0.png
   :align: center
   :alt: A screenshot of a computer interface showing a section labeled "Saving Options" with buttons including "Save GP Model," "Save GP Info," "RV Data," and "Qol Data." There is a highlighted correlation between the "Save GP Model" button and a selected file in a directory list below, named "SimGpModel.json," indicating that clicking the button will save information related to a GP model in the JSON file format.
   :figclass: align-center
   :width: 800

.. note::
   * Do not change the name of ``templatedir_SIM``. **SurrogateGP Info and model** file names may be changed.
   * When location of the files are changed, ``templatedir_SIM`` should be always located in the directory same to the **SurroateGP Info file**.

.. warning::
   Do not place above surrogate model files in your root, downloads, or desktop folder as when the application runs it will copy the contents on the directories and subdirectories containing these files multiple times. If you are like us, your root, Downloads or Documents folders contains and awful lot of files and when the backend workflow runs you will slowly find you will run out of disk space!

2. Restart the quoFEM (or press **UQ tab**) and select Dakota sensitivity analysis method.

.. figure:: figures/SUR2-VER1.png
   :align: center
   :alt: Screenshot of a software interface with various input fields and dropdown menus for running simulations. The section header "UQ Engine" displays "Dakota" selected from a dropdown menu. Under "FEM," the "Dakota Method Category" is set to "Sensitivity Analysis," and the "Method" dropdown menu shows "LHS" selected. Below, there's a field for "# Samples" filled with "200," and a "Seed" field containing the number "852." A checkbox labeled "Parallel Execution" is unchecked in the top right corner. A note at the bottom indicates "Expected number of FEM calls = (#Samples)*(#RVs+2)." The sections labeled "RV," "QoI," and "RES" on the left side are not expanded and show no additional information.
   :figclass: align-center
   :width: 1200

3. Select the **FEM tab**  from the input panel and choose **SurrogateGP** application. For the **SurrogateGP Info field**, copy the path to ``SimGpModel.json`` or click choose and navigate to the file. Similarly, the  **SurroateGP Model field** calls ``SimGpModel.pkl`` file. Once the first file is imported, additional options will be displayed. Here, the user can specify the Maximum Allowable Normalized Variance level. The exceedance percentage is provided to help the user's decision along with the pre-informed accuracy of the surrogate model obtained after the training session. Select continue to use only surrogate model predictions.

.. figure:: figures/SUR2-VER2.png
   :align: center
   :alt: Screenshot of a software interface with tabs labeled UQ, FEM, RV, EDP, and RES. The FEM tab is selected, displaying fields for SurrogateGP Info (.json) and SurrogateGP Model (.pkl) with file paths, and Options for Maximum Allowable Normalized Variance with a note that around 55.3% of new samples in training range will exceed the tolerance limit, and when the surrogate model gives imprecise prediction at certain sample locations. There are radio buttons for Stop Analysis and Ignore and Continue, and a dropdown menu for GP output with 'Median (representative) predictio' visible.
   :figclass: align-center
   :width: 1200


.. note::
	The **Continue** option should be used only when users are familiar with potential issues.


4. Once the SurrogateGP Info field in the **FEM** tab is entered, the **RV tab** is automatically populated. The user can select the distribution and its parameters. This example applied the following distributions.

.. figure:: figures/SUR2-VER4.png
   :align: center
   :alt: Screenshot of a software interface with a section titled "Input Random Variables". There are rows of input data with fields including 'Variable Name', 'Distribution', 'Mean', and 'Standard Dev'. The variables listed are 'w', 'wR', 'k', 'alp', and 'Fy' with respective distributions 'Normal', 'Normal', 'Normal', 'Normal', and 'Gumbel'. Mean and standard deviation values are provided for each. Buttons for actions such as 'Add', 'Clear All', 'Correlation Matrix', 'Export', 'Import', and 'Show PDF' are visible at the top of the section.
   :figclass: align-center
   :width: 1200

5. Users need to manually fill in the **QoI tab**. Users do not need to include here all the QoIs used for the training, but the users may not add new QoIs or modify the names of existing QoIs. **[Tip]** List of the trained QoI names can be found and copied in the **FEM tab**.

6. Click on the **Run** button. This will cause the backend application to launch dakota.

7. When done, the **RES** tab will be selected and the results will be displayed. 

* Surrogate model prediction

.. figure:: figures/SUR2-VER5.png
   :align: center
   :alt: Screenshot of a data analysis software interface displaying "Node_2_Disp_1_Mean Sobol' indices" and "Node_2_Disp_1_Std Sobol' indices" with corresponding tables and bar charts. The tables list random variables 'w', 'wR', 'k', 'alp', and 'Fy' with associated 'Main' and 'Total' effect values. The bar charts visually represent these Sobol' indices, with 'w' having the most significant effects in both mean and standard deviation assessments. Options for navigating through different sections such as 'Summary', 'Data Values', 'FEM', 'RV', 'EDP', and 'RES' are provided in a side menu, and a 'Save Results' button is positioned at the bottom right.
   :figclass: align-center
   :width: 1200

| **Surrogate model training time**: 1.4 min. (number of FEM runs: 50)
| **Sensitivity analysis time**: 10.3 min. (number of surrogate model evaluations: 1400)

* Reference simulation model results

.. figure:: figures/SUR2-VER6.png
   :align: center
   :alt: Screenshot of a software interface displaying two tables and corresponding bar graphs under the headings "Node_2_Disp_1_Mean Sobol' indices" and "Node_2_Disp_1_Std Sobol' indices." Each table lists random variables (w, wR, k, alp, Fy) with numerical values for 'Main' and 'Total' effects. Next to the tables, the bar graphs visually represent these values, with bars of different lengths for 'Main' and 'Total.' The colors blue and green distinguish between the two types of effects. A button for 'Save Results' is visible at the bottom right corner.
   :figclass: align-center
   :width: 1200

| **Sensitivity analysis time**: 83.7 min. (number of FEM runs: 1400)