
# units kN & mm

#
# set some parameters
#

pset E 205
pset P 25
pset Ao 250
pset Au 500

#
# build the model
#

model Basic -ndm 2 -ndf 2

node 1     0    0
node 2  4000    0
node 3  8000    0
node 4 12000    0
node 5  4000 4000
node 6  8000 4000

fix 1 1 1
fix 4 0 1

uniaxialMaterial Elastic 1 $E

element truss 1 1 2 $Ao 1
element truss 2 2 3 $Ao 1
element truss 3 3 4 $Ao 1
element truss 4 1 5 $Au 1
element truss 5 5 6 $Au 1
element truss 6 6 4 $Au 1
element truss 7 2 5 $Ao 1
element truss 8 3 6 $Ao 1
element truss 9 5 3 $Ao 1
timeSeries Linear 1
pattern Plain 1 1 {
    load 2 0 [expr -$P]
    load 3 0 [expr -$P]
}

#
# create a recorder
#

recorder Node -file node.out -scientific -precision 10 -node 1 2 3 4 5 6 -dof 1 2 disp

#
# build and perform the analysis
#

algorithm Linear
integrator LoadControl 1.0
system ProfileSPD
numberer RCM
constraints Plain
analysis Static
analyze 1

#
# remove the recorders
#

remove recorders
