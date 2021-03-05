# create file handler to write results to output & list into which we will put results
set resultFile [open results.out w]
set results []

# get list of valid nodeTags
set nodeTags [getNodeTags]

# for each quanity in list of QoI passed
#  - get nodeTag
#  - get nodal displacement if valid node, output 0.0 if not
#  - for valid node output displacement, note if dof not provided output 1'st dof

foreach edp $listQoI {
	set splitEDP [split $edp "_"]	
	set nodeTag [lindex $splitEDP 1]
	if {$nodeTag in $nodeTags} {
	    set nodeDisp [nodeDisp $nodeTag]
	    if {[llength $splitEDP] == 3} {
		set result [expr abs([lindex $nodeDisp 0])]
	    } else {
		set result [expr abs([lindex $nodeDisp [expr [lindex $splitEDP 3]-1]])]
	    }
	} else {
	    set result 0.
	}
	lappend results $result
}

# send results to output file & close the file
puts $resultFile $results
close $resultFile
