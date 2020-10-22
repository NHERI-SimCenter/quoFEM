
import math

from parameters import *  

x1 = "RV.x1"
y1 = "RV.y1"
x2 = "RV.x2"
y2 = "RV.y2"

D = math.sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1))

f = open("results.out", "w")
f.write(str(D))
f.close()

