
# note: the variables X1, Y1, X2, Y2 will be provided when runs in workflow
# as long as they are defined as random variables!

set D [expr sqrt(($x2-$x1)*($x2-$x1)+($y2-$y1)*($y2-$y1))]

set f [open "results.out" "w"]
puts $f $D
close $f

