#!/bin/bash          
#
# usage: wrapper dirName mainScript.tcl 
#   dirName: directory containing the mainScript
#   mainScript: the main input file to run
#
# written: fmk

platform='unknown'

platform=$(uname)

if [[ $platform == 'Darwin' ]]; then
    export DAKOTA_PATH=$HOME/dakota/bin
    export OPENSEES_PATH=$HOME/bin
    export PATH=$PATH:$OPENSEES_PATH:$DAKOTA_PATH    
    source $HOME/.profile
elif [[ $platform == 'Linux' ]]; then
    export DAKOTA_PATH=$HOME/dakota/dakota-6.5/bin
    export LD_LIBRARY_PATH=$HOME/dakota/dakota-6.5/lib
    export OPENSEES_PATH=$HOME/bin
    export PATH=$PATH:$OPENSEES_PATH:$DAKOTA_PATH
    source $HOME/.bashrc
else
    echo "PLATFORM NOT RECOGNIZED"
fi


#
# input parameters
#

echo $@

dirNAME=$1
scriptNAME=$2
#filenameDAKOTA=$3

if [ "$#" -ne 2 ] || ! [ -d "$1" ]; then
  echo "Usage: $0 dirName scriptName filenameDakota" >&2
  exit 1
fi

#
# cd to directory
# make a tmp directory and copy all files & subdir to it
# 
mkdir $dirNAME/tmp.SimCenter
mkdir $dirNAME/tmp.SimCenter/templatedir
cp /Users/simcenter/NHERI/DakotaFEM2/localApp/parseJson.py $dirNAME/tmp.SimCenter/templatedir
cd $dirNAME
#mkdir tmp.SimCenter
#mkdir tmp.SimCenter/templatedir
rsync -av --exclude ./tmp.SimCenter ./ ./tmp.SimCenter/templatedir 
cp $dirName/dakota.json ./tmp.SimCenter/templatedir/dakota.json
cd ./tmp.SimCenter/templatedir
python parseJson.py $filenameDAKOTA
mv dakota.in ../
chmod 'u+x' fem_driver
mv fem_driver ../
cd ..

#
# execute dakota
#

dakota -input dakota.in -output dakota.out -error dakota.err

#
# copy dakota.out & remove stufff up to word Kurtosis
#

#cp dakota.out dakota.tmp

#if [[ $platform == 'Darwin' ]]; then
#    sed -i '' '1,/Kurtosis/d' dakota.tmp
#else
#    sed -i '1,/Kurtosis/d' dakota.tmp
#fi

#
# copy results to main directory
#

#cp dakota.tmp ../dakota.out
cp dakota.out ..
cp dakotaTab.out ..
cd ..

#
# remove tmp directory
#

rm -fr tmp.SimCenter
