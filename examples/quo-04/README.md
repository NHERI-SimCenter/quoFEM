

# Sensitivity Analysis

|  |  |
|----------|------|
| Problem files | [quo-04](https://github.com/claudioperez/SimCenterDocumentation/tree/examples/docs/common/user_manual/examples/desktop/quoFEM/src/quo-04) |

This example uses quoFEM to perform a global sensitivity analysis of an OpenSees FE model.

Consider a stochastic model of a two-dimensional truss structure like that shown in the following figure. A [sensitivity analysis](/common/user_manual/usage/desktop/DakotaSensitivity.html) procedure is coordinated by quoFEM which will estimate the sensitivities of the response quantities of interest with respect to the problem's random variables.
![Truss schematic diagram](truss/truss.png)



The following parameters are defined in the **RV** tab of quoFEM:


1. Elastic modulus, `E`: **Weibull** distribution with a  scale parameter $(\lambda)$ of $210.0$,  shape parameter $(k)$ of $20.0$, 

1. Load magnitude, `P`: **Beta** distribution with a  first shape parameter $(\alpha)$ of $2.0$,  second shape parameter $(\beta)$ of $2.0$,  lower bound $(L_B)$ of $20.0$,  upper bound $(U_B)$ of $30.0$, 

1. Cross sectional area for the other six bars, `Ao`: **Lognormal** distribution with a  mean $(\mu)$ of $250.0$,  standard deviation $(\sigma)$ of $50.0$, 

1. Cross sectional area for the upper three bars, `Au`: **Normal** distribution with a  mean $(\mu)$ of $500.0$,  standard deviation $(\sigma)$ of $100.0$, 




## UQ Workflow


To define the uncertainty workflow in quoFEM, select **Sensitivity Analysis** for the **Dakota Method Category**, and enter the following inputs:



|   |   |
|---|---|
| **Method** | LHS |
| **Samples** | 1000 |
| **Seed** | 175 |



## Model Files


The following files make up the **FEM** model definition.


#. [model.tcl](https://raw.githubusercontent.com/claudioperez/SimCenterExamples/master/static/truss/model.tcl): This file is an OpenSees Tcl script that constructs and runs a finite element analysis of the truss for a given realization of the problem's random variables. It is supplied to the **Input File** field of the **FEM** tab.

#. [post.tcl](https://raw.githubusercontent.com/claudioperez/SimCenterExamples/master/static/truss/post.tcl): This file is an OpenSees Tcl script that processes the QoI identifiers supplied in the **QoI** tab, and writes the relevant response quantities to `results.out` from an OpenSees process. It is supplied to the **Postprocess File** field of the **FEM** tab.



<!-- <div class="admonition warning">Do not place the files in your root, downloads, or desktop folder as when the application runs it will copy the contents on the directories and subdirectories containing these files multiple times. If you are like us, your root, Downloads or Documents folders contains and awful lot of files and when the backend workflow runs you will slowly find you will run out of disk space!</div> -->

## Results

Once the analysis is complete the **RES** tab will be automatically selected and the results will be displayed as shown in the following figure:

![Sensitivity analysis results for simple truss.](truss/trussSensitivity-RES.png)


<div id="vis"></div>
<script>
    // Assign the specification to a local variable vlSpec.
    var vlSpec = {
    $schema: 'https://vega.github.io/schema/vega-lite/v4.json',
    data: {
        values: [
        {a: 'C', b: 2},
        {a: 'C', b: 7},
        {a: 'C', b: 4},
        {a: 'D', b: 1},
        {a: 'D', b: 2},
        {a: 'D', b: 6},
        {a: 'E', b: 8},
        {a: 'E', b: 4},
        {a: 'E', b: 7}
        ]
    },
    mark: 'bar',
    encoding: {
        y: {field: 'a', type: 'nominal'},
        x: {
        aggregate: 'average',
        field: 'b',
        type: 'quantitative',
        axis: {
            title: 'Average of b'
        }
        }
    }
    };

    // Embed the visualization in the container with id `vis`
    vegaEmbed('#vis', vlSpec);
</script>
