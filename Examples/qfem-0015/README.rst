
Surrogate Modeling for One Story Building Earthquake Response
===============================================================

+----------------+------------------------------------------+
| Problem files  | :github:`Github <Examples/qfem-0015/>`   |
+----------------+------------------------------------------+

.. note::
   This example may run up to 60 minutes depending on the computer performance. For a quick test, the user may want to reduce **Max Computation Time**.
   

Outline
-------

This example constructs a surrogate model for response of a building structure given a ground motion time history. We are interested in 14 structural parameters and maximum inter-story drift/acceleration of structural response. Gaussian process model is used.


.. figure:: figures/SUR-Concept.PNG
   :align: center
   :figclass: align-center
   :width: 600


Problem description
-------------------

14 parameters are defined as follows. 

================== ============ =========
Random Variable    lower bound  upper bound
					      
================== ============ =========
Material Quality   0.0          1.0
Plan Area          800          2400
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

User wants to make a surrogate model for drift ratio and roof acceleration of structural response under ground motion of 

.. figure:: figures/SUR-GM.PNG
   :align: center
   :figclass: align-center
   :width: 600

Input files
-------------
Once user selects OpenSeesPy as FEM applications, below three fields are requested.

1. Input Script - ``CWH.py``: This file is the main Python script which implements builds the model and run the analysis. It is supplied to the Input Script field of the FEM tab. Because this file write directly to results.out, it obviates the need for supplying a post process Script. 

2. Postprocess Script (Optional) - *None*: This optional script is not required.

3. Parameters File - ``params.py``: This file is a Python script which defines the problem’s random variables as objects in the Python runtime. It is supplied to the Parameters File field of the FEM tab. The literal values which are assigned to variables in this file will be varied at runtime by the UQ engine.

**The other subsidary scripts (including ground motion time history) are stored in the same directory of the main input script.**


UQ Workflow
-------------

1. Start the application and the **UQ** Selection will be highlighted. In the panel for the UQ selection, change the UQ Engine to **SimCenterUQ** and the Method Category to **Training GP Surrogate Model**. Since the model is provided, Training Dataset can be obtained by **Sampling and Simulation**. The analysis will stop when either (1) target accuracy (2) Maximum number of model runs or (3) Maximum computation time is reached. Since it is known that peak drift and acceleration values are always positive, log-transform is introduced. Since a trend is expected, linear trend function is introduced. Number of Initial Design of Experiments are set to 10.


.. figure:: figures/SUR-UQtab1.png
   :align: center
   :figclass: align-center

2. Select the **FEM** tab from the input panel. Choose the engine to be OpenSeesPy. For the main script copy the path name to ``CWH.py`` or select choose and navigate to the file. Post-process script field can remain empty and ``params.py`` file is called in Parameters File field.


.. figure:: figures/SUR-FEMtab.png
   :align: center
   :figclass: align-center

3. Select the **RV** panel from the input panel. This should be pre-populated with 14 random variables once ``params.py`` is imported. For each variable, distribution is fixed to be **Uniform** and only the lower and upper bounds should be specified by users. 

.. figure:: figures/SUR-RVtab.png
   :align: center
   :figclass: align-center

.. note::
   When user need to manually specify random variables with **add** button, eg. when using a custom FEM application, the user should use drop-down menu to set the distribution uniform.


4. Next select the **QoI** tab. Here enter two output names as ``InterstoryDrift_x`` and ``Accel_Roof_x``. 

.. figure:: figures/SUR-QoItab.png
   :align: center
   :figclass: align-center

5. Next click on the **Run** button. This will cause the back-end application to run SimCenterUQ Engine.

6. When done, the **RES** tab will be selected and the results will be displayed.


* Summary of Results:

.. figure:: figures/SUR-REStab1st1.png
   :align: center
   :figclass: align-center


* Leave-out-one cross-validation (LOOCV) predictions:

.. figure:: figures/SUR-REStab1st2.png
   :align: center
   :figclass: align-center


The prediction is accurate when all the scatter plots are located in the diagonal line. The detailed parameter values of the trained Gaussian process model can be found by clicking **Save GP Info** button below.

.. figure:: figures/SUR-REStab3.png
   :align: center
   :figclass: align-center
   :width: 600

7.  **Continue Analysis**: If the users are not satisfied with the results, they can continue training by saving sample points. Save the samples of RV and QoI by clicking **RV Data** and **QoI Data** buttons, respectively. Then the user may go back to **UQ tab** and activate **Start with Existing Dataset** option. The User can directly import the sample files generated by quoFEM.

.. figure:: figures/SUR-UQtab2.png
   :align: center
   :figclass: align-center

| Click on the **Run** button, and surrogate model training will be continued. Followings are the results after 144 more simulations.

