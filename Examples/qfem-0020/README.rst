Hierarchical Bayesian Calibration of Material Model Parameters using CustomUQ Engine
=================================================

+---------------+----------------------------------------------+
| Problem files | :github:`Github <Examples/qfem-0020/>`       |
+---------------+----------------------------------------------+

Outline
-------
In this example, simulated data of the response of 36 steel coupons, subjected to six different strain histories are used to calibrate the the parameters of the STEEL02 material model in OpenSees corresponding to each coupon, as well as the parameters of a multivariate normal distribution defining the distribution of the calibrated STEEL02 model parameters across the set of coupons, using Hierarchical Bayesian Calibration. The synthetic data is generated using the STEEL02 model to represent the response of steel coupons, where each coupon's material parameters are randomly sampled from a multivariate normal distribution. 

Problem description
-------------------
Data
++++
The stress-strain data generated synthetically from simulation of a cyclic test on a coupon prepared from a bar of reinforcing steel is shown in :numref:`figExperimentalDataSteelCoupon`. 


.. _figExperimentalDataSteelCoupon:

.. figure:: figures/qf-0019-StressStrainData.png
   :align: center
   :alt: Image showing error in description
   :width: 400
   :figclass: align-center
   
   Sample stress-strain curve from cyclic test on test coupon from reinforcing steel bar.


This sample stress-strain data was obtained corresponding to a randomized strain history similar to those observed experimentally in reinforcing steel during seismic tests, as shown in :numref:`figExperimentalDataSteelCouponStrain`. 

.. _figExperimentalDataSteelCouponStrain:

.. figure:: figures/qf-0019-StrainHistory.png
   :align: center
   :alt: Image showing error in description
   :width: 400
   :figclass: align-center
   
   The test coupon was subjected to this strain history.


Six such strain histories, shown in :numref:`figCouponStrainHistories` are used to generate synthetic stress-strain data from the Steel02 model in OpenSees. Six sets of stress-strain data are obtained corresponding to each of the six strain histories, thereby resulting in a set of 36 synthetic stress-strain curves, that are used in this hierarchical Bayesian calibration example. To generate the stress response, a cyclic uniaxial test was simulated with the Steel02 model in OpenSees, with the material model parameter values sampled randomly from an assumed multivariate Gaussian distribution for the parameters. Zero-mean idenically distributed independent Gaussian noise was added to the stress response obtained from the OpenSees model to generate the stress-strain data.

.. _figCouponStrainHistories:

.. figure:: figures/StrainHistories.png
   :align: center
   :alt: Image showing error in description
   :width: 400
   :figclass: align-center
   
   Strain histories used to generate synthetic data.


Hierarchical Model
++++++++++++++++++
For this example, the STEEL02 material model in OpenSees was selected to represent the cyclic stress-strain response of the steel reinforcing bar in finite element simulations. The STEEL02 material model in OpenSees can take a total of 11 parameter values as input, as described in the `documentation <https://opensees.berkeley.edu/wiki/index.php/Steel02_Material_--_Giuffré-Menegotto-Pinto_Model_with_Isotropic_Strain_Hardening>`_. Of these 11 parameters, the value of 7 parameters shown in `Table 1`_ will be calibrated in this example.

.. _Table 1:

Table 1: Parameters of the STEEL02 material model whose values are being calibrated. 

==========================================================  
Variable                                                    
==========================================================  
Yield strength :math:`f_y`                                  
Initial elastic tangent :math:`E`                           
Strain hardening ratio :math:`b`                            
Elastic-plastic transition parameter 1 :math:`cR_1`    	   
Elastic-plastic transition parameter 2 :math:`cR_2`         
Isotropic hardening parameter for compression :math:`a_1`   
Isotropic hardening parameter for tension :math:`a_3`       
==========================================================  
	 

The value of the other four parameters are kept fixed at:

==========================================================  =========== 
Variable                                                    Value
==========================================================  =========== 
Elastic-plastic transition parameter :math:`R_0`            20
Isotropic hardening parameter for compression :math:`a_2`   1
Isotropic hardening parameter for tension :math:`a_4`       1
Initial stress value :math:`sigInit`                        0
==========================================================  =========== 

