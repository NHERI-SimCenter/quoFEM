from openseespy.opensees import *
from sys import platform

from BuildModel import build_model
from RunAnalysis import run_analysis
import random
import json
if platform == "darwin":  # MACOS
    import openseespymac.opensees as ops
    from openseespymac.opensees import *
else:
    import openseespy.opensees as ops
    from openseespy.opensees import *

from params import *

numStories = 1  # 1 or 2
crippleWallTag = 1      # O (no) or 1 (yes)
constructionEra = "post-1950"   # "pre-1950" or "post-1950"
exteriorMaterial = "Stucco"   # "Wood Siding" or "Stucco"


model_params={}
model_params["numStories"] = numStories	# 1 or 2
model_params["crippleWallTag"] = crippleWallTag			# O (no) or 1 (yes)
model_params["constructionEra"] = constructionEra		# "pre-1950" or "post-1950"
model_params["exteriorMaterial"] = exteriorMaterial		# "Wood Siding" or "Stucco"
model_params['materialQuality']	= materialQuality			# float, [0,1], no units
model_params['planArea'] = planArea/144		# float, [800., 2400.] inch
model_params['aspectRatio'] = aspectRatio	# float, [0.4, 0.8], no units
model_params['damping']	 = damping		# float, [0.02, 0.1] percent
model_params['extDensity_x'] = extDensity_x		# float, [0.5, 0.8], no units (where extDensity_x*Perimeter = length of exterior wall on first floor in x direction)
model_params['extDensity_z'] = extDensity_z		# float, [0.5, 0.8], no units (where extDensity_z*Perimeter = length of exterior wall on first floor in z direction)
model_params['intDensity'] = intDensity		# float, [0.06, 0.08], no units (where rho_int*Area =  total length of interior wall on first floor)
model_params['extEcc_x'] = extEcc_x		# float, [0.0, 0.5], no units (offset from CoD to x-direction exterior wall)
model_params['extEcc_z'] = extEcc_z					# float, [0.0, 0.5], no units (offset from CoD to z-direction exterior wall)
model_params['intEcc_x'] = intEcc_x					# float, [0.0, 0.5], no units (offset from CoD to x-direction exterior wall)
model_params['intEcc_z'] = intEcc_z				# float, [0.0, 0.5], no units (offset from CoD to z-direction exterior wall)
model_params['floor1Weight'] = floor1Weight		# float, [10.0, 15.5] psf
model_params['floor2Weight'] = floor2Weight	# float, psf (range dependent on construction era)
model_params['roofWeight']	 = roofWeight			# float, psf (range dependent on construction era)

#print(model_params)
build_model(model_params)


with open('RSN30.json') as f:
  GMs = json.load(f)

TS_List = [GMs["data_x"],  GMs["data_y"]]
with open('Xdir.dat', 'w') as filehandle:
    for listitem in GMs["data_x"]:
        filehandle.write('%s\n' % listitem)

with open('Ydir.dat', 'w') as filehandle:
    for listitem in GMs["data_y"]:
        filehandle.write('%s\n' % listitem)

with open('EDP_specs.json') as f:
  EDP_specs = json.load(f)

G = 386.0 * 5
ops.timeSeries('Path', 2, '-dt', GMs["dT"], '-filePath', 'Xdir.dat', '-factor', G) # define acceleration vector from file (dt=0.005 is associated with the input file gm)
#ops.timeSeries('Path', 3, '-dt', GMs["dT"], '-filePath', 'Ydir.dat', '-factor', G) # define acceleration vector from file (dt=0.005 is associated with the input file gm)
ops.pattern('UniformExcitation', 2, 1, '-accel', 2)              # define where and how (pattern tag, dof) acceleration is applied
#ops.pattern('UniformExcitation', 20, 2, '-accel', 3)     

d1 = run_analysis(GM_dt=GMs["dT"], GM_npts=len(GMs["data_x"]), TS_List=TS_List,  EDP_specs=EDP_specs)
'''
d1_str = str(d1_tmp).replace("\'", "\"")
print(d1_str)
d1     = json.loads(d1_str)

with open('results_tmp.out', 'w') as f:
    f.write('{}'.format(d1))
    #for k,v in d1.items():
    #  if is_int32(k):
    #    k = int(k)
    #  if is_int32(v):
    #    v = int(v)
d2 ={}
with open('results_tmp.out', 'r') as f:
    #f.read(d2)
     d2 = json.load(f)

with open('results.out', 'w') as f:
    json.dump(d1["PID"], f, sort_keys=True, indent=4)
'''
with open('results.out', 'w') as f:
    f.write('{} {}'.format(d1["PID"][11][0], d1["PFA"][1][0]))


