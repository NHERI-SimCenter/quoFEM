import numpy as np
# from pathlib import Path
import os

class OutputProcessor:

    def __init__(self, index):
        # filePath = Path("./results.out")
        filePath = os.path.join(os.getcwd(), 'results.out')
        self.numberOfColumns = 0
        self.numberOfLines = 0
        response = []
        
        # Open again and read data
        with open(filePath) as f:           
            for line in f:
                currentLine = line.split()
                
                if len(currentLine) != 0:
                    response.append(currentLine)
        
        if len(response) != 0: 
            self.qoi = np.vstack(response)
        else:
            self.qoi = np.empty(shape=(0,0))            