For each coupon, the estimated parameter values will be different, since the data was generated using parameters drawn randomly from a multivariate Gaussian distribution. The variability in the estimated parameters from coupon-to-coupon is also modeled using a multivariate Gaussian distribution with unknown parameters being the mean vector and the covariance matrix of this multivariate Gaussian distribution.

The prediction error for each experiment is assumed to be independent. The standard deviation of the prediction error for each coupon are also estimated.

 
Parameter estimation setup
++++++++++++++++++++++++++
In this example, the values of the parameters shown in `Table 1`_ are being estimated, along with the hyperparameters which are the mean vector and the covariance matrix of the multivariate Gaussian distribution defining the distribution of the population of the estimated, and the standard deviation of the prediction errors for each coupon. The unknown parameters are the material model parameters of each coupon, :math:`\mathbf{\theta_i}=(f_y, E, b, cR_1, cR_2, a_1, a_3)^T; i \in {1, \ldots, 36}`, which are estimated using the data of the stress response corresponding to the strain history for that coupon, the mean vector :math:`\mathbf{\mu_\theta}`, the covariance matrix :math:`\mathbf{\Sigma_theta}`, and the standard deviation of the prediction errors :math:`\sigma_i`. 

The prediction errors (i.e. the differences between the finite element prediction of the stress history and the experimentally obtained stress history) are assumed to follow a zero-mean Gaussian distribution. The components of the error vector are assumed to be statistically independent and identically distributed. Under this assumption, the standard deviation of the error is also an unknown parameter of the likelihood model and is also estimated during the calibration process.

All of these parameters are jointly estimated using Bayesian calibration.


Files required
--------------
The exercise requires one script file and two data files. The user should download these files and place them in a **new** folder. 

.. warning::
   Do not place the files in your root, downloads, or desktop folder as when the application runs it will copy the contents on the directories and subdirectories containing these files multiple times. If you are like us, your root, Downloads or Documents folders contains a lot of files.

1. :qfem-0019:`matTestAllParamsReadStrain.tcl <src/matTestAllParamsReadStrain.tcl>` - This is an OpenSees script written in tcl which simulates a material test and writes the stress response (in a file called ``results.out``) when subjected to the chosen strain history, for a given value of the parameters of the material model. 

2. :qfem-0019:`stress.1.coords <src/stress.1.coords>` - This file contains the strain history that is used as input during the finite element simulation of the material response. The strain values stored in this file are read in by the tcl script performing the OpenSees analysis.


3. :qfem-0019:`calDataField.csv <src/calDataField.csv>` - This is a csv file that contains the stress data. There is one row of data, which implies that the data is obtained from one experiment. If additional data are available from other experiments, then the data from each experiment must be provided on separate lines.

.. note::
   Since the tcl script creates a ``results.out`` file when it runs, no postprocessing script is needed. 

UQ workflow
-----------
.. note::
	Selecting the ``Material Model: Bayesian Calibration with TMCMC`` example in the quoFEM Examples menu will autopopulate all the input fields required to run this example. 
	The procedure outlined below demonstrates how to manually set up this problem in quoFEM.

The steps involved are as follows:

1. Start the application and the **UQ** panel will be highlighted. In the **UQ Engine** drop down menu, select the **UCSD_UQ** engine. In the **Method** category drop down menu the **Transitional Markov chain Monte Carlo** option will be highlighted. Enter the values in this panel as shown in the figure below. If manually setting up this problem, choose the path to the file containing the calibration data on your system. 

.. figure:: figures/qf-0019-UQ.png
   :align: center
   :figclass: align-center


2. Next select the **FEM** panel from the input panel selection. This will default to the **OpenSees** FEM engine. In the **Input Script** field, enter the path to the ``matTestAllParamsReadStrain.tcl`` file or select **Choose** and navigate to the file. 

.. figure:: figures/qf-0019-FEM.png
   :align: center
   :figclass: align-center


3. Next select the **RV** tab from the input panel. This panel should be pre-populated with seven random variables. If not, press the **Add** button to create new fields to define the input random variables. Enter the same variable names, as required in the model script. 

For each variable, specify the prior probability distribution and its parameters, as shown in the figure below. 

.. figure:: figures/qf-0019-RV.png
   :align: center
   :figclass: align-center


4. In the **QoI** panel denote that the variable named ``stress`` is not a scalar response variable, but has a length of 342.

.. figure:: figures/qf-0019-QOI.png
   :align: center
   :figclass: align-center

5. Next click on the **Run** button. This will cause the backend application to launch the **UCSD_UQ** engine, which performs Bayesian calibration using the TMCMC algorithm. When done, the **RES** tab will be selected and the results will be displayed as shown in the figure below. The results show the first four moments of the posterior marginal probability distribution of the parameters estimated in this example. Also shown are the moments of the additional parameter of the likelihood function. Finally, the moments of the predictions of the model corresponding to the samples of the parameter values from their posterior probability distribution are also shown in this panel (not visible in this figure - you can see them by scrolling down in the application).


.. figure:: figures/qf-0019-RES1.png
   :align: center
   :figclass: align-center


If the user selects the **Data Values** tab in the results panel, they will be presented with both a graphical plot and a tabular listing of the data.

.. figure:: figures/qf-0019-RES2.png
   :align: center
   :figclass: align-center


Comaparison with deterministic calibration results
--------------------------------------------------
For the same data and choice of material model to represent the data, deterministic estimation of the parameters of the material model shown in `Table 1`_ was also conducted in quoFEM using the non-linear least squares minimization algorithm available through the **Dakota** UQ engine. 

The bounds and the starting point of the search for the optimum parameter values are shown in `Table 2`_.

.. _Table 2:
 
Table 2: Parameters of the STEEL02 material model whose optimum values are being estimated.

==========================================================  =========== =========== =============
Variable                                                    lower bound upper bound initial point
==========================================================  =========== =========== =============
Yield strength :math:`f_y`                                  300		    700			500
Initial elastic tangent :math:`E`                           100000	    300000		200000
Strain hardening ratio :math:`b`                            1e-6        1			0.5
Elastic-plastic transition parameter 1 :math:`cR_1`    	    1e-6        2			1
Elastic-plastic transition parameter 2 :math:`cR_2`         1e-6    	2			1
Isotropic hardening parameter for compression :math:`a_1`   1e-6    	0.5			0.25
Isotropic hardening parameter for tension :math:`a_3`       1e-6    	0.5			0.25
==========================================================  =========== =========== =============

Like in the Bayesian paramter estimation case, the value of the other four parameters are kept fixed at:

==========================================================  =====
Variable                                                    Value
==========================================================  =====
Elastic-plastic transition parameter :math:`R_0`            20
Isotropic hardening parameter for compression :math:`a_2`   1
Isotropic hardening parameter for tension :math:`a_4`       1
Initial stress value :math:`sigInit`                        0
==========================================================  =====


Solution using quoFEM
+++++++++++++++++++++

.. note::
	Selecting the ``Material Model: Deterministic Calibration`` example in the quoFEM Examples menu will autopopulate all the input fields required to run this example. 

The inputs in the **FEM** and the **QoI** panels are the same as in the Bayesian parameter estimation case. The inputs that differ from the Bayesian parameter estimation case are shown in the figures below:


**UQ** panel:

.. figure:: figures/qf-0018-UQ.png
   :align: center
   :figclass: align-center

**RV** panel:

.. figure:: figures/qf-0018-RV.png
   :align: center
   :figclass: align-center


Results
+++++++
After conducting the deterministc parameter estimation, the results obtained are shown in the figure below:

.. figure:: figures/qf-0018-RES1.png
   :align: center
   :figclass: align-center
   
The optimum parameter values estimated in this example match closely match the mean value of the posterior samples shown in the figure of the summary tab of the results panel for the Bayesian parameter estimation case.


The fit corresponding to the optimum parameter values is shown in the figures below:

.. figure:: figures/qf-0019-StressResults.png
   :align: center
   :figclass: align-center

.. figure:: figures/qf-0019-DeterministicCalibrationResults.png
   :align: center
   :figclass: align-center
   