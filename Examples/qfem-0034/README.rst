.. _qfem-0034:

Surrogate-aided Bayesian Calibration of a Reinforced Concrete Column Using Experimental Cyclic Test Data
========================================================================================================

+---------------+----------------------------------------------+
| Problem files | :github:`Github <Examples/qfem-0034/>`       |
+---------------+----------------------------------------------+

.. _rc_column_calibration:

Outline
-------

This example demonstrates the application of surrogate-aided Bayesian calibration [Taflanidis_et_al_2025]_ to identify material and section parameters of a reinforced concrete column model using experimental cyclic loading data. The study showcases quoFEM's new **Use Approximation** feature for Bayesian calibration, which employs Gaussian Process (GP) surrogates within an adaptive Bayesian framework to dramatically reduce computational costs while maintaining calibration accuracy.

**Problem Setup**: A cantilever RC column (0.4m x 0.4m x 1.6m) subjected to cyclic lateral loading is modeled using OpenSeesPy with a lumped plasticity approach. Three critical parameters are calibrated: crack factor (stiffness degradation ratio), yield moment (Mp), and peak moment capacity (Mc) using 548 experimental force-displacement measurements. The experimental data is sourced from the PEER Structural Performance Database [PEER_SPD]_, specifically from a test conducted by [Soesianawati_et_al_1986]_.

**Methodology**: The surrogate-aided approach uses the GP-AB (Gaussian Process - Aided Bayesian calibration) algorithm [Taflanidis_et_al_2025]_, which iteratively builds a GP surrogate model through adaptive design of experiments. The method strategically selects training points using both exploitation (high-probability regions) and exploration (unexplored parameter space) strategies, requiring only 2 x (number of calibration parameters) evaluations per iteration.

**Key Findings**: The surrogate-aided calibration achieves substantial computational efficiency gains compared to direct TMCMC sampling, requiring orders of magnitude fewer model evaluations (in this case, 66 vs 78,000) while reducing wall-clock time by approximately an order of magnitude (13.4 vs 95.7 minutes). Despite this dramatic acceleration, the method maintains equivalent posterior accuracy with percentage differences in parameter means below 0.5%.

**Broader Impact**: This demonstration validates the surrogate-aided approach as a transformative tool for practical Bayesian calibration of complex structural models. The method enables rigorous uncertainty quantification for computationally expensive finite element models that would otherwise make Bayesian calibration prohibitively expensive, opening new possibilities for model validation and reliability assessment in structural engineering applications.

Files Required
--------------
1. :qfem-0034:`ColumnModel.py <src/ColumnModel.py>`: OpenSeesPy structural analysis script
2. :qfem-0034:`ColumnParams.py <src/ColumnParams.py>`: Parameter definitions for calibration
3. :qfem-0034:`experimentDisp.csv <src/experimentDisp.csv>`: Experimental displacement time history - used by the model
4. :qfem-0034:`experimentForce.csv <src/experimentForce.csv>`: Experimental force measurements for calibration

The structural model represents a cantilever reinforced concrete column subjected to cyclic lateral loading under constant axial force. The column exhibits nonlinear behavior including concrete cracking, plastic hinge formation, and hysteretic energy dissipation. Accurate parameter identification is essential for reliable seismic performance assessment of similar structural systems.

Experimental Setup and Numerical Model
--------------------------------------

Reinforced Concrete Column Test Program
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The calibration example uses experimental data from a cantilever reinforced concrete column test conducted by [Soesianawati_et_al_1986]_. This test is part of the PEER Structural Performance Database [PEER_SPD]_, which collates over 400 cyclic lateral-load tests of reinforced concrete columns from multiple research institutions worldwide. The database provides a comprehensive resource for validating computational models and benchmark data for calibration studies across various column geometries, reinforcement configurations, and loading protocols.

   .. figure:: figures/column_configuration.png
       :align: center
       :figclass: align-center

       Test specimen configuration [Soesianawati_et_al_1986]_

**Test Specimen and Setup**: The column specimen has a 400mm x 400mm square cross-section with a height of 1600mm, representing typical building column proportions. The reinforcement consists of 12 HD16 longitudinal bars with 13mm concrete cover, providing representative detailing for seismic design. The specimen is tested under combined loading conditions: constant axial compression (744 kN) and displacement-controlled cyclic lateral loading.

   .. figure:: figures/column_reinforcement.png
       :align: center
       :figclass: align-center

       Test specimen reinforcement details [Soesianawati_et_al_1986]_

**Loading Protocol and Response**: The test employs a quasi-static cyclic loading protocol that progressively increases displacement amplitude from small inelastic deformations to ultimate capacity at ±78mm (~5% drift ratio). This systematic approach applies multiple cycles at each amplitude level to characterize key behavioral transitions: concrete cracking, steel yielding, post-yield behavior, and near-collapse performance. The protocol captures strength degradation and stiffness deterioration under repeated loading, providing comprehensive data across all performance limit states relevant to seismic design. The resulting force-displacement relationship exhibits the full spectrum of reinforced concrete behavior including pronounced hysteretic energy dissipation and progressive deterioration characteristics essential for validating computational models.

   .. figure:: figures/hysteresis_curve_boxed.png
       :align: center
       :figclass: align-center
       
       Experimental force-displacement hysteretic response showing nonlinear behavior and degradation

**Measured Data**: The experimental dataset comprises 548 synchronized measurements of applied displacement and resulting lateral force, providing comprehensive information for parameter identification across multiple complete hysteretic cycles. This rich dataset spans the full spectrum of structural response and serves as the calibration target for the numerical model.

   .. figure:: figures/displacement_plot.png
       :align: center
       :figclass: align-center
       
       Experimental displacement time history used as input to the numerical model

   .. figure:: figures/force_plot.png
       :align: center
       :figclass: align-center
       
       Experimental force time history used for calibration

**Reference and Database Context**: This experimental dataset is documented in [Soesianawati_et_al_1986]_ and forms part of the PEER Structural Performance Database (SPD) [PEER_SPD]_, which contains over 400 cyclic lateral-load tests of reinforced concrete columns. The database is available at `https://nisee.berkeley.edu/spd/index.html <https://nisee.berkeley.edu/spd/index.html>`_ and serves as a comprehensive validation resource for computational models, providing benchmark data for calibration studies across various column geometries, reinforcement details, and loading protocols.

For additional background on uncertainty quantification in structural engineering and more details of this calibration problem, refer to the SimCenter educational module on **Uncertainty Quantification (UQ) for structural models** [SimCenter_Educational_Modules]_.

Numerical Model Development
~~~~~~~~~~~~~~~~~~~~~~~~~~~

A lumped plasticity finite element model is developed in OpenSeesPy to reproduce the experimental conditions and enable parameter identification through direct comparison with the measured response. The model serves as a computational analog to the physical test, replicating the geometry, loading conditions, and material configurations.

   .. figure:: figures/model.png
       :align: center
       :figclass: align-center
       
       Numerical model schematic showing lumped plasticity approach with calibration parameters. Figure from [SimCenter_Educational_Modules]_.

The computational model employs a lumped plasticity approach where nonlinear behavior is concentrated in a rotational spring at the column base, while the remainder of the structure is treated as elastic. This modeling strategy efficiently captures essential global response characteristics while maintaining computational efficiency. Only the top half of the column is modeled, taking advantage of the symmetry of the double-ended cantilever test specimen configuration. 

**Key Modeling Components**: The model consists of an elastic beam-column element representing the column shaft with effective stiffness accounting for cracking, connected to a nonlinear rotational spring at the base that captures plastic hinge behavior through a hysteretic material model. Applied loads match the experimental conditions with 744 kN axial compression and the 548-point displacement history, while P-Delta geometric transformation is included to account for second-order effects at large displacements.

**Computational Considerations**: Each OpenSeesPy analysis requires approximately 2 seconds per iteration. Direct Bayesian calibration methods typically require thousands of model evaluations, resulting in potentially hours of computational time. This computational demand provides strong motivation for the surrogate-aided approach demonstrated in this example. However, the model is still simple enough to allow direct TMCMC sampling for validation purposes. In more realistic scenarios involving complex finite element models, individual evaluations may require minutes or hours rather than seconds, making direct Bayesian calibration prohibitively expensive. The surrogate-aided approach enables practical calibration of such complex models by dramatically reducing the number of required evaluations.

Calibration Framework
~~~~~~~~~~~~~~~~~~~~~

