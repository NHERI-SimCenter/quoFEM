

# making it to nd-digit pricision
set nd 5
set w   [expr { double(round($w    * pow(10,$nd -ceil(log10($w))))    / pow(10,$nd -ceil(log10($w)))) }]
set wR  [expr { double(round($wR   * pow(10,$nd -ceil(log10($wR))))   / pow(10,$nd -ceil(log10($wR)))) }]
set k   [expr { double(round($k1   * pow(10,$nd -ceil(log10($k1))))   / pow(10,$nd -ceil(log10($k1)))) }]
set Fy  [expr { double(round($Fy1  * pow(10,$nd -ceil(log10($Fy1))))  / pow(10,$nd -ceil(log10($Fy1)))) }]
set alp [expr { double(round($alp1 * pow(10,$nd -ceil(log10($alp1)))) / pow(10,$nd -ceil(log10($alp1)))) }]

# building the model
model BasicBuilder -ndm 2 -ndf 2

node 1 0 0 
node 2 0 0 144 -mass [expr $w/$g]  [expr $w/$g] # The node coordinate (0,0,144) is not really used in the analysis becase we only output the horizontal displacement (not drift) and do not consider p-delta.
node 3 0 0 288 -mass [expr $w/$g]  [expr $w/$g] 
node 4 0 0 432 -mass [expr $w/$g]  [expr $w/$g] 
node 5 0 0 576 -mass [expr $w/$g]  [expr $w/$g] 
node 6 0 0 720 -mass [expr $w/$g]  [expr $w/$g] 
node 7 0 0 864 -mass [expr $wR/$g] [expr $wR/$g]

fix 1 1 1 

uniaxialMaterial Steel01  1 $Fy   $k $alp 
uniaxialMaterial Elastic  2 1e+10
uniaxialMaterial Steel01  3 $Fy   $k $alp 
uniaxialMaterial Elastic  4 1e+10
uniaxialMaterial Steel01  5 $Fy   $k $alp 
uniaxialMaterial Elastic  6 1e+10
uniaxialMaterial Steel01  7 $Fy   $k $alp 
uniaxialMaterial Elastic  8 1e+10
uniaxialMaterial Steel01  9 $Fy   $k $alp 
uniaxialMaterial Elastic 10 1e+10
uniaxialMaterial Steel01 11 $Fy   $k $alp
uniaxialMaterial Elastic 12 1e+10

element zeroLength 1 1 2 -mat  1  2 -dir 1 2 -doRayleigh 1
element zeroLength 2 2 3 -mat  3  4 -dir 1 2 -doRayleigh 1
element zeroLength 3 3 4 -mat  5  6 -dir 1 2 -doRayleigh 1
element zeroLength 4 4 5 -mat  7  8 -dir 1 2 -doRayleigh 1
element zeroLength 5 5 6 -mat  9 10 -dir 1 2 -doRayleigh 1
element zeroLength 6 6 7 -mat 11 12 -dir 1 2 -doRayleigh 1
