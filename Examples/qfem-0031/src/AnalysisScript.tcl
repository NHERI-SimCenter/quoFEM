# Define PM4Sand material model parameters
#pset Dr 0.650000;
#pset G0 350.000000;
#pset hpo 0.070000;

set phicv 35.600000;
set emax 0.740000;
set emin 0.490000;
set cz 200.000000;
set zmax 2.000000;
set rho 1.260000;
set Patm 101.300000;
set h0 -1.000000;
set nb 0.500000;
set nd 0.100000;
set Ado -1.000000;
set ce -1.000000;
set nu 0.300000;
set Cgd 2.000000;
set Cdr -1.000000;
set ckaf -1.000000;
set Q 10.000000;
set R 1.500000;
set m_m 0.010000;
set Fsed_min -1.000000;
set p_sedo -1.000000;

# Initialize variables
set Cyclelist []
set CSRlist "0.105 0.130 0.151 0.172 0.200"
open nCyclesResultD.dat w

# Perform simulations
foreach CSR $CSRlist {
	source "DSS_quad_DispControlModified.tcl"
}

# Create results.out file
set resultFile [open results.out w]
set Cyclelist [linsert $Cyclelist 0 [lindex $Cyclelist 0]]
puts $resultFile $Cyclelist
close $resultFile

puts "Analysis complete"




