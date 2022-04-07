

timeSeries Path 1 -filePath $GMfile.dat -dt $dT -factor [expr $factor*$g]
pattern UniformExcitation  1 1 -accel 1

recorder EnvelopeNode -file $nodefile.out -node 1 2 3 4 5 6 7 -dof 1 2 disp
recorder Element -file $Fofile.out -time -ele 1 force;
recorder Drift -file $Drfile.out -time -iNode 1 -jNode 2 -dof 1 -perpDirn 3;


set xDamp 0.05;
set lambdaN [eigen 3];
set lambdaI [lindex $lambdaN [expr 1 -1]];
set lambdaJ [lindex $lambdaN [expr 2 -1]];
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
