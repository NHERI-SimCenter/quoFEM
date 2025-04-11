# #########################################################
#                                                         #
# Effective stress site response analysis for a layered   #
# soil profile located on a level ground and underlain by #
# an elastic half-space.  SSPquadUP elements are used.    #
# The finite rigidity of the elastic half space is        #
# considered through the use of a viscous damper at the   #
# base.                                                   #
#                                                         #
#   Created by:  Chris McGann                             #
#                HyungSuk Shin                            #
#                Pedro Arduino                            #
#                Peter Mackenzie-Helnwein                 #
#   Modified by: Long Chen                                #
#                Andrew Makdisi                           #
#                Alborz Ghofrani                          #
#              --University of Washington--               #
#                                                         #
# ---> Basic units are kN and m   (unless specified)      #
#                                                         #
# #########################################################

wipe

# ---------------------------------------------------------
#  1. DEFINE SOIL AND MESH GEOMETRY
# ---------------------------------------------------------

# Material parameters
#pset Dr 0.61
#pset G0 255
#pset hpo 0.14
set grade 3.0
set layerT 3.0

# ---SOIL GEOMETRY
# number of soil layers
set numLayers       3
# layer thicknessess (m)
set layerThick(3)  2.0
set layerThick(2)  $layerT
set layerThick(1)  1.0

# depth of water table, dry 
set waterTable     2.0

# ---MESH GEOMETRY
# number of elements in horizontal direction
set nElemX  1
# number of nodes in horizontal direction
set nNodeX  [expr $nElemX + 1]
# horizontal element size (m)
set sElemX  0.25
# number of elements in vertical direction for each layer
set nElemY(3)  8
set nElemY(2)  [expr int($layerT * 4)]
set nElemY(1)  4

# define grade of slope (%)
set g -9.81


set N160 10.0
set Cd   46.0
# set Dr   [expr {sqrt($N160 / $Cd)} ]
set Gs 2.67
set emax 0.8
set emin 0.5
set void [expr $emax - $Dr * ($emax - $emin)]
set por [expr $void / (1 + $void)]
set rho_d [expr $Gs / (1 + $void)]
set rho_s [expr $rho_d *(1.0+$void/$Gs)]

set K0 0.5
set nu [expr $K0 / (1 + $K0)]

# define properties of the underlying rock
set rockVS        182.0
set rockDen       $rho_s

# ---GROUND MOTION PARAMETERS
# define velocity time history file
set velocityFile RSN766_G02_000_VEL.txt
# time step in ground motion record
set motionDT     0.005
# number of steps in ground motion record
# set motionSteps 20
set motionSteps 7998


# ---RAYLEIGH DAMPING PARAMETERS
set pi      3.141592654
set fmin 5.01
set Omegamin  [expr $fmin * 2.0 * $pi]
set ximin     0.025

# factor to mass matrix
set a0  [expr $ximin * $Omegamin]
# factor to stiffness matrix
set a1  [expr $ximin / $Omegamin]


# calculate the thickness of soil profile
set soilThick 0.0
for {set i 1} {$i <= $numLayers} {incr i} {
	set soilThick [expr $soilThick + $layerThick($i)]
}

# total number of elements in vertical direction
set nElemT     0
set layerBound(1) 0
for {set i 1} {$i <= $numLayers} {incr i} {
	incr nElemT     [expr $nElemY($i)*$nElemX]
    set  sElemY($i) [expr $layerThick($i)/$nElemY($i)]
	set  layerBound([expr $i+1]) [expr $layerBound($i) + $nElemY($i)]
    puts "size:  $sElemY($i)"
	puts "layerBound $layerBound([expr $i+1])"
}
set layerBound(1) 1

# number of nodes in vertical direction in each layer
set nNodeT 0
for {set k 1} {$k < $numLayers} {incr k 1} {
    set nNodeL($k)  [expr $nNodeX*$nElemY($k)]
    puts "number of nodes in layer $k: $nNodeL($k)"
    set nNodeT  [expr $nNodeT + $nNodeL($k)]
}
set nNodeL($numLayers) [expr $nNodeX*($nElemY($numLayers) + 1)]
puts "number of nodes in layer $numLayers: $nNodeL($numLayers)"
set nNodeT  [expr $nNodeT + $nNodeL($numLayers)]
puts "total number of nodes: $nNodeT"

#-----------------------------------------------------------------------------------------
#  2. CREATE PORE PRESSURE NODES AND FIXITIES
#-----------------------------------------------------------------------------------------
model BasicBuilder -ndm 2 -ndf 3

set yCoord  0.0 
set count   0
set gwt     1
set waterHeight [expr $soilThick-$waterTable]
set nodesInfo [open nodesInfo.dat w]
# loop over layers
for {set k 1} {$k <= $numLayers} {incr k 1} {
	# loop over nodes
	for {set j 1} {$j <= $nNodeL($k)} {incr j $nNodeX} {
		for {set i 1} {$i <= $nNodeX} {incr i} {
			node  [expr $j+$count+$i-1]    [expr ($i-1)*$sElemX]     $yCoord  
			puts $nodesInfo "[expr $j+$count+$i-1]    [expr ($i-1)*$sElemX]     $yCoord"
		
			# designate nodes above water table
			if {$yCoord>=$waterHeight} {
				set dryNode($gwt) [expr $j+$count+$i-1]
				set gwt [expr $gwt+1]
			}
		}

		set yCoord  [expr $yCoord + $sElemY($k)]
	}
	set count  [expr $count + $nNodeL($k)]
}
close $nodesInfo


# define fixities for pore pressure nodes at base of soil column
for {set i 1} {$i <= $nNodeX} {incr i} {
	fix $i  0 1 0
	# puts "fix $i  0 1 0"
	if {$i > 1} {
		equalDOF 1 $i 1
		# puts "equalDOF 1 $i 1"
	}
}
puts "Finished creating all -ndf 3 boundary conditions..."


# define equal degrees of freedom for pore pressure nodes
for {set j [expr $nNodeX + 1]} {$j < $nNodeT} {incr j $nNodeX} {
	for {set i $j} {$i < [expr $j + $nNodeX-1]} {incr i} {
		equalDOF $j [expr $i+1] 1 2
		# puts "equalDOF $j [expr $i+1] 1 2"
	}
}
puts "Finished creating equalDOF for pore pressure nodes..."

# define pore pressure boundaries for nodes above water table
for {set i 1} {$i < $gwt} {incr i 1} {
    fix $dryNode($i)  0 0 1
}

#-----------------------------------------------------------------------------------------
#  3. CREATE SOIL MATERIALS
#-----------------------------------------------------------------------------------------
set slope [expr atan($grade/100.0)]
                                                                             
nDMaterial PM4Sand 3 $Dr $G0 $hpo $rho_d 101.3 -1.00   $emax  $emin 0.5  0.1  -1.0  -1.0  250.0  -1.00  33.0  $nu
puts "nDMaterial PM4Sand 2 $Dr $G0 $hpo $rho_d 101.3 -1.00   $emax  $emin 0.5  0.1  -1.0  -1.0  250.0  -1.00  33.0  $nu"
set thick(3) 1.0
set xWgt(3)  [expr $g*sin($slope)]
set yWgt(3)  [expr $g*cos($slope)]
set uBulk(3) 2.2e6
set hPerm(3) 1.0e-7
set vPerm(3) 1.0e-7
set eInit(3) $void

nDMaterial PM4Sand 2 $Dr $G0 $hpo $rho_s 101.3 -1.00   $emax  $emin 0.5  0.1  -1.0  -1.0  250.0  -1.00  33.0  $nu                                     
puts "nDMaterial PM4Sand 1 $Dr $G0 $hpo $rho_s 101.3 -1.00   $emax  $emin 0.5  0.1  -1.0  -1.0  250.0  -1.00  33.0  $nu"
set thick(2) 1.0
set xWgt(2)  [expr $g*sin($slope)]
set yWgt(2)  [expr $g*cos($slope)]
set uBulk(2) 2.2e6
set hPerm(2) 1.0e-7
set vPerm(2) 1.0e-7
set eInit(2) $void

set E [expr 2 * $rho_s * $rockVS * $rockVS * (1 + 0.3)]
nDMaterial ElasticIsotropic 1 $E 0.3 $rho_s
set thick(1) 1.0
set xWgt(1)  [expr $g*sin($slope)]
set yWgt(1)  [expr $g*cos($slope)]
set uBulk(1) 2.2e6
set hPerm(1) 1.0e-7
set vPerm(1) 1.0e-7
set eInit(1) [expr 0.2 / (1 - 0.2)]

