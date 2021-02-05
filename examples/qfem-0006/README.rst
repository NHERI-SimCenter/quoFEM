
Optimization
============

+-----------------+------------------------------------------------------------------------------------------------------------------------------------------------+
| Problem files   | `quo-06 <https://github.com/claudioperez/SimCenterDocumentation/tree/examples/docs/common/user_manual/examples/desktop/quoFEM/src/quo-06>`__   |
+-----------------+------------------------------------------------------------------------------------------------------------------------------------------------+

In this example, a **parameter estimation** routine is used to solve a
classical optimization problem for which an analytic solution is known.

The Rosenbrock function is a *test function* that is often used to
evaluate numerical optimization algorithms. It is given by the following
expression:

.. math::  f(x, y)=(a-x)^{2}+b\left(y-x^{2}\right)^{2} 

The following parameters are defined in the **RV** tab of quoFEM:

1. First variable, ``X``: **Uniform** distribution with a lower bound
   :math:`(L_B)` of :math:`-2.0`, upper bound :math:`(U_B)` of
   :math:`2.0`,

2. Second variable, ``Y``: **Uniform** distribution with a lower bound
   :math:`(L_B)` of :math:`-2.0`, upper bound :math:`(U_B)` of
   :math:`2.0`,

UQ Workflow
-----------

To define the uncertainty workflow in quoFEM, select **Parameters
Estimation** for the **Dakota Method Category**, and enter the following
inputs:

+--------------------------+----------+
| **Convergence Tol**      | 1e-10    |
+--------------------------+----------+
| **Scaling Factors**      |          |
+--------------------------+----------+
| **Max No. Iterations**   | 50       |
+--------------------------+----------+
| **Method**               | NL2SOL   |
+--------------------------+----------+

Model Files
-----------

The following files make up the **FEM** model definition.

#. `rosenbrock.py <https://raw.githubusercontent.com/claudioperez/SimCenterExamples/master/static/rosenbrock/rosenbrock.py>`__:
   This file is a Python script which implements the Rosenbrock
   function. It is supplied to the **Input Script** field of the **FEM**
   tab. Because this file write directly to ``results.out``, it obviates
   the need for supplying a **Postprocess Script**. When invoked in the
   workflow, the Python routine is supplied a set of random variable
   realizations through the star-import of the script supplied to the
   **Parameters File** field.

#. `params.py <https://raw.githubusercontent.com/claudioperez/SimCenterExamples/master/static/rosenbrock/params.py>`__:
   This file is a Python script which defines the problem's random
   variables as objects in the Python runtime. It is supplied to the
   **Parameters File** field of the **FEM** tab. *The literal values
   which are assigned to variables in this file will be varied at
   runtime by the UQ engine.*


Results
-------

With :math:`50` iterations, our solution converges to :math:`x= 0.34548`
and :math:`y=0.11`

