# LA Post-Northridge 3 story North-South no cover plates, no column splice
# from FEMA-355C/Sept. 2000  (https://www.nehrp.gov/pdf/fema355c.pdf)
#
# units kips,inches
# load procedures in other files

source Steel2d.tcl

# set some variables
set in 1.0;
set g 386.4;				# acceleration due to gravity

pset pDamp 0.05
pset motion el_centro

pset Es1 29000.0;  # modulus of elasticity for steel
pset Es2 29000.0;  # modulus of elasticity for steel
pset Es3 29000.0;  # modulus of elasticity for steel
pset Esb 29000.0

pset Fy1 50.0; 	 # yield stress of steel
pset Fy2 50.0; 	 # yield stress of steel
pset Fy3 50.0; 	 # yield stress of steel
pset Fyb 50.0

pset b1 0.01;	 # strain hardening ratio
pset b2 0.01;	 # strain hardening ratio
pset b3 0.01;	 # strain hardening ratio
pset bb 0.01

# set up my lists
set floorOffsets {156. 156. 156.}
set colOffsets   {360. 360. 360.} 

set roofMass  [expr 70.9/2.]; # 2 frames per dirn
set floorMass [expr 65.53/2.]
set roofBeamLoad  0.12;  #96*30*15  lbs per external beam /360in /1000 kips  
set floorBeamLoad 0.10375; # 83*30*15 lbs per external beam /360 in /1000

set numFloor [expr [llength $floorOffsets]+1]
set numCline [expr [llength $colOffsets]+1]

set  massesCMD     "set masses {0. [expr $floorMass/$numCline] [expr $floorMass/$numCline] [expr $roofMass/$numCline]}"
eval $massesCMD

set colSizes       {W14X311 W14X311 W14X311};
set colESizes      {W14X257 W14X257 W14X257};
set beamSizes      {W30X116 W30X116 W24X62};

# build colLocations and floorLocations
set floorLocations 0; set floorLoc 0; 
set colLocations 0; set colLoc 0; 

for {set i 1} {$i < $numFloor} {incr i 1} {
    set floorLoc [expr $floorLoc + [lindex $floorOffsets [expr $i-1]]]
    lappend floorLocations $floorLoc;
}
for {set i 1} {$i < $numCline} {incr i 1} {
    set colLoc [expr $colLoc + [lindex $colOffsets [expr $i-1]]]
    lappend colLocations $colLoc;
}

# check of list dimensions for errors
if {[llength $masses] != $numFloor} {puts "ERROR: massX"; quit}
if {[llength $colSizes] != [expr $numFloor-1]} {puts "ERROR: colSizes"; quit}
if {[llength $beamSizes] != [expr $numFloor-1]} {puts "ERROR: beamSizes"; quit}
if {$numFloor >= 10} {puts "ERROR: too many floors"; quit}
if {$numCline >= 10} {puts "ERROR: too many colun lines"; quit}


model BasicBuilder -ndm 2 -ndf 3;  # Define the model builder, ndm = #dimension, ndf = #dofs

# create materials for each column story and the beams
uniaxialMaterial Steel02 1 $Fy1 $Es1 $b1 20 0.925 0.15
uniaxialMaterial Steel02 2 $Fy2 $Es2 $b2 20 0.925 0.15
uniaxialMaterial Steel02 3 $Fy3 $Es3 $b3 20 0.925 0.15
uniaxialMaterial Steel02 4 $Fyb $Esb $bb 20 0.925 0.15

# Build the Nodes
for {set floor 1} {$floor <= $numFloor} {incr floor 1} {
    set floorLoc [lindex $floorLocations [expr $floor-1]]
    
    set mass [lindex $masses [expr $floor-1]]
    
    for {set colLine 1} {$colLine <= $numCline} {incr colLine 1} {
	set colLoc [lindex $colLocations [expr $colLine-1]]
	node $colLine$floor $colLoc $floorLoc -mass $mass $mass 0.
	if {$floor == 1} {
	    fix $colLine$floor 1 1 1
	} else {
	    fix $colLine$floor 0 0 0
	}
    }
}

# build the columns
geomTransf PDelta 1
for {set colLine 1} {$colLine <= $numCline} {incr colLine 1} {
    for {set floor1 1} {$floor1 < $numFloor} {incr floor1 1} {
	set floor2 [expr $floor1+1]
	if {$colLine == 1 || $colLine == $numCline} {
	    set theSection [lindex $colESizes [expr $floor1 -1]]
	} else {
	    set theSection [lindex $colSizes [expr $floor1 -1]]
	}
	ForceBeamWSection2d $colLine$floor1$colLine$floor2 $colLine$floor1 $colLine$floor2 $theSection $floor1 1 -nip 5
	#ElasticBeamWSection2d $colLine$floor1$colLine$floor2 $colLine$floor1 $colLine$floor2 $theSection 1 1 -nip 5
	#ElasticBeamWSection2d $colLine$floor1$colLine$floor2 $colLine$floor1 $colLine$floor2 $theSection $Es 1 
    }
}

# build the beams
geomTransf Linear 2
for {set colLine1  1} {$colLine1 < $numCline} {incr colLine1 1} {
    set colLine2 [expr $colLine1 + 1]
    for {set floor 2} {$floor <= $numFloor} {incr floor 1} {
	set theSection [lindex $beamSizes [expr $floor -2]]
	ForceBeamWSection2d $colLine1$floor$colLine2$floor $colLine1$floor $colLine2$floor $theSection 4 2
	#ElasticBeamWSection2d $colLine1$floor$colLine2$floor $colLine1$floor $colLine2$floor $theSection $Es 2
	#element elasticBeamColumn $colLine1$floor$colLine2$floor $colLine1$floor $colLine2$floor 100 $Es 1e14 1
    }
}


set eigenValues [eigen 2]
puts "Moment Frame Eigenvalues: $eigenValues"

#
# use eigevalues to impose load on structure
#

set P1 [nodeEigenvector 12 1 1]
set P2 [nodeEigenvector 13 1 1]
set P3 [nodeEigenvector 14 1 1]

timeSeries Constant 1
pattern Plain 2 1 {
    load 12 $P1 0 0
    load 13 $P2 0 0
    load 14 $P3 0 0
}

analysis Static
analyze 1

set d1 [nodeDisp 12 1]
set d2 [nodeDisp 13 1]
set d3 [nodeDisp 14 1]

#
# Shear Building model from applied load and displacements
#

wipe

model Basic -ndm 1 -ndf 1
set k1 [expr ($P1+$P2+$P3)/$d1]
set k2 [expr ($P2+$P3)/($d2-$d1)]
set k3 [expr $P3/($d3-$d2)]

node 11 0
node 12 0 -mass $floorMass
node 13 0 -mass $floorMass
node 14 0 -mass $roofMass

uniaxialMaterial Elastic 1 $k1
uniaxialMaterial Elastic 2 $k2
uniaxialMaterial Elastic 3 $k3

element zeroLength 1 11 12 -mat 1 -dir 1
element zeroLength 2 12 13 -mat 2 -dir 1
element zeroLength 3 13 14 -mat 3 -dir 1
fix 11 1

set Tol 1.0e-6;    
constraints Plain;	
numberer RCM;		
system BandGeneral;	
test NormDispIncr $Tol 10;
algorithm Newton;		
integrator Newmark 0.5 0.25
analysis Transient

set eigenValues [eigen 2]
puts "Shear Building Eigenvalues: $eigenValues"


source ReadRecord.tcl
ReadRecord $motion.AT2 $motion.g3 dt nPt;
timeSeries Path 2 -filePath $motion.g3 -dt $dt -factor $g
pattern UniformExcitation 2 1 -accel 2

set cmd "recorder EnvelopeNode -file floorAccEnv.out  -timeSeries 2 -node 12 13 14 -dof 1 accel"; eval $cmd;
set cmd "recorder EnvelopeNode -file floorDispEnv.out  -node 12 13 14 -dof 1 disp"; eval $cmd;

set tFinal [expr $nPt * $dt]

constraints Plain
numberer RCM
system BandGeneral
test NormUnbalance 1.0e-1 10 
algorithm Newton
integrator Newmark 0.5 0.25
analysis Transient 5 10

#
# add rayleigh damping
#

set lambda [eigen 2]
set omegaI [expr pow([lindex $lambda 0],0.5)];
set omegaJ [expr pow([lindex $lambda 1],0.5)];
set alphaM [expr $pDamp*(2*$omegaI*$omegaJ)/($omegaI+$omegaJ)];	
set betaKcomm [expr 2.*$pDamp/($omegaI+$omegaJ)]; 
rayleigh $alphaM 0. 0. $betaKcomm


set ok 0
set currentTime 0.0
while {$ok == 0 && $currentTime < $tFinal} {
    set ok [analyze 1 $dt]
    if {$ok != 0} {
	test NormDispIncr 1.0e-6 2000 1
	algorithm ModifiedNewton -initial
	set ok [analyze 1 $dt]
	test NormDispIncr 1.0e-6 10 
	algorithm Newton
    } 
    set currentTime [getTime]
}

wipe

set a [open floorDispEnv.out r]
set lineDisp [gets $a]; set lineDisp [gets $a]; set lineDisp [gets $a]
close $a

set a [open floorAccEnv.out r]
set lineAccel [gets $a]; set lineAccel [gets $a]; set lineAccel [gets $a]
close $a

set out [open results.out w]
puts $out "$lineDisp $lineAccel"
close $out
