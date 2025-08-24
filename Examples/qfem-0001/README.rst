.. _qfem-0001:

Two-Dimensional Truss: Sampling, Reliability and Sensitivity
============================================================

Consider the problem of uncertainty quantification in a two-dimensional truss structure shown in the following figure.

.. figure:: figures/truss.png
   :align: center
   :alt: A schematic diagram of a truss structure with labeled nodes and applied loads. The top beam of the truss is divided into three sections, each 4 meters in length, with the logo of OpenSees near the right end. Below are angled and vertical elements connecting to the ground at four nodes, with fixed supports at the outer nodes and rollers at the inner nodes. Point loads labeled as "P" are applied downwards at the two central nodes at the bottom of the vertical elements. The truss is depicted in a white line drawing style on a black background.
   :width: 600

The structure has uncertain properties that all follow normal distribution:

1. Elastic modulus(``E``): mean :math:`\mu_E=205 kN/{mm^2}` and standard deviation :math:`\sigma_E =15 kN/{mm^2}` (COV = 7.3%)
2. Load (``P``): mean :math:`\mu_P =25 kN` and a standard deviation of :math:`\sigma_P = 3 kN`, (COV = 12%).
3. Cross sectional area for the upper three bars (``Au``): mean :math:`\mu_{Au} = 500 mm^2`, and a standard deviation of :math:`\sigma_{Au} = 25mm^2`  (COV = 5%)
4. Cross sectional area for the other six bars (``Ao``): mean :math:`\mu_{Ao} = 250mm^2`, and :math:`\sigma_{Ao} = 10mm^2` (COV = 4%)

The goal of the exercise is to estimate the mean and standard deviation of the vertical displacement at node 3. The exercise requires two files. The user is required to download these files and place them in a **NEW** folder. The two files are: 

1. Main file: :qfem-0001:`TrussTemplate.tcl <src/TrussModel.tcl>`

   .. literalinclude:: ../qfem-0001/src/TrussModel.tcl
      :language: tcl

.. note::
   
   The first lines containing ``pset`` will be read by the application when the file is selected and the application will autopopulate the Random Variables input panel with these same variable names. It is of course possible to explicitly use Random Variables without the ``pset`` command as is demonstrated in the verification section.

The ``TrussPost.tcl`` script shown below will accept as input any of the 6 nodes in the domain and for each of the two dof directions.

2. Postprocessing file: :qfem-0001:`TrussPost.tcl <../qfem-0001/src/TrussPost.tcl>`. 

   .. literalinclude:: ../qfem-0001/src/TrussPost.tcl
      :language: tcl


