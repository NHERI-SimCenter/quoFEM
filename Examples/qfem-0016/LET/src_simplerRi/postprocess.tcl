set resultFile [open results.out w]
set results []

# create file handler to write results to output & list into which we will put results

set nodeIn [open nodeDisp.out r]
while { [gets $nodeIn data] >= 0 } {
     set maxDisplacement $data
}
puts $maxDisplacement

set nodeIn [open nodeAcc.out r]
while { [gets $nodeIn data] >= 0 } {
     set maxAccelaration $data
}
puts $maxAccelaration

foreach edp $listQoI {
	set splitEDP [split $edp "_"]	
	set nodeTag [lindex $splitEDP 1]
        if {[llength $splitEDP] == 3} {
           set dof 1
	} else {
           set dof [lindex $splitEDP 3]
	} 

    #set nodeDisp [lindex $maxDisplacement [expr (($nodeTag-1)*2)+$dof-1]]
	if {[lindex $splitEDP 2] == {Disp}} {
		set quantity1 [lindex $maxDisplacement [expr (($nodeTag-1)*2)+$dof-1]]
	}
	if {[lindex $splitEDP 2] == {Acc}} {
		set quantity1 [lindex $maxAccelaration [expr (($nodeTag-1)*2)+$dof-1]]
	}

	lappend results $quantity1
}

# send results to output file & close the file
puts $resultFile $results
close $resultFile
