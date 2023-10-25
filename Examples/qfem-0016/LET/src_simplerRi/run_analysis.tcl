
set nPts 1992
set dT 0.01
timeSeries Path 1 -filePath Rinaldi.AT2 -dt $dT -factor [expr $factor*$g/0.8740]

pattern UniformExcitation  1 1 -accel 1

recorder EnvelopeNode -file nodeDisp.out -node 1 2 3 4 5 6 7 -dof 1 2 disp
recorder EnvelopeNode -file nodeAcc.out -node 1 2 3 4 5 6 7 -dof 1 2 accel
recorder Element -file Fcol1.out -time -ele 1 force;
recorder Node -file Drift-Story1.out -time -node 1 2 -dof 1;
recorder Element -file Fcol2.out -time -ele 2 force;
recorder Node -file Drift-Story2.out -time -node 2 3 -dof 1;
recorder Element -file Fcol3.out -time -ele 3 force;
recorder Node -file Drift-Story3.out -time -node 3 4 -dof 1;
recorder Element -file Fcol4.out -time -ele 4 force;
recorder Node -file Drift-Story4.out -time -node 4 5 -dof 1;
recorder Element -file Fcol5.out -time -ele 5 force;
recorder Node -file Drift-Story5.out -time -node 5 6 -dof 1;
recorder Element -file Fcol6.out -time -ele 6 force;
recorder Node -file Drift-Story6.out -time -node 6 7 -dof 1;


set xDamp 0.05;
set lambdaN [eigen 3];
set lambdaI [lindex $lambdaN [expr 1 -1]];
set lambdaJ [lindex $lambdaN [expr 3 -1]];
set omegaI [expr pow($lambdaI,0.5)];
set omegaJ [expr pow($lambdaJ,0.5)];
set alphaM [expr $xDamp*(2*$omegaI*$omegaJ)/($omegaI+$omegaJ)];
set betaKinit [expr 1 *2.0*$xDamp/($omegaI+$omegaJ)];
set betaKcomm [expr 0 *2.0*$xDamp/($omegaI+$omegaJ)];
set betaKcurr [expr 0 *2.0*$xDamp/($omegaI+$omegaJ)];
rayleigh $alphaM $betaKcurr $betaKinit $betaKcomm;

numberer RCM
constraints Transformation
system Umfpack
integrator Newmark 0.5 0.25
test NormUnbalance 1.0e-2 10 
algorithm Newton
analysis Transient -numSubLevels 2 -numSubSteps 10

set tFinal [expr $nPts * $dT]
set tCurrent [getTime]
set ok 0

while {$ok == 0 && $tCurrent < $tFinal} {
    
    set ok [analyze 1 $dT]
    
    # if the analysis fails try initial tangent iteration
    if {$ok != 0} {
	puts "regular newton failed .. lets try an initail stiffness for this step"
	test NormDispIncr 1.0e-8  1000 1
	algorithm ModifiedNewton -initial
	set ok [analyze 1 $dT]
	if {$ok == 0} {puts "that worked .. back to regular newton"}
	test NormDispIncr 1.0e-12  10 
	algorithm Newton
    }
    
    set tCurrent [getTime]
}

# REMOVE RECORDERS .. VERY IMPORTANT
remove recorders
