wipe;
model testUniaxial;

set matTag 1
set filename "stress.1.coords"
set f [open $filename r]
set strainHistory [split [string trim [read $f]]]
close $f

# Output file names
set fileOut "results.out"
set filePred "hysteresis.txt"

# Initialize strings to store outputs
set stressList ""
set strainStressList ""

# Define the material
uniaxialMaterial Steel02 1 $fy $E $b 20.0 $cR1 $cR2 $a1 1.0 $a3 1.0 0.0

uniaxialTest $matTag

# Initial conditions
set strain 0.0
set iTime 0

# Apply initial strain and record output
strainUniaxialTest $strain
set stress [stressUniaxialTest]
append strainStressList "$strain $stress\n"

foreach strainExtremeVal $strainHistory {
    set numIncr 1
    set strainIncr [expr ($strainExtremeVal - $strain)/$numIncr]

    for {set i 0} {$i < $numIncr} {incr i 1} {
        set strain [expr $strain + $strainIncr]
        strainUniaxialTest $strain
        set stress [stressUniaxialTest]
        set tangent [tangUniaxialTest]
        set iTime [expr $iTime + 1]

        # append strainStressList "$strain $stress\n"
        set line [format "%.12g %.12g\n" $strain $stress]
        append strainStressList $line

        # append stressList "$stress\n"
        set stressLine [format "%.12g\n" $stress]
        append stressList $stressLine

    }
}

# Write accumulated output at once
set out [open $fileOut w]
puts $out $stressList
close $out

set out2 [open $filePred w]
puts $out2 $strainStressList
close $out2

puts "MATERIAL TESTER RAN SUCCESSFULLY!"
wipe;