* Summary:

.. figure:: figures/SUR-REStab2nd1.png
   :align: center
   :figclass: align-center


* Leave-out-one cross-validation (LOOCV) predictions:

.. figure:: figures/SUR-REStab2nd2.png
   :align: center
   :figclass: align-c
   :width: 800
   
Note that outliers are presented in the acceleration predictions. Users may want to perform additional simulations in a similar way.

.. note::
   Note that in the second training period, 150 initial samples were provided from the data files and 144 more simulations were conducted. However, the number of total samples used to train the surrogate model is only 293 since one simulation is consumed to check the consistency between the user provided model (in the **FEM tab**) and the dataset (in the **UQ tab**).



Verification of the Surrogate model
-----------------------------------

Once surrogate model is constructed, it can be used for various UQ/optimization applications. Here we perform a simple forward analysis to verify the constructed surrogate model. 

1. The constructed surrogate model can be saved by **Save GP Model** button. Two files and a folder will be saved which are **SurroateGP Info File** (default name: ``SimGpModel.json``), **SurroateGP model file** (default name: ``SimGpModel.pkl``) and **Simulation template directory** which contains the simulation model information (``templatedir_SIM``).

.. figure:: figures/SUR-VER1.png
   :align: center
   :figclass: align-center

.. note::
   * Do not change the name of ``templatedir_SIM``. **SurrogateGP Info and model** file names may be changed.
   * When location of the files are changed, ``templatedir_SIM`` should be always located in the directory same to the **SurroateGP Info file**.

2. Restart the quoFEM (or press **UQ tab**) and select Dakota forward UQ method.

.. figure:: figures/SUR-VER2.png
   :align: center
   :figclass: align-center

3. Select the **FEM tab**  from the input panel and choose **SurrogateGP** application. For the **SurrogateGP Info field**, copy the path name to ``SimGpModel.json`` or select choose and navigate to the file. Similarly, the  **SurroateGP Model field** calls ``SimGpModel.pkl`` file. Once the first file is imported, additional options will pop-up. Here, user can specify the Maximum Allowable Normalized Variance level. The exceedance percentage are provided to help users decision along with the pre-informed accuracy of the surrogate model obtained after training session. In this example, we would like to compare two cases: **(CASE 1)** (Ignore the precision tolerance and) **Continue** analysis, **(CASE 2)** **Run Exact FEM simulation** for those samples prediction variance is high. The Maximum Allowable Normalized Variance level is set as 0.08.


.. figure:: figures/SUR-VER3.png
   :align: center
   :figclass: align-center


4. Once the SurrogateGP Info field in the **FEM** tab is entered, the **RV tab** is automatically populated. The user can select the distribution and its parameters. This example applied the following conditions.


.. figure:: figures/SUR-VER4.png
   :align: center
   :figclass: align-center

5. Once the SurrogateGP Info field in the **FEM** tab is entered, the **QoI tab** is automatically populated. Users are allowed to remove some of the QoIs if not interested but may not add new QoIs or modify the names of existing QoIs.

.. figure:: figures/SUR-VER5.png
   :align: center
   :figclass: align-center

5. Click on the **Run** button. This will cause the back-end application to launch dakota.

6. When done, the **RES** tab will be selected and the results will be displayed. Below tables compare the results from the two cases.

* InterstoryDrift_x

================== ============ ======================== ======================================
Prob. Moments      Reference    CASE1 (only surrogate)   CASE1 (surrogate and simulation)					     
================== ============ ======================== ======================================
Mean               4.33e-4      4.33e-4                  4.33e-4
StdDev             1.66e-5      1.71e-5                  1.70e-5
Skewness           1.21         1.20                     1.23
Kurtosis           6.74         6.51                     6.54
================== ============ ======================== ======================================

* Accel_Roof_x

================== ============ ======================== ======================================
Prob. Moments      Reference    CASE1 (only surrogate)   CASE1 (surrogate and simulation)		
================== ============ ======================== ======================================
Mean               3.74e1       3.73e1                   3.75e1
StdDev             1.84         1.62                     1.80
Skewness           3.88e-1      5.87e-2                  2.00e-1
Kurtosis           1.85         2.12                     1.97
================== ============ ======================== ======================================


* CASE1:

.. figure:: figures/SUR-VER6.png
   :align: center
   :figclass: align-center


* CASE2: 

.. figure:: figures/SUR-VER7.png
   :align: center
   :figclass: align-center

.. note::
   If the user wants to inspect the simulation status or check error/warning messages related to the surrogate model, they can refer to the messages written at: ``{Local Jobs Directory}/tmp.SimCenter/surrogateLog.err``. (Note: ``{Local Jobs Directory}`` is set in the file-preference.)