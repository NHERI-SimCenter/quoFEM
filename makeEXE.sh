#!/bin/bash

#
# build backend
#

cd backend
mkdir -p build
cd build
conan install .. --build missing
cmake -DCMAKE_BUILD_TYPE=Release ..
status=$?; if [[ $status != 0 ]]; then echo "cmake failed"; exit $status; fi
make
status=$?; if [[ $status != 0 ]]; then echo "make backend failed"; exit $status; fi
make install
status=$?; if [[ $status != 0 ]]; then echo "install backend failed"; exit $status; fi

#
# build UI
#

cd ..
cd ..
mkdir -p build
cd build
conan install .. --build missing
status=$?; if [[ $status != 0 ]]; then echo "conan install failed"; exit $status; fi
qmake ../quoFEM.pro
status=$?; if [[ $status != 0 ]]; then echo "qmake failed"; exit $status; fi
make
status=$?; if [[ $status != 0 ]]; then echo "make failed"; exit $status; fi

#
# cd back to were we started
#
cd ..



