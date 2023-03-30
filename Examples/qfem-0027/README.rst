.. _qfem-0027:

Forward propagation of uncertainty with multiple simulation models
==================================================================

+---------------+----------------------------------------------+
| Problem files | :github:`Github <Examples/qfem-0027/>`       |
+---------------+----------------------------------------------+

Outline
-------
In this example, two models of a 6-story structure are used to predict the maximum base shear by conducting a nonlinear time history analysis of the structure subjected to seismic excitation. The beliefs represent the uncertainty about which of these two models are the best model to be used in the analysis, and in this example, equal belief is placed in the two models.

The structure is modeled as having flexible columns and stiff elastic beams. The structure is subjected to an earthquake excitation at the base which is strong enough to result in nonlinear response. 

Model 1 uses Steel01 material for the columns and model 2 uses Steel02 for the columns, with R0 set to 18, and cR1 and cR2 set to their recommended values of 0.925 and 0.15, respectively.

There are three random variables in each model which correspond to three parameters in the uniaxial material models Steel01 and Steel02: the initial elastic stiffness of the columns (k), the yield strength of the columns (Fy), and the strain hardening ratio of the columns (alp), i.e., the ratio between the post-yield stiffness and the initial elastic stiffness. 

.. literalinclude:: create_model_1.tcl
   :linenos: 
   :language: tcl
   :lines: 24-35


.. literalinclude:: create_model_2.tcl
   :linenos: 
   :language: tcl
   :lines: 24-35

The output quantity of interest (QoI) is the amplitude of the maximum base shear.

 
Files required
--------------
The exercise requires one main script file and one supplementary file. The user should download these files and place them in a **NEW** folder. 

.. warning::
   Do NOT place the files in your root, downloads, or desktop folder as when the application runs it will copy the contents on the directories and subdirectories containing these files multiple times. If you are like us, your root, Downloads or Documents folders contains a lot of files.

1. :qfem-0025:`rosenbrock.py <src/rosenbrock.py>` - This is a python script that computes the model. The script is based on :ref:`this example<qfem-0005>`, but with :math:`y=0`. Further,  :math:`\varepsilon(x)` is generated using numpy random sampler.

   .. literalinclude:: src/rosenbrock.py
            :language: py
            :caption: rosenbrock.py

2. :qfem-0025:`params.py <src/params.py>`- This file defines the input variable(s) that the surrogate model will take

      
   .. literalinclude:: src/params.py
            :language: py
            :caption: params.py

.. note::
   Since the python script creates a ``results.out`` file when it runs, no postprocessing script is needed. 


UQ workflow
-----------

The steps involved are as follows:

1. Start the application and the **UQ** panel will be highlighted. In the **UQ Method** drop down menu, select the **Forward Propagation** option. In the **UQ Engine** dropdown menu select **Dakota** option. Enter the values in this panel as shown in the figure below. 

.. figure:: figures/UQ.png
   :align: center
   :width: 400
   :figclass: align-center


2. Next in the **FEM** panel , select **Multi Model** FEM engine. Use the **Add** or **Remove** option as needed to have two tabs, one for each of the two models being used in this example, and enter the values shown in the following figures:

.. figure:: figures/FEM1.png
   :align: center
   :figclass: align-center
   :width: 1200

.. figure:: figures/FEM2.png
   :align: center
   :figclass: align-center
   :width: 1200


3. Select the **RV** tab from the input panel. This panel should be pre-populated with the names of the variables that were defined in the model scripts. If not, press the **Add** button to create a new field to define the input random variable. Enter the same variable name, as required in the model script. For this example, choose the Uniform probability distribution for all the random variabels and enter the Min. and Max. values for each distribution as shown in the figure below:

.. figure:: figures/RV.png
   :align: center
   :figclass: align-center
   :width: 1200


4. In the **EDP** panel create an output quantity of length 1 with a descriptive name.

.. figure:: figures/EDP.png
   :align: center
   :figclass: align-center
   :width: 1200


5. Click on the **Run** button. This will create the necessary input files to perform a forward propagation analysis with Dakota, run the analysis, and display the results when the analysis is completed.

When we plot the QoI vs MultiModel-FEM, we see a systematic difference in the range of the predicted QoIs - model 2 predicts lower max base shear values than model 1. The example runs in < 1 minute on my Mac, for 100 samples.

* Summary of results obtained by using the two models:

.. figure:: figures/RES1.png
   :align: center
   :figclass: align-center
   :width: 1200

* Maximum base shear vs model index:

.. figure:: figures/RES2.png
   :align: center
   :figclass: align-center
   :width: 600

This scatter plot shows that the maximum base shear predicted by Model 1 is higher than that predicted by Model 2.

* Histogram of maximum base shear:

.. figure:: figures/RES3.png
   :align: center
   :figclass: align-center
   :width: 600

If only Model 1 was used:

* Summary of results:

.. figure:: figures/RES4.png
   :align: center
   :figclass: align-center
   :width: 1200

* Histogram of maximum base shear:

.. figure:: figures/RES5.png
   :align: center
   :figclass: align-center
   :width: 600

If only Model 2 was used:

* Summary of results:

.. figure:: figures/RES6.png
   :align: center
   :figclass: align-center
   :width: 1200

* Histogram of maximum base shear:

.. figure:: figures/RES7.png
   :align: center
   :figclass: align-center
   :width: 600


We observe from the summary statistics and the histograms that if it is not known which of the two models considered is the best model to be used to represent the behavior of the structure, the predicted range of the maximum base shear is larger (i.e., the standard deviation of the maximum base shear is higher when using the two models than in the case when either of the two models are used on their own). The mean value of the base shear predicted in this case will be the mean of the maximum base shear predicted by the two models when used on their own (here we numerically estimate the mean by sampling values of the inputs from the specified distribution, hence there is sampling variablity in the estimate of the mean). 
