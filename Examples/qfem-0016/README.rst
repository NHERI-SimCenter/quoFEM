
Surrogate Modeling to Predict Mean and Variance of Earthquake Response
======================================================================

+----------------+------------------------------------------+
| Problem files  | :github:`Github <Examples/qfem-0015/>`   |
+----------------+------------------------------------------+

.. note::
   This example may run up to 60 minutes depending on the computer performance. For a quick test, the user may want to reduce **Max Computation Time**.

Outline
-------

This example constructs a surrogate model for mean and standard deviation of a building structure given ten ground motion set. We are interested in 5 structural parameters. Gaussian process-based surrogate model is used.


Problem description
-------------------

14 parameters are defined as follows. 

============================= ============ =========
Random Variable               lower bound  upper bound			      
============================= ============ =========
weight (w)                    0.0          1.0
roof weight (wR)              800          2400
initial stiffness (k)         0.4          0.8
post-yield stiff. ratio (alp) 0.02         0.1
yield strength (Fy)           0.5          0.8
============================= ============ =========

User wants to make a surrogate model for mean and standard deviation of the peak displacement response at node 1.

.. figure:: figures/SUR2-sturcture.PNG
   :align: center
   :figclass: align-center
   :width: 600

Input files
-------------
Once user selects OpenSeesPy as FEM applications, below two fields are requested.

1. Input Script - ``ShearBuilding_NL.tcl``: This file is the main OpenSees input script which implements builds the model, reads ground motion time histories, and run the analysis repeatedly. It is supplied to the Input Script field of the FEM tab. 

2. Postprocess Script (Optional) - ``postprocess.tcl``: This file is a post script that connects QoI name to the output value. According to this post scipt, the QoI should be entered as either in the format of ``Node_i_Disp_j_Mean`` or ``Node_i_Disp_j_Std``, where i and j respectively denote the node number and degree of freedon.

**The other subsidary scripts (including ground motion time histories) are stored in the same directory of the main input script.**


UQ Workflow
-------------

1. Since the model is provided, Training Dataset can be obtained by **Sampling and Simulation**. The analysis stops when either (1) target accuracy (2) Maximum number of model runs or (3) Maximum computation time is reached. Since it is known that mean and variance of peak drift are always positive, log-transform is introduced. Since a trend is expected, linear trend function is introduced. Number of Initial Design of Experiments are set to 10.


.. figure:: figures/SUR2-UQtab.png
   :align: center
   :figclass: align-center

2. Select the **FEM** tab from the input panel. Choose the engine to be OpenSeesPy. For the main script copy the path name to ``ShearBuilding_NL.tcl`` or select choose and navigate to the file. For the post-process script field, repeat the same procedure for the ``postprocess.tcl`` script.


.. figure:: figures/SUR2-FEMtab.png
   :align: center
   :figclass: align-center

3. Select the **RV** panel from the input panel. This should be pre-populated with 5 random variables by detecting ``pset`` command in ``ShearBuilding_NL.tcl``. For each variable, distribution option is fixed to be **Uniform** and only the lower and upper bounds needs to be specified as given in the table.

.. figure:: figures/SUR2-RVtab.png
   :align: center
   :figclass: align-center

.. note::
When user need to manually add random variables with **add** button, eg. when using a custom FEM application, the user should set the distribution to be **Uniform** using the drop-down menu.


4. Select the **QoI** tab. Here enter two output names as ``Node_2_Disp_1_Mean`` and ``Node_2_Disp_1_Std``. Note that Node_2_Disp_1 means x-direction displacement of second story floor. 

.. figure:: figures/SUR2-QoItab.png
   :align: center
   :figclass: align-center

5. Click on the **Run** button. This will cause the back-end application to run SimCenterUQ Engine.

6. When done, the **RES** tab will be selected and the results will be displayed.

* Summary of Results:

.. figure:: figures/SUR2-REStab1.png
   :align: center
   :figclass: align-center

* Leave-out-one cross-validation (LOOCV) predictions:

.. figure:: figures/SUR2-REStab2.png
   :align: center
   :figclass: align-center
   :width: 800

7. Save the surrogate model by clicking ``Save GP Surrogate``





Sensitivity analysis using the Surrogate model
-----------------------------------------------

Once surrogate model is constructed, it can be used for various UQ/optimization applications. Here we perform a sensitivity analysis and compare it with the results from simulation model.

1. The constructed surrogate model can be saved by **Save GP Model** button. Two files and a folder will be saved which are **SurroateGP Info File** (default name: ``SimGpModel.json``), **SurroateGP model file** (default name: ``SimGpModel.pkl``) and **Simulation template directory** which contains the simulation model information (``templatedir_SIM``).

.. figure:: figures/SUR2-VER0.png
   :align: center
   :figclass: align-center

.. note::
   * Do not change the name of ``templatedir_SIM``. **SurrogateGP Info and model** file names may be changed.
   * When location of the files are changed, ``templatedir_SIM`` should be always located in the directory same to the **SurroateGP Info file**.

2. Restart the quoFEM (or press **UQ tab**) and select Dakota sensitivity analysis method.

.. figure:: figures/SUR2-VER1.png
   :align: center
   :figclass: align-center

3. Select the **FEM tab**  from the input panel and choose **SurrogateGP** application. For the **SurrogateGP Info field**, copy the path name to ``SimGpModel.json`` or select choose and navigate to the file. Similarly, the  **SurroateGP Model field** calls ``SimGpModel.pkl`` file. Once the first file is imported, additional options will pop-up. Here, user can specify the Maximum Allowable Normalized Variance level. The exceedance percentage are provided to help users decision along with the pre-informed accuracy of the surrogate model obtained after training session. Select continue to use only surrogate model predictions.

.. figure:: figures/SUR2-VER2.png
   :align: center
   :figclass: align-center


.. note::
	The **continue** option is recommended only when users have confidence in constructed surrogate model.


4. Once the SurrogateGP Info field in the **FEM** tab is entered, the **RV tab** is automatically populated. The user can select the distribution and its parameters. This example applied the following distributions.

.. figure:: figures/SUR2-VER4.png
   :align: center
   :figclass: align-center

   Also correlation between floor weight and roof weight is assumed to be 0.3.

.. figure:: figures/SUR2-VER3.png
   :align: center
   :figclass: align-center

5. Once the SurrogateGP Info field in the **FEM** tab is entered, the **QoI tab** is automatically populated by ``Node_2_Disp_1_Mean`` and ``Node_2_Disp_1_Std``. Users are allowed to remove some of the QoIs if not interested but may not add new QoIs or modify the names of existing QoIs.

5. Click on the **Run** button. This will cause the back-end application to launch dakota.

6. When done, the **RES** tab will be selected and the results will be displayed. 

* Surrogate model prediction

.. figure:: figures/SUR2-VER5.png
   :align: center
   :figclass: align-center

| Surrogate model training time: 59.3 min.(number of simulation model runs: 293)
| Analysis time: 13.3 min.(number of surrogate evaluations: 1200)

* Reference simulation model results

.. figure:: figures/SUR2-VER6.png
   :align: center
   :figclass: align-center

| Analysis time: 71.1 min. (number of simulation model runs: 1200)