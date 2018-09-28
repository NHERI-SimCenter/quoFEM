import os
import warnings
import numpy as np
import scipy.stats as st
import statsmodels as sm
import sys



#############################################################################################
# Create models from data
def best_fit_distribution(data, bins, ax=None):
    """Model data by finding best fit distribution to data"""
    # Get histogram of original data
    y, x = np.histogram(data, bins=bins, density=True)
    x = (x + np.roll(x, -1))[:-1] / 2.0

    # Distributions to check
    DISTRIBUTIONS = [        
        st.alpha,st.beta,st.betaprime,st.cauchy,st.chi,st.chi2,st.dgamma,st.dweibull,st.expon,st.exponnorm,st.exponweib,
        st.exponpow,st.f,st.foldcauchy,st.foldnorm,st.frechet_r,st.frechet_l,st.genpareto,st.gennorm,st.genexpon,
        st.gausshyper,st.gamma,st.gengamma,st.gumbel_r,st.halfnorm,st.halfgennorm,st.invgamma,st.invgauss,
        st.invweibull,st.laplace,st.lognorm,st.norm,st.pareto,st.pearson3,st.powerlaw,st.powernorm,st.rdist,st.reciprocal,st.rayleigh,
        st.rice,st.recipinvgauss,st.semicircular,st.t,st.triang,st.truncexpon,st.truncnorm,st.uniform,st.vonmises
    ]

  #DISTRIBUTIONS = [        
  #       st.alpha,st.anglit,st.arcsine,st.beta,st.betaprime,st.bradford,st.burr,st.cauchy,st.chi,st.chi2,st.cosine,
  #      st.dgamma,st.dweibull,st.erlang,st.expon,st.exponnorm,st.exponweib,st.exponpow,st.f,st.fatiguelife,st.fisk,
  #      st.foldcauchy,st.foldnorm,st.frechet_r,st.frechet_l,st.genlogistic,st.genpareto,st.gennorm,st.genexpon,
  #      st.genextreme,st.gausshyper,st.gamma,st.gengamma,st.genhalflogistic,st.gilbrat,st.gompertz,st.gumbel_r,
  #      st.gumbel_l,st.halfcauchy,st.halflogistic,st.halfnorm,st.halfgennorm,st.hypsecant,st.invgamma,st.invgauss,
  #      st.invweibull,st.johnsonsb,st.johnsonsu,st.ksone,st.kstwobign,st.laplace,st.levy,st.levy_l,st.levy_stable,
  #      st.logistic,st.loggamma,st.loglaplace,st.lognorm,st.lomax,st.maxwell,st.mielke,st.nakagami,st.ncx2,st.ncf,
  #      st.nct,st.norm,st.pareto,st.pearson3,st.powerlaw,st.powerlognorm,st.powernorm,st.rdist,st.reciprocal,
  #      st.rayleigh,st.rice,st.recipinvgauss,st.semicircular,st.t,st.triang,st.truncexpon,st.truncnorm,st.tukeylambda,
  #      st.uniform,st.vonmises,st.vonmises_line,st.wald,st.weibull_min,st.weibull_max,st.wrapcauchy
  #  ]

    # Best holders
    best_distribution = st.norm
    best_params = (0.0, 1.0)
    best_sse = np.inf

    # Estimate distribution parameters from data
    for distribution in DISTRIBUTIONS:

        # Try to fit the distribution
        try:
            # Ignore warnings from data that can't be fit
            with warnings.catch_warnings():
                warnings.filterwarnings('ignore')

                # fit dist to data
                params = distribution.fit(data)

                # Separate parts of parameters
                arg = params[:-2]
                loc = params[-2]
                scale = params[-1]

                # Calculate fitted PDF and error with fit in distribution
                pdf = distribution.pdf(x, loc=loc, scale=scale, *arg)
                sse = np.sum(np.power(y - pdf, 2.0))

                # if axis pass in add to plot
                try:
                    if ax:
                        pd.Series(pdf, x).plot(ax=ax)
                    end
                except Exception:
                    pass

                # identify if this distribution is better
                if best_sse > sse > 0:
                    best_distribution = distribution
                    best_params = params
                    best_sse = sse

        except Exception:
            pass

    return (best_distribution.name, best_params)
#############################################################################################


#############################################################################################
def make_pdf(dist, params, size=1000):
    """Generate distributions's Probability Distribution Function """

    # Separate parts of parameters
    arg = params[:-2]
    loc = params[-2]
    scale = params[-1]

    # Get sane start and end points of distribution
    start = dist.ppf(0.01, *arg, loc=loc, scale=scale) if arg else dist.ppf(0.01, loc=loc, scale=scale)
    end = dist.ppf(0.99, *arg, loc=loc, scale=scale) if arg else dist.ppf(0.99, loc=loc, scale=scale)

    # Build PDF
    x = np.linspace(start, end, size)
    y = dist.pdf(x, loc=loc, scale=scale, *arg)

    return x, y
#############################################################################################

if __name__ == '__main__':
    #you hard-coded the path for the data_input.txt and you MUST change it so that it is generic for Mac or Windows etc. 
    os.getcwd()
    #data = np.loadtxt("/Users/simcenter/Codes/SimCenter/build-uqFEM-Desktop_Qt_5_11_1_clang_64bit-Debug/uqFEM.app/Contents/MacOS/data_input.txt")
    data = np.loadtxt(str(sys.argv[1]))

    # Find best fit distribution
    best_fit_name, best_fit_params = best_fit_distribution(data, len(data))
    best_dist = getattr(st, best_fit_name)
    x, y = make_pdf(best_dist, best_fit_params)

    f = open("Best_fit.out", "w")
    for k in range(len(x)):
        f.write(str(x[k])+", "+str(y[k])+"\n")
    f.close()

    # Get best distribution names, best parameter names and values
    dist = getattr(st, best_fit_name)
    best_fit_parameters_name = dist.shapes



    arg = best_fit_params[:-2]
    loc = best_fit_params[-2]
    scale = best_fit_params[-1]



    
    f = open("data_fit_info.out", "w")
    f.write("Best distribution name is: "+str(dist.name)+"\n\n")
    f.write("Best distribution parameters are:\n\n")

    count = 1
    for i in range(len(arg)):
        f.write(str(i+1)+". "+str(best_fit_parameters_name[i])+" = "+str(np.round(arg[i],4))+"\n\n")
        count = count + 1

    f.write(str(count)+". loc = "+str(np.round(loc,4))+"\n \n")
    count = count + 1
    f.write(str(count)+". scale = "+str(np.round(scale,4))+"\n \n")
#    f.write("For more information on continuous distributions\n")
    f.write("See https://docs.scipy.org/doc/scipy/reference/stats.html \n\n")
    f.write("for further information on distribution parameters\n")
#    f.write("**********************************************************\n")
    f.close()
#############################################################################################
