import os
import glob

import numpy as np
from scipy.stats import multivariate_normal as mvn


def read_synthetic_data(folder_location):

    strain_all_tests = []
    stress_all_tests = []

    data_files = glob.glob(os.path.join(folder_location, "*.csv"))

    counter = 1

    for data_file in data_files:
        data = np.genfromtxt(data_file, delimiter=" ")
        strain_history = data[:, 0]
        stress_data = data[:, 1]

        strain_all_tests.append(strain_history)
        stress_all_tests.append(stress_data)
        counter += 1

    return strain_all_tests, stress_all_tests


# def create_synthetic_data(folder_location, seed=0):
#     np.random.seed(seed=seed)
