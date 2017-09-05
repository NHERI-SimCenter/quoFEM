# print out some results from recorded data
set a [open floorDispEnv.out r]
set line [gets $a]; set line [gets $a]; set line [gets $a]
close $a
set roofDisp [lindex $line [expr [llength $line] -1]]

set a [open floorAccEnv.out r]
set line [gets $a]; set line [gets $a]; set line [gets $a]
close $a
set roofAccel [lindex $line [expr [llength $line] -1]]


