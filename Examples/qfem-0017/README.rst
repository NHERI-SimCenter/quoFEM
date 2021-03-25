
Custom UQ for UQpy
=============================================

+---------------+----------------------+
| Problem files | :qfem-0020:`/`       |
+---------------+----------------------+


The python package **UQpy** is imported in custom UQ engine. 


This example illustrates how quoFEM can be interface an custom UQ engine. A simple forward propagation procedure is run using UQpy, a python package for general uncertainty quantification problems. Consider the problem of uncertainty quantification in a two-dimensional truss structure introduced in (link to qfem-0001)

.. figure:: figures/truss2.png
   :align: center
   :figclass: align-center
   :width: 600


The structure has uncertain properties that all follow uniform distribution:

================================================ ============ =========
Random Variable                                  lower bound  upper bound
================================================ ============ =========
Elastid modulus, E                               150          200
Load, P                                          15           35
Cross sectional area of the upper three bars, Au 400          600
Cross sectional area of the other six bars, Ao   200          300
================================================ ============ =========

The goal of the exercise is to implement UQpy as a **custom UQ engine** and estimate the mean and standard deviation of the vertical displacement at node2.



UQ Workflow
-------------


1. Start the application and the **UQ** Selection will be highlighted. In the panel for the UQ selection, change the UQ Engine to **CustomUQ**. In UQ Application Name, type **Other-UQ**. 

.. figure:: figures/CUS_UQtab.png
   :align: center
   :figclass: align-center

Next, provide the configuration input file path.  In the configuration file, users specify the interface for the parameters (type, name, values of each parameters) required for the custom UQ analysis. The provided script will generate 1 combo box to define sample types, 1 spin boxes to define number of samples, concurrent tasks, and notes, and one line edit to specify the UQ Driver.

.. literalinclude:: ../qfem-0020/src/UQpySimpleExample.json
   :language: json
   :caption: UQpySimpleExample.json

Note that configuration input file specifies the front-end interfaces while the back-end interface should be modified in (file path). Once the ``UQpySimpleExample.json`` is called, the following customized user interface will appear in UQ panel.

.. figure:: figures/CUS_UQtab2.png
   :align: center
   :figclass: align-center

We choose to run 50 round of Latine Hyper Cube sampling.


2. Select the **FEM** tab from the input panel. For the main script copy the path name to ``TrussModel.tcl`` or select choose and navigate to the file. For the post-process script field, repeat the same procedure for the ``TrussPost.tcl script``.


.. figure:: figures/CUS_FEMtab.png
   :align: center
   :figclass: align-center


3. Select the **RV** panel from the input panel. This should be pre-populated with four random variables with same names as those having ``pset`` in the tcl script. For each variable, from the drop down menu change them uniform one and then provide the lower and upper bounds specified for the problem.

.. figure:: figures/CUS_RVtab.png
   :align: center
   :figclass: align-center

.. note::
   Only **uniform distribution** is supported in this preliminary example.

4. Next select the **QoI** tab. Here enter ``Node_2_Disp_2`` for the one variable.

.. figure:: figures/CUS_QoItab.png
   :align: center
   :figclass: align-center

5.  Next click on the **Run** button. This will cause the backend application to launch dakota.

6.  When done the **RES** panel will be selected and the results will be displayed. The results show the values the mean and standard deviation.


Summary:
* Mean = 7.322
* Std = 4.29
* Skewness = 1.89
* Kurtosis = 7.13


Data Table:

.. figure:: figures/CUS_REStab.png
   :align: center
   :figclass: align-center
