#pset Fy_1 480.0
#pset E0_1 208000.0
#pset b_1 0.02
#pset a1_1 0.02
#pset a3_1 0.025

wipe
model testUniaxial
set matTag 1
set filename "stress.1.coords"
set f [open $filename r]
set strainHistory [split [string trim [read $f]]]
close $f
set fileOut "results.out"
set filePred "hysteresis.txt"
set out [open $fileOut w]
set out2 [open $filePred w]
# uniaxialMaterial('Steel01', matTag, Fy, E0, b, a1, a2, a3, a4)
uniaxialMaterial Steel01 $matTag $Fy_1 $E0_1 $b_1 $a1_1  1.0 $a3_1 1.0
uniaxialTest $matTag
set strain 0.0
set count 1
set iTime 0
set strain [expr $strain]
strainUniaxialTest $strain
set stress [stressUniaxialTest]
set tangent [tangUniaxialTest]
set iTime [expr $iTime+1]
puts $out2 "$strain $stress"
foreach strainExtremeVal $strainHistory {
		set numIncr 1
		set strainIncr [expr ($strainExtremeVal - $strain)/$numIncr]
		for {set i 0} {$i < $numIncr} {incr i 1} {
			set strain [expr $strain+$strainIncr]
			strainUniaxialTest $strain
			set stress [stressUniaxialTest]
			set tangent [tangUniaxialTest]
			set iTime [expr $iTime+1]
			puts $out2 "$strain $stress"
			puts $out "$stress"
		}
}
close $out
close $out2
puts "MATERIAL TESTER RAN SUCCESSFULLY!"
wipe
