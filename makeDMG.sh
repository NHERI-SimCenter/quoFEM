#!/bin/bash

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

macdeployqt ./quoFEM.app


cp /usr/local/opt/libomp/lib/libomp.dylib ./quoFEM.app/Contents/MacOS/applications/performUQ/SimCenterUQ
install_name_tool -change /usr/local/opt/libomp/lib/libomp.dylib @executable_path/libomp.dylib ./quoFEM.app/Contents/MacOS/applications/performUQ/SimCenterUQ/nataf_gsa

#
# now before we codesign and verify, check userID file exists
#

userID="../userID.sh"

if [ ! -f "$userID" ]; then
    echo "No password & credential file to continue with codesig and App store verification"
    hdiutil create quoFEM_Mac_Download.dmg -fs HFS+ -srcfolder ./quoFEM.app -format UDZO -volname quoFEM    
    exit
fi

source $userID
echo $appleID

#codesign files in app dir
echo "xcrun altool --notarize-app -u $appleID -p $appleAppPassword -f ./$quoFEM_Mac_Download.dmg --primary-bundle-id altool"
codesign --deep --force --verbose --options=runtime  --sign "$appleCredential" quoFEM.app

# create dmg
rm -fr quoFEM_Mac_Download.dmg
echo "hdiutil create quoFEM_Mac_Download.dmg -fs HFS+ -srcfolder ./quoFEM.app -format UDZO -volname quoFEM"

hdiutil create quoFEM_Mac_Download.dmg -fs HFS+ -srcfolder ./quoFEM.app -format UDZO -volname quoFEM

if [[ $status != 0 ]]
then
    echo "FAILED to create DMG .. cd to build and issue the following:"
    echo "hdiutil create quoFEM_Mac_Download.dmg -fs HFS+ -srcfolder ./quoFEM.app -format UDZO -volname quoFEM"    
    echo "codesign --force --sign "$appleCredential" quoFEM_Mac_Download.dmg"
    echo "xcrun altool --notarize-app -u $appleID -p $appleAppPassword -f ./quoFEM_Mac_Download.dmg --primary-bundle-id altool"    
    echo "xcrun altool --notarization-info ID -u $appleID -p $applePassword"
    echo "xcrun stapler staple \"quoFEM\" quoFEM_Mac_Download.dmg"    
    exit $status;
fi

#codesign dmg
codesign --force --sign "$appleCredential" quoFEM_Mac_Download.dmg
echo "codesign --force --sign "$appleCredential" quoFEM_Mac_Download.dmg"
echo "xcrun altool --notarize-app -u $appleID -p $appleAppPassword -f ./quoFEM_Mac_Download.dmg --primary-bundle-id altool"
xcrun altool --notarize-app -u $appleID -p $appleAppPassword -f ./quoFEM_Mac_Download.dmg --primary-bundle-id altool

echo "Finally Done "
echo "returns id: ID"
echo "xcrun altool --notarization-info ID -u $appleID -p $applePassword"

echo "xcrun stapler staple \"quoFEM\" quoFEM_Mac_Download.dmg"
