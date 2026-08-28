.. _qfem-0000:

Example to test all the supported random variables
==================================================

This example uses all the 11 marginal probability distrbutions that are currently supported in the SimCenterUQ engine. Some of the random variables are correlated with one another. The definitions of the random variables and correlation matrix are as shown in the figures :numref:`fig-RV1`, :numref:`fig-RV2`, :numref:`fig-RV3`, and :numref:`fig-RV4` below:

.. _fig-RV1:

.. figure:: figures/RV1.png
  :align: center
  :alt: Screenshot of a software interface displaying a section titled "Input Random Variables" with rows of input parameters for variables such as pDamp, Es1, Es2, Es3, and Esb. Each row includes fields for Variable Name, Input Type, and Distribution type (Uniform, Normal, Lognormal, Beta, etc.), along with specific parameters like Min, Max, Mean, Standard Deviation, lambda, zeta, alpha, and beta. The interface also has buttons for Add, Clear All, Correlation Matrix, Export, and Import. On the left side, there are tabs labeled UQ, FEM, RV, EDP, and RES, with the RV tab highlighted, indicating the current view is focused on random variables.
  :figclass: align-center

  Random variable definitions.

.. _fig-RV2:

.. figure:: figures/RV2.png
  :align: center
  :alt: Screenshot of a software interface with a menu on the left side labeled UQ, FEM, RV, EDP, and RES, with RV highlighted. The main section is titled "Input Random Variables" with a table beneath it listing five variables, each with fields for Variable Name, Input Type, Distribution, and associated parameters. The distributions listed are Weibull, Gumbel, Gamma, Chisquare, and Exponential with various parameters set for each. Buttons for "Add," "Clear All," "Correlation Matrix," "Export," and "Import" are displayed at the top of the section.
  :figclass: align-center

  Random variable definitions (continued).

.. _fig-RV3:

.. figure:: figures/RV3.png
  :align: center
  :alt: Screenshot of a software interface for inputting random variables with options for specifying the variable name, input type, and distribution. Several variables such as "Fyb," "b1," "b2," "b3," and "bb" are listed with associated distributions like Chi-square, Exponential, Discrete, Truncated exponential, and Normal. Parameters for these distributions are editable, and there are buttons to "Show PDF" or "Show PMF" for the probability distributions. The interface has a sidebar with categories such as "UQ," "FEM," "RV," "EDP," and "RES" highlighted, and there are buttons at the top for "Add," "Clear All," and "Correlation Matrix," as well as "Export" and "Import" options.
  :figclass: align-center

  Random variable definitions (continued).


.. _fig-RV_corr_matrix:

.. figure:: figures/CorrMatrix.png
  :align: center
  :alt: A matrix table displaying the relationship between various elements denoted by codes such as "pDamp", "Es1", "Fy1", "b1", etc. Each row and column header represents a different element, and the cells contain numerical values that indicate the degree of interaction or correspondence, with "1.0" representing a full match or presence and "0.5" indicating partial. Most cells contain a "0", which suggests no interaction or absence of a relationship between those specific elements. The background of alternating rows is shaded to enhance readability.
  :figclass: align-center

  Correlation matrix definition.
