.. _qfem-0034:

Surrogate-aided Bayesian Calibration of a Reinforced Concrete Column Using Experimental Cyclic Data
===================================================================================================

+---------------+----------------------------------------------+
| Problem files | :github:`Github <Examples/qfem-0034/>`       |
+---------------+----------------------------------------------+

.. _rc_column_calibration:

Outline
-------

This example demonstrates the application of surrogate-aided Bayesian calibration to identify material and section parameters of a reinforced concrete column model using experimental cyclic loading data. The study showcases quoFEM's new **Use Approximation** feature, which employs Gaussian Process (GP) surrogates within an adaptive Bayesian framework to dramatically reduce computational costs while maintaining calibration accuracy.

**Problem Setup**: A cantilever RC column (0.4m x 0.4m x 1.6m) subjected to cyclic lateral loading is modeled using OpenSeesPy with a lumped plasticity approach. Three critical parameters are calibrated: crack factor (stiffness degradation ratio), yield moment (Mp), and peak moment capacity (Mc) using 548 experimental force-displacement measurements.

**Methodology**: The surrogate-aided approach uses the GP-AB (Gaussian Process - Aided Bayesian calibration) algorithm, which iteratively builds a GP surrogate model through adaptive design of experiments. The method strategically selects training points using both exploitation (high-probability regions) and exploration (unexplored parameter space) strategies, requiring only 2xn_θ evaluations per iteration.

**Key Findings**: The surrogate-aided calibration achieves substantial computational efficiency gains compared to direct TMCMC sampling, requiring orders of magnitude fewer model evaluations (in this case, 66 vs 78,000) while reducing wall-clock time by approximately an order of magnitude (13.4 vs 95.7 minutes). Despite this dramatic acceleration, the method maintains equivalent posterior accuracy with percentage differences in parameter means below 0.5%.

**Parameter Identification Results**: All three parameters show significant uncertainty reduction (89-98%) compared to prior distributions. The calibrated crack factor of ~0.25 indicates realistic stiffness degradation, while the moment capacities reveal appropriate strength hierarchy (Mp < Mc) consistent with RC behavior under cyclic loading.

**Broader Impact**: This demonstration validates the surrogate-aided approach as a transformative tool for practical Bayesian calibration of complex structural models. The method enables rigorous uncertainty quantification for computationally intensive finite element models that would otherwise make Bayesian calibration prohibitively expensive, opening new possibilities for model validation and reliability assessment in structural engineering applications.

Files Required
--------------
1. :qfem-0034:`ColumnModel.py <src/ColumnModel.py>`: OpenSeesPy structural analysis script  
2. :qfem-0034:`ColumnParams.py <src/ColumnParams.py>`: Parameter definitions for calibration
3. :qfem-0034:`experimentDisp.csv <src/experimentDisp.csv>`: Experimental displacement time history - used by the model
4. :qfem-0034:`experimentForce.csv <src/experimentForce.csv>`: Experimental force measurements for calibration

The structural model represents a cantilever reinforced concrete column subjected to cyclic lateral loading under constant axial force. The column exhibits nonlinear behavior including concrete cracking, plastic hinge formation, and hysteretic energy dissipation. Accurate parameter identification is essential for reliable seismic performance assessment of similar structural systems.

Model Description
-----------------

Structural Model
~~~~~~~~~~~~~~~~

The reinforced concrete column is modeled in OpenSeesPy using a lumped plasticity approach. The column has a geometry of 0.4m x 0.4m cross-section and a height of 1.6m, representing a cantilever configuration. It is subjected to a constant axial load of 744 kN along with cyclic lateral displacement. The modeling approach utilizes an elastic beam-column element with a plastic hinge located at the base. Moment-rotation behavior is captured using a hysteretic material model, and secondary effects such as P-Delta geometric transformation are included in the analysis.

Model Parameters to be Calibrated
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following three parameters are selected for calibration based on their influence on the force-displacement response:

.. list-table:: Model Parameters for Calibration
   :header-rows: 1
   :widths: 20 20 20 20

   * - Parameter
     - Physical Significance
     - Prior Distribution
     - Units
   * - **crack_factor**
     - Ratio of cracked to uncracked section moment of inertia
     - Uniform(0.05, 0.8)
     - Dimensionless
   * - **Mp**
     - Positive yield moment of plastic hinge
     - Uniform(100, 500)
     - kN-m
   * - **Mc**
     - Positive peak moment capacity of plastic hinge
     - Uniform(100, 600)
     - kN-m

Experimental Data
~~~~~~~~~~~~~~~~~

The calibration data comprises force-displacement measurements obtained from cyclic testing of the reinforced concrete column. The loading protocol involved displacement-controlled cyclic loading with progressively increasing amplitude. During the test, lateral force and top displacement were measured at each step, resulting in a total of 548 displacement steps and corresponding force measurements. The recorded response exhibits pronounced hysteretic behavior, including strength degradation, which is characteristic of reinforced concrete columns subjected to cyclic loading.

