# HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH #
# 2D Undrained Cyclic Direct Simple Shear Test			        #
# element: SSP-quad/SSP-quadUP                                          #
# const model: PM4Sand                                                  #
# 									#
# University of Washington, Department of Civil and Environmental Eng   #
# Computational Geomechanics Group - P. Arduino, Long Chen              #        
# Basic Units are m, kN and s unless otherwise specified		#
#                                                                       #
# HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH #

wipe

set maxCycles 	60
set maxStrain 	0.035
set sigvo  		-101.3
set K0 			0.5
set alpha  		0.000

# ------------------------ #
# Test Specific parameters #
# ------------------------ #
set rho 1.42; # Soil Density
# strain increment
set strainIncr 5.0e-6
# set Poisson's ratio to match user specified K0 for applying initial confinement
set nu [expr $K0 / (1+$K0)]
# set nu $K0
# Permeablity
set perm 1.0e-2

# ---------------------------------------------
# Rayleigh damping parameter
set damp   0.02
set omega1 0.2
set omega2 20.0
set a1 [expr 2.0*$damp/($omega1+$omega2)]
set a0 [expr $a1*$omega1*$omega2]

# HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
# HHHHHHHHHHHHHHHHHHHHHHHHHHHCreate ModelHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
# HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH

# Create a 2D model with 3 Degrees of Freedom
model BasicBuilder -ndm 2 -ndf 2

# Create nodes
node 1	0.0	0.0
node 2	1.0	0.0
node 3 	1.0	1.0	
node 4	0.0	1.0

# Create Fixities
fix 1 	1 1
fix 2 	1 1
fix 3	0 0
fix 4 	0 0
 
equalDOF 3 4 1 2

# Create material
#          PM4Sand  tag    Dr   G0   hpo   den  Patm  h0   emax   emin  nb  nd  Ado   zmax    cz    ce     phicv  nu 
nDMaterial PM4Sand   1    $Dr  $G0  $hpo  $rho $Patm $h0  $emax $emin  $nb  $nd $Ado  $zmax  $cz   $ce	  $phicv  $nu $Cgd $Cdr $ckaf $Q $R $m_m $Fsed_min $p_sedo
					 
# Create element
element SSPquad   1     1 2 3 4   1  "PlaneStrain" 1.0 

# Create analysis
constraints Transformation
test        NormDispIncr 1.0e-5 35 1
algorithm   Newton
numberer    RCM
system      FullGeneral
integrator  Newmark [expr 5.0 / 6.0] [expr  4.0 / 9.0]
rayleigh    $a0 $a1 0.0 0.0
analysis    Transient

# Apply consolidation pressure
set pNode [expr $sigvo / 2.0]
pattern Plain 1 {Series -time {0 100 1e10} -values {0 1 1} -factor 1} {
	load 3  0.0  $pNode
	load 4  0.0  $pNode
}
updateMaterialStage -material 1 -stage 0

analyze 100 1
set vDisp [nodeDisp 3 2]
set ts1 "{Series -time {100 80000 1.0e10} -values {1.0 1.0 1.0} -factor 1}"
eval "pattern Plain 2 $ts1 {
	sp 3 2 $vDisp
	sp 4 2 $vDisp
}"

analyze 25 1
puts "Removed drainage fixities."

updateMaterialStage -material 1 -stage 1
setParameter -value 0 -ele 1 FirstCall 1
analyze 25 1

puts "finished update fixties"

# Create recorders
recorder Node  -nodeRange 1 4  -time -file CycdispD.out  -dof 1 2 disp
recorder Element -ele 1 -time -file CycstressD.out stress
recorder Element -ele 1 -time -file CycstrainD.out strain

# update Poisson's ratio for analysis
setParameter -value 0.3 -ele 1 poissonRatio 1
set controlDisp [expr 1.1 * $maxStrain]
puts "controlDisp = $controlDisp  strainIncr = $strainIncr"
set nCyclesIO [open nCyclesResultD.dat a];
set numCycle 0.25
set break_flag 0
while {$numCycle <= $maxCycles} {
	#impose 1/4 cycle
	set hDisp [nodeDisp 3 1]
	set currentTime [getTime]
	set steps [expr $controlDisp / $strainIncr]
	eval "timeSeries Path 2 -time {$currentTime [expr $currentTime + $steps] 1.0e10} -values {$hDisp $controlDisp $controlDisp} -factor 1"
	pattern Plain 3 2 {
		sp 3 1 1.0
	}
	set b [eleResponse 1 stress]
	while {[lindex $b 2] < [expr $CSR * -1.0 * $sigvo]} {
		analyze 1 1.0
		set b [eleResponse 1 stress]
		set hDisp [nodeDisp 3 1]
		if {$hDisp >= $maxStrain} {
			# puts $nCyclesIO " $CSR $numCycle"	
			set break_flag 1	
			break
		}
	}
	if {$break_flag == 1} break

	set numCycle [expr $numCycle + 0.25]
	set hDisp [nodeDisp 3 1]
	set currentTime [getTime]
	
	remove loadPattern 3
	remove timeSeries 2
	remove sp 3 1
	
	#impose 1/2 cycle 
	set steps [expr ($controlDisp + $hDisp)/ $strainIncr]
	eval "timeSeries Path 2 -time {$currentTime [expr $currentTime + $steps] 1.0e10} -values {$hDisp [expr -1.0 * $controlDisp] [expr -1.0 * $controlDisp]} -factor 1"
	
	pattern Plain 3 2 {
		sp 3 1 1.0
	}
	
	while {[lindex $b 2] > [expr $CSR * $sigvo]} {
		analyze 1 1.0
		set b [eleResponse 1 stress]
		set hDisp [nodeDisp 3 1]
		if {$hDisp <= [expr -1.0 * $maxStrain]} {
			# puts $nCyclesIO " $CSR $numCycle"			
			#exit
			set break_flag 1	
			break
		}
	}
	if {$break_flag == 1} break
	#impose 1/4 cycle
	set numCycle [expr $numCycle + 0.5]
	set hDisp [nodeDisp 3 1]
	set currentTime [getTime]
	
	remove loadPattern 3
	remove timeSeries 2
	remove sp 3 1
	set steps [expr ($controlDisp + $hDisp)/ $strainIncr]
	eval "timeSeries Path 2 -time {$currentTime [expr $currentTime + $steps] 1.0e10} -values {$hDisp $controlDisp $controlDisp} -factor 1"
	
	pattern Plain 3 2 {
		sp 3 1 1.0
	}
	while {[lindex $b 2] <= 0.0} {
		analyze 1 1.0
		set b [eleResponse 1 stress]
		set hDisp [nodeDisp 3 1]
		if {$hDisp >= $maxStrain} {
			#puts $nCyclesIO " $CSR $numCycle"
			#lappend CSRlist $CSR
			#lappend Cyclelist $numCycle
			#exit
			set break_flag 1	
			break
		}
	}
	if {$break_flag == 1} break

	set numCycle [expr $numCycle + 0.25]
	remove loadPattern 3
	remove timeSeries 2
	puts " numCycle = $numCycle  maxCycles = $maxCycles"	
}

puts $nCyclesIO " $CSR $numCycle"
close $nCyclesIO
# lappend CSRlist $CSR
lappend Cyclelist $numCycle	
puts "EOF tcl"

wipe
