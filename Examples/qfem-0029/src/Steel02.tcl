#pset  Fy_2 480.0
#pset  E0_2 208000.0
#pset   b_2 0.02
#pset cR1_2 0.87
#pset cR2_2 0.06
#pset  a1_2 0.02
#pset  a3_2 0.025

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
# uniaxialMaterial Steel02 matTag fy? E? b? <R0? cR1? cR2? <a1? a2? a3? a4?> sigInit>
uniaxialMaterial Steel02 $matTag $Fy_2 $E0_2 $b_2 20.0 $cR1_2 $cR2_2 $a1_2  1.0 $a3_2 1.0 0.0
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
