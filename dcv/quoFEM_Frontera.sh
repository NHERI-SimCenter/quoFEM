#/bin/bash
module load gcc
module load python3/3.9.2
source /home1/00477/tg457427/python/python39-r2d/bin/activate
export SIMCENTER_PYTHON=/home1/00477/tg457427/python/python39-r2d/bin/python3
export SIMCENTER_DAKOTA=/work2/00477/tg457427/frontera/dakota/dakota-6.14.0/bin/runDakota.sh
export SIMCENTER_Backend=/work2/00477/tg457427/frontera/SimCenterBackendApplications/v4.2.0
export PATH=/home1/00477/tg457427/bin:$PATH
export PATH=/work2/00477/tg457427/frontera/dakota/dakota-6.14.0/bin:$PATH
#export LD_LIBRARY_PATH=/work2/00477/tg457427/frontera/dakota/dakota-6.14.0/bin:$LD_LIBRARY_PATH
#export LD_LIBRARY_PATH=/work2/00477/tg457427/frontera/dakota/dakota-6.14.0/lib:$LD_LIBRARY_PATH 
export LD_LIBRARY_PATH=/work2/00477/tg457427/frontera/pcre2/lib:$LD_LIBRARY_PATH 
/work2/00477/tg457427/frontera/quoFEM/build2/quoFEM
