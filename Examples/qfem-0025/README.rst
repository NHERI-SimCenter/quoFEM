.. _qfem-0025:

Heteroscedastic Gaussian Process modeling
=================================================

+---------------+----------------------------------------------+
| Problem files | :github:`Github <Examples/qfem-0025/>`       |
+---------------+----------------------------------------------+

Outline
-------
In this example, demonstrates the heteroscedastic Gaussian process model for surrogating noisy response.

Problem description
-------------------

Let us consider the basic python model in :ref:`this example<qfem-0005>` with fixed :math:`y=0`. 

.. math::  
   g(x)=(a-x)^{2}+bx^{4}+\varepsilon

where :math:`\varepsilon` is an additive Gaussian noise that represent the variability in the output response surface. Let us assume :math:`\varepsilon` has zero mean and non-homogeneous variance of :math:`\sigma(x)^2 = (x+2.0)^2`. 

.. _figObservations:

.. figure:: figures/qf-0025-data.png
   :align: center
   :alt: Image showing error in description
   :width: 400
   :figclass: align-center
   
   Observation dataset


We would like to create a surrogate model of this function for the input range of :math:`x\in[-2,2]`

 
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

1. Start the application and the **UQ** panel will be highlighted. In the **UQ Engine** drop down menu, select the **SimCenterUQ** engine. In the **Method** select **Train GP Surrogate** option. Enter the values in this panel as shown in the figure below. 

.. figure:: figures/qf-0025-UQ.png
   :align: center
   :alt: Image showing error in description
   :figclass: align-center
   :width: 1200

2. Next in the **FEM** panel , select **Python** FEM engine. Enter the paths to the ``rosenbrock.py`` and ``params.py`` or select **Choose** and navigate to the files. 

.. figure:: figures/qf-0025-FEM.png
   :align: center
   :alt: Image showing error in description
   :figclass: align-center
   :width: 1200

.. note::
   Since the python script creates a ``results.out`` file when it runs, no postprocessing script is needed. 

3. Select the **RV** tab from the input panel. This panel should be pre-populated with :math:`x`. If not, press the **Add** button to create a new field to define the input random variable. Enter the same variable name, as required in the model script. Specify the input range of interest

.. figure:: figures/qf-0025-RV.png
   :align: center
   :alt: Image showing error in description
   :figclass: align-center
   :width: 1200


4. In the **QoI** panel enter a variable named ``f`` with length 1.

.. figure:: figures/qf-0025-EDP.png
   :align: center
   :alt: Image showing error in description
   :figclass: align-center
   :width: 1200


5. Click on the **Run** button. This will cause the backed application to launch the **SimCenterUQ** engine, which performs the surrogate training. 


6. When done, the **RES** tab will be selected and the results will be displayed.

* Summary of Results:

.. figure:: figures/qf-0025-RES1.png
   :align: center
   :alt: Image showing error in description
   :figclass: align-center
   :width: 1200

* Leave-one-out cross-validation (LOOCV) predictions:

.. figure:: figures/qf-0025-RES2.png
   :align: center
   :alt: Image showing error in description
   :figclass: align-center
   :width: 600

Note that the goodness-of-fit measures above do not perfectly capture *goodness* as it is evaluated assuming aleatory variability (or the measurement noise) zero. However, the cross-validation plot with bounds provides a rough estimate of whether the observation data safely lies on the GP prediction bounds. A better validation measure for the case with high nugget variance will be included in the next release. 

.. figure:: figures/qf-0025-RES3.png
   :align: center
   :alt: A screenshot of a data analysis interface displaying a scatter plot with prediction bounds and a corresponding data table. The scatter plot on the left shows a collection of blue points with vertical error bars along with an orange prediction band, indicating variance, mapped across an x-axis labeled 'X' and a y-axis with numerical values. One of the data points is highlighted with a red dot. On the right side of the image, a table displays rows labeled 'Run #' with corresponding 'X' and 'f' values, with the 13th run highlighted, showing an X value of about 1.0458 and an f value of about 11.13. The interface includes buttons for various data saving options such as 'Save Table,' 'Save Columns Separately,' 'Save RVs,' 'Save QoIs,' and 'Save Surrogate Predictions.'
   :figclass: align-center
   :width: 1200


The scatter plot compares the observation data with the predicted mean and variance obtained by cross-validations. The confidence interval (CI, shown in red) is the bounds for the 'mean' of the response function, while the prediction interval (PI, shown in gray) shows those of the observations (i.e. by adding the range of the measurement noise)