set resultFile [open results.out w]
set results []



# create file handler to write results to output & list into which we will put results

set nodeIn [open node1.out r]
while { [gets $nodeIn data] >= 0 } {
     set maxDisplacement $data
}
puts $maxDisplacement
set sum $maxDisplacement
set sqSum [lmap b $maxDisplacement { expr pow($b,2) }]



set nodefilenames { node2 node3 node4 node5 node6 node7 node8 node9 node10 }

foreach nodefile $nodefilenames {
	set nodeIn [open $nodefile.out r]
	while { [gets $nodeIn data] >= 0 } {
	     set maxDisplacement $data
	}
	puts $maxDisplacement
	set sum [lmap a $sum b $maxDisplacement { expr {$a + $b} }]
	set sqSum [lmap a $sqSum b $maxDisplacement { expr {$a + pow($b,2)} }]
}


set mean_val [lmap a $sum { expr {$a/10} }]
set std_val [lmap a $sqSum b $mean_val { expr {sqrt($a/10 - pow($b,2))} }]

#set listQoI {Node_3_Disp_1_Mean Node_3_Disp_1_Std}
# for each quanity in list of QoI passed
#  - get nodeTag
#  - get nodal displacement if valid node, output 0.0 if not
#  - for valid node output displacement, note if dof not provided output 1'st dof

foreach edp $listQoI {
	set splitEDP [split $edp "_"]	
	set nodeTag [lindex $splitEDP 1]
        if {[llength $splitEDP] == 3} {
           set dof 1
	} else {
           set dof [lindex $splitEDP 3]
	} 

    #set nodeDisp [lindex $maxDisplacement [expr (($nodeTag-1)*2)+$dof-1]]
	if {[lindex $splitEDP 4] == {Mean}} {
		set quantity1 [lindex $mean_val [expr (($nodeTag-1)*2)+$dof-1]]
	}
	if {[lindex $splitEDP 4] == {Std}} {
		set quantity1 [lindex $std_val [expr (($nodeTag-1)*2)+$dof-1]]
	}

	lappend results $quantity1
}

puts $mean_val
puts $std_val

# send results to output file & close the file
puts $resultFile $results
close $resultFile
