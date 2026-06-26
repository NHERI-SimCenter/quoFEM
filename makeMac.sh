#!/bin/bash
set -euo pipefail

# Usage: makeMac.sh [--arch <x86_64 or arm64>] [--release] [--wipe]
# Defaults: arch = "", NO release

PROJECT_NAME="quoFEM"

CONAN_PROFILE="default"
BUILD_DIR="build"
ARCH=""
RELEASE="NO"
RELEASE_FLAG="-DNO_RELEASE"
APPLICATIONS_DIR=$(pwd)/../SimCenterBackendApplications/applications
WIPE="FALSE"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --arch|-a)
            ARCH="$2"
            if [[ "$ARCH" != "x86_64" && "$ARCH" != "arm64" ]]; then
                echo "Error: Invalid architecture '$ARCH'."
                echo "Supported architectures: x86_64, arm64"
                exit 1
            fi

            CONAN_PROFILE="macos-$ARCH"
            BUILD_DIR="build_$ARCH"
            APPLICATIONS_DIR="${APPLICATIONS_DIR}_$ARCH"

            if [[ "$ARCH" == "x86_64" ]]; then
		pathToOpenSees="${HOME}/bin/OpenSeesLatest_x86_64"
		pathToDakota="${HOME}/dakota/dakota-6.24.0-x86"
	    else
		pathToOpenSees="${HOME}/bin/OpenSeesLatest_arm64"
		pathToDakota="${HOME}/dakota/dakota-6.24.0"
            fi	    
            
            shift 2
            ;;
	--release|-r)
	    RELEASE="YES"
	    RELEASE_FLAG="-D_SC_RELEASE"
	    shift 1
	    ;;
	--wipe|-w)
	    WIPE="TRUE"
	    shift 1
	    ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 --arch <arm64|x86_64>"
            exit 1
            ;;
    esac
done

echo "Architecture: ${ARCH}"
echo "Conan profile : ${CONAN_PROFILE}"
echo "Build directory: ${BUILD_DIR}"

# Define some helpers to print messages
die() {
    echo ""    
    echo "$1"
    echo ""    
    exit 1
}

msg() {
    echo ""
    echo "$1"
    echo ""
}

#
# Check for Conan .. 
#

if ! command -v conan &> /dev/null; then
    die "'FAIL: conan' command not found. Please install it first."
fi

#
# Remove old build dir
#

if [[ "$WIPE" == "TRUE" ]]; then
    if [[ -n "$BUILD_DIR" && "$BUILD_DIR" != "$HOME" ]]; then
	echo "Cleaning up old build directory..."
	rm -rf "${BUILD_DIR}"
    fi
fi

#
# Conan build dependencies
#

msg "Running conan install..."
# copy conanfile2 to conanfile till i remove old build
if [ -f "conanfile2.py" ]; then
    mv conanfile.py conanfile.old
    cp conanfile2.py conanfile.py
fi

conan install . --output-folder="${BUILD_DIR}" --build missing -s build_type=Release -pr "${CONAN_PROFILE}" || die "FAIL : Conan install failed."

if [ -f "conanfile2.py" ]; then
    mv conanfile.old conanfile.py
fi


#
# Configure CMake
#

CMAKE_ARCH_FLAG=""
if [ -n "${ARCH}" ]; then
    CMAKE_ARCH_FLAG="-DCMAKE_OSX_ARCHITECTURES=${ARCH}"
fi

msg "Configuring CMake..."
set -x

cmake -B "${BUILD_DIR}" -S . \
    -DCMAKE_TOOLCHAIN_FILE="${BUILD_DIR}/conan_toolchain.cmake" ${CMAKE_ARCH_FLAG} \
    -DCMAKE_CXX_FLAGS=${RELEASE_FLAG} -DCMAKE_BUILD_TYPE=Release || die "FAIL: CMake configure failed."
 
set +x
msg "Building with 8 cores..."

#
# in case no wipe, removing app and touchingh WorkflowApp and main
#

touch WorkflowApp_quoFEM.cpp main.cpp
rm -fr "${BUILD_DIR}/${PROJECT_NAME}.app"

#
# Now build with CMake
#

cmake --build "${BUILD_DIR}" --parallel 8 || die "FAIL: CMake build failed."

APP_NAME="${PROJECT_NAME}.app"
APP_DIR="${BUILD_DIR}/${APP_NAME}"

#
# Copy Examples folder
#

cp -fr Examples "${APP_DIR}/Contents/MacOS/"
rm -fr "${APP_DIR}/Contents/MacOS/Examples/.archive"
rm -fr "${APP_DIR}/Contents/MacOS/Examples/.aurore"
rm -fr "${APP_DIR}/Contents/MacOS/Examples/.gitignore"
find "${APP_DIR}/Contents/MacOS/Examples" -type d -name "figures" -prune -exec rm -rf {} +
find "${APP_DIR}/Contents/MacOS/Examples" -type f -name "*.rst" -delete

