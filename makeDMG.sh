# remove & rebuild app and macdeploy it

DMG_METHOD="NEW"

release=${1:-"NO_RELEASE"}

#
#PARAMETERS
#

APP_NAME="quoFEM"
APP_FILE=$APP_NAME".app"
DMG_FILENAME=$APP_NAME"_Mac_Download.dmg"
pathApp=`pwd`/build/$APP_FILE

pathToBackendApps="/Users/fmckenna/NHERI/SimCenterBackendApplications"
pathToOpenSees="/Users/fmckenna/bin/OpenSees3.6.0"
pathToDakota="/Users/fmckenna/dakota/dakota-6.16.0"
QTDIR="/Users/fmckenna/Qt/5.15.2/clang_64/"


mkdir -p build
cd build
rm -fr ${APP_FILE} ${DMG_FILENAME}

#
# build UI
#

conan install .. --build missin
status=$?; if [[ $status != 0 ]]; then echo "conan install failed"; exit $status; fi

if [ -n "$release" ] && [ "$release" = "release" ]; then
    echo "******** RELEASE BUILD *************"    
    qmake QMAKE_CXXFLAGS+="-D_SC_RELEASE" ../qFEM.pro
    status=$?; if [[ $status != 0 ]]; then echo "qmake failed"; exit $status; fi    
else
    echo "********* NON RELEASE BUILD ********"
    qmake ../quoFEM.pro
    status=$?; if [[ $status != 0 ]]; then echo "qmake failed"; exit $status; fi    
fi



#
# make
#

touch ../WorkflowApp_quoFEM.cpp
make -j 4
status=$?; if [[ $status != 0 ]]; then echo "make failed"; exit $status; fi

#
# Check to see if the app built
#

if ! [ -x "$(command -v open $pathApp)" ]; then
	echo "$APP_FILE did not build. Exiting."
	exit 
fi

#
# macdeployqt it
#

macdeployqt ./${APP_FILE}

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

mkdir  ./$APP_FILE/Contents/MacOS/applications/opensees
mkdir  ./$APP_FILE/Contents/MacOS/applications/dakota
cp -fr $pathToOpenSees/* ./$APP_FILE/Contents/MacOS/applications/opensees
cp -fr $pathToDakota/*  ./$APP_FILE/Contents/MacOS/applications/dakota

#
# Copy Example files
#

cp -fr ../Examples ./quoFEM.app/Contents/MacOS/
rm -fr ./quoFEM.app/Contents/MacOS/Examples/.archive
rm -fr ./quoFEM.app/Contents/MacOS/Examples/.aurore
rm -fr ./quoFEM.app/Contents/MacOS/Examples/.gitignore

#
# clean up:
#   1/ remove any __pyache__ files
#

find ./$APP_FILE -name __pycache__ -exec rm -rf {} +;

#
# load my credential file
#

userID="../userID.sh"

if [ ! -f "$userID" ]; then

    echo "creating dmg $DMG_FILENAME"
    hdiutil create $DMG_FILENAME -fs HFS+ -srcfolder ./$APP_FILE -format UDZO -volname $APP_NAME
    echo "No password & credential file to continue with codesig and App store verification"
    exit
fi

source $userID


#
# create dmg
#

if [ "${DMG_METHOD}" == "NEW" ]; then
    
    #
    # mv app into empty folder for create-dmg to work
    # brew install create-dmg
    #

    echo "codesign --deep --force --verbose --options=runtime  --sign "$appleCredential" $APP_FILE"
    codesign --deep --force --verbose --options=runtime  --sign "$appleCredential" $APP_FILE    
    
    mkdir app
    mv $APP_FILE app
    appDir=$PWD/app
    echo "appDir: $appDir"    
    
    # swoop
    #create-dmg \
	#  --volname "${APP_NAME}" \
	#  --background "../background/background1.png" \
	#  --window-pos 200 120 \
	#  --window-size 550 400 \
	#  --icon-size 150 \
	#  --icon "${APP_NAME}.app" 150 190 \
	#  --hide-extension "${APP_NAME}.app" \
	#  --app-drop-link 400 185 \
	#  "${DMG_FILENAME}" \
	#  "app"
    
    # vertical 
    #create-dmg \
	#  --volname "${APP_NAME}" \
	#  --background "../background/background2.png" \
	#  --window-pos 200 120 \
	#  --window-size 475 550 \
	#  --icon-size 150 \
	#  --icon "${APP_NAME}.app" 235 125 \
	#  --hide-extension "${APP_NAME}.app" \
	#  --app-drop-link 235 400 \
	#  "${DMG_FILENAME}" \
	#  "app"
    
    #horizontal
    ../macInstall/create-dmg \
	--volname "${APP_NAME}" \
	--background "../macInstall/background3.png" \
	--window-pos 200 120 \
	--window-size 600 350 \
	--no-internet-enable \
	--icon-size 125 \
	--icon "$quoFEM.app" 125 130 \
	--hide-extension "${APP_NAME}.app" \
	--app-drop-link 450 130 \
	--codesign $appleCredential \
	"quoFEM_Mac_Download.dmg" \
	"$appDir"
    
    #  --notarize $appleID $appleAppPassword \
	
    mv ./app/$APP_FILE ./
    rm -fr app

    echo "hdiutil create $DMG_FILENAME -fs HFS+ -srcfolder ./$APP_FILE -format UDZO -volname $APP_NAME"    
else

    echo "codesign --deep --force --verbose --options=runtime  --sign "$appleCredential" $APP_FILE"
    codesign --deep --force --verbose --options=runtime  --sign "$appleCredential" $APP_FILE
        
    echo "hdiutil create $DMG_FILENAME -fs HFS+ -srcfolder ./$APP_FILE -format UDZO -volname $APP_NAME"
    hdiutil create $DMG_FILENAME -fs HFS+ -srcfolder ./$APP_FILE -format UDZO -volname $APP_NAME

    echo "Issue: codesign --force --sign "$appleCredential" $DMG_FILENAME"
    #codesign --force --sign "$appleCredential" $DMG_FILENAME
    
fi

echo "Issue the following: " 
echo "xcrun altool --notarize-app -u $appleID -p $appleAppPassword -f ./$DMG_FILENAME --primary-bundle-id altool"
echo ""
echo "returns id: ID .. wait for email indicating success"
echo "To check status"
echo "xcrun altool --notarization-info ID  -u $appleID  -p $appleAppPassword"
echo ""
echo "Finally staple the dmg"
echo "xcrun stapler staple \"$APP_NAME\" $DMG_FILENAME"
