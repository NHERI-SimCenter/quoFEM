#!/bin/bash

# build backend
cd backend
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
make install

# build UI
cd ..
cd ..
mkdir -p build
cd build
conan install .. --build missing
qmake ../quoFEM.pro
make

# back to where we started
cd ..