quoFEM Setup
------------

The calibration is performed using quoFEM with the following configuration:

**Step 1: UQ Tab - Bayesian Calibration Settings**: In the UQ tab, select **Bayesian Calibration** as the method and choose **UCSD-UQ** as the UQ engine. For the model type, select **Non-hierarchical**, which utilizes the TMCMC algorithm for posterior sampling. Set the sample size to 1000 and the random seed to 20 to ensure reproducibility. Specify the calibration data file by providing the path to `experimentForce.csv`. To accelerate the calibration process, enable the **Use Approximation** option, which allows surrogate-aided Bayesian calibration. The figure below illustrates the recommended UQ tab configuration.

   .. figure:: figures/UQ.png
       :align: center
       :figclass: align-center

**Step 2: Forward Model (FEM) Tab**: In the FEM tab, choose **Python** as the FEM engine. Provide the full path to `ColumnModel.py` as the input script and the full path to `ColumnParams.py` as the parameters script.

   .. figure:: figures/FEM.png
       :align: center
       :figclass: align-center

**Step 3: Input Random Variables (RV) Tab**: Define the prior distribution for the three parameters to be calibrated:

.. list-table:: Random Variables for Calibration
   :header-rows: 1
   :widths: 20 20 20 20

   * - Variable Name
     - Distribution
     - Min.
     - Max.
   * - crack_factor
     - Uniform
     - 0.05
     - 0.8
   * - Mp
     - Uniform
     - 100
     - 500
   * - Mc
     - Uniform
     - 100
     - 600

.. figure:: figures/RV_panel.png
      :align: center
      :figclass: align-center

**Step 4: Output Quantities of Interest (QoI) Tab**: Define the response quantities to match against experimental data:

   .. code-block:: none

       QoI1: 
       - Variable Name: force
       - Length: 548 (matching experimental data points)

   .. figure:: figures/QoI.png
       :align: center
       :figclass: align-center

**Step 5: Execution**

- Click the **RUN** button to start the calibration process on your local machine.
- Or, click the **RUN at DesignSafe** button to submit the job to DesignSafe-CyberInfrastructure and utilize the parallel computing resources provided by DesignSafe.

Results
-------
Upon completion of the calibration, quoFEM generates summary statistics and visualizations to help assess the results:

1. **Summary statistics**: In the **Summary** tab of the results (**RES**) panel, key metrics such as the posterior mean and posterior standard deviation for each calibrated parameter are provided. These statistics are also shown for each component of the force response.

   .. figure:: figures/RES1.png
       :align: center
       :figclass: align-center

2. **Posterior samples**: The **Data Values** tab of the results (**RES**) panel displays charts for visualizing the posterior samples and a table containing the values drawn from the posterior distribution of the parameters and the corresponding model responses. Posterior samples can be exported as text files for further analysis by clicking the buttons in the **Data Values** tab. Clicking on the header rows of the chart will sort the values in ascending or descending order. 

   .. figure:: figures/RES2.png
       :align: center
       :figclass: align-center

You can toggle between visualizations such as scatter plots, histograms, and empirical cumulative distribution functions (CDFs) in the chart by left- and right-clicking on cells inside the chart area (not on the header rows). Right-clicking on a cell inside a column will plot the variable in that column along the x-axis, while left-clicking will plot that variable along the y-axis. 

   .. figure:: figures/RES3.png
       :align: center
       :figclass: align-center

   .. figure:: figures/RES4.png
       :align: center
       :figclass: align-center


By left- and right-clicking within the same column, you can plot the CDF of that variable. 

   .. figure:: figures/RES8.png
       :align: center
       :figclass: align-center


By right- and left-clicking the same column, you can plot the histogram of that variable. 

   .. figure:: figures/RES9.png
       :align: center
       :figclass: align-center

You can expand the chart by dragging the area between the chart and the table.

   .. figure:: figures/RES10.png
       :align: center
       :figclass: align-center


Discussion
----------
The posterior distributions of the calibrated parameters indicate a significant reduction in uncertainty compared to the prior distributions. The posterior mean and standard deviation for the parameters are as follows: (these results will not match exactly in another run due to the stochastic nature of the algorithm)




Comparison with results without surrogate
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The results obtained using surrogate-aided Bayesian calibration (with the **Use Approximation** option enabled) are consistent with those obtained without using a surrogate (with the **Use Approximation** option disabled). The posterior distributions of the calibrated parameters are similar in both cases. This indicates that the surrogate model effectively captures the essential behavior of the structural model, allowing for efficient calibration without significant loss of accuracy.

   .. figure:: figures/RES5.png
       :align: center
       :figclass: align-center

   .. figure:: figures/RES6.png
       :align: center
       :figclass: align-center

   .. figure:: figures/RES7.png
       :align: center
       :figclass: align-center