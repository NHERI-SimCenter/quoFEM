
Two-Dimensional Truss: Global Sensitivity Analysis using SimCenterUQ Engine
===========================================================================

+----------------+------------------------------------------+
| Problem files  | :github:`Github <Examples/qfem-0009/>`   |
+----------------+------------------------------------------+


This example uses quoFEM to perform a global sensitivity analysis of an
OpenSees FE model.

Consider a stochastic model of a two-dimensional truss structure like
that shown in the following figure. A sensitivity
analysis procedure is coordinated by quoFEM which will estimate the sensitivities
of the response quantities of interest with respect to the problem’s
random variables.


.. figure:: figures/qfem-0009.png
   :align: center
   :width: 400
   :figclass: align-center


The following parameters are defined in the **RV** tab of quoFEM:

1. Elastic modulus, ``E``: **Weibull** distribution with a scale
   parameter :math:`(\lambda)` of :math:`210.0`, shape parameter
   :math:`(k)` of :math:`20.0`,

2. Load magnitude, ``P``: **Beta** distribution with a first shape
   parameter :math:`(\alpha)` of :math:`2.0`, second shape parameter
   :math:`(\beta)` of :math:`2.0`, lower bound :math:`(L_B)` of
   :math:`20.0`, upper bound :math:`(U_B)` of :math:`30.0`,

3. Cross sectional area for the other six bars, ``Ao``: **Lognormal**
   distribution with a mean :math:`(\mu)` of :math:`250.0`, standard
   deviation :math:`(\sigma)` of :math:`50.0`,

4. Cross sectional area for the upper three bars, ``Au``: **Normal**
   distribution with a mean :math:`(\mu)` of :math:`500.0`, standard
   deviation :math:`(\sigma)` of :math:`100.0`,


.. figure:: figures/trussSensitivity-input-SimUQ.png
   :alt: Random variables for simple truss.
   :align: center


   Random variables for simple truss.


.. figure:: figures/trussPDF.png
   :alt: Random variables for simple truss.
   :align: center


   Probability density function of E (press Show PDF button)

The vertical displacement at node 2 is considered as a response. It is specified in the **QoI** tab as ``Node_2_Disp_2``.

UQ Workflow
-----------

To define the uncertainty workflow in quoFEM, select the **SimCenterUQ** engine
and specify **Sensitivity Analysis** and **Monte Carlo** for the 
**UQ Method Category** and **Method** combination boxes, respectively.
Finally, enter the following parameters for the remaining inputs.

=========== ====
**Samples** 1000
**Seed**    175
=========== ====

Model Files
-----------

The following files make up the **FEM** model definition.

#. :qfem-0009:`TrussModel.tcl <src/TrussModel.tcl>`:
   This file is an OpenSees Tcl script that constructs and runs a finite
   element analysis of the truss for a given realization of the
   problem’s random variables. It is supplied to the **Input File**
   field of the **FEM** tab.

#. :qfem-0009:`TrussPost.tcl <src/TrussPost.tcl>`:
   This file is an OpenSees Tcl script that processes the QoI
   identifiers supplied in the **QoI** tab, and writes the relevant
   response quantities to ``results.out`` from an OpenSees process. It
   is supplied to the **Postprocess File** field of the **FEM** tab.


Results
-------

Once the analysis is complete the **RES** panel will be automatically
selected and the results will be displayed as shown in the following
figure:

.. figure:: figures/trussSensitivity-RES-SimUQ.png
   :alt: Sensitivity analysis results for simple truss.
   :align: center

   Sensitivity analysis results for simple truss.


.. |Truss schematic diagram| image:: ./qfem-0009.png


Comparison with Dakota engine
-----------------------------

QuoFEM also supports MCS-based sensitivity analysis method offered in Dakota engine. In this section, the sensitivity analysis is conducted using different number of samples, and the results were compared with those from Dakota engine. 

.. list-table:: SimCenterUQ: Sensitivity indices from different number of samples 
   :header-rows: 1
   :align: center

   * - RV
     - N=10
     - N=50
     - N=100
     - N=500
     - N=1000
   * - E
     - 0.18
     - 0.23
     - 0.23
     - 0.12
     - 0.13
   * - P
     - 0.60
     - 0.36
     - 0.29
     - 0.24
     - 0.25
   * - Ao
     - 0.06
     - 0.45
     - 0.42
     - 0.43
     - 0.46
   * - Au
     - 0.26
     - 0.29
     - 0.25
     - 0.18
     - 0.16

.. list-table:: Dakota: Sensitivity indices from different number of samples (the first raw is the user-specified number of samples and the second raw is the actual number of simulation calls)
   :header-rows: 2
   :align: center

   * - RV
     - N=10 
     - N=50
     - N=100
     - N=500
     - N=2000     
   * - 
     - **(60)**
     - **(300)**
     - **(600)**
     - **(3000)**
     - **(12000)**
   * - E
     - 0.06
     - 0.17
     - 0.18
     - 0.12
     - 0.13
   * - P
     - 0.17
     - 0.22
     - 0.20
     - 0.28
     - 0.22
   * - Ao
     - 0.19
     - 0.41
     - 0.52
     - 0.46
     - 0.47
   * - Au
     - 0.03
     - 0.12
     - 0.23
     - 0.20
     - 0.18

In the current example, the probability model (PM) based approximation method in the SimCenterUQ engine converges faster within a smaller number of simulations. However, caution should be taken when using PM-based method. It is noteworthy that while the MCS method in the Dakota engine always converges to exact value when a sufficiently large number of simulations are involved, such asymptotic exactness is not guaranteed for the PM-based method. Still, the sensitivity rank between the variables is relatively robust.


Result with correlated inputs
------------------------------
Consider the case with correlated random variables. For example, below shows correlation matrix where the correlation between the variables ``Au`` and ``Ao`` are :math:`\rho=0.5`.

.. figure:: figures/trussSensitivity-corr.png
   :alt: Sensitivity analysis results for simple truss.
   :align: center

   Correlation matrix

The group-wise Sobol indices for {:math:`Au,Ao`} can be defined at the **QoI** tab.

.. figure:: figures/trussSensitivity-advanced.png
   :alt: Sensitivity analysis results for simple truss.
   :align: center

   Setting advanced options for group-wise Sobol indices

The results of sensitivity analysis:

.. figure:: figures/trussSensitivity-RES-SimUQ2.png
   :alt: Sensitivity analysis results for simple truss.
   :align: center

   Sensitivity analysis results for simple truss (with correlations)


