import numpy as np
import openseespy.opensees as ops

from modelParams import fy, E, b, cR1, cR2, a1, a3

def evaluateModel(strain_input):
    R0 = 20.0

    ops.wipe()
    materialTag = 0

    ops.uniaxialMaterial('Steel02', materialTag, fy, E, b, R0, cR1, cR2, a1, 1.0, a3, 1.0, 0.0)

    ops.testUniaxialMaterial(materialTag)

    stress = []

    for eps in strain_input:
        ops.setStrain(eps)
        stress.append(ops.getStress())

    return np.asarray(stress)