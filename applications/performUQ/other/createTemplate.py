from pathlib import Path

def createTemplate(variableNames, templateName):
    filePath = Path("./" + templateName)

    with open(filePath, 'w') as f:
        f.write('{}\n'.format(len(variableNames)))

        for name in variableNames:
            f.write('{} <{}>\n'.format(name, name))
        
        f.close()    