.. note::

   The use has the option to provide no post-process script (in which case the main script must create a ``results.out`` file containing a single line with as many space separated numbers as QoI or the user may provide a Python script that also performs the postprocessing. Below is an example of a postprocessing Python script.

   Alternative postprocessing file: :qfem-0001:`TrussPost.py <src/TrussPost.py>`

      .. literalinclude:: ../qfem-0001/src/TrussPost.py
         :language: python



.. warning::

   Do not place the files in your root, downloads, or desktop folder as when the application runs it will copy the contents on the directories and subdirectories containing these files multiple times. If you are like us, your root, Downloads or Documents folders contains and awful lot of files and when the backend workflow runs you will slowly find you will run out of disk space!


Sampling Analysis
^^^^^^^^^^^^^^^^^

+----------------+------------------------------------------+
| Problem files  | :github:`Download <Examples/qfem-0001>`  |
+----------------+------------------------------------------+

To perform a sampling or forward propagation uncertainty analysis the user would perform the following steps:

1. Start the application and the UQ Selection will be highlighted. In the panel for the UQ selection, keep the UQ engine as that selected, i.e. Dakota, and the **UQ Method Category** as Forward Propagation, and the Forward Propagation method as LHS (Latin Hypercube). Change the **#samples** to 1000 and the **seed** to 20 as shown in the figure.


.. figure:: figures/trussUQ.png
   :align: center
   :alt: Screenshot of a software interface with a left-side vertical navigation bar having entries for FEM, RV, EDP, and RES, highlighted on UQ. The main panel shows settings for 'UQ Method' with a dropdown menu set to 'Forward Propagation', an 'UQ Engine' with 'Dakota' selected, checkboxes for 'Parallel Execution' and 'Save Working dirs', and a section for 'Method' set to 'LHS' with inputs for '# Samples' set to '1000' and 'Seed' set to '20'.
   :figclass: align-center

* Sample size is related to the confidence interval of the estimates. Please refer to :ref:`here<lblDakotaForward>`.
* Random seed is used to ensure that results are reproducible.

2. Next select the **FEM** panel from the input panel. This will default in the OpenSees FEM engine. For the main script copy the path name to ``TrussModel.tcl`` or select **choose** and navigate to the file. For the **post-process script** field, repeat the same procedure for the ``TrussPost.tcl`` script.

.. figure:: figures/trussFEM.png
   :align: center
   :alt: Screenshot of a software interface with a focus on the Finite Element Method (FEM) configuration. It shows dropdown selections and fields with file paths for an "Input Script" and a "Postprocess Script," both pointing to Tcl script files within a directory structure. To the right of each file path field is a "Choose" button, likely for browsing and selecting files. On the left side, there are other abbreviated category labels such as UQ, RV, EDP, and RES, suggesting different modules or steps in the software process. The FEM tab is currently active, indicated by its darker shade.
   :figclass: align-center

3. Next select the **RV** panel from the input panel. This should be pre-populated with four random variables with same names as those having ``pset`` in the tcl script. For each variable, from the drop down menu change them from having a constant distribution to a normal one and then provide the means and standard deviations specified for the problem.

.. figure:: figures/trussRV.png
   :align: center
   :alt: Screenshot of a user interface for inputting random variables in a software application. There are fields for variable name, distribution, mean, and standard deviation, with options to add new variables, clear all, or access a correlation matrix. Current variables include 'E' with a lognormal distribution, mean of 205, and standard deviation of 15; 'P' with a normal distribution, mean of 25, and standard deviation of 3; 'Ao' with a lognormal distribution, mean of 250, and standard deviation of 10; and 'Au' with a normal distribution, mean of 500, and standard deviation of 25. Buttons for exporting, importing, and showing the probability distribution function (PDF) are visible.
   :figclass: align-center

4. Next select the **QoI** tab. Here enter ``Node_3_Disp_2`` for the one variable. 

.. figure:: figures/trussQoI.png
   :align: center
   :alt: Screenshot of a computer interface with a panel titled "Quantities of Interest" displaying a list with one item. The item shown is a variable named "Node_3_Disp_2" with a length value of "1". Two buttons labeled "Add" and "Clear all" are available above the list. On the left side, there is a vertical menu with abbreviated options "UQ", "FEM", "RV", highlighted "EDP", and "RES". The overall color scheme is shades of blue and gray.
   :figclass: align-center


.. note::   

   The user can add additional QoI by selecting add and then providing additional names. As seen from the post-process script any of the 6 nodes may be specified and for any node either the 1 or 2 DOF direction.

5. Next click on the **Run** button. This will cause the backend application to launch dakota. When done the **RES** panel will be selected and the results will be displayed. The results show the values the mean and standard deviation.

.. figure:: figures/trussRES1.png
   :align: center
   :alt: Screenshot of a section of a software interface displaying statistical data. On the left is a vertical navigation bar with the labels UQ, FEM, RV, EDP, and RES, with the RV section highlighted. On the right, there is a tabbed section with 'Summary' and 'Data Values' tabs, with 'Summary' shown. Below, there's a table with headings: Name, Mean, StdDev, Skewness, and Kurtosis. One entry is visible under Name, 'Node_3_Disp_2', with corresponding statistics: Mean of 7.36686, Standard Deviation of 1.06435, Skewness of 0.210605, and Kurtosis of 3.10805. The background colors are a mix of whites and light grays, with the navigation bar having a darker gray or blue shade.
   :figclass: align-center


If the user selects the **Data** tab in the results panel, they will be presented with both a graphical plot and a tabular listing of the data.

.. figure:: figures/trussRES2.png
   :align: center
   :alt: A screenshot of a computer interface displaying scientific data analysis. On the left side of the image, a scatter plot graph labeled "Samples" and "Run # 6" shows a cluster of blue data points, with the axes labeled "Node_3_Disp_2." On the right side, a spreadsheet table lists data under columns "Run #," "P," "Au," "E," "Ao," and "Node_3_Disp_2," with some cells highlighted in blue indicating selected data points. At the top right corner, there are buttons to "Save Table" and "Save Columns Separately."
   :figclass: align-center

Various views of the graphical display can be obtained by left and right clicking in the columns of the tabular data. If a singular column of the tabular data is pressed with both right and left buttons a frequency and CDF will be displayed, as shown in figure below.

.. figure:: figures/trussRES5.png
   :align: center
   :alt: The image displays three sections of statistical analysis data. The left section shows a histogram representing frequency distribution, with the frequency percentage on the y-axis and an unknown variable labeled 'Node_3_Disp_2' on the x-axis. The central section presents a cumulative frequency distribution curve, with cumulative probability on the y-axis and the same 'Node_3_Disp_2' variable on the x-axis. The right section contains a data table with columns for 'Run #', 'P', 'Au', 'E', 'Ao', and 'Node_3_Disp_2'. Multiple rows of numerical data are shown with a specific value in the 'Node_3_Disp_2' column highlighted in blue. The top of the right section includes buttons labeled 'Save Table' and 'Save Columns Separately'.
   :figclass: align-center

Reliability Analysis
^^^^^^^^^^^^^^^^^^^^

+----------------+--------------------------------------------+
| Problem files  | :github:`Download <Examples/qfem-0003/>`   |
+----------------+--------------------------------------------+

If one is interested in the probability that a particular response measure will be exceeded, an alternate strategy is to perform a reliability analysis. In order to perform a reliability analysis the steps above would be repeated with the exception that the user would select a reliability analysis method instead of a Forward Propagation method. To obtain reliability results using the Second-Order Reliability Method (SORM) for the truss problem the user would follow the same sequence of steps as previously. The difference would be in the **UQ** panel in which the user would select a Reliability as the Dakota Method Category and then choose Local reliability. In the figure the user is specifying that they are interested in the probability that the displacement will exceed certain response levels.


.. figure:: figures/trussSORM-UQ.png
   :align: center
   :alt: Screenshot of a software interface with settings for Uncertainty Quantification (UQ) methods. Includes fields such as UQ Method set to Reliability Analysis, UQ Engine set to Dakota, options for Parallel Execution and Save Working dirs, Reliability Method set to Local Reliability, and several dropdown menus for Local Approximation, Design Point (MPP) Search Method, Approximation Order, and Reliability Levels. The reliability levels include numerical values such as 0.02, 0.20, 0.40, 0.60, 0.80, and 0.99.
   :figclass: align-center

After the user fills in the rest of the tabs as per the previous section, the user would then press the **RUN** button. The application (after spinning for a while with the wheel of death) will present the user with the results.

.. figure:: figures/trussSORM-RES.png
   :align: center
   :alt: A line graph displaying a probability level on the y-axis, which ranges from 0.00 to 1.00, against Node_3_Disp_2 on the x-axis, ranging approximately from 5.44 to 10.23. The line in the graph shows a steady increase in the probability level as Node_3_Disp_2 increases. Below the graph is a table with two columns. The left column lists percentages, while the right column lists corresponding values for Node_3_Disp_2, aligning with points on the graph. The user interface elements on the left side, such as UQ, FEM, RV, QoI, and RES, appear to be part of an application menu, and are not directly related to the content of the graph.
   :figclass: align-center


Global Sensitivity
^^^^^^^^^^^^^^^^^^

+----------------+------------------------------------------+
| Problem files  | :github:`Download <Examples/qfem-0004/>` |
+----------------+------------------------------------------+

In a global sensitivity analysis the user is wishing to understand what is the influence of the individual random variables on the quantities of interest. This is typically done before the user launches large scale forward uncertainty problems in order to limit the number of random variables used so as to limit the number of simulations performed.

.. figure:: figures/trussSens-UQ.png
   :align: center
   :alt: Screenshot of a user interface for uncertainty quantification (UQ) analysis settings. Two dropdown menus are labeled "UQ Method" and "Sensitivity Analysis." Below the menus, "UQ Engine" is set to "Dakota," with options checked for "Parallel Execution" and "Save Working dirs." The UQ method chosen is "LHS" with "# Samples" set to 1000 and "Seed" set to 175. A note at the bottom indicates the expected number of FEM calls as a function of samples and RVs (random variables). The interface element is minimalistic with a white background.
   :figclass: align-center

After the user fills in the rest of the tabs as per the previous section, the user would then press the **RUN** button. The application (after spinning for a while with the wheel of death) will present the user with the results.

.. figure:: figures/trussSensitivity-RES.png
   :align: center
   :alt: A screenshot of a software interface displaying "Node_3_Disp_2 Sobol' indices" with a table and a corresponding horizontal bar chart. The table lists 'Random Variable' with entries P, Au, E, and Ao, showing numerical values under 'Main' and 'Total'. The bar chart illustrates these values with bars, where P has the highest value and is represented with a longer bar in comparison to the others. There are tabs labeled 'Summary', 'Data Values', 'UQ', 'FEM', 'RV', 'EDP', and 'RES' on the left side, and a 'Save Results' button at the bottom right.
   :figclass: align-center

