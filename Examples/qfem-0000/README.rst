.. _qfem-0000:

Example to test all the supported random variables
==================================================

This example uses all the 11 marginal probability distrbutions that are currently supported in the SimCenterUQ engine. Some of the random variables are correlated with one another. The definitions of the random variables and correlation matrix are as shown in the figures :numref:`fig-RV1`, :numref:`fig-RV2`, :numref:`fig-RV3`, and :numref:`fig-RV4` below:

.. _fig-RV1:

.. figure:: figures/RV1.png
  :align: center
   :alt: A screenshot of a software interface for inputting random variables with options to add, clear, and analyze correlations. The interface shows a list of variables such as 'pDamp' with a uniform distribution and specified minimum and maximum values, 'Es1' with a normal distribution and specified mean and standard deviation, 'Es2' with a lognormal distribution and parameters lambda and zeta, 'Es3' with a beta distribution and parameters alpha and beta, and 'Esb' with a uniform distribution and specified minimum and maximum values. Each variable entry has an option to show its probability distribution function (PDF). On the left side, there are tabs labeled UQ, FEM, RV, EDP, and RES, indicating different modules or sections of the software.
  :figclass: align-center

  Random variable definitions.

.. _fig-RV2:

.. figure:: figures/RV2.png
  :align: center
   :alt: Screenshot of a software interface for inputting random variables with a focus on statistical distributions. The interface includes options for adding, clearing, and importing variables along with a button to access a correlation matrix. Five variables are listed, each with different types of distributions: Weibull, Gumbel, Gamma, Chisquare, and Exponential, with associated parameters such as scale and shape for Weibull, alpha and beta for Gumbel, k and lambda for Gamma and Chisquare, and lambda for Exponential. Each variable row includes options to edit or show the probability density function (PDF). On the left side, navigation tabs labeled UQ, FEM, RV, EDP, and RES are visible, with the RV (Random Variables) tab highlighted.
  :figclass: align-center

  Random variable definitions (continued).

.. _fig-RV3:

.. figure:: figures/RV3.png
  :align: center
   :alt: Screenshot of a software interface for inputting random variables with fields for variable name, input type, and distribution. There are buttons to add new variables, clear all entries, and access a correlation matrix, as well as to import or export configurations. Several example variables are listed with different distributions like Chi-square, Exponential, Discrete, Truncated Exponential, and Normal, along with parameters like `k`, `lambda`, values, weights, minimum, and maximum. Options to show probability distribution functions (PDF) or probability mass functions (PMF) for each variable are also visible.
  :figclass: align-center

  Random variable definitions (continued).


.. _fig-RV_corr_matrix:

.. figure:: figures/CorrMatrix.png
  :align: center
   :alt: A matrix with cells filled with numerical data in a table format. Rows are labeled from 'pDamp' to 'bb', and columns with the same labels. The cells contain values of either 0, 0.5, or 1.0, indicating some kind of relational data or measurements between the row and column entities. The matrix seems to represent a symmetric relationship, as the table is mirrored along the diagonal where all values are 1.0. The background alternates between white and gray for each row, possibly for ease of reading.
  :figclass: align-center

  Correlation matrix definition.