puts "Finished creating all soil materials..."

#-----------------------------------------------------------------------------------------
#  4. CREATE SOIL ELEMENTS
#-----------------------------------------------------------------------------------------
set elemInfo [open elementInfo.dat w]
set count 0
for {set i 1} {$i <= $numLayers} {incr i 1} {
	for {set j 1} {$j <= $nElemY($i)} {incr j 1} {
		for {set k 1} {$k <= $nElemX} {incr k} {
			set nI  [expr ($nNodeX)*($j+$count-1) + $k]
			set nJ  [expr $nI + 1]
			set nK  [expr $nI + $nNodeX + 1]
			set nL  [expr $nI + $nNodeX]
		
          # permeabilities are initially set at 1.0 m/s for gravity analysis, values are updated post-gravity
            element SSPquadUP [expr ($nElemX)*($j+$count-1) + $k] $nI $nJ $nK $nL $i $thick($i) $uBulk($i) 1.0 1.0 1.0 $eInit($i) 1.0e-7 $xWgt($i) $yWgt($i)
			puts $elemInfo "[expr ($nElemX)*($j+$count-1) + $k] $nI $nJ $nK $nL $i"
		}
	}
	set count [expr $count + $nElemY($i)]
}
close $elemInfo
puts "Finished creating all soil elements..."

#-----------------------------------------------------------------------------------------
#  6. LYSMER DASHPOT
#-----------------------------------------------------------------------------------------
model BasicBuilder -ndm 2 -ndf 2

# define dashpot nodes
set dashF [expr $nNodeT+1]
set dashS [expr $nNodeT+2]

node $dashF  0.0 0.0
node $dashS  0.0 0.0

# define fixities for dashpot nodes
fix $dashF  1 1
fix $dashS  0 1

# define equal DOF for dashpot and base soil node
equalDOF 1 $dashS  1
puts "Finished creating dashpot nodes and boundary conditions..."

# define dashpot material
set colArea       [expr $sElemX*$thick(1)]
set dashpotCoeff  [expr $rockVS*$rockDen]
uniaxialMaterial Viscous [expr $numLayers+1] [expr $dashpotCoeff*$colArea] 1

# define dashpot element
element zeroLength [expr $nElemT+1]  $dashF $dashS -mat [expr $numLayers+1]  -dir 1
puts "Finished creating dashpot material and element..."

#-----------------------------------------------------------------------------------------
#  7. CREATE GRAVITY RECORDERS
#-----------------------------------------------------------------------------------------

# create list for pore pressure nodes
set nodeList3 {}
set channel [open "nodesInfo.dat" r]
set count 0;
foreach line [split [read -nonewline $channel] \n] {
set count [expr $count+1];
set lineData($count) $line
set nodeNumber [lindex $lineData($count) 0]
lappend nodeList3 $nodeNumber
}
close $channel

# # record nodal displacment, acceleration, and porepressure
# # MODIFY FOR NODE AT SURFACE 
# eval "recorder Node -file Gdisplacement.out -time -node 49 50 -dof 1 2  disp"
# # eval "recorder Node -file Gacceleration.out -time -node $nodeList3 -dof 1 2  accel"
# # eval "recorder Node -file GporePressure.out -time -node $nodeList3 -dof 3 vel"
# # record elemental stress and strain (files are names to reflect GiD gp numbering)
# recorder Element -file Gstress.out   -time  -eleRange 1 $nElemT stress 3
# recorder Element -file Gstrain.out   -time  -eleRange 1 $nElemT strain
# puts "Finished creating gravity recorders..."

 
# -----------------------------------------------------------------------------------------
#  8. CREATE GID FLAVIA.MSH FILE FOR POSTPROCESSING
# -----------------------------------------------------------------------------------------

set meshFile [open freeFieldEffective.flavia.msh w]
puts $meshFile "MESH ffBrick dimension 2 ElemType Quadrilateral Nnode 4"
puts $meshFile "Coordinates"
puts $meshFile "#node_number   coord_x   coord_y"
set yCoord  0.0 
set count   0
# loop over layers
for {set k 1} {$k <= $numLayers} {incr k 1} {
	# loop over nodes
	for {set j 1} {$j <= $nNodeL($k)} {incr j $nNodeX} {
		for {set i 1} {$i <= $nNodeX} {incr i} {
			puts $meshFile  "[expr $j+$count+$i-1]    [expr ($i-1)*$sElemX]     $yCoord"
		}	
		set yCoord  [expr $yCoord + $sElemY($k)]
	}
	set count  [expr $count + $nNodeL($k)]
}
puts $meshFile "end coordinates"
puts $meshFile "Elements"
puts $meshFile "# element   node1   node2   node3   node4"
set count 0
# loop over layers 
for {set i 1} {$i <= $numLayers} {incr i 1} {
	for {set j 1} {$j <= $nElemY($i)} {incr j 1} {
		for {set k 1} {$k <= $nElemX} {incr k} {
			set nI  [expr ($nNodeX)*($j+$count-1) + $k]
			set nJ  [expr $nI + 1]
			set nK  [expr $nI + $nNodeX + 1]
			set nL  [expr $nI + $nNodeX]
			puts $meshFile  "[expr $j+$count] $nI $nJ $nK $nL"
			}
    }
    set count [expr $count + $nElemY($i)]
}
puts $meshFile "end elements"
close $meshFile


#-----------------------------------------------------------------------------------------
#  9. DEFINE ANALYSIS PARAMETERS
#-----------------------------------------------------------------------------------------

# duration of ground motion (s)
set duration    [expr $motionDT*$motionSteps]

set AnalysisdT  0.0001
set nSteps  [expr int(floor($duration/$AnalysisdT)+1)]
set dT      $AnalysisdT

puts "number of steps in analysis: $nSteps"
puts "analysis time step: $dT"

#---ANALYSIS PARAMETERS
# Newmark parameters

set gamma  [expr 5.0/6.0]
set beta   [expr 4.0/9.0]

#-----------------------------------------------------------------------------------------
#  10. GRAVITY ANALYSIS
#-----------------------------------------------------------------------------------------

# update materials to ensure elastic behavior
for {set i 1} {$i <= $numLayers} {incr i} {
	updateMaterialStage -material $i -stage 0
}

# fix bottom nodes for additional stability
# fix 1 1 1 0

constraints Transformation
test        NormDispIncr 1e-4 35 1
algorithm   Newton
numberer    RCM
system      SparseGeneral
integrator  Newmark $gamma $beta
analysis    Transient

set startT  [clock seconds]
analyze     10 1.0
puts "Finished with elastic gravity analysis..."

# update materials to consider plastic behavior
for {set i 1} {$i <= $numLayers} {incr i} {
	updateMaterialStage -material $i -stage 1
}
for {set i 1} {$i <= $numLayers} {incr i} {
	setParameter -value 0 -eleRange $layerBound($i) $layerBound([expr $i+1]) FirstCall $i
	setParameter -value 0.3 -eleRange $layerBound($i) $layerBound([expr $i+1]) poissonRatio $i
}
# plastic gravity loading
analyze     10 1.0

# remove extra bottom fixity for dynamic analysis
# remove sp 1 1 
puts "Finished with plastic gravity analysis..."

#-----------------------------------------------------------------------------------------
#  11. UPDATE ELEMENT PERMEABILITY VALUES FOR POST-GRAVITY ANALYSIS
#-----------------------------------------------------------------------------------------

# update permeability parameters for each element
for {set i 1} {$i <= $numLayers} {incr i} {
    # input dynamic permeability which is hydraulic conductivity divided by unit weight of water
	setParameter -value [expr $hPerm($i) / 9.81] -eleRange $layerBound($i) $layerBound([expr $i+1]) hPerm
	setParameter -value [expr $vPerm($i) / 9.81] -eleRange $layerBound($i) $layerBound([expr $i+1]) vPerm
}
puts "Finished updating permeabilities for dynamic analysis..."

#-----------------------------------------------------------------------------------------
#  12. CREATE POST-GRAVITY RECORDERS
#-----------------------------------------------------------------------------------------

# reset time and analysis
setTime 0.0
wipeAnalysis
remove recorders

# recorder time step
set recDT  0.001

set nodeListP {}
for {set j 1} {$j <= 49} {incr j 2} {
    lappend nodeListP $j
}
# puts $nodeListP

