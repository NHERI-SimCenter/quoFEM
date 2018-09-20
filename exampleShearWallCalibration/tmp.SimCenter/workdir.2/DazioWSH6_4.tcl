

model BasicBuilder -ndm 2 -ndf 2

#These are the problem variables
#pset beta 5.462261077231849e-01   
#pset Ap 9.665784174420770e-02 
#pset An 3.528346194898871e+00 
#pset Bn 4.914497665322617e-01

# define nodes: node nodeTag x y
node 1 0 0 
node 2 7.5785 0 
node 3 7.5785 11.3329 
node 4 0 11.3329 
node 5 7.5785 22.6657 
node 6 0 22.6657 
node 7 7.5785 33.9986 
node 8 0 33.9986 
node 9 7.5785 45.3314 
node 10 0 45.3314 
node 11 7.5785 56.6643 
node 12 0 56.6643 
node 13 7.5785 67.9971 
node 14 0 67.9971 
node 15 7.5785 79.33 
node 16 0 79.33 
node 17 15.157 0 
node 18 15.157 11.3329 
node 19 15.157 22.6657 
node 20 15.157 33.9986 
node 21 15.157 45.3314 
node 22 15.157 56.6643 
node 23 15.157 67.9971 
node 24 15.157 79.33 
node 25 27.2635 0 
node 26 27.2635 11.3329 
node 27 27.2635 22.6657 
node 28 27.2635 33.9986 
node 29 27.2635 45.3314 
node 30 27.2635 56.6643 
node 31 27.2635 67.9971 
node 32 27.2635 79.33 
node 33 39.37 0 
node 34 39.37 11.3329 
node 35 39.37 22.6657 
node 36 39.37 33.9986 
node 37 39.37 45.3314 
node 38 39.37 56.6643 
node 39 39.37 67.9971 
node 40 39.37 79.33 
node 41 51.4765 0 
node 42 51.4765 11.3329 
node 43 51.4765 22.6657 
node 44 51.4765 33.9986 
node 45 51.4765 45.3314 
node 46 51.4765 56.6643 
node 47 51.4765 67.9971 
node 48 51.4765 79.33 
node 49 63.583 0 
node 50 63.583 11.3329 
node 51 63.583 22.6657 
node 52 63.583 33.9986 
node 53 63.583 45.3314 
node 54 63.583 56.6643 
node 55 63.583 67.9971 
node 56 63.583 79.33 
node 57 71.1615 0 
node 58 71.1615 11.3329 
node 59 71.1615 22.6657 
node 60 71.1615 33.9986 
node 61 71.1615 45.3314 
node 62 71.1615 56.6643 
node 63 71.1615 67.9971 
node 64 71.1615 79.33 
node 65 78.74 0 
node 66 78.74 11.3329 
node 67 78.74 22.6657 
node 68 78.74 33.9986 
node 69 78.74 45.3314 
node 70 78.74 56.6643 
node 71 78.74 67.9971 
node 72 78.74 79.33 
node 73 7.5785 90.6629 
node 74 0 90.6629 
node 75 7.5785 101.996 
node 76 0 101.996 
node 77 7.5785 113.329 
node 78 0 113.329 
node 79 7.5785 124.661 
node 80 0 124.661 
node 81 7.5785 135.994 
node 82 0 135.994 
node 83 7.5785 147.327 
node 84 0 147.327 
node 85 7.5785 158.66 
node 86 0 158.66 
node 87 15.157 90.6629 
node 88 15.157 101.996 
node 89 15.157 113.329 
node 90 15.157 124.661 
node 91 15.157 135.994 
node 92 15.157 147.327 
node 93 15.157 158.66 
node 94 27.2635 90.6629 
node 95 27.2635 101.996 
node 96 27.2635 113.329 
node 97 27.2635 124.661 
node 98 27.2635 135.994 
node 99 27.2635 147.327 
node 100 27.2635 158.66 
node 101 39.37 90.6629 
node 102 39.37 101.996 
node 103 39.37 113.329 
node 104 39.37 124.661 
node 105 39.37 135.994 
node 106 39.37 147.327 
node 107 39.37 158.66 
node 108 51.4765 90.6629 
node 109 51.4765 101.996 
node 110 51.4765 113.329 
node 111 51.4765 124.661 
node 112 51.4765 135.994 
node 113 51.4765 147.327 
node 114 51.4765 158.66 
node 115 63.583 90.6629 
node 116 63.583 101.996 
node 117 63.583 113.329 
node 118 63.583 124.661 
node 119 63.583 135.994 
node 120 63.583 147.327 
node 121 63.583 158.66 
node 122 71.1615 90.6629 
node 123 71.1615 101.996 
node 124 71.1615 113.329 
node 125 71.1615 124.661 
node 126 71.1615 135.994 
node 127 71.1615 147.327 
node 128 71.1615 158.66 
node 129 78.74 90.6629 
node 130 78.74 101.996 
node 131 78.74 113.329 
node 132 78.74 124.661 
node 133 78.74 135.994 
node 134 78.74 147.327 
node 135 78.74 158.66 

# define uniaxialMaterial for rebars, uniaxialMaterial matType? $Fy $E0 $b
uniaxialMaterial Steel01 1 84.6555 29000 0.01 
uniaxialMaterial Steel01 2 83.5388 29000 0.01 
uniaxialMaterial Steel01 3 75.2574 29000 0.01 

# nDMaterial PlaneStressRebarMaterial matTag matTag_for_uniaxialMaterial angle_of_rebar 
nDMaterial PlaneStressRebarMaterial 1 1 90 
nDMaterial PlaneStressRebarMaterial 2 2 90 
nDMaterial PlaneStressRebarMaterial 3 3 0 


# define pars for PlasticDamageConcretePlaneStress
set E 4635.43;# ksi 
set nu 0.2 
set fc [expr 6.61349*0.7] ;# ksi 
set ft [expr $fc*0.25] ;# ksi 

#source parameters.tcl

set outPutFileName outputs.out 
nDMaterial PlasticDamageConcretePlaneStress 4 $E $nu $ft $fc $beta $Ap $An $Bn ;# concrete 

# nDMaterial PlaneStressLayeredMaterial matTag num_of_layers layer_1_Tag  layer_1_thickness 1 layer_2_Tag  layer_2_thickness layer_3_Tag  layer_3_thickness
nDMaterial PlaneStressLayeredMaterial 5 3 4 5.85852 1 0.0316793 3 0.0148032  
nDMaterial PlaneStressLayeredMaterial 6 3 4 5.81253 2 0.0693821 3 0.0230882  
nDMaterial PlaneStressLayeredMaterial 7 3 4 5.85852 1 0.0316793 3 0.0148032  
nDMaterial PlaneStressLayeredMaterial 8 3 4 5.81253 2 0.0693821 3 0.0230882  

# define elements: element quad eleTag i j k l thickness PlaneStress matTag
element quad 0 1 2 3 4 1 PlaneStress 6
element quad 1 4 3 5 6 1 PlaneStress 6
element quad 2 6 5 7 8 1 PlaneStress 6
element quad 3 8 7 9 10 1 PlaneStress 6
element quad 4 10 9 11 12 1 PlaneStress 6
element quad 5 12 11 13 14 1 PlaneStress 6
element quad 6 14 13 15 16 1 PlaneStress 6
element quad 7 2 17 18 3 1 PlaneStress 6
element quad 8 3 18 19 5 1 PlaneStress 6
element quad 9 5 19 20 7 1 PlaneStress 6
element quad 10 7 20 21 9 1 PlaneStress 6
element quad 11 9 21 22 11 1 PlaneStress 6
element quad 12 11 22 23 13 1 PlaneStress 6
element quad 13 13 23 24 15 1 PlaneStress 6
element quad 14 17 25 26 18 1 PlaneStress 5
element quad 15 18 26 27 19 1 PlaneStress 5
element quad 16 19 27 28 20 1 PlaneStress 5
element quad 17 20 28 29 21 1 PlaneStress 5
element quad 18 21 29 30 22 1 PlaneStress 5
element quad 19 22 30 31 23 1 PlaneStress 5
element quad 20 23 31 32 24 1 PlaneStress 5
element quad 21 25 33 34 26 1 PlaneStress 5
element quad 22 26 34 35 27 1 PlaneStress 5
element quad 23 27 35 36 28 1 PlaneStress 5
element quad 24 28 36 37 29 1 PlaneStress 5
element quad 25 29 37 38 30 1 PlaneStress 5
element quad 26 30 38 39 31 1 PlaneStress 5
element quad 27 31 39 40 32 1 PlaneStress 5
element quad 28 33 41 42 34 1 PlaneStress 5
element quad 29 34 42 43 35 1 PlaneStress 5
element quad 30 35 43 44 36 1 PlaneStress 5
element quad 31 36 44 45 37 1 PlaneStress 5
element quad 32 37 45 46 38 1 PlaneStress 5
element quad 33 38 46 47 39 1 PlaneStress 5
element quad 34 39 47 48 40 1 PlaneStress 5
element quad 35 41 49 50 42 1 PlaneStress 5
element quad 36 42 50 51 43 1 PlaneStress 5
element quad 37 43 51 52 44 1 PlaneStress 5
element quad 38 44 52 53 45 1 PlaneStress 5
element quad 39 45 53 54 46 1 PlaneStress 5
element quad 40 46 54 55 47 1 PlaneStress 5
element quad 41 47 55 56 48 1 PlaneStress 5
element quad 42 49 57 58 50 1 PlaneStress 6
element quad 43 50 58 59 51 1 PlaneStress 6
element quad 44 51 59 60 52 1 PlaneStress 6
element quad 45 52 60 61 53 1 PlaneStress 6
element quad 46 53 61 62 54 1 PlaneStress 6
element quad 47 54 62 63 55 1 PlaneStress 6
element quad 48 55 63 64 56 1 PlaneStress 6
element quad 49 57 65 66 58 1 PlaneStress 6
element quad 50 58 66 67 59 1 PlaneStress 6
element quad 51 59 67 68 60 1 PlaneStress 6
element quad 52 60 68 69 61 1 PlaneStress 6
element quad 53 61 69 70 62 1 PlaneStress 6
element quad 54 62 70 71 63 1 PlaneStress 6
element quad 55 63 71 72 64 1 PlaneStress 6
element quad 56 16 15 73 74 1 PlaneStress 8
element quad 57 74 73 75 76 1 PlaneStress 8
element quad 58 76 75 77 78 1 PlaneStress 8
element quad 59 78 77 79 80 1 PlaneStress 8
element quad 60 80 79 81 82 1 PlaneStress 8
element quad 61 82 81 83 84 1 PlaneStress 8
element quad 62 84 83 85 86 1 PlaneStress 8
element quad 63 15 24 87 73 1 PlaneStress 8
element quad 64 73 87 88 75 1 PlaneStress 8
element quad 65 75 88 89 77 1 PlaneStress 8
element quad 66 77 89 90 79 1 PlaneStress 8
element quad 67 79 90 91 81 1 PlaneStress 8
element quad 68 81 91 92 83 1 PlaneStress 8
element quad 69 83 92 93 85 1 PlaneStress 8
element quad 70 24 32 94 87 1 PlaneStress 7
element quad 71 87 94 95 88 1 PlaneStress 7
element quad 72 88 95 96 89 1 PlaneStress 7
element quad 73 89 96 97 90 1 PlaneStress 7
element quad 74 90 97 98 91 1 PlaneStress 7
element quad 75 91 98 99 92 1 PlaneStress 7
element quad 76 92 99 100 93 1 PlaneStress 7
element quad 77 32 40 101 94 1 PlaneStress 7
element quad 78 94 101 102 95 1 PlaneStress 7
element quad 79 95 102 103 96 1 PlaneStress 7
element quad 80 96 103 104 97 1 PlaneStress 7
element quad 81 97 104 105 98 1 PlaneStress 7
element quad 82 98 105 106 99 1 PlaneStress 7
element quad 83 99 106 107 100 1 PlaneStress 7
element quad 84 40 48 108 101 1 PlaneStress 7
element quad 85 101 108 109 102 1 PlaneStress 7
element quad 86 102 109 110 103 1 PlaneStress 7
element quad 87 103 110 111 104 1 PlaneStress 7
element quad 88 104 111 112 105 1 PlaneStress 7
element quad 89 105 112 113 106 1 PlaneStress 7
element quad 90 106 113 114 107 1 PlaneStress 7
element quad 91 48 56 115 108 1 PlaneStress 7
element quad 92 108 115 116 109 1 PlaneStress 7
element quad 93 109 116 117 110 1 PlaneStress 7
element quad 94 110 117 118 111 1 PlaneStress 7
element quad 95 111 118 119 112 1 PlaneStress 7
element quad 96 112 119 120 113 1 PlaneStress 7
element quad 97 113 120 121 114 1 PlaneStress 7
element quad 98 56 64 122 115 1 PlaneStress 8
element quad 99 115 122 123 116 1 PlaneStress 8
element quad 100 116 123 124 117 1 PlaneStress 8
element quad 101 117 124 125 118 1 PlaneStress 8
element quad 102 118 125 126 119 1 PlaneStress 8
element quad 103 119 126 127 120 1 PlaneStress 8
element quad 104 120 127 128 121 1 PlaneStress 8
element quad 105 64 72 129 122 1 PlaneStress 8
element quad 106 122 129 130 123 1 PlaneStress 8
element quad 107 123 130 131 124 1 PlaneStress 8
element quad 108 124 131 132 125 1 PlaneStress 8
element quad 109 125 132 133 126 1 PlaneStress 8
element quad 110 126 133 134 127 1 PlaneStress 8
element quad 111 127 134 135 128 1 PlaneStress 8

# define time series
timeSeries Constant 1 -factor 0
timeSeries Constant 2 -factor 0
timeSeries Constant 3 -factor 0
timeSeries Path 4 -dt 1 -values { 0 0.0873127 0.134938 0.1905 0.254001 0.317501 0.412751 0.381001 0.309563 0.198438 0.111125 0.0793752 0.00793752 -0.0238125 -0.0873127 -0.174625 -0.301626 -0.309563 -0.22225 -0.111125 -0.0317501 0.111125 0.317501 0.484188 0.658814 0.825502 1.02394 0.992189 0.777877 0.500064 0.198438 0.0714376 -0.0873127 -0.269876 -0.428626 -0.619126 -0.801689 -0.992189 -0.960439 -0.849314 -0.595314 -0.349251 -0.134938 0.0635001 0.261938 0.531814 0.769939 0.904877 1.07156 1.32557 1.51607 1.50019 1.32557 1.12713 0.881064 0.658814 0.444501 0.238125 0.111125 -0.0952502 -0.373063 -0.595314 -0.801689 -1.016 -1.16681 -1.524 -1.46844 -1.25413 -0.976314 -0.738189 -0.452438 -0.182563 0.0396876 0.428626 0.714376 1.0795 1.42082 1.61925 1.89707 2.03994 1.9685 1.60338 1.2065 0.896939 0.547689 0.309563 0.182563 0.0396876 -0.15875 -0.428626 -0.666751 -0.904877 -1.143 -1.42875 -1.66688 -1.97644 -1.93675 -1.66688 -1.397 -1.03981 -0.793752 -0.579439 -0.301626 -0.0555626 0.15875 0.754064 1.22238 1.60338 1.93675 2.27013 2.50826 2.54794 2.35744 1.9685 1.5875 1.32557 1.19063 1.016 0.825502 0.611189 0.460376 0.309563 0.206375 0.127 -0.277813 -0.666751 -1.2065 -1.524 -1.92882 -2.21457 -2.48444 -2.53207 -2.35744 -2.02407 -1.63513 -1.24619 -0.928689 -0.555626 -0.261938 -0.0238125 0.555626 1.11919 1.57163 1.9685 2.34157 2.63526 2.92894 3.06388 2.97657 2.57969 2.159 1.88913 1.57957 1.23825 0.722314 0.428626 0.238125 0.0714376 -0.325438 -0.833439 -1.10331 -1.53988 -1.82563 -2.2225 -2.56382 -2.86544 -3.00038 -3.04801 -2.98451 -2.72257 -2.38919 -1.96057 -1.5875 -1.2065 -0.817564 -0.412751 -0.198438 0.0238125 0.595314 1.08744 1.50813 1.88119 2.29394 2.79401 3.19882 3.44488 3.57982 3.69888 3.32582 2.91307 2.61144 2.36538 2.02407 1.60338 1.016 0.603251 0.269876 0.0238125 -0.531814 -1.03188 -1.651 -2.159 -2.69082 -3.09563 -3.39726 -3.44488 -3.50044 -3.54807  }
set nPts 208 

 # Fix The Bottom Nodes
 # pattern Plain patternTag timeSeriesTag 
pattern Plain 1 1 {
  # sp nodeTag dof value
  sp 1 1 0
  sp 1 2 0
  sp 2 1 0
  sp 2 2 0
  sp 17 1 0
  sp 17 2 0
  sp 25 1 0
  sp 25 2 0
  sp 33 1 0
  sp 33 2 0
  sp 41 1 0
  sp 41 2 0
  sp 49 1 0
  sp 49 2 0
  sp 57 1 0
  sp 57 2 0
  sp 65 1 0
  sp 65 2 0
}

 # Vertical Loads on Top, do not include weight 
pattern Plain 2 2 {
  # load nodeTag force_at_dof_1 force_at_dof_2
  load 85 0.0 -36.8672
  load 86 0.0 -36.8672
  load 93 0.0 -36.8672
  load 100 0.0 -36.8672
  load 107 0.0 -36.8672
  load 114 0.0 -36.8672
  load 121 0.0 -36.8672
  load 128 0.0 -36.8672
  load 135 0.0 -36.8672
}

 # Some Horizontal Load 
pattern Plain 3 3 {
  load 86 0.5 0.0
  load 86 0.0 0.244983
  load 135 0.5 0.0
  load 135 0.0 -0.244983
}

 # DisplacementControlAtTop 
pattern Plain 4 4 {
  sp 85 1 1
  sp 86 1 1
  sp 93 1 1
  sp 100 1 1
  sp 107 1 1
  sp 114 1 1
  sp 121 1 1
  sp 128 1 1
  sp 135 1 1
}

# add self weight

set numNodes 135 
set rho 8.60914e-05 
set g -1.0
set volumeOfWall 73770.5

pattern Plain 100 "Constant" { 
load 1 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 2 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 3 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 4 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 5 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 6 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 7 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 8 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 9 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 10 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 11 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 12 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 13 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 14 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 15 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 16 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 17 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 18 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 19 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 20 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 21 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 22 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 23 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 24 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 25 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 26 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 27 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 28 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 29 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 30 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 31 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 32 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 33 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 34 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 35 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 36 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 37 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 38 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 39 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 40 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 41 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 42 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 43 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 44 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 45 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 46 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 47 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 48 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 49 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 50 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 51 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 52 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 53 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 54 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 55 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 56 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 57 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 58 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 59 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 60 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 61 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 62 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 63 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 64 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 65 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 66 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 67 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 68 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 69 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 70 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 71 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 72 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 73 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 74 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 75 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 76 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 77 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 78 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 79 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 80 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 81 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 82 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 83 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 84 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 85 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 86 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 87 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 88 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 89 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 90 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 91 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 92 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 93 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 94 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 95 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 96 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 97 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 98 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 99 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 100 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 101 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 102 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 103 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 104 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 105 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 106 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 107 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 108 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 109 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 110 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 111 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 112 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 113 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 114 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 115 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 116 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 117 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 118 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 119 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 120 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 121 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 122 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 123 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 124 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 125 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 126 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 127 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 128 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 129 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 130 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 131 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 132 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 133 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 134 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
load 135 0.0 [expr ($volumeOfWall*$rho*$g)/$numNodes];#SELF-WEIGHT
} 

# specify the solver
constraints Plain
system UmfPack
test NormDispIncr 1.0e-3 30
algorithm ModifiedNewton
numberer RCM
integrator LoadControl 1
analysis Static
analyze 1

puts "gravity is done."



lappend EDPNodes 85
lappend EDPNodes 86
lappend EDPNodes 93
lappend EDPNodes 100
lappend EDPNodes 107
lappend EDPNodes 114
lappend EDPNodes 121
lappend EDPNodes 128
lappend EDPNodes 135
eval "recorder Node -file $outPutFileName -time -node $EDPNodes -dof 1 reaction"

system UmfPack 
constraints Penalty 1.0e10 1.0e10 
test NormDispIncr 1.0e-4 50 0 
algorithm Newton 
numberer RCM 
integrator LoadControl 1. 
analysis Static 
for {set i 0} {$i < [expr $nPts]} {incr i 1} { 
set ok [analyze 1] 
if {$ok != 0} { 
test NormDispIncr 1.0e-3 1000 0 
algorithm Newton -initial 
set ok [analyze 1] 
test NormDispIncr 1.0e-4 500 0 
algorithm Newton 
} 
if {$ok != 0} {  
set i [expr $nPts]; 
} 
} 
puts "cyclic is done."

remove recorders 
wipe  
