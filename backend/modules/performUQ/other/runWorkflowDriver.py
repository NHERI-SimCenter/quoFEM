import os
import fire
from sys import platform
import shutil

def runWorkflow(index):
    index = int(index)

    shutil.copy(os.path.join(os.getcwd(), 'InputFiles', 'params_' + str(index) + '.template'),
                os.path.join(os.getcwd(), 'params.in'))    

    command2 = "blank"
    if platform == "linux" or platform == "linux2" or platform == "darwin":
        command2 = os.path.join(os.getcwd(), "workflow_driver")
    elif platform == "win32":
        command2 = os.path.join(os.getcwd(), "workflow_driver.bat")

    # os.system(command1)
    os.system(command2)    

if __name__ == '__main__':
    fire.Fire(runWorkflow)
