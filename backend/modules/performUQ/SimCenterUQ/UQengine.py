import numpy as np
import shutil
import os
import glob
import subprocess
import time
import sys
import stat



class UQengine:

    def __init__(self, inputArgs):
        self.work_dir = inputArgs[1].replace(os.sep, '/')
        self.inputFile = inputArgs[2]
        self.workflowDriver = inputArgs[3]
        self.os_type = inputArgs[4]
        self.run_type = inputArgs[5]
        
        # self.workflowDriver = "workflow_driver"
        # if self.os_type.lower().startswith('win'):
        #     self.workflowDriver = "workflow_driver.bat"
        self.create_errLog()

    def cleanup_workdir(self):
        # Cleanup working directory if needed
        del_paths = glob.glob(os.path.join(self.work_dir, 'workdir*'))
        for del_path in del_paths:
            # change permission for  workflow_driver.bat
            self.workflowDriver_path = os.path.join(del_path, self.workflowDriver)
            # if os.path.exists(self.workflowDriver_path):
            #     os.chmod(self.workflowDriver_path, stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)

            #Change permission
            for root, dirs, files in os.walk(del_path):
                for d in dirs:
                    os.chmod(os.path.join(root, d), stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)
                for f in files:
                    os.chmod(os.path.join(root, f), stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)

            try:
               shutil.rmtree(del_path)
            except Exception as msg:
                self.exit(str(msg))

        del_outputs = glob.glob(os.path.join(self.work_dir, '*out'))
        for del_out in del_outputs:
            os.remove(del_out)

        del_pkls = glob.glob(os.path.join(self.work_dir, '*pkl'))
        for del_pkl in del_pkls:
            os.remove(del_pkl)

        try:
            del_errs = glob.glob(os.path.join(self.work_dir, '*err'))
            for del_err in del_errs:
                os.remove(del_err)
        except:
            pass

        print("working directory cleared")

    def set_FEM(self, rv_name, do_parallel, y_dim, t_init, t_thr):
        self.rv_name = rv_name
        self.do_parallel = do_parallel
        self.y_dim = y_dim
        self.t_init = t_init
        self.t_thr = t_thr
        self.total_sim_time = 0

    def run_FEM_batch(self, X, id_sim, runIdx=0):
        if runIdx == -1:
            #dummy run
            return X, np.zeros((0,self.y_dim)), id_sim
        workflowDriver = self.workflowDriver
        #
        # serial run
        #

        X = np.atleast_2d(X)
        nsamp = X.shape[0]
        if not self.do_parallel:
            Y = np.zeros((nsamp,self.y_dim))
            for ns in range(nsamp):
                Y_tmp, id_sim_current = run_FEM(X[ns,:],id_sim+ns, self.rv_name, self.work_dir, workflowDriver, runIdx)
                if Y_tmp.shape[0] != self.y_dim:
                    msg = "model output <results.out> contains {} value(s) while the number of QoIs specified in quoFEM is {}".format(Y_tmp.shape[0],y_dim)
                    self.exit(msg)
                Y[ns,:] = Y_tmp
                if time.time() - self.t_init > self.t_thr:
                    X = X[:ns, :]
                    Y = Y[:ns, :]
                    break
            Nsim = id_sim_current - id_sim + 1

        #
        # parallel run
        #

        if self.do_parallel:
            print("Running {} simulations in parallel".format(nsamp))
            tmp = time.time()
            iterables = ((X[i, :][np.newaxis], id_sim + i,  self.rv_name, self.work_dir, self.workflowDriver, runIdx) for i in range(nsamp))
            try:
                result_objs = list(self.pool.starmap(run_FEM, iterables))
                print("Simulation time = {} s".format(time.time() - tmp));
            except KeyboardInterrupt:
                print("Ctrl+c received, terminating and joining pool.")
                try:
                    self.pool.shutdown()
                except Exception:
                    sys.exit()

            Nsim = len(list((result_objs)))
            Y = np.zeros((Nsim, self.y_dim))
            for val, id in result_objs:
                if isinstance(val, str):
                    self.exit(val)
                elif val.shape[0]:
                    if val.shape[0] != self.y_dim:
                        msg = "model output <results.out> contains {} value(s) while the number of QoIs specified in quoFEM is {}".format(
                            val.shape[0], self.y_dim)
                        self.exit(msg)

                if np.isnan(np.sum(val)):
                    Nsim = id - id_sim
                    X = X[:Nsim, :]
                    Y = Y[:Nsim, :]
                else:
                    Y[id - id_sim, :] = val

        return X, Y, id_sim + Nsim

    def readJson(self):
        pass

    def make_pool(self, ):
        if self.run_type.lower() == 'runninglocal':
            from multiprocessing import Pool
            n_processor = os.cpu_count()
            pool = Pool(n_processor)
        else:
            from mpi4py import MPI
            from mpi4py.futures import MPIPoolExecutor
            self.world = MPI.COMM_WORLD
            n_processor = self.world.Get_size()
            pool = MPIPoolExecutor()
        return n_processor, pool

    #
    # Someplace to write down error messages
    #


    def create_errLog(self):
        self.errfile = open('{}/dakota.err'.format(self.work_dir), "w")

    def exit(self, msg):
        print(msg)
        self.errfile.write(msg)
        self.errfile.close()
        exit(-1)

    def terminate_errLog(self):
        self.errfile.close()

    #
    # To read text
    #



def run_FEM(X, id_sim, rv_name, work_dir, workflowDriver, runIdx=0):

    if runIdx == 0:
        templatedirFolder = '/templatedir'
        workdirFolder = '/workdir.' + str(id_sim + 1)
    else:
        templatedirFolder = '/templatedir.' + str(runIdx)
        workdirFolder = '/workdir.' + str(runIdx) + '.' + str(id_sim + 1)

    X = np.atleast_2d(X)
    x_dim = X.shape[1]

    if X.shape[0] > 1:
        msg = 'do one simulation at a time'
        return msg, id_sim
    #
    # (1) create "workdir.idx " folder :need C++17 to use the files system namespace
    #

    current_dir_i = work_dir + workdirFolder
    try:
        shutil.copytree(work_dir + templatedirFolder, current_dir_i)
    except Exception as ex:
        try:

            shutil.copytree(work_dir + templatedirFolder, current_dir_i)

        except Exception as ex:
            msg = 'Error running FEM: ' + str(ex)
            return msg, id_sim

    #
    # (2) write param.in file
    #

    outF = open(current_dir_i + '/params.in', 'w')
    outF.write('{}\n'.format(x_dim))
    for i in range(x_dim):
        outF.write('{} {}\n'.format(rv_name[i], X[0, i]))
    outF.close()

    #
    # (3) run workflow_driver.bat
    #

    os.chdir(current_dir_i)
    workflow_run_command = '{}/{}'.format(current_dir_i, workflowDriver)
    subprocess.check_call(workflow_run_command, shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
    if runIdx==0:
        print("RUNNING FEM: working directory {} created".format(id_sim + 1))
    else:
        print("RUNNING FEM: working directory {}-{} created".format(runIdx,id_sim + 1))
    #subprocess.check_call(workflow_run_command, shell=True, stdout=FNULL, stderr=subprocess.STDOUT)
    #
    # (4) reading results
    #

    if glob.glob('results.out'):
        g = np.loadtxt('results.out').flatten()
    else:
        msg = 'Error running FEM: results.out missing at ' + current_dir_i
        return msg, id_sim

    if g.shape[0] == 0:
        msg = 'Error running FEM: results.out is empty'
        return msg, id_sim

    os.chdir("../")

    if np.isnan(np.sum(g)):
        msg = 'Error running FEM: Response value at workdir.{} is NaN'.format(id_sim + 1)
        return msg, id_sim

    return g, id_sim


    # def readCSV(self):
    #     pass
    #     return
    # def MCS(self):
    #     pass
    # def makePool(self):
    #     pass
#
# class simcenterUQ(UQengine):
#     def __init__(self):
#         pass
#         #
#
#
# class surrogate(simcenterUQ):
#     def __init__(self):
#         pass
#     def readUQ:
#         pass
#     def designExp(self):
#         # (1) generate samples
#
#             Y = self.runFEM()
#         # (2) calibrat
#         # loop
#
#
