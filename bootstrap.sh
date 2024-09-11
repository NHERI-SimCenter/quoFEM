#!/bin/bash

# Set Timezone (needed for python install)
ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Updates the package lists for upgrades and new package installations
apt-get update
apt-get install sudo -y
apt-get install wget -y
sudo apt install -y software-properties-common cmake git

# Installs the  python 3.9 package. The '-y' flag automatically answers yes to prompts.
sudo add-apt-repository ppa:deadsnakes/ppa -y
sudo apt update 
sudo apt install python3.9 -y
sudo apt install python3.9-distutils -y

# Install pip
wget https://bootstrap.pypa.io/get-pip.py
python3.9 get-pip.py

# Upgrades pip (Python package installer) to the latest version
sudo python3.9 -m pip install -U pip

# Installs the Conan package manager
python3.9 -m pip install conan==1.60.1
python3.9 -m pip install nheri-simcenter

python3.9 --version

which python3.9

which conan

wget https://github.com/snl-dakota/dakota/releases/download/v6.15.0/dakota-6.15.0-public-src-cli.tar.gz
sudo apt-get install -y libboost-dev libboost-all-dev libopenmpi-dev openmpi-bin xorg-dev libmotif-dev 
tar zxBf dakota-6.15.0-public-src-cli.tar.gz 
mv dakota-6.15.0-public-src-cli dakota-6.15.0 
cd dakota-6.15.0 
mkdir build; cd build 
cmake .. 
cmake --build . --config Release 
cmake --install . 
cd ../..; rm -fr dakot*   

sudo apt-get install -y cmake liblapack-dev libomp-dev libssl-dev apt-transport-https ca-certificates wget         
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null 
sudo apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" 
sudo apt-get update 
sudo apt-get install -y cmake gfortran gcc g++ 
git clone -b $versionOpenSees --single-branch https://github.com/OpenSees/OpenSees.git 
cd OpenSees 
mkdir build; cd build 
conan install .. --build missing 
cmake .. 
cmake --build . --config Release 
cmake --install . 
sudo mv ./lib/* /usr/local/lib 
cd ../..; rm -fr OpenSees