**Parameter Selection**: Three parameters are identified for calibration based on their direct influence on the global force-displacement response and their inherent uncertainty in engineering practice: crack factor (affecting initial stiffness), yield moment Mp (controlling yield transition), and peak moment capacity Mc (governing ultimate strength). These parameters collectively determine the key characteristics of the hysteretic envelope and are subject to significant uncertainty due to material variability and modeling assumptions.

**Surrogate-Aided Approach**: The parameter identification employs the GP-AB (Gaussian Process - Adaptive Bayesian) algorithm, which uses Gaussian process surrogates within an adaptive Bayesian framework to dramatically reduce computational cost while maintaining calibration accuracy. This approach transforms a computationally intensive problem requiring several thousands of model evaluations into a practical tool requiring only tens or hundreds of strategically selected evaluations.

**Educational Context**: This example demonstrates practical application of advanced uncertainty quantification methods for structural model calibration. The combination of experimental data from the PEER database with modern computational methods illustrates how historical experimental investments continue to enable methodological advances in application of uncertainty quantification in structural engineering. For background on uncertainty quantification concepts in structural engineering, see the SimCenter educational resources on `UQ for Structural Models <https://simcenter.designsafe-ci.org/knowledge-hub/teaching-gallery/>`_.

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

Comparison with Direct TMCMC Results
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For validation purposes, the same calibration was performed without the surrogate approximation (i.e., with the **Use Approximation** option disabled). The following figures show the comparison between surrogate-aided and direct TMCMC results, demonstrating that the surrogate model effectively captures the essential behavior of the structural model while providing substantial computational savings:

   .. figure:: figures/RES5.png
       :align: center
       :figclass: align-center

   .. figure:: figures/RES6.png
       :align: center
       :figclass: align-center

   .. figure:: figures/RES7.png
       :align: center
       :figclass: align-center

Discussion
----------

The Bayesian calibration successfully identified the three key parameters of the reinforced concrete column model using the experimental cyclic loading data. The results demonstrate a significant reduction in parameter uncertainty compared to the initial prior distributions, indicating that the experimental data provides valuable information for constraining the model parameters.

Parameter Calibration Results
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following table compares the prior and posterior statistical moments for the three calibrated parameters:

.. list-table:: Prior vs Posterior Parameter Statistics
   :header-rows: 1
   :widths: 20 15 15 15 15 20

   * - Parameter
     - Prior Mean
     - Prior Std Dev
     - Posterior Mean
     - Posterior Std Dev
     - Uncertainty Reduction*
   * - **crack_factor**
     - 0.425
     - 0.217
     - 0.249
     - 0.004
     - 98.0%
   * - **Mp** (kN-m)
     - 300.0
     - 115.5
     - 294.3
     - 4.31
     - 96.3%
   * - **Mc** (kN-m)
     - 350.0
     - 144.3
     - 459.6
     - 15.1
     - 89.6%

*Uncertainty reduction = (1 - Posterior Std Dev / Prior Std Dev) x 100%*

Key Findings
~~~~~~~~~~~~

**Crack Factor Parameter**: The posterior distribution of the crack factor shows the most dramatic uncertainty reduction (98.0%), with the calibrated value converging to approximately 0.25. This suggests that the experimental data strongly constrains the ratio of cracked to uncracked section moment of inertia, indicating that the column exhibits significant stiffness degradation due to cracking under cyclic loading.

**Yield Moment (Mp)**: The calibrated yield moment of 294.3 kN-m is close to the prior mean of 300.0 kN-m, but with a substantial reduction in uncertainty (96.3%). The narrow posterior distribution (standard deviation of 4.31 kN-m) indicates high confidence in the identified yield capacity of the plastic hinge.

**Peak Moment Capacity (Mc)**: The posterior mean of 459.6 kN-m is significantly higher than the prior mean of 350.0 kN-m, suggesting that the experimental data reveals a higher moment capacity than initially expected. Despite having the largest absolute posterior standard deviation (15.1 kN-m), this parameter still shows an 89.6% reduction in uncertainty.

Comparison with Results Without Surrogate
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To validate the effectiveness of the surrogate-aided calibration approach, a comparison was performed between results obtained with and without the surrogate approximation:

