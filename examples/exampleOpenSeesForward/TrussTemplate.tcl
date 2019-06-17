pset E 20500
pset P 25000
pset A1 250
pset A2 500

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
element truss 1 1 2 $A1 1
element truss 2 2 3 $A1 1
element truss 3 3 4 $A1 1
element truss 4 1 5 $A2 1
element truss 5 5 6 $A2 1
element truss 6 6 4 $A2 1
element truss 7 2 5 $A1 1
element truss 8 3 6 $A1 1
element truss 9 5 3 $A1 1
timeSeries Constant 1
pattern Plain 1 1 {
    load 2 0 -$P
    load 3 0 -$P
}

recorder Node -file node.out -node 1 2 3 4 5 6 -dof 2 disp

algorithm Linear
integrator LoadControl 1.0
system ProfileSPD
numberer RCM
constraints Plain
analysis Static
analyze 1
