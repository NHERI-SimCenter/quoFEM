# written: Aakash Bangalore Satish @ NHERI SimCenter, UC Berkeley

import json
import os
import sys
import time
import importlib

from uqRunner import UqRunner


class HeirBayesRunner(UqRunner):
    def __init__(self) -> None:
        super().__init__()
        self.n_samples = 0
        self.n_burn_in = 0
        self.tuning_interval = 0
        self.seed = 0

    def storeUQData(self, uqData):
        for val in uqData["Parameters"]:
            if val["name"] == "File To Run":
                self.file_to_run = val["value"]
            elif val["name"] == "# Samples":
                self.n_samples = int(val["value"])
            elif val["name"] == "# Burn-in":
                self.n_burn_in = int(val["value"])
            elif val["name"] == "Tuning Interval":
                self.tuning_interval = int(val["value"])
            elif val["name"] == "Seed":
                self.seed = int(val["value"])

    def performHeirBayesSampling(self):
        self.dir_name = os.path.dirname(self.file_to_run)
        sys.path.append(self.dir_name)
        module_name = os.path.basename(self.file_to_run)
        module = importlib.import_module(module_name[:-3])
        self.heir_code = module.HeirBayesSampler()

        self.trace, self.time_taken, self.inf_object = self.heir_code.perform_sampling(
            n_samples=self.n_samples,
            n_burn_in=self.n_burn_in,
            tuning_interval=self.tuning_interval,
            seed=self.seed
        )

    def saveResultsToPklFile(self):
        self.saved_pickle_filename = self.heir_code.save_results(
            self.trace, self.time_taken, self.inf_object, prefix="realdata_filtered"
        )

    def createHeadingStringsList(self):
        self.params = ["fy", "E", "b", "cR1", "cR2", "a1", "a3"]
        self.num_params = len(self.params)
        self.num_coupons = 34

        # self.heading_list = ["interface"]
        self.heading_list = ["Sample#", "interface"]
        for i in range(self.num_coupons):
            for j in range(self.num_params):
                self.heading_list.append(
                    "".join(["Coupon_", str(i + 1), "_", self.params[j]])
                )

        for row in range(self.num_params):
            for col in range(row + 1):
                self.heading_list.append("".join(["Cov_", str(row + 1), str(col + 1)]))

        for par in self.params:
            self.heading_list.append("".join(["Mean_", par]))

        for sig in range(self.num_coupons):
            self.heading_list.append("".join(["ErrorVariance_", str(sig + 1)]))

    def makeHeadingRow(self, separator="\t"):
        self.headingRow = separator.join([item for item in self.heading_list])

    def makeOneRowString(self, sample_num, sample, separator="\t"):
        initial_string = separator.join([str(sample_num), "1"])
        coupon_string = separator.join(
            [
                str(sample[i][j])
                for i in range(self.num_coupons)
                for j in range(self.num_params)
            ]
        )
        cov_string = separator.join(
            [
                str(sample[self.num_coupons][row][col])
                for row in range(self.num_params)
                for col in range(row + 1)
            ]
        )
        mean_string = separator.join(
            [
                str(sample[self.num_coupons + 1][par_num])
                for par_num in range(self.num_params)
            ]
        )
        error_string = separator.join(
            [str(sample[-1][coupon_num]) for coupon_num in range(self.num_coupons)]
        )
        row_string = separator.join(
            [initial_string, coupon_string, cov_string, mean_string, error_string]
        )
        return row_string

    def makeTabularResultsFile(self, save_file_name="tabularResults.out", separator="\t"):
        self.createHeadingStringsList()
        self.makeHeadingRow(separator=separator)

        cwd = os.getcwd()
        save_file_dir = os.path.dirname(cwd)
        save_file_full_path = os.path.join(save_file_dir, save_file_name)
        with open(save_file_full_path, "w") as f:
            f.write(self.headingRow)
            f.write("\n")
            for sample_num, sample in enumerate(self.trace):
                row = self.makeOneRowString(
                    sample_num=sample_num, sample=sample, separator=separator
                )
                f.write(row)
                f.write("\n")

    # def makePlots(self):
    #     from temp_postprocess_real_data import make_plots

    #     make_plots(self.saved_pickle_filename)

    def startTimer(self):
        self.startingTime = time.time()

    def computeTimeElapsed(self):
        self.timeElapsed = time.time() - self.startingTime

    def printTimeElapsed(self):
        self.computeTimeElapsed()
        print("Time elapsed: {:0.2f} minutes".format(self.timeElapsed / 60))

    def startSectionTimer(self):
        self.sectionStartingTime = time.time()

    def resetSectionTimer(self):
        self.startSectionTimer()

    def computeSectionTimeElapsed(self):
        self.sectionTimeElapsed = time.time() - self.sectionStartingTime

    def printSectionTimeElapsed(self):
        self.computeSectionTimeElapsed()
        print("Time elapsed: {:0.2f} minutes".format(self.sectionTimeElapsed / 60))

    @staticmethod
    def printEndMessages():
        print("Heirarchical Bayesian estimation done!")

    def runUQ(
        self,
        uqData,
        simulationData,
        randomVarsData,
        demandParams,
        workingDir,
        runType,
        localAppDir,
        remoteAppDir,
    ):
        """
        This function configures and runs hierarchical Bayesian estimation based on the
        input UQ configuration, simulation configuration, random variables,
        and requested demand parameters

        Input:
        uqData:         JsonObject that UQ options as input into the quoFEM GUI
        simulationData: JsonObject that contains information on the analysis package to run and its
                        configuration as input in the quoFEM GUI
        randomVarsData: JsonObject that specifies the input random variables, their distributions,
                        and associated parameters as input in the quoFEM GUI
        demandParams:   JsonObject that specifies the demand parameters as input in the quoFEM GUI
        workingDir:     Directory in which to run simulations and store temporary results
        runType:        Specifies whether computations are being run locally or on an HPC cluster
        localAppDir:    Directory containing apps for local run
        remoteAppDir:   Directory containing apps for remote run
        """
        self.startTimer()
        self.storeUQData(uqData=uqData)
        os.chdir(workingDir)
        self.performHeirBayesSampling()
        self.saveResultsToPklFile()
        self.makeTabularResultsFile()
        # self.makePlots()
        self.printTimeElapsed()
        self.printEndMessages()


class testRunUQ:
    def __init__(self, json_file_path_string) -> None:
        self.json_file_path_string = json_file_path_string
        self.getUQData()
        self.createRunner()
        self.runTest()

    def getUQData(self):
        with open(os.path.abspath(self.json_file_path_string), "r") as f:
            input_data = json.load(f)

        self.ApplicationData = input_data["Applications"]
        self.uqData = input_data["UQ_Method"]
        self.simulationData = self.ApplicationData["FEM"]
        self.randomVarsData = input_data["randomVariables"]
        self.demandParams = input_data["EDP"]
        self.localAppDir = input_data["localAppDir"]
        self.remoteAppDir = input_data["remoteAppDir"]
        self.workingDir = input_data["workingDir"]
        self.workingDir = os.path.join(self.workingDir, "tmp.SimCenter", "templateDir")
        self.runType = "runningLocal"

    def createRunner(self):
        self.runner = HeirBayesRunner()

    def runTest(self):
        self.runner.runUQ(
            uqData=self.uqData,
            simulationData=self.simulationData,
            randomVarsData=self.randomVarsData,
            demandParams=self.demandParams,
            workingDir=self.workingDir,
            runType=self.runType,
            localAppDir=self.localAppDir,
            remoteAppDir=self.remoteAppDir,
        )


def main():
    filename = "/Users/aakash/Desktop/SimCenter/Joel/heirarchical-refactor/scInput.json"
    testRunUQ(filename)


if __name__ == "__main__":
    main()