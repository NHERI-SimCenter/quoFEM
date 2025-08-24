.. _qfem-0003:

Reliability Analysis
====================

+-----------------+--------------------------------------------+
| Problem files   | :github:`Download <Examples/qfem-0003/>`   |
+-----------------+--------------------------------------------+

This example uses quoFEM to perform a second-order reliability analysis
(SORM) of an OpenSees FE model.

Consider the stochastic response of a two-dimensional truss structure
shown in the following figure with uncertain section dimensions,
material moduli, and loading magnitude. Two input scripts are used to
define a `local reliability </common/user_manual/usage/desktop/DakotaReliability.html>`__
procedure to be coordinated by quoFEM which will estimate response
magnitudes whose probabilities of exceedance are 0.02, 0.2, 0.4, 0.6,
0.8, and 0.99.

.. figure:: qfem-0003.png
   :width: 500px
   :alt: Engineering diagram of a truss structure with labeled joints 1 through 6 and spans marked as 4 meters each. Two vertical forces labeled 'P' act downwards at joints 2 and 3. Joint 1 is fixed, and joint 4 is on a roller support. The entire structure is 12 meters in length and 4 meters in height.
   :align: center

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

UQ Workflow
-----------

To define the uncertainty workflow in quoFEM, select **Reliability
Analysis** for the **Dakota Method Category**, and enter the following
inputs:

+--------------------------+------------------------------------+
| **Integration Method**   | Second Order                       |
+--------------------------+------------------------------------+
| **Level Type**           | Probability Levels                 |
+--------------------------+------------------------------------+
| **Local Method**         | Most Probable Point                |
+--------------------------+------------------------------------+
| **Reliability Method**   | Local Reliability                  |
+--------------------------+------------------------------------+
| **MPP Search Method**    | no_approx                          |
+--------------------------+------------------------------------+
| **Probability Levels**   | [0.02, 0.2, 0.4, 0.6, 0.8, 0.99]   |
+--------------------------+------------------------------------+

Model Files
-----------

The following files make up the **FEM** model definition.

#. `model.tcl <https://raw.githubusercontent.com/claudioperez/SimCenterExamples/master/static/truss/model.tcl>`__:
   This file is an OpenSees Tcl script that constructs and runs a finite
   element analysis of the truss for a given realization of the
   problem's random variables. It is supplied to the **Input File**
   field of the **FEM** tab.

#. `post.tcl <https://raw.githubusercontent.com/claudioperez/SimCenterExamples/master/static/truss/post.tcl>`__:
   This file is an OpenSees Tcl script that processes the QoI
   identifiers supplied in the **QoI** tab, and writes the relevant
   response quantities to ``results.out`` from an OpenSees process. It
   is supplied to the **Postprocess File** field of the **FEM** tab.


Results
-------

Once the analysis is complete the **RES** tab will be automatically
selected and the results will be displayed as shown in the following
figure:

.. figure:: figures/trussSORM-RES.png
   :alt: Reliability analysis (Second-order approximation)



Other reliability methods
-------------------------

**First Order** Integration Method: The user can change only the integration method to get the first order approximation (instead of the second order approximation) results.

+--------------------------+------------------------------------+
| **Integration Method**   | First Order Reliability            |
+--------------------------+------------------------------------+
| **Level Type**           | Probability Levels                 |
+--------------------------+------------------------------------+
| **Local Method**         | Most Probable Point                |
+--------------------------+------------------------------------+
| **Reliability Method**   | Local Reliability                  |
+--------------------------+------------------------------------+
| **MPP Search Method**    | no_approx                          |
+--------------------------+------------------------------------+
| **Probability Levels**   | [0.02, 0.2, 0.4, 0.6, 0.8, 0.99]   |
+--------------------------+------------------------------------+

.. figure:: figures/trussFORM-RES2.png
   :alt: Reliability analysis (First order approximation). A screenshot of a computer interface with a line graph and an accompanying data table. The graph plots 'Probability level' on the y-axis against 'Node_3_Disp_2' on the x-axis and shows a steady increase from lower left to upper right, suggesting a positive correlation between the variables. The x-axis ranges from 5.10 to 11.50, and the y-axis ranges from 0.00 to 1.00. Below the graph, there is a table with two columns labeled 'Pr(Node_3_Disp_2)' and 'Node_3_Disp_2', containing nine pairs of numerical data values corresponding to points on the graph. The sidebar on the left side of the image contains unidentified acronyms such as 'UQ', 'FEM', 'RV', 'QoI', and 'RES'.
   

**Global reliability** and **Importance sampling** methods can to identify probability levels corresponding to given response thresholds.

.. note::
   Global reliability and importance sampling cannot read **probability thresholds** as inputs.

Global reliability:

+--------------------------+------------------------------------+
| **GP Approx. Method**    | u-space                            |
+--------------------------+------------------------------------+
| **Response Levels**      | [5.0 6.0 7.0 8.0 9.0]              |
+--------------------------+------------------------------------+

.. figure:: figures/trussGP-RES2.png
   :alt: Global reliability results. A line graph displaying a positive linear relationship, with the horizontal axis labeled "Node_3_Disp_2" ranging from 5.00 to 9.00 and the vertical axis labeled "Probability Level" ranging from 0.00 to 1.00. The line graph depicts a steady increase in probability level as Node_3_Disp_2 increases. Below the graph is a table with two columns headed "Pr(Node_3_Disp_2)" and "Node_3_Disp_2," showing corresponding probability values and node values, ranging from approximately 0.000818 to about 0.839396 for the probabilities and 5 to 9 for the Node_3_Disp_2 values.
   

Importance Sampling (IS):

+--------------------------+------------------------------------+
| **# Samples**            | 100                                |
+--------------------------+------------------------------------+
| **Seed**                 | 159                                |
+--------------------------+------------------------------------+
| **IS Method**            | Basic Simulation                   |
+--------------------------+------------------------------------+
| **Response Levels**      | [5.0 6.0 7.0 8.0 9.0]              |
+--------------------------+------------------------------------+

.. figure:: figures/trussIS-RES2.png
   :alt: Importance Sampling results. The image displays a line graph with the horizontal x-axis labeled "Node_3_Disp_2" ranging from approximately 5.00 to 9.00, and the vertical y-axis labeled "Probability Level" ranging from 0.00 to 1.00. The graph shows a blue line that starts at a lower point on the y-axis near 0.00 and gradually increases to a higher point near 1.00 as it moves from left to right along the x-axis. Below the graph, there is a table with two columns; the left column is unlabeled with rows numbered 1 through 5 showing numeric values that seem to correspond to the data points on the graph, while the right column, labeled "Node_3_Disp_2," has rows numbered 5 through 9 without corresponding values shown. To the left of the graph, there is a vertical navigation bar with various options like UQ, FEM, RV, QoI, and RES highlighted.
   
