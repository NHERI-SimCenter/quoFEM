"""
@author: Mukesh, Maitreya, Conte, Aakash

"""

import numpy as np
import scipy.stats as stats


class Uniform:
    # Method with in this uniform class
    def __init__(self, lower, upper):  # method receives instance as first argument automatically
        # the below are the instance variables
        self.lower = lower
        self.upper = upper

    # Method to generate random numbers
    def generate_rns(self, N):
        return (self.upper - self.lower) * np.random.rand(N) + self.lower

    # Method to compute log of the pdf at x
    def log_pdf_eval(self, x):
        if (x - self.upper) * (x - self.lower) <= 0:
            lp = np.log(1 / (self.upper - self.lower))
        else:
            lp = -np.Inf
        return lp


class Halfnormal:
    def __init__(self, sig):
        self.sig = sig

    def generate_rns(self, N):
        return self.sig * np.abs(np.random.randn(N))

    def log_pdf_eval(self, x):
        if x >= 0:
            lp = -np.log(self.sig) + 0.5 * np.log(2 / np.pi) - ((x * x) / (2 * self.sig * self.sig))
        else:
            lp = -np.Inf
        return lp


class Normal:
    def __init__(self, mu, sig):
        self.mu = mu
        self.sig = sig

    def generate_rns(self, N):
        return self.sig * np.random.randn(N) + self.mu

    def log_pdf_eval(self, x):
        lp = -0.5 * np.log(2 * np.pi) - np.log(self.sig) - 0.5 * (((x - self.mu) / self.sig) ** 2)
        return lp


class TrunNormal:
    def __init__(self, mu, sig, a, b):
        self.mu = mu
        self.sig = sig
        self.a = a
        self.b = b

    def generate_rns(self, N):
        return stats.truncnorm((self.a - self.mu) / self.sig, (self.b - self.mu) / self.sig, loc=self.mu,
                               scale=self.sig).rvs(N)

    def log_pdf_eval(self, x):
        lp = stats.truncnorm((self.a - self.mu) / self.sig, (self.b - self.mu) / self.sig, loc=self.mu,
                             scale=self.sig).logpdf(x)
        return lp


class mvNormal:
    def __init__(self, mu, E):
        self.mu = mu
        self.E = E
        self.d = len(mu)
        self.logdetE = np.log(np.linalg.det(self.E))
        self.Einv = np.linalg.inv(E)

    def generate_rns(self, N):
        return np.random.multivariate_normal(self.mu, self.E, N)

    def log_pdf_eval(self, x):
        xc = (x - self.mu)
        return -(0.5 * self.d * np.log(2 * np.pi)) - (0.5 * self.logdetE) - (0.5 * np.transpose(xc) @ self.Einv @ xc)


class InvGamma:
    def __init__(self, a, b):
        self.a = a
        self.b = b

    def generate_rns(self, N):
        dist = stats.invgamma(self.a, scale=self.b)
        return dist.rvs(size=N)

    def log_pdf_eval(self, x):
        dist = stats.invgamma(self.a, scale=self.b)
        return dist.logpdf(x)


class BetaDist:
    def __init__(self, alpha, beta, lowerbound, upperbound):
        self.alpha = alpha
        self.beta = beta
        self.lowerbound = lowerbound
        self.upperbound = upperbound

    def generate_rns(self, N):
        dist = stats.beta(self.alpha, self.beta, self.lowerbound, self.upperbound)
        return dist.rvs(size=N)

    def log_pdf_eval(self, x):
        dist = stats.beta(self.alpha, self.beta, self.lowerbound, self.upperbound)
        return dist.logpdf(x)


class LogNormDist:
    def __init__(self, mu, sigma):
        self.mu = mu
        self.sigma = sigma
        self.s = self.sigma
        self.loc = 0
        self.scale = np.exp(mu)

    def generate_rns(self, N):
        dist = stats.lognorm(s=self.s, loc=self.loc, scale=self.scale)
        return dist.rvs(size=N)

    def log_pdf_eval(self, x):
        dist = stats.lognorm(s=self.s, loc=self.loc, scale=self.scale)
        return dist.logpdf(x)


class GumbelDist:
    def __init__(self, alpha, beta):
        self.alpha = alpha
        self.beta = beta

    def generate_rns(self, N):
        dist = stats.gumbel_r(loc=self.beta, scale=(1 / self.alpha))
        return dist.rvs(size=N)

    def log_pdf_eval(self, x):
        dist = stats.gumbel_r(loc=self.beta, scale=(1 / self.alpha))
        return dist.logpdf(x)


class WeibullDist:
    def __init__(self, shape, scale):
        self.shape = shape
        self.scale = scale

    def generate_rns(self, N):
        dist = stats.weibull_min(c=self.shape, scale=self.scale)
        return dist.rvs(size=N)

    def log_pdf_eval(self, x):
        dist = stats.weibull_min(c=self.shape, scale=self.scale)
        return dist.logpdf(x)
