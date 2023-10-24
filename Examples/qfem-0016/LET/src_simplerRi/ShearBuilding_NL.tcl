wipe all;

# some parameters
pset w 100.0
pset wR 50
pset k 326.32
pset Fy 50
pset alp 0.2
pset factor 0.1

set g 386.1

model BasicBuilder -ndm 2 -ndf 3
node 1 0 0
fix 1 1 1 1 
node 2 0 144 -mass [expr $w/$g]  [expr $w/$g] 5.4522e-11 
fix 2 0 0 0 
node 3 0 288 -mass [expr $w/$g]  [expr $w/$g]  5.4522e-11 
fix 3 0 0 0 
node 4 0 432 -mass [expr $w/$g]  [expr $w/$g] 5.4522e-11 
fix 4 0 0 0 
node 5 0 576 -mass [expr $w/$g]  [expr $w/$g]  5.4522e-11 
fix 5 0 0 0 
node 6 0 720 -mass [expr $w/$g]  [expr $w/$g]  5.4522e-11 
fix 6 0 0 0 
node 7 0 864 -mass [expr $wR/$g]  [expr $wR/$g]  5.4522e-11 
fix 7 0 0 0 
uniaxialMaterial Steel02  1 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Elastic 2 1e+10
uniaxialMaterial Steel02  3 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Elastic 4 1e+10
uniaxialMaterial Steel02  5 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Elastic 6 1e+10
uniaxialMaterial Steel02  7 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Elastic 8 1e+10
uniaxialMaterial Steel02  9 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Elastic 10 1e+10
uniaxialMaterial Steel02  11 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Elastic 12 1e+10
element zeroLength 1 1 2 -mat 1 2 -dir 1 2 -doRayleigh 1
element zeroLength 2 2 3 -mat 3 4 -dir 1 2 -doRayleigh 1
element zeroLength 3 3 4 -mat 5 6 -dir 1 2 -doRayleigh 1
element zeroLength 4 4 5 -mat 7 8 -dir 1 2 -doRayleigh 1
element zeroLength 5 5 6 -mat 9 10 -dir 1 2 -doRayleigh 1
element zeroLength 6 6 7 -mat 11 12 -dir 1 2 -doRayleigh 1

source run_analysis.tcl