if [[ "$RELEASE" != "YES" ]]; then

    # Copy backend, don't bother cleanup unwanted as if arch specifdied, cannot set backend to it
    mkdir  -p "${APP_DIR}/Contents/MacOS/applications"
    cp -fr "${APPLICATIONS_DIR}"/* "$APP_DIR/Contents/MacOS/applications"        
    
    die "Build complete!"
fi

# ===============================
# Release Build
# ===============================

echo "Now Doing Stuff for a Release"

#
# macdeploy the application
#

macdeployqt ${APP_DIR}

#
# Copy needed files (backend, opensees,dakota)
#

msg "copying Backend, OpenSees and Dakota to bundle"

mkdir  "${APP_DIR}/Contents/MacOS/applications"
mkdir  "${APP_DIR}/Contents/MacOS/applications/opensees"
mkdir  "${APP_DIR}/Contents/MacOS/applications/dakota"

cp -fr "${APPLICATIONS_DIR}/performUQ"  "${APP_DIR}/Contents/MacOS/applications" 
cp -fr "${APPLICATIONS_DIR}/performFEM" "${APP_DIR}/Contents/MacOS/applications" 
cp -fr "${APPLICATIONS_DIR}/Workflow"   "${APP_DIR}/Contents/MacOS/applications" 

cp -fr "${pathToOpenSees}"/* "./${APP_DIR}/Contents/MacOS/applications/opensees"
cp -fr "${pathToDakota}"/*   "./${APP_DIR}/Contents/MacOS/applications/dakota"


#
# clean up any __pyacache_ files
#

find $APP_DIR -name __pycache__ -exec rm -rf {} +;

userID="userID.sh"

if [ ! -f "$userID" ]; then
    echo "No password & credential file to continue with codesig and App store verification"
    exit 1
else
    source userID.sh    
fi


#
# now codesign
#

msg "codesigning with: codesign --force --deep --verbose --timestamp --options=runtime --sign "$appleCredential" "$APP_DIR""
codesign --force --deep --verbose --timestamp --options=runtime --sign "$appleCredential" "$APP_DIR" || die "FAIL: codesign failed."

#
# Sign QtWebEngineProcess with its bundled entitlements as --deep on APP overwrites this, making plots appear empty in notarized builds
#

WEBENGINE_PROCESS="${APP_DIR}/Contents/Frameworks/QtWebEngineCore.framework/Versions/A/Helpers/QtWebEngineProcess.app"
msg "codesigning ${WEBENGINE_PROCESS}"
WEBENGINE_ENTITLEMENTS="${WEBENGINE_PROCESS}/Contents/Resources/QtWebEngineProcess.entitlements"
if [ -f "${WEBENGINE_ENTITLEMENTS}" ]; then
    codesign --force --timestamp --options=runtime \
             --entitlements "${WEBENGINE_ENTITLEMENTS}" \
             --sign "$appleCredential" "${WEBENGINE_PROCESS}" || die "FAIL: QtWebEngineProcess codesign failed."
fi


#
# build dmg bundle
#

DMG_METHOD="NEW"
if [ "${DMG_METHOD}" = "NEW" ]; then

    cd ${BUILD_DIR}
    mkdir app
    mv $APP_NAME app
    appDir=$PWD/app
    set -x    
    
    ../macInstall/create-dmg \
	--volname "${PROJECT_NAME}" \
	--background "../macInstall/background3.png" \
	--window-pos 200 120 \
	--window-size 600 350 \
	--no-internet-enable \
	--icon-size 125 \
	--icon "${APP_NAME}" 125 130 \
	--hide-extension "${APP_NAME}" \
	--app-drop-link 450 130 \
	--codesign $appleCredential \
	"quoFEM_Mac_Download_${ARCH}.dmg" \
	"app"

    set +x
    mv ./app/${APP_NAME} ./
    rm -fr app

else

    echo "hdiutil create quoFEM_Mac_Download_${ARCH}.dmg -fs HFS+ -srcfolder ./$APP_FILE -format UDZO -volname $APP_NAME"
    hdiutil create quoFEM_Mac_Download_${ARCH}.dmg -fs HFS+ -srcfolder ./$APP_NAME -format UDZO -volname $APP_NAME
    
fi


#
# now xcrun stuff to get it signed and stapled
#

msg "DMG built .. now uploading to Apple for validation"

xcrun notarytool submit ./quoFEM_Mac_Download_${ARCH}.dmg  --apple-id $appleID --password $appleAppPassword --team-id $appleCredential

msg "Manual Mode! ... "
echo ""
echo "last command returns id: ID,  wait awhile for apple to verify then check status & log with: "
echo "   .. info for status and if succesull staple, otherwise look at log file and correct!"
echo "xcrun notarytool info ID  --apple-id $appleID --password $appleAppPassword --team-id $appleCredential"
echo "xcrun notarytool log ID --apple-id $appleID --password $appleAppPassword --team-id $appleCredential"
echo ""
echo "Finally staple the dmg"
echo "xcrun stapler staple  ${BUILD_DIR}/quoFEM_Mac_Download_${ARCH}.dmg"


echo "Release Build complete!"
