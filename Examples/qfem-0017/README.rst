.. _qfem-0017:

Custom UQ for UQpy
=============================================

+----------------+------------------------------------------+
| Problem files  | :github:`Github <Examples/qfem-0017/>`   |
+----------------+------------------------------------------+

This example illustrates how quoFEM can interface a custom UQ engine. A simple forward propagation is conducted using UQpy_, a python package for general uncertainty quantification problems. 

.. figure:: figures/qfem-0017.png
   :align: center
   :alt: A technical drawing showing a truss structure with labeled nodes and applied loads. The top of the truss has horizontal sections each marked as 4 meters in length. Vertical and diagonal members connect to create the truss, with downward forces indicated at three joints and one roller support at the right end. The left end shows a fixed support. A watermark with the letters "UQ" and an emblem resembling a bird is visible over the image.
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
   :alt: Screenshot of a software interface with a sidebar on the left showing options "UQ", "FEM", "RV", "EDP", "RES" with "UQ" highlighted. In the main pane, there is a header titled "UQ Engine" with a dropdown menu set to "CustomUQ" and a section labeled "Configuration Input File" with an input field and a "Choose" button.
   :figclass: align-center

Next, provide the configuration input file path.  In the configuration file, users specify the interface for the parameters (type, name, values of each parameters) required for the custom UQ analysis. The provided script will generate 1 combo box to define sample types, 2 line edit fields to define the number of samples and seed, 3 spin boxes to define number of concurrent tasks, nodes, and cores per task, and 1 line edit field to specify the UQ Driver.

.. literalinclude:: src/UQpySimpleExample.json
   :language: json
   :caption: UQpySimpleExample.json

Note that configuration input file specifies the front-end interfaces while the back-end interface should be modified in the backend directory (Refer the :ref:`Configuring CustomUQ Engine <lblCustomUQ>` section for details). As an example, UQpy has been already implemented in the customized backend where users can find at ``{Backend Applications Directory}/applications/performUQ/other``, where the ``{Backend Applications Directory}`` is specified from the file-preference in the menu bar). Once the ``UQpySimpleExample.json`` is called, the following customized user interface will appear in UQ panel.

.. figure:: figures/CUS_UQtab2.png
   :align: center
   :alt: Screenshot of a software interface with various input fields for uncertainty quantification. The interface shows a section labeled "UQ Engine" with options such as "CustomUQ," a field for "Configuration Input File" with a file path provided, options for "Sampling Method" with "LHS" selected, and input fields for "Number of Samples," "Seed," "Number of Concurrent Tasks," "Number of Nodes," "Cores per Task," and "UQ Driver," all of which are currently blank or set to zero. The screen also includes tabs on the left side labeled "FEM," "RV," "EDP," "RES," and "UQ," indicating different sections of the software.
   :figclass: align-center

Let us sample 50 samples by Latin hypercube sampling (LHS). The UQ Driver field must be filled in as **UQpy**, as shown in the following figure.

.. figure:: figures/CUS_UQtab3.png
   :align: center
   :alt: A screenshot of a software interface with tabs on the left side labeled UQ, FEM, RV, EDP, and RES. The active tab is UQ, which shows settings for a simulation or uncertainty quantification task. Options include 'UQ Engine' set to 'CustomUQ', a 'Configuration Input File' path, 'Sampling Method' selected as 'LHS', 'Number of Samples' set to 50, 'Seed' set to 1, 'Number of Concurrent Tasks', 'Number of Nodes', and 'Cores per Task' all set to 1, and 'UQ Driver' selected as 'UQpy'. There is a 'Choose' button next to the file path field. The interface is clean with a modern design.
   :figclass: align-center

2. Select the **FEM** tab from the input panel. For the main script copy the path to the ``TrussModel.tcl`` or select choose and navigate to the file. For the post-process script field, repeat the same procedure for the ``TrussPost.tcl script``. (See example :ref:`Two-Dimensional Truss: Sampling, Reliability and Sensitivity <qfem-0001>` for the model details)


.. figure:: figures/CUS_FEMtab.png
   :align: center
   :alt: A screenshot of a computer interface showing part of a finite element method (FEM) software application. The menu on the left includes options for UQ, FEM, RV, EDP, and RES. The FEM tab is selected, and the screen shows fields for "Input Script" and "Postprocess Script", each with a file path entered and a "Choose" button beside them. The selected software from a dropdown menu at the top is OpenSees.
   :figclass: align-center


3. Select the **RV** tab from the input panel. This should be pre-populated with four random variables with same names as those having ``pset`` in the tcl script. For each variable, from the drop down menu change them to uniform and provide the lower and upper bounds specified for the problem.

.. figure:: figures/CUS_RVtab.png
   :align: center
   :alt: A screenshot of a software interface with a section titled "Input Random Variables." Four rows list variables named E, P, Au, and Ao with a uniform distribution, each having specified minimum and maximum values. There are buttons for adding a new variable, clearing all entries, showing a probability distribution function (PDF) for each variable, and for correlation matrix, export, and import functions. To the left, a vertical navigation menu has options for UQ, FEM, RV, EDP, and RES, with RV highlighted, indicating the current section. The interface has a clean, modern layout with a color scheme of blues, grays, and white.
   :figclass: align-center

.. note::
   Only **uniform distribution** is supported in this preliminary example.

4. Next select the **QoI** tab. Here enter ``Node_3_Disp_2`` for the one variable.

.. figure:: figures/CUS_QoItab.png
   :align: center
   :alt: A screenshot of a user interface with a navigation menu on the left side showing the abbreviated categories UQ, FEM, RV, EDP (highlighted in light blue), and RES. On the right side, there is a section titled "Quantities of Interest" with an input form including a pre-filled row labeled 'Variable Name' with the entry 'Node_3_Disp_2' and a column for 'Length' with the value '1'. Above the input form, there are buttons labeled 'Add' and 'Clear all'.
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
   :alt: The image is a screenshot of a computer interface that shows a scatter chart and a data table. The chart is in the upper section, with data points scattered along with the axes labeled "Run #" for the x-axis and "Node_3_Disp_2" for the y-axis. The data table in the lower section displays numerical values across columns labeled "Run #", "E", "P", "Au", "Ao", and "Node_3_Disp_2". There are buttons for "Summary" and "Data Values" above the chart and a "Save Data" button below the table. On the left side, there are menu options "UQ", "FEM", "RV", "EDP", and "RES", with the "RES" highlighted in blue.
   :figclass: align-center
