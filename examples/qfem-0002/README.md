
# Forward Propagation - OpenSeesPy

|  |  |
|----------|------|
| Problem files | [quo-02](https://github.com/claudioperez/SimCenterDocumentation/tree/examples/docs/common/user_manual/examples/desktop/quoFEM/src/quo-02) |

This example illustrates how quoFEM interacts with OpenSeesPy. A simple forward propagation procedure is run to estimate the first and second central moments of a FE model's response, given the marginal distributions of various random parameters.

Consider the problem of uncertainty quantification in a two-dimensional truss structure shown in the following figure. Two input scripts are used to define a forward propagation procedure to be coordinated by quoFEM which will estimate the mean and standard deviation of the vertical displacement at node 3 using Latin hypercube sampling.

![Truss schematic diagram](figures/truss.png)


The following parameters are defined in the **RV** tab of quoFEM:


1. Elastic modulus, `E`: **Weibull** distribution with a  scale parameter $(\lambda)$ of $210.0$,  shape parameter $(k)$ of $20.0$, 

1. Load magnitude, `P`: **Beta** distribution with a  first shape parameter $(\alpha)$ of $2.0$,  second shape parameter $(\beta)$ of $2.0$,  lower bound $(L_B)$ of $20.0$,  upper bound $(U_B)$ of $30.0$, 

1. Cross sectional area for the other six bars, `Ao`: **Lognormal** distribution with a  mean $(\mu)$ of $250.0$,  standard deviation $(\sigma)$ of $50.0$, 

1. Cross sectional area for the upper three bars, `Au`: **Normal** distribution with a  mean $(\mu)$ of $500.0$,  standard deviation $(\sigma)$ of $100.0$, 



## UQ Workflow


To define the uncertainty workflow in quoFEM, select **Forward Propagation** for the **Dakota Method Category**, and enter the following inputs:


|   |   |
|---|---|
| **Method** | LHS |
| **Samples** | 200 |
| **Seed** | 949 |



## Model Files


The following files make up the **FEM** model definition.


#. [model.py](https://raw.githubusercontent.com/claudioperez/SimCenterExamples/master/static/truss/model.py): This file is a Python script which takes a given realization of the problem's random variables, and runs a finite element analysis of the truss with OpenSeesPy. It is supplied to the **Input Script** field of the **FEM** tab, and obviates the need for supplying a **Postprocess Script**. When this script is invoked in the workflow, it receives the list of the identifiers supplied in the **QoI** tab through the operating system's `stdout` variable, and a set of random variable realizations by star-importing the **Parameters File** from the **FEM** tab.

#. [params.py](https://raw.githubusercontent.com/claudioperez/SimCenterExamples/master/static/truss/params.py): This file is a Python script which defines the problem's random variables as objects in the Python runtime. It is supplied to the **Parameters File** field of the **FEM** tab. *The literal values which are assigned to variables in this file will be varied at runtime by the UQ engine.*



<!-- <div class="admonition warning">Do not place the files in your root, downloads, or desktop folder as when the application runs it will copy the contents on the directories and subdirectories containing these files multiple times. If you are like us, your root, Downloads or Documents folders contains and awful lot of files and when the backend workflow runs you will slowly find you will run out of disk space!</div> -->

## Results

The results from this analysis with a maximum of $200$ iterations are as follows: 

**Node 3**:

- $\mu = 7.6986$
- $\sigma = 1.5666$

**Node 2**:

- $\mu = 9.3967$
- $\sigma = 1.8628$

If the user selects **Data** in the **RES** tab, they will be presented with both a graphical plot and a tabular listing of the data. Various views of the graphical display can be obtained by left- and right-clicking the columns of the tabular data. If a singular column of the tabular data is selected with simultaneous right and left clicks, a frequency and CDF will be displayed.


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
