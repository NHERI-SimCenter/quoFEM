
#set motion elCentro

set E 29000.0
set A 1e4
set Ic [expr $k*144*144*144/(24.*$E)]
set Ib 1e12
set g 386.1

puts [expr $w/$g]
puts [expr $wR/$g]

# the model
#model BasicBuilder -ndm 2 -ndf 3
#node 1  0.0    0.0 
#node 2  0.0 144.0 -mass [expr $w/$g]  [expr $w/$g]   0.
#node 3  0.0 288.0 -mass [expr $w/$g]  [expr $w/$g]   0.
#node 4  0.0 432.0 -mass [expr $wR/$g] [expr $wR/$g] 0.
#node 5  288.   0.0 
#node 6  288. 144.0 
#node 7  288. 288.0 
#node 8  288. 432.0 
#fix 1 1 1 1
#fix 5 1 1 1
#equalDOF 2 6 1
#equalDOF 3 7 1
#equalDOF 4 8 1
#geomTransf Linear 1
#element elasticBeamColumn 1 1 2 $A $Ic $E 1
#element elasticBeamColumn 2 2 3 $A $Ic $E 1
#element elasticBeamColumn 3 3 4 $A $Ic $E 1
#element elasticBeamColumn 4 5 6 $A $Ic $E 1
#element elasticBeamColumn 5 6 7 $A $Ic $E 1
#element elasticBeamColumn 6 7 8 $A $Ic $E 1
#element elasticBeamColumn 7 2 6 $A $Ib $E 1
#element elasticBeamColumn 8 3 7 $A $Ib $E 1
#element elasticBeamColumn 9 4 8 $A $Ib $E 1

model BasicBuilder -ndm 3 -ndf 6
node 1 0 0 0 
fix 1 1 1 1 1 1 1 
node 2 0 0 144 -mass [expr $w/$g]  [expr $w/$g] 0.0  0.0  0.0 5.4522e-11 
fix 2 0 0 1 1 1 0 
node 3 0 0 288 -mass [expr $w/$g]  [expr $w/$g]  0.0  0.0  0.0 5.18016e-11 
fix 3 0 0 1 1 1 0 
node 4 0 0 432 -mass [expr $w/$g]  [expr $w/$g] 0.0  0.0  0.0 5.4522e-11 
fix 4 0 0 1 1 1 0 
node 5 0 0 576 -mass [expr $w/$g]  [expr $w/$g]  0.0  0.0  0.0 5.18016e-11 
fix 5 0 0 1 1 1 0 
node 6 0 0 720 -mass [expr $w/$g]  [expr $w/$g]  0.0  0.0  0.0 5.18016e-11 
fix 6 0 0 1 1 1 0 
node 7 0 0 864 -mass [expr $wR/$g]  [expr $wR/$g]  0.0  0.0  0.0 5.18016e-11 
fix 7 0 0 1 1 1 0 
uniaxialMaterial Steel02  1 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Steel02  2 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Elastic 3 1e+10
uniaxialMaterial Steel02  4 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Steel02  5 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Elastic 6 1e+10
uniaxialMaterial Steel02  7 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Steel02  8 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Elastic 9 1e+10
uniaxialMaterial Steel02  10 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Steel02  11 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Elastic 12 1e+10
uniaxialMaterial Steel02  13 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Steel02  14 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Elastic 15 1e+10
uniaxialMaterial Steel02  16 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Steel02  17 $Fy $k $alp 15 0.925 0.15
uniaxialMaterial Elastic 18 1e+10
element zeroLength 1 1 2 -mat 1 2 3 -dir 1 2 6 -doRayleigh 1
element zeroLength 2 2 3 -mat 4 5 6 -dir 1 2 6 -doRayleigh 1
element zeroLength 3 3 4 -mat 7 8 9 -dir 1 2 6 -doRayleigh 1
element zeroLength 4 4 5 -mat 10 11 12 -dir 1 2 6 -doRayleigh 1
element zeroLength 5 5 6 -mat 13 14 15 -dir 1 2 6 -doRayleigh 1
element zeroLength 6 6 7 -mat 16 17 18 -dir 1 2 6 -doRayleigh 1
