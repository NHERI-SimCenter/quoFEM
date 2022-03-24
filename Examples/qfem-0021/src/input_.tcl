wipe;
pset x1 5
pset x2 5

set z [expr cos($x1) * cos($x2) * (exp(-pow(($x1 + 2), 2) - pow(($x2 - 2), 2)) + 0.5 * exp(-pow(($x1 - 2), 2) - pow(($x2 + 2), 2)))]


set resultFile [open results.out w]

puts $resultFile "$z";
close $resultFile;