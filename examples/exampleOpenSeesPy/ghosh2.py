
import math

from parameters import *  

D = math.sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1))

f = open("results.out", "w")
f.write(str(D))
f.close()