# record nodal displacment, acceleration, and porepressure
eval "recorder Node -file displacement.out -time -dT $recDT -node $nodeListP -dof 1 disp"
# eval "recorder Node -file acceleration.out -time -dT $recDT -node $nodeList3 -dof 1 2  accel"
eval "recorder Node -file porePressure.out -time -dT $recDT -node $nodeList3 -dof 3 vel"
# record elemental stress and strain (files are names to reflect GiD gp numbering)
recorder Element -file stress.out   -time -dT $recDT  -eleRange 1 $nElemT  stress 3
recorder Element -file strain.out   -time -dT $recDT  -eleRange 1 $nElemT  strain
# puts "Finished creating all recorders..."

#-----------------------------------------------------------------------------------------
#  13. DYNAMIC ANALYSIS
#-----------------------------------------------------------------------------------------

model BasicBuilder -ndm 2 -ndf 3

# define constant scaling factor for applied velocity
set cFactor [expr $colArea*$dashpotCoeff]

# timeseries object for force history
set mSeries "Path -dt $motionDT -filePath $velocityFile -factor $cFactor"

# loading object
pattern Plain 10 $mSeries {
    load 1  1.0 0.0 0.0
}
puts "Dynamic loading created..."

set gamma  0.5
set beta   0.25


constraints Transformation
test        NormDispIncr 1.0e-4 35 1
algorithm   Newton
numberer    RCM
system      SparseGeneral
integrator  Newmark $gamma $beta
rayleigh    $a0 $a1 0.0 0.0
analysis    Transient

# Analyze and use substepping if needed
set remStep $nSteps
set success 0

proc subStepAnalyze {dT subStep} {
	if {$subStep > 10} {
		return -10
	}
	for {set i 1} {$i < 3} {incr i} {
		puts "Try dT = $dT"
		set success [analyze 1 $dT]
		if {$success != 0} {
			set success [subStepAnalyze [expr $dT/2.0] [expr $subStep+1]]
			if {$success == -10} {
				puts "Did not converge."
				return $success
			}
		} else {
			if {$i==1} {
				puts "Substep $subStep : Left side converged with dT = $dT"
			} else {
				puts "Substep $subStep : Right side converged with dT = $dT"
			}
		}
	}
	return $success
}

puts "Start analysis"
set startT [clock seconds]

while {$success != -10} {
	set subStep 0
	set success [analyze $remStep  $dT]
	if {$success == 0} {
		puts "Analysis Finished"
		break
	} else {
		set curTime  [getTime]
		puts "Analysis failed at $curTime . Try substepping."
		set success  [subStepAnalyze [expr $dT/2.0] [incr subStep]]
        set curStep  [expr int($curTime/$dT + 1)]
        set remStep  [expr int($nSteps-$curStep)]
		puts "Current step: $curStep , Remaining steps: $remStep"
	}
}
set endT [clock seconds]
puts "loading analysis execution time: [expr $endT-$startT] seconds."

puts "Finished with dynamic analysis..."

#-----------------------------------------------------------------------------------------
#  14. WRITING MAX AND RESIDUAL DISPLACEMENTS OF TOP NODE TO RESULTS FILE FOR QUOFEM
#-----------------------------------------------------------------------------------------

wipe

set OutputFile [open "results.out" w]

 # looping between the nodes    
for {set z 1} {$z <= 25} {incr z} { 
    set dispfile [open "displacement.out" "r"]
    # one row from displacement file
    gets $dispfile row 
    set resz 0
    set maxz 0
    if {$z == 1} {
    	set disp1_0 [lindex [split $row " "] 1]
    	# puts $disp1_0
    }
    # gets displacement at first row of the node
    set dispz0 [lindex [split $row " "] $z]
    while {1 == 1} {
        set cnt [gets $dispfile row]
        if {$cnt < 0} {
            set resz [expr abs($reldispzt)]
            break
        }
        set dispzt [expr ([lindex [split $row " "] $z] - $dispz0)]
        set reldispzt [expr ($dispzt -([lindex [split $row " "] 1] - $disp1_0))]
        if {[expr abs($reldispzt) > $maxz]} {set maxz [expr abs($reldispzt)]}
    }
    puts -nonewline $OutputFile "$maxz $resz " 
    close $dispfile
}


close $OutputFile

wipe
