#!/bin/bash
#
# make build dir, remove old app
#

mkdir -p build
cd build
rm -fr quoFEM.app quoFEM

#
# build UI
#

conan install .. --build missing
status=$?; if [[ $status != 0 ]]; then echo "conan install failed"; exit $status; fi

qmake ../quoFEM.pro
status=$?; if [[ $status != 0 ]]; then echo "qmake failed"; exit $status; fi
make
status=$?; if [[ $status != 0 ]]; then echo "make failed"; exit $status; fi

#
# Copy applications from SimCentreBackend
#

mkdir ./quoFEM.app/Contents/MacOS/applications
cp -fr ../../SimCenterBackendApplications/applications/performUQ ./quoFEM.app/Contents/MacOS/applications
cp -fr ../../SimCenterBackendApplications/applications/performFEM ./quoFEM.app/Contents/MacOS/applications
cp -fr ../../SimCenterBackendApplications/applications/Workflow ./quoFEM.app/Contents/MacOS/applications

cp /usr/local/opt/libomp/lib/libomp.dylib ./quoFEM.app/Contents/MacOS/applications/performUQ/SimCenterUQ
install_name_tool -change /usr/local/opt/libomp/lib/libomp.dylib @executable_path/libomp.dylib ./quoFEM.app/Contents/MacOS/applications/performUQ/SimCenterUQ/nataf_gsa


#
# Copy OpenSees and Dakota
#

mkdir  ./quoFEM.app/Contents/MacOS/applications/opensees
mkdir  ./quoFEM.app/Contents/MacOS/applications/dakota
cp -fr /Users/fmckenna/bin/OpenSees3.2.2/* ./quoFEM.app/Contents/MacOS/applications/opensees
cp -fr /Users/fmckenna/dakota-6.12.0/* ./quoFEM.app/Contents/MacOS/applications/dakota

#
# Copy Example files
#

cp -fr ../Examples ./quoFEM.app/Contents/MacOS/
rm -fr ./quoFEM.app/Contents/MacOS/Examples/.archive
rm -fr ./quoFEM.app/Contents/MacOS/Examples/.aurore
rm -fr ./quoFEM.app/Contents/MacOS/Examples/.gitignore

#
# cd back to were we started
#

cd ..





