
Surrogate Modeling for One Story Building Earthquake Response
===============================================================

+---------------+----------------------+
| Problem files | :qfem-0015:`/`       |
+---------------+----------------------+

.. note::
   This example will round for 60 mins. For a quick test, the user may want to reduce **Max Computation Time**.
   

Outline
-------

This example constructs a surrogate model for response of a building structure given a ground motion time history. We are interested in 14 structrual parameters and maximum interstory drift/acceleration of structural response. Gaussin process model is used.

Problem description
-------------------

14 parameters are defined as follows. 

================== ============ =========
Random Variable    lower bound  upper bound
					      
================== ============ =========
Material Quality   0.0          1.0
Plan Area          800          2400
Aspect Ratio       0.4          0.8
Damping            0.02         0.1
external Density_x 0.5          0.8
external Density_y 0.5          0.8
intternal Density  0.06         0.08
extEcc_x           0.0          0.5
extEcc_z           0.0          0.5
intEcc_x           0.0          0.5
intEcc_z           0.0          0.5
floor1 Weight      10.0         15.5
flower2 Weight     10.0         15.5
roof Weight        10.0         15.5
================== ============ =========

User want to make a surrogate model for drift ratio and roof accleration


Input discreption
-------------------

UQ tab
^^^^^^
Once UQ Engine is set to SimCenterUQ and Method Category is TrainGP Surrogate Model.


FEM tab
^^^^^^^^
Once user selects OpenSeesPy as FEM applications, three fields are required.

1. Input Script - CWH.py: This file is the main Python script which implements builds the model and run the analysis. It is supplied to the Input Script field of the FEM tab. Because this file write directly to results.out, it obviates the need for supplying a Postprocess Script. 

2. Postprocess Script - (empty): This optional script is not required.

3. Parameters File - params.py: This file is a Python script which defines the problem’s random variables as objects in the Python runtime. It is supplied to the Parameters File field of the FEM tab. The literal values which are assigned to variables in this file will be varied at runtime by the UQ engine.

The other subsidary scripts (including ground motion time history) are stored in the same directory of the main input script.

Results
-------

The results are as follows



Use surrogate model
-------------------

If user want to train more samples, they may,


