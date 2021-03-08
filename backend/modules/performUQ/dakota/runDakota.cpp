
#incluude <iostream>
#include <filesystem>
#include <string>
namespace fs = std::filesystem;

int main(int argc, char **argv) {

  if (argc < 4) {
    std::cerr << "runDakota - not enough args: runDakota workDir workdirTmp runType\n";
    exit(-1);
  }

  std::string path = fs::currentPath();
  
  char *workdir_main = argv[1];
  char *workdir_temp = argv[2];
  char *run_type = argv[3];

  std::string OpenSees;
  std::string Feap;
  std::string Dakota;
  std::string workflow_driver;
  
  if (strcmp(runType,"runningLocal") == 0) {
#ifdef _WIN32
    OpenSees = "OpenSees";
    Feap = "Feappv41.exe";
    Dakota = "dakota";
    workflow_driver = "workflow_driver.bat";
#else
    OpenSees = "OpenSees";
    Feap = "feappv";
    Dakota = "dakota";
    workflow_driver = "workflow_driver";
#endif

  } else if (strcmp(run_type,"runningRemote") == 0) {
    OpenSees = "/home1/00477/tg457427/bin/OpenSees";
    Feap = "/home1/00477/tg457427/bin/feappv";
    Dakota = "dakota";
    workflow_driver = "workflow_driver";
  }

  
# change workdir to the templatedir
os.chdir(workdir_temp)
cwd = os.getcwd()

# open the dakota json file
with open('dakota.json') as data_file:    
    data = json.load(data_file)

uq_data = data["UQ_Method"]
fem_data = data["fem"]
rnd_data = data["randomVariables"]
my_edps = data["EDP"]

myScriptDir = os.path.dirname(os.path.realpath(__file__))
inputFile = "dakota.json"

osType = platform.system()
preprocessorCommand = '"{}/preprocessDakota" {} {} {} {}'.format(myScriptDir, inputFile, workflow_driver, run_type, osType)
subprocess.Popen(preprocessorCommand, shell=True).wait()
print("DONE RUNNING PREPROCESSOR\n")


# edps = samplingData["edps"]
numResponses = 0
responseDescriptors=[]

for edp in my_edps:
    responseDescriptors.append(edp["name"])
    numResponses += 1

femProgram = fem_data["program"]
print(femProgram)


if run_type in ['runningLocal']:
    os.chmod(workflow_driver, stat.S_IXUSR | stat.S_IRUSR | stat.S_IXOTH)

command = Dakota + ' -input dakota.in -output dakota.out -error dakota.err'

#Change permision of workflow driver
st = os.stat(workflow_driver)
os.chmod(workflow_driver, st.st_mode | stat.S_IEXEC)

# copy the dakota input file to the main working dir for the structure
shutil.move("dakota.in", "../")

# change dir to the main working dir for the structure
os.chdir("../")

cwd = os.getcwd()
print(cwd)

if run_type in ['runningLocal']:

#    p = Popen(command, stdout=PIPE, stderr=PIPE, shell=True)
#    for line in p.stdout:
#        print(str(line))

    dakotaCommand = "dakota -input dakota.in -output dakota.out -error dakota.err"
    print('running Dakota: ', dakotaCommand)
    try:
        result = subprocess.check_output(dakotaCommand, stderr=subprocess.STDOUT, shell=True)
        returncode = 0
    except subprocess.CalledProcessError as e:
        result = e.output
        returncode = e.returncode



