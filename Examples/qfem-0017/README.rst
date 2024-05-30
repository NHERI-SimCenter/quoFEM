.. _qfem-0017:

Custom UQ for UQpy
=============================================

+----------------+------------------------------------------+
| Problem files  | :github:`Github <Examples/qfem-0017/>`   |
+----------------+------------------------------------------+

This example illustrates how quoFEM can interface a custom UQ engine. A simple forward propagation is conducted using UQpy_, a python package for general uncertainty quantification problems. 

.. figure:: figures/qfem-0017.png
   :align: center
   :alt: A technical drawing of a truss structure used in engineering. This schematic includes labeled nodes and applied forces with dimensions given in meters. The truss is shown in a side view with supports at the ends, and force arrows are applied at three joints of the truss indicating points of load. The background contains a watermark logo with the letters U, Q, and a stylized bird icon.
   :figclass: align-center
   :width: 600

.. _UQpy: https://sites.google.com/site/jhusurg/UQpy

Consider a two-dimensional truss structure that has uncertain properties that all follow uniform distribution:

================================================ ============ =========
Random Variable                                  lower bound  upper bound
================================================ ============ =========
Elastic modulus, E                               150          200
Load, P                                          15           35
Cross sectional area of the upper three bars, Au 400          600
Cross sectional area of the other six bars, Ao   200          300
================================================ ============ =========

The goal of the exercise is to implement UQpy as a **custom UQ engine** and estimate the mean and standard deviation of the vertical displacement at node 3.

.. note::
   The UQpy_ python package is required to be installed to run this example, e.g. pip install UQpy


UQ Workflow
-------------

1. Start the application and the **UQ** Selection will be highlighted. In the panel for the UQ selection, change the UQ Engine to **CustomUQ**. 

.. figure:: figures/CUS_UQtab.png
   :align: center
   :alt: Screenshot of a software interface with a menu on the left side showing options UQ, FEM, RV, EDP, and RES, highlighted on UQ. The main panel is titled "UQ Engine" with a dropdown menu labeled "CustomUQ," and below there is a field titled "Configuration Input File" with a "Browse" button on the right. The interface has a clean, modern design with a color scheme of blue, gray, and white.
   :figclass: align-center

Next, provide the configuration input file path.  In the configuration file, users specify the interface for the parameters (type, name, values of each parameters) required for the custom UQ analysis. The provided script will generate 1 combo box to define sample types, 2 line edit fields to define the number of samples and seed, 3 spin boxes to define number of concurrent tasks, nodes, and cores per task, and 1 line edit field to specify the UQ Driver.

.. literalinclude:: src/UQpySimpleExample.json
   :language: json
   :caption: UQpySimpleExample.json

Note that configuration input file specifies the front-end interfaces while the back-end interface should be modified in the backend directory (Refer the :ref:`Configuring CustomUQ Engine <lblCustomUQ>` section for details). As an example, UQpy has been already implemented in the customized backend where users can find at ``{Backend Applications Directory}/applications/performUQ/other``, where the ``{Backend Applications Directory}`` is specified from the file-preference in the menu bar). Once the ``UQpySimpleExample.json`` is called, the following customized user interface will appear in UQ panel.

.. figure:: figures/CUS_UQtab2.png
   :align: center
   :alt: Screenshot of a software interface with multiple input fields and settings. The interface includes a side panel with tabs labeled UQ, FEM, RV, EDP, and RES. The main area shows a section for the UQ Engine with a dropdown menu set to 'CustomUQ' and a field for a Configuration Input File with a filepath entered. Other options visible include Sampling Method set to 'LHS', and blank fields for Number of Samples, Seed, Number of Concurrent Tasks, Number of Nodes, Cores per Task, and UQ Driver.
   :figclass: align-center

Let us sample 50 samples by Latin hypercube sampling (LHS). The UQ Driver field must be filled in as **UQpy**, as shown in the following figure.

.. figure:: figures/CUS_UQtab3.png
   :align: center
   :alt: Screenshot of a software interface with various input fields related to an uncertainty quantification (UQ) engine. The interface shows options labeled "UQ Engine," "Configuration Input File," "Sampling Method," "Number of Samples," "Seed," "Number of Concurrent Tasks," "Number of Nodes," "Cores per Task," and "UQ Driver." A file path is provided for a JSON configuration file, and the sampling method selected is Latin Hypercube Sampling (LHS). The interface also includes increment controls for numerical values and a 'Choose' button for file selection.
   :figclass: align-center

2. Select the **FEM** tab from the input panel. For the main script copy the path to the ``TrussModel.tcl`` or select choose and navigate to the file. For the post-process script field, repeat the same procedure for the ``TrussPost.tcl script``. (See example :ref:`Two-Dimensional Truss: Sampling, Reliability and Sensitivity <qfem-0001>` for the model details)


.. figure:: figures/CUS_FEMtab.png
   :align: center
   :alt: Screenshot of a user interface for a software application with a sidebar menu on the left showing options such as UQ, FEM, RV, EDP, and RES, with "FEM" highlighted. The main panel displays fields for "Input Script" and "Postprocess Script," each with file paths and adjacent "Choose" buttons. The top right corner shows a dropdown menu labeled "OpenSees."
   :figclass: align-center


3. Select the **RV** tab from the input panel. This should be pre-populated with four random variables with same names as those having ``pset`` in the tcl script. For each variable, from the drop down menu change them to uniform and provide the lower and upper bounds specified for the problem.

.. figure:: figures/CUS_RVtab.png
   :align: center
   :alt: Screenshot of a software interface for inputting random variables, displaying a section titled "Input Random Variables" with four entries labeled "E," "P," "Au," and "Ao." Each entry has a "Uniform" distribution selected and fields for minimum and maximum values, accompanied by buttons to "Show PDF" for probability density function visualization. Options to add more variables, clear all entries, access a correlation matrix, export, and import data are available on the toolbar. The sidebar indicates more sections of the software, likely pertaining to a statistical or engineering application, with tabs labeled "FEM," "RV," "EDP," and "RES," with "RV" currently selected.
   :figclass: align-center

.. note::
   Only **uniform distribution** is supported in this preliminary example.

4. Next select the **QoI** tab. Here enter ``Node_3_Disp_2`` for the one variable.

.. figure:: figures/CUS_QoItab.png
   :align: center
   :alt: Screenshot of a graphical user interface featuring a sidebar with the acronyms UQ, FEM, RV, EDP, and RES. The main panel is titled 'Quantities of Interest' with a field that reads 'Variable Name' filled with 'Node_3_Disp_2' and a 'Length' input box with the number 1. There are 'Add' and 'Clear all' buttons to the right of the input fields.
   :figclass: align-center

5.  Next click on the **Run** button. This will cause the backend application to launch the CustomUQ engine which will run UQpy.

6.  When done the **RES** panel will be selected and the results will be displayed. The results show the values the mean and standard deviation.


Summary:

* Mean = 5.77619
* Std = 2.4664
* Skewness = 0.456808
* Kurtosis = 2.7146


Data Table:

.. figure:: figures/CUS_REStab.png
   :align: center
   :alt: Screenshot of a software interface showing scientific data. The top portion displays a scatter plot with horizontal numerical axis labeled "Run #" and vertical axis labeled "Node_3_Disp_2." Data points are scattered across the graph. Below the graph is a table with rows of numerical data across columns labeled "Run #," "E," "P," "Au," "Ao," and "Node_3_Disp_2." On the left side, there's a vertical navigation bar with acronyms like "UQ," "FEM," "RV," "EDP," and "RES" highlighted. A "Save Data" button is visible on the lower left corner.
   :figclass: align-center
