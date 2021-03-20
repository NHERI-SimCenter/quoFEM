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

FILE=quoFEM.exe
if [ "$(uname)" == "Darwin" ]; then
   FILE=quoFEM.app
fi

if [ -e "$FILE" ]; then
    echo "build succeeded $FILE"
else 
    echo "failed to build: $FILE."
    cd ..    
    exit 1
fi

cd ..



