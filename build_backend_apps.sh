#!/bin/bash

git clone --branch master --depth 1 https://github.com/NHERI-SimCenter/SimCenterBackendApplications.git

cp ./SimCenterBackendApplications/modules/performUQ/SimCenterUQ/nataf_gsa/CMakeLists.txt.UBUNTU ./SimCenterBackendApplications/modules/performUQ/SimCenterUQ/nataf_gsa/CMakeLists.txt

rm -fr ~/.conan

sudo apt-get install -y liblapack-dev libomp-dev libssl-dev apt-transport-https ca-certificates \
 
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test

sudo apt-get update

sudo apt-get install -y gcc-10 g++-10 gfortran-10

export CC=gcc-10

export CXX=g++-10

export FC=gfortran-10

conan remote add simcenter https://nherisimcenter.jfrog.io/artifactory/api/conan/simcenter

cd SimCenterBackendApplications

mkdir build

cd build

conan install .. --build missing

cmake ..

cmake --build . --config Release

cmake --install .

cd ../..
