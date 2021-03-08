#Generalized eigenValue Problem

# units: in, kips

wipe

#model builder
model BasicBuilder -ndm 2 -ndf 3	

#input

#masses
#total mass of story distributed to two nodes
set m1 [expr 0.52/2.];
set m2 [expr 0.26/2.];

#number of modes  
set numModes 2

#material properties

#column
set Ac 110.0 
#input from the algorithm

#pset k1 958.61
#pset k2 958.61

set E 29000.0 
#beam
set Ib 10e+12 
set Ab 10e+12 

#geometry
set L 400. 
set h  120. 

#nodes
node 1   0.  0. ;
node 2   $L  0. ;
node 3   0.  $h ;
node 4   $L  $h ;
node 5   0.  [expr 2*$h];
node 6   $L  [expr 2*$h];

#fixity of base
fix 1 1 1 1;
fix 2 1 1 1;

#vertical translation constraint
fix 3 0 1 0;
fix 5 0 1 0;
fix 4 0 1 0;
fix 6 0 1 0;

#equalDoF constraints for horizontal DoF
equalDOF 3 4 1
equalDOF 5 6 1 

# assign mass
mass 3 $m1 0. 0. ;
mass 4 $m1 0. 0. ;
mass 5 $m2 0. 0. ;
mass 6 $m2 0. 0. ;


# define geometric transformation:
set TransfTag 1;
geomTransf Linear $TransfTag ;

# define elements:
# columns
element elasticBeamColumn 1 1 3 $Ac $E  [expr 2.*$k1*(pow($h,3.))/48./$E] $TransfTag; #story stiffness k = 48*E*Icolumn/h^3
element elasticBeamColumn 2 3 5 $Ac $E  [expr 2.*$k2*(pow($h,3.))/48./$E] $TransfTag;
element elasticBeamColumn 3 2 4 $Ac $E  [expr 2.*$k1*(pow($h,3.))/48./$E] $TransfTag;
element elasticBeamColumn 4 4 6 $Ac $E  [expr 2.*$k2*(pow($h,3.))/48./$E] $TransfTag;
# beams
element elasticBeamColumn 5 3 4 $Ab $E  $Ib $TransfTag; #beam flexurally rigid mimicing a rigid diaghraphm
element elasticBeamColumn 6 5 6 $Ab $E  $Ib $TransfTag;

# eigenValue
set eigenValues [eigen -fullGenLapack $numModes]

# eigenVector and normalize
set eig11 [nodeEigenvector 3 1 1];
set eig12 [nodeEigenvector 5 1 1];

set eig21 [nodeEigenvector 3 2 1];
set eig22 [nodeEigenvector 5 2 1];

set Eig1 {};
lappend Eig1 [expr $eig11/$eig11];
lappend Eig1 [expr $eig12/$eig11];

set Eig2 {};
lappend Eig2 [expr $eig21/$eig21];
lappend Eig2 [expr $eig22/$eig21];


# print first eigenValue and first eigenVector
set OutputFile [open "results.out" w]
puts $OutputFile "[lindex $eigenValues 0] [lindex $Eig1 1]"
close $OutputFile
wipe
