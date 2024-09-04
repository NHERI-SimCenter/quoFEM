pset fy 480
pset E 20800
pset b 0.18
pset cR1 0.87
pset cR2 0.06
pset a1 0.02
pset a3 0.025
wipe
model testUniaxial
set matTag 1
set filename "input_data.txt"
set f [open $filename r]
set strainHistory [split [string trim [read $f]]]
close $f
set fileOut "results.out"
set out [open $fileOut w]
uniaxialMaterial Steel02 1 $fy $E $b 20.0 $cR1 $cR2 $a1  1.0 $a3 1.0 0.0
uniaxialTest $matTag
set strain 0.0
set count 1
set iTime 0
set strain [expr $strain]
strainUniaxialTest $strain
set stress [stressUniaxialTest]
set iTime [expr $iTime+1]
foreach strainExtremeVal $strainHistory {
		set numIncr 1
		set strainIncr [expr ($strainExtremeVal - $strain)/$numIncr]
		for {set i 0} {$i < $numIncr} {incr i 1} {
			set strain [expr $strain+$strainIncr]
			strainUniaxialTest $strain
			set stress [stressUniaxialTest]
			set iTime [expr $iTime+1]
			puts $out "$stress"
		}
}
close $out
puts "MATERIAL TESTER RAN SUCCESSFULLY!"
wipe
