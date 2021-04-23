#units kips/in .. model with beam and column elements, stiff beams

# some parameters
pset w 100.0
pset wR 50.0
pset k 326.32
pset alp 0.1
pset Fy 20

#
# GM1
# 
source make_model.tcl

set nPts 5999
set dT 0.01
set factor 0.211672147
set GMfile R331
set nodefile node1
set Fofile Fcol1
set Drfile Drift-Story1

source do_analysis.tcl

#
# GM2
# 

wipe all;

source make_model.tcl

set nPts 6499
set dT 0.01
set factor 0.120469761
set GMfile R338
set nodefile node2
set Fofile Fcol2
set Drfile Drift-Story2

source do_analysis.tcl

#
# GM3
# 

wipe all;

source make_model.tcl

set nPts 1179
set dT 0.01
set factor 0.858739356
set GMfile R430
set nodefile node3
set Fofile Fcol3
set Drfile Drift-Story3

source do_analysis.tcl

#
# GM4
# 

wipe all;

source make_model.tcl

set nPts 1819
set dT 0.01
set factor 0.889871968
set GMfile R432
set nodefile node4
set Fofile Fcol4
set Drfile Drift-Story4


source do_analysis.tcl

#
# GM5
# 

wipe all;

source make_model.tcl

set nPts 7998
set dT 0.005
set factor 0.36099198 
set GMfile R793
set nodefile node5
set Fofile Fcol5
set Drfile Drift-Story5

source do_analysis.tcl

#
# GM6
# 

wipe all;

source make_model.tcl

set nPts 8800
set dT 0.005
set factor 2.735349699
set GMfile R1846
set nodefile node6
set Fofile Fcol6
set Drfile Drift-Story6

source do_analysis.tcl

#
# GM7
# 

wipe all;

source make_model.tcl

set nPts 9600
set dT 0.005
set factor 1.69876478
set GMfile R1867
set nodefile node7
set Fofile Fcol7
set Drfile Drift-Story7

source do_analysis.tcl

#
# GM8
# 

wipe all;

source make_model.tcl
set Fofile Fcol8
set Drfile Drift-Story8

set nPts 9400
set dT 0.005
set factor 1.361057572
set GMfile R2280
set nodefile node8

source do_analysis.tcl

#
# GM8
# 


wipe all;

source make_model.tcl

set nPts 12999
set dT 0.005
set factor 0.504275014
set GMfile R2364
set nodefile node9
set Fofile Fcol9
set Drfile Drift-Story9

source do_analysis.tcl

#
# GM9
# 

wipe all;

source make_model.tcl

set nPts 7750
set dT 0.004
set factor 1.055491494
set GMfile R3325
set nodefile node10
set Fofile Fcol10
set Drfile Drift-Story10

source do_analysis.tcl

#
# Postprocess
# 
source postprocess.tcl