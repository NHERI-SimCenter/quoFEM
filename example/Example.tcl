# written: fmk
# units: kip & in

# set some variables
set gMotion NSK-E
#set gMotion el_centro
set scale 4.0

#set roofWeight  450; #kips; 
#set floorWeight 650;

set roofWeight   900; #kips; 
set floorWeight 1300;

set percentLoadFrame [expr 15./120.] 
set percentMassFrame 0.5; # %mass frame takes

set dampRatio 0.02
set mode1 1
set mode2 3

# set up my lists
set floorOffsets {180. 180. 180.}
set colOffsets   {288. 288. 288.} 
set colSizes     {W14X370 W14X370 W14X211};
set colExtSizes  {W14X370 W14X370 W14X211};
set beamSizes    {W33X141 W33X130 W27X102};

source SteelMomentFrame2d_UniformExcitation.tcl

source ProcessExample.tcl