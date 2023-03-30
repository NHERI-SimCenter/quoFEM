# This postprocess script calculates mean and variance of peak responses
# and parse QoInames give as $listQoI

set resultFile [open results.out w]

set col 1
set forceIn [open Fcol1.out r]
set maxForce 0
while { [gets $forceIn line] >= 0 } {
	set word [expr abs([lindex [split $line " "] $col])]
	if {$word > $maxForce} {
		set maxForce $word
	}
}
# send results to output file & close the file
puts $resultFile $maxForce
close $resultFile