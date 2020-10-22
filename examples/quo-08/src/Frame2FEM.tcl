
# units kips, in

set numMeasurements 5

set measuredLambdas {1025.21 1138.11 1099.39 1002.41 1052.69}
set sigmaLambda 54.203

set measuredPhis {1.53 1.24 1.38 1.50 1.35}
set sigmaPhi 0.0705

set PI 3.14159

# 
# some paramaters
#

set m1 0.52
set m2 0.26

set L  400.0
set h  120.0
set E 29000.0 

set Ac 110.0 
pset Ic1 1190.0
pset Ic2 1190.0
set Ib 10e+8
set Ab 10e+8 

#
# build the model
#

model BasicBuilder -ndm 2 -ndf 3	

# nodes
node 1   0.  0. ; 
node 2   $L  0. ;
node 3   0.  $h          -mass [expr $m1/4.] 0 0
node 4   $L  $h          -mass [expr $m1/4.0] 0 0
node 5   0.  [expr 2*$h] -mass [expr $m2/4.0] 0 0
node 6   $L  [expr 2*$h] -mass [expr $m2/4.0] 0 0

# constraints
fix 1 1 1 1;
fix 2 1 1 1;
equalDOF 3 4 1;
equalDOF 5 6 1 

# elements
set TransfTag 1;
geomTransf Linear $TransfTag ;
element elasticBeamColumn 1 1 3 $Ac $E $Ic1 $TransfTag; 
element elasticBeamColumn 2 3 5 $Ac $E $Ic2 $TransfTag; 
element elasticBeamColumn 3 2 4 $Ac $E $Ic1 $TransfTag; 
element elasticBeamColumn 4 4 6 $Ac $E $Ic2 $TransfTag; 
element elasticBeamColumn 5 3 4 $Ab $E $Ib $TransfTag; 
element elasticBeamColumn 6 5 6 $Ab $E $Ib $TransfTag;

# calculate eigenvalues
set numModes 2
set eigenValues [eigen -fullGenLapack $numModes]

set phi [expr [nodeEigenvector 5 1 1] / [nodeEigenvector 3 1 1]]
set lambda [lindex $eigenValues 0]

set error {}
for {set m 0} {$m < $numMeasurements} {incr m 1} {
    lappend error [expr ([lindex $measuredLambdas $m] - $lambda) / $sigmaLambda]
}
for {set m 0} {$m < $numMeasurements} {incr m 1} {
    lappend error [expr ([lindex $measuredPhis $m] - $phi) / $sigmaPhi]
}

# create results file
set outFile [open "results.out" w]
puts $outFile "$error"
close $outFile
