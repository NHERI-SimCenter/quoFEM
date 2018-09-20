# padhye written

import numpy as np
file_1 = np.loadtxt("outputs.out")
file_2 = np.loadtxt("Reference_solution.out")

opt = file_1.astype(np.float32)
ref = file_2.astype(np.float32)


if(opt.shape[0]==ref.shape[0]):

    error = 0.0
    for i in range(opt.shape[0]):
        ref_row = ref[ref[:,0]==i][:,1:]
        opt_row = opt[opt[:,0]==i][:,1:]

        error = error + np.sum((ref_row - opt_row)**2.0)

else:
    error = 1.0e10

# Write to file so that Dakota can read the error
text_file = open("results.out", "w")
text_file.write("%f" % error)
text_file.close()
#print "Error is: ", error