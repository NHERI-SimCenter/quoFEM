#!/bin/bash

# Updates the package lists for upgrades and new package installations
sudo apt-get update

# Installs the 'sudo' package. The '-y' flag automatically answers yes to prompts.
apt-get install -y sudo python3.9

# Install the needed packages
sudo apt install -y software-properties-common cmake python3-pip git

# Upgrades pip (Python package installer) to the latest version
sudo python3 -m pip install -U pip

# Installs the Conan package manager
pip3 install conan==1.60.1
pip3 install nheri-simcenter

python3 --version

which python3

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
