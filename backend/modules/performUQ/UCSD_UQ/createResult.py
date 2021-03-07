"""
@author: Mukesh, Maitreya, and Conte 

"""

import pickle
import csv
#from mainscript import resultsLocation

filename = "/results.csv" 

with open('mytrace.pickle', 'rb') as handle1:
    mytrace = pickle.load(handle1)

dataToWrite = mytrace[-1][0]

#with open(resultsLocation + filename, 'w', newline='') as csvfile:
with open(filename, 'w', newline='') as csvfile:  
    
    csvwriter = csv.writer(csvfile)      
    csvwriter.writerows(dataToWrite)
    
