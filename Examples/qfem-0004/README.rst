.. _qfem-0004:

Sensitivity Analysis
====================

+-----------------+--------------------------------------------+
| Problem files   | :github:`Download <Examples/qfem-0004/>`   |
+-----------------+--------------------------------------------+

This example uses quoFEM to perform a global sensitivity analysis of an
OpenSees FE model.

Consider a stochastic model of a two-dimensional truss structure like
that shown in the following figure. A `sensitivity
analysis </common/user_manual/usage/desktop/DakotaSensitivity.html>`__
procedure is coordinated by quoFEM which will estimate the sensitivities
of the response quantities of interest with respect to the problem's
random variables.

.. figure:: qfem-0004.png
   :width: 500px
   :alt: A technical drawing of a truss structure with labeled points and support indicators. The truss is composed of straight segments joined at endpoints forming two triangles and a rectangle-like shape. It spans horizontally with markings every 4 meters along the top. Vertical arrows labeled "P" indicate downward forces at two joints, and there are three types of support symbols at points 1, 2, and 4, indicating different constraints. Point 1 has a support with a filled triangle, point 2 is supported by a simple line with an arrow pointing down, and point 4 has a support with a circle and a filled triangle, suggesting a roller support. The entire truss structure has a vertical height of 4 meters labeled on the right side.
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

To define the uncertainty workflow in quoFEM, select **Sensitivity
Analysis** for the **Dakota Method Category**, and enter the following
inputs:

+---------------+--------+
| **Method**    | LHS    |
+---------------+--------+
| **Samples**   | 1000   |
+---------------+--------+
| **Seed**      | 175    |
+---------------+--------+

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

.. figure:: figures/trussSensitivity-RES.png
   :alt: Sensitivity analysis results for simple truss.
   :alt: A screenshot of a software interface displaying a table titled "Node_3_Disp_2 Sobol' indices" with columns for Random Variable, Main, and Total. The table contains four rows of random variables (E, P, Ao, Au) with their corresponding Main and Total Sobol' index values in scientific notation. The interface includes tabs labeled Summary and Data Values, with the Summary tab currently selected.

   Sensitivity analysis results for simple truss.