.. list-table:: Surrogate vs Direct TMCMC Comparison
   :header-rows: 1
   :widths: 20 15 15 15 15 15 15

   * - Parameter
     - With Surrogate Mean
     - With Surrogate Std
     - Without Surrogate Mean  
     - Without Surrogate Std
     - Mean Difference
     - Std Difference
   * - **crack_factor**
     - 0.249
     - 0.004
     - 0.249
     - 0.005
     - 0.0002
     - 0.0006
   * - **Mp** (kN-m)
     - 294.3
     - 4.31
     - 294.5
     - 4.49
     - 0.17
     - 0.18
   * - **Mc** (kN-m)
     - 459.6
     - 15.1
     - 457.4
     - 15.8
     - 2.19
     - 0.73

**Validation of Surrogate Model Performance**: The comparison demonstrates excellent agreement between the two approaches, with percentage differences in posterior means being less than 0.5% for all parameters. The small differences in standard deviations indicate that both methods provide similar uncertainty quantification. The largest absolute difference occurs in the Mc parameter (2.19 kN-m difference in mean), but this represents less than 0.5% of the parameter value, which is negligible for practical engineering applications.

Computational Efficiency
~~~~~~~~~~~~~~~~~~~~~~~~~

The log file analysis reveals dramatic computational efficiency gains achieved by the surrogate-aided approach:

.. list-table:: Computational Performance Comparison
   :header-rows: 1
   :widths: 30 25 25 20

   * - Metric
     - Surrogate-Aided Method
     - Direct TMCMC Method
     - Efficiency Gain
   * - **Total Runtime**
     - 13.4 minutes
     - 95.7 minutes
     - 7.1x faster
   * - **Model Evaluations**
     - 66
     - 78,000
     - 1,182x fewer

The surrogate-aided method demonstrates transformative computational efficiency by requiring only 66 strategically selected model evaluations compared to 78,000 needed for direct TMCMC sampling. This remarkable 1,182-fold reduction in computational demand is achieved through an intelligent adaptive design of experiments strategy that systematically adds 6 training points per iteration (following the 2xn_θ = 2x3 pattern for the three calibration parameters). Each iteration uses both exploitation points (focused on high-posterior-probability regions) and exploration points (covering unexplored parameter space) to maximize information gain from the GP surrogate model.

The 7.1x speedup in wall-clock time represents an 86% reduction in computational time, transforming what would be a 95.7-minute direct calibration into a 13.4-minute surrogate-aided process. This efficiency gain becomes even more significant for complex finite element models where individual evaluations may require minutes or hours rather than < 2 seconds. The surrogate-aided approach thus enables practical Bayesian calibration of computationally intensive structural models that would otherwise be prohibitively expensive to analyze.

Model Performance Assessment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The calibration results indicate that the experimental data is highly informative for all three parameters, as evidenced by the substantial uncertainty reductions across all variables. The posterior parameter values are physically reasonable:

- The crack factor of 0.25 represents a realistic level of stiffness degradation for reinforced concrete under cyclic loading
- The yield and peak moment capacities (294.3 and 459.6 kN-m, respectively) show appropriate strength hierarchy with Mp < Mc
- The moment capacity values are consistent with the column's geometry (0.4m x 0.4m section) and loading conditions

The successful calibration demonstrates that the lumped plasticity model with hysteretic behavior effectively captures the essential nonlinear response characteristics observed in the experimental cyclic test. The calibrated model can now be used with increased confidence for seismic performance assessment of similar reinforced concrete column systems.

References
----------

.. [Taflanidis_et_al_2025] 
   Alexandros A. Taflanidis, B.S. Aakash, Sang-ri Yi, and Joel P. Conte, "Surrogate-aided Bayesian calibration with adaptive learning strategies," *Mechanical Systems and Signal Processing*, Volume 237, 2025, 113014, https://doi.org/10.1016/j.ymssp.2025.113014.

.. [Soesianawati_et_al_1986] 
   Soesianawati, M.T., Park, R., and Priestley, M.J.N. (1986). "Limited Ductility Design of Reinforced Concrete Columns." Report 86-10, Department of Civil Engineering, University of Canterbury, Christchurch, New Zealand.

.. [PEER_SPD] 
   PEER Structural Performance Database (SPD). Available at https://nisee.berkeley.edu/spd/index.html

.. [SimCenter_Educational_Modules] 
   SimCenter Educational Modules: UQ for Structural Models. Available at https://simcenter.designsafe-ci.org/knowledge-hub/teaching-gallery/