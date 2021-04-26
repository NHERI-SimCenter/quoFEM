import time
import pickle as pickle
import numpy as np
import os
import sys
import json as json
import shutil
import subprocess

def main(params_dir,surrogate_dir,json_dir,result_file):
    global error_file

    os_type='win'
    run_type ='runninglocal'

    msg0 = os.path.basename(os.getcwd()) + " : "
    file_object = open('../surrogateLog.err', 'a')

    #
    # read json -- current input file
    #
    f = open('dakota.json') # current input file
    try:
        inp_tmp = json.load(f)
    except ValueError:
        msg = 'invalid json format - dakota.json'
        error_file.write(msg)
        error_file.close()
        file_object.write(msg0+msg)
        file_object.close()
        exit(-1)
    f.close()
    #norm_var_thr = 0.0001
    norm_var_thr = inp_tmp["fem"]["varThres"]
    when_inaccurate = inp_tmp["fem"]["femOption"]

    #when_inaccurate='do_original'
    #when_inaccurate='give_error'
    #when_inaccurate='do_nothing'


    # if no g and rv,


    #
    # read json -- original input for training surrogate
    #
    f = open(json_dir)
    try:
        sur = json.load(f)
    except ValueError:
        msg = 'invalid json format'
        error_file.write(msg)
        error_file.close()
        exit(-1)
    f.close()
    did_logtransform = sur["doLogtransform"]
    kernel = sur["kernName"]
    if kernel == 'Radial Basis':
        kern_name = 'rbf'
    elif kernel == 'Exponential':
        kern_name = 'Exponential'
    elif kernel == 'Matern 3/2':
        kern_name = 'Mat32'
    elif kernel == 'Matern 5/2':
        kern_name = 'Mat52'

    # from json
    g_name_sur = list()
    ng_sur = 0
    for g in sur['ylabels']:
        g_name_sur += [g]
        ng_sur += 1

    rv_name_sur = list()
    nrv_sur = 0
    for rv in sur['xlabels']:
        rv_name_sur += [rv]
        nrv_sur += 1

    # to read:::
    # kern_name='Mat52'
    #did_logtransform=True

    # at ui



    # REQUIRED: rv_name, y_var

    t_total = time.process_time()

    with open(params_dir, "r") as x_file:
        data = x_file.readlines()
        nrv = int(data[0])
        if nrv != nrv_sur:
            msg = 'Error importing input data: Number of dimension inconsistent: surrogate model requires {} RV(s) but input has {} RV(s).\n'.format(nrv_sur, nrv)
            print(msg)
            error_file.write(msg)
            error_file.close()
            file_object.write(msg0 + msg)
            file_object.close()
            exit(-1)

        #rv_name = list()
        rv_val = np.zeros((1, nrv))
        for i in range(nrv):
            name, val = data[i + 1].split()
            try:
                id_map = rv_name_sur.index(name)
            except ValueError:
                msg = 'Error importing input data: variable "{}" not identified.'.format(name)
                print(msg)
                error_file.write(msg)
                error_file.close()
                file_object.write(msg0 + msg)
                file_object.close()
                exit(-1)
            rv_val[0, id_map] = float(val)


        g_idx = []
        for edp in (inp_tmp["EDP"]):
            try:
                id_map = g_name_sur.index(edp["name"])
            except ValueError:
                msg = 'Error importing input data: qui "{}" not identified.'.format(name)
                print(msg)
                error_file.write(msg)
                error_file.close()
                file_object.write(msg0 + msg)
                file_object.close()
                exit(-1)
            g_idx += [id_map]

    # f = open(work_dir + '/templatedir/dakota.json')
    # inp = json.load(f)
    # f.close()


    try:
        f = open(surrogate_dir, 'rb')
    except OSError:
        msg = 'Could not open/read surrogate model from: ' + surrogate_dir + '\n'
        print(msg)
        error_file.write(msg)
        error_file.close()
        file_object.write(msg0+msg)
        file_object.close()
        exit(-1)
    with f:
        m_list = pickle.load(f)


    # with open(work_dir + '/' + filename + '.pkl', 'rb') as file:
    #    m = pickle.load(file)

    # read param in file and sort input

    y_dim = len(m_list)
    nsamp = np.sum(m_list[0].X[:, -1] == 0)

    y_pred=np.zeros(y_dim)
    y_pred_var=np.zeros(y_dim)
    y_data_var=np.zeros(y_dim)
    y_pred_prior_var = np.zeros(y_dim)

    for ny in range(y_dim):
        y_pred_tmp, y_pred_var_tmp  = m_list[ny].predict(rv_val)
        y_pred[ny]=y_pred_tmp
        if did_logtransform:
            # y_var_val = np.var(np.log(m_list[ny].Y))
            log_mean = np.mean((m_list[ny].Y))
            log_var = np.var((m_list[ny].Y))
            y_data_var[ny] = np.exp(2 * log_mean + log_var) * (np.exp(log_var) - 1)  # in linear space
            y_pred_var[ny] = np.exp(2 * y_pred_tmp + y_pred_var_tmp) * (np.exp(y_pred_var_tmp) - 1)
        else:
            y_data_var[ny] = np.var(m_list[ny].Y)
            y_pred_var[ny] = y_pred_var_tmp
        for parname in m_list[ny].parameter_names():
            if (kern_name in parname) and parname.endswith('variance'):
                exec('y_pred_prior_var[ny]=m_list[ny].' + parname)

    error_ratio1 = y_pred_var.T / y_pred_prior_var
    error_ratio2 = y_pred_var.T / y_data_var
    idx = np.argmax(error_ratio2) + 1

    '''
    if np.max(error_ratio1) > norm_var_thr:

        is_accurate = False
        idx = np.argmax(error_ratio1) + 1

        msg = 'Prediction error of output {} is {:.2f}%, which is greater than threshold={:.2f}%  '.format(idx, np.max(
            error_ratio1)*100, norm_var_thr*100)
    '''

    if np.max(error_ratio2) > norm_var_thr:
        is_accurate = False
        msg1 = 'Prediction error of output {} is {:.2f}%, which is greater than threshold={:.2f}%  '.format(idx, np.max(
            error_ratio2)*100, norm_var_thr*100)
    else:
        is_accurate = True

    if not is_accurate:

        if when_inaccurate == 'doSimulation':
            #
            # run original simulations HERE
            #

            # change directory, copy params.in
            sim_dir = os.path.join(os.getcwd(), 'templatedir_SIM')
            shutil.copyfile(os.path.join(os.getcwd(), 'params.in'), os.path.join(sim_dir, 'params.in'))
            os.chdir(sim_dir)

            # run workflowDriver
            if os_type.lower().startswith('win') and run_type.lower() == 'runninglocal':
                workflowDriver = "workflow_driver.bat"
            else:
                workflowDriver = "workflow_driver"

            workflow_run_command = '{}/{}'.format(sim_dir, workflowDriver)
            subprocess.Popen(workflow_run_command, shell=True).wait()

            # back to directory, copy result.out
            #shutil.copyfile(os.path.join(sim_dir, 'results.out'), os.path.join(os.getcwd(), 'results.out'))

            with open('results.out', 'r') as f:
                table = np.loadtxt(f)

            os.chdir("../")

            with open('results.out', 'w') as f:
                if table.size==1:
                    f.write(str(table))
                else:
                    result_values = table[g_idx].tolist()
                    f.write(' '.join(map(str, result_values)))

            msg2 = msg0+msg1+'- RUN original model\n'
            print(msg2)
            error_file.write(msg2)
            file_object.write(msg2)
            error_file.close()
            file_object.close()
            exit(-1)
        elif when_inaccurate == 'giveError':
            msg2 = msg0+msg1+'- STOP\n'
            print(msg2)
            error_file.write(msg2)
            file_object.write(msg2)
            error_file.close()
            exit(-1)
        elif when_inaccurate == 'continue':
            msg2 = msg0+msg1+'- CONTINUE [Warning: results may not be accurate]\n'
            print(msg2)
            error_file.write(msg2)
            file_object.write(msg2)
            error_file.close()
    else:
        msg3 = 'Prediction error of output {} is {:.2f}%\n'.format(idx, np.max(error_ratio2)*100)
        file_object.write(msg0+msg3)
        file_object.close()

    if did_logtransform:
        y_pred=np.exp(y_pred)

    np.savetxt(result_file, np.array([y_pred[g_idx]]), fmt='%.5e')

if __name__ == "__main__":
    error_file = open('surrogate.err', "w")
    inputArgs = sys.argv    

    if not inputArgs[2].endswith('.json'):
        msg = 'ERROR: surrogte information file (.json) not set'
        print(msg)
        error_file.write(msg)
        exit(-1)

    elif not inputArgs[3].endswith('.pkl'):
        msg = 'ERROR: surrogte model file (.pkl) not set'
        print(msg)
        error_file.write(msg)
        exit(-1)

    elif len(inputArgs) < 4 or len(inputArgs) > 4:
        msg = 'ERROR: put right number of argv'
        print(msg)
        error_file.write(msg)
        exit(-1)

    '''
    params_dir = 'params.in'
    surrogate_dir = 'C:/Users/yisan/Desktop/quoFEMexamples/surrogates/SimGpModel_2_better.pkl'
    result_file = 'results_GP.out'
    '''
    '''
    try:
        opts, args = getopt.getopt(argv)
    except getopt.GetoptError:
        print
        'surrogate_pred.py -i <dir_params.in> -o <dir_SimGpModel.pkl>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print
            'surrogate_pred.py -i <dir_params.in> -o <dir_SimGpModel.pkl>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfile = arg
        elif opt in ("-o", "--ofile"):
            outputfile = arg
   '''



    params_dir = inputArgs[1]
    surrogate_dir = inputArgs[3]
    surrogate_meta_dir = inputArgs[2]
    result_file="results.out"

    sys.exit(main(params_dir,surrogate_dir,surrogate_meta_dir,result_file))

