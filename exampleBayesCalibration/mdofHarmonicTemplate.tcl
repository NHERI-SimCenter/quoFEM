# parameters student played with and cannot remember

pset roofFactor 0.5
pset dampRatio 0.02


# other paramaters he used 

set w 100.0;   # typ floor weight (kips)
set k 326.32;  # typ. story stiffness (kips/in)

#
# the model
#

wipe
model basic -ndm 1 -ndf 1

set m [expr $w/386.4]
node  1  0.
node  2  0. -mass $m
node  3  0. -mass $m
node  4  0. -mass [expr $roofFactor*$m]
    
uniaxialMaterial Elastic 1 $k
element zeroLength 1 1 2 -mat 1 -dir 1
element zeroLength 2 2 3 -mat 1 -dir 1
element zeroLength 3 3 4 -mat 1 -dir 1
    
fix 1 1

# harmonic force propertires

set P  1000.
set periodForce 0.1

# add load pattern
timeSeries Trig 1 0.0 [expr 100.0*$periodForce] $periodForce -factor $P
pattern Plain 1 1 {
    load 2 1.0 
}

#
# the analysis
#

constraints Plain
numberer Plain
algorithm Linear
system FullGeneral
integrator Newmark 0.5 0.25
analysis Transient

set eigenValues [eigen -fullGenLapack 3]
modalDamping $dampRatio

#
# output specification
#

recorder Node -file node.txt -time -node 3 4 -dof 1 disp

#
# run the analysis 
#

analyze 1000 0.001

