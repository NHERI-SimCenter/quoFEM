@echo off
setlocal enabledelayedexpansion

:: makeWin.bat -- Build PBE for Windows using MSVC + Ninja + Conan
::
:: Prerequisites (edit the CONFIG section below to match your machine):
::   - Visual Studio 2022 Community with C++ workload
::   - Qt 6 installed at QT path
::   - Conan 2 on PATH
::   - 7-Zip installed at SEVENZIP path
::   - For --release: OpenSees, Dakota, and a Python 3.12 embedded zip
::
:: Usage:
::   makeWin.bat              -- dev build only
::   makeWin.bat --release    -- full deployable bundle + zip
::   makeWin.bat --wipe       -- delete build dir before building
::   makeWin.bat -r -w        -- wipe build folder then release build

:: =====================================================================
:: CONFIG -- adjust these paths for your machine
:: =====================================================================
set "QT=C:\Qt6\6.10.2\msvc2022_64"
set "SEVENZIP=C:\Program Files\7-Zip\7z.exe"
set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
set "pathToOpenSees=%USERPROFILE%\bin\OpenSees3.8.0"
set "pathToDakota=%USERPROFILE%\dakota\dakota6.24"
set "pathToPython=%USERPROFILE%\python_releases\python3.12.zip"
set "APPLICATIONS_DIR=%CD%\..\SimCenterBackendApplications\applications"
:: =====================================================================

set "PROJECT_NAME=quoFEM"
set "CONAN_PROFILE=default"
set "BUILD_DIR=build"
set "PREFIX=%CD%\DEPS"
set "RELEASE=NO"
set "RELEASE_FLAG=-DNO_RELEASE"
set "WIPE=FALSE"

:: ---- Argument parsing ----

:parse_args
if "%~1"=="" goto :args_done
if /i "%~1"=="--release" goto :arg_release
if /i "%~1"=="-r"        goto :arg_release
if /i "%~1"=="--wipe"    goto :arg_wipe
if /i "%~1"=="-w"        goto :arg_wipe

echo Unknown argument: %~1
echo Usage: %~nx0 [--release^|-r] [--wipe^|-w]
exit /b 1

:arg_release
set "RELEASE=YES"
set "RELEASE_FLAG=-D_SC_RELEASE"
shift
goto :parse_args

:arg_wipe
set "WIPE=TRUE"
shift
goto :parse_args

:args_done

echo.
echo Project      : %PROJECT_NAME%
echo Conan profile: %CONAN_PROFILE%
echo Build dir    : %BUILD_DIR%
echo Release      : %RELEASE%
echo.

:: ---- Pre-flight checks ----

if not exist "%VCVARS%" (
    echo FAIL: vcvars64.bat not found at %VCVARS%
    exit /b 1
)
if not exist "%QT%\bin\qmake.exe" (
    echo FAIL: Qt not found at %QT%
    exit /b 1
)
if not exist "%SEVENZIP%" (
    echo FAIL: 7-Zip not found at %SEVENZIP%
    exit /b 1
)

where conan >nul 2>&1
if errorlevel 1 (
    echo FAIL: 'conan' not found on PATH. Please install Conan 2.
    exit /b 1
)

:: Release-only pre-flight checks
if "%RELEASE%"=="YES" (
    if not exist "%APPLICATIONS_DIR%" (
        echo FAIL: SimCenterBackendApplications not found at %APPLICATIONS_DIR%
        exit /b 1
    )
    if not exist "%pathToOpenSees%" (
        echo FAIL: OpenSees not found at %pathToOpenSees%
        exit /b 1
    )
    if not exist "%pathToDakota%" (
        echo FAIL: Dakota not found at %pathToDakota%
        exit /b 1
    )
    if not exist "%pathToPython%" (
        echo FAIL: Python zip not found at %pathToPython%
        exit /b 1
    )
)


:: ---- Initialize MSVC environment (required for Ninja) ----

call "%VCVARS%"

:: ---- Wipe build dir if requested ----

if "%WIPE%"=="TRUE" (
    if exist "%BUILD_DIR%" (
        echo Cleaning up old build directory...
        rmdir /s /q "%BUILD_DIR%"
    )
)

:: ---- Conan install ----

echo.
echo Running conan install...
echo.

set "CONAN_FILE_SWAPPED=NO"
if exist conanfile2.py (
    move conanfile.py conanfile.old >nul
    copy conanfile2.py conanfile.py >nul
    set "CONAN_FILE_SWAPPED=YES"
)

conan install . --build missing -s build_type=Release -pr "%CONAN_PROFILE%" -c tools.cmake.cmaketoolchain:generator=Ninja
if errorlevel 1 (
    if "%CONAN_FILE_SWAPPED%"=="YES" if exist conanfile.old move /y conanfile.old conanfile.py >nul
    echo.
    echo FAIL: Conan install failed.
    echo.
    exit /b 1
)

if "%CONAN_FILE_SWAPPED%"=="YES" if exist conanfile.old move /y conanfile.old conanfile.py >nul

:: ---- CMake configure ----

echo.
echo Configuring CMake...
echo.

:: Temporarily hide preset files — Conan-generated Ninja presets include
:: an architecture field that CMake rejects even with strategy:external.
:: The cmake call below passes everything explicitly so presets aren't needed.
if exist CMakePresets.json     ren CMakePresets.json     CMakePresets.json.bak
if exist CMakeUserPresets.json ren CMakeUserPresets.json CMakeUserPresets.json.bak

cmake -B "%BUILD_DIR%" -S . ^
    -G Ninja ^
    -DCMAKE_TOOLCHAIN_FILE="%BUILD_DIR%\Release\generators\conan_toolchain.cmake" ^
    -DCMAKE_PREFIX_PATH="%PREFIX%;%QT%" ^
    -DCMAKE_CXX_FLAGS="%RELEASE_FLAG%" ^
    -DCMAKE_DAKOTA_VERSION=624 ^
    -DCMAKE_BUILD_TYPE=Release

if exist CMakePresets.json.bak     ren CMakePresets.json.bak     CMakePresets.json
if exist CMakeUserPresets.json.bak ren CMakeUserPresets.json.bak CMakeUserPresets.json

if errorlevel 1 (
    echo.
    echo FAIL: CMake configure failed.
    echo.
    exit /b 1
)

:: Touch main entry points so the version string baked into them is always fresh
copy /b WorkflowAppquoFEM.cpp +,, >nul 2>&1
copy /b main.cpp +,, >nul 2>&1

if exist "%BUILD_DIR%\%PROJECT_NAME%.exe" del /f /q "%BUILD_DIR%\%PROJECT_NAME%.exe"

:: ---- Build ----

echo.
echo Building...
echo.

cmake --build "%BUILD_DIR%"
if errorlevel 1 (
    echo.
    echo FAIL: CMake build failed.
    echo.
    exit /b 1
)

:: ---- Set up deploy directory ----

set "APP_DIR=%BUILD_DIR%\%PROJECT_NAME%_Windows_Download"
if exist "%APP_DIR%" rmdir /s /q "%APP_DIR%"
mkdir "%APP_DIR%"

:: ---- Copy executable and run windeployqt ----

copy "%BUILD_DIR%\%PROJECT_NAME%.exe" "%APP_DIR%\" >nul
"%QT%\bin\windeployqt.exe" "%APP_DIR%\%PROJECT_NAME%.exe"
if errorlevel 1 (
    echo.
    echo FAIL: windeployqt failed.
    echo.
    exit /b 1
)

for %%f in ("%BUILD_DIR%\*.dll") do copy "%%f" "%APP_DIR%\" >nul
copy "%QT%\bin\Qt6Core5Compat.dll" "%APP_DIR%\" >nul

if "%RELEASE%"=="NO" (
    echo.
    echo Build complete: %APP_DIR%
    echo.
    exit /b 0
)


:: ===============================
:: Release Build
:: ===============================

echo.
echo Building release bundle...
echo.

:: ---- Copy QS3hark resources, backend applications, OpenSees, Dakota ----

xcopy /s /e /i /q "%USERPROFILE%\NHERI\QS3hark\resources" "%APP_DIR%\resources\" >nul

mkdir "%APP_DIR%\applications"
mkdir "%APP_DIR%\applications\performFEM"
mkdir "%APP_DIR%\applications\performUQ"
mkdir "%APP_DIR%\applications\Workflow"
mkdir "%APP_DIR%\applications\opensees"
mkdir "%APP_DIR%\applications\dakota"
mkdir "%APP_DIR%\applications\common"
mkdir "%APP_DIR%\Examples"

xcopy /s /e /Q    ".\Examples"                                "%APP_DIR%\Examples"
xcopy /s /e /i /q "%APPLICATIONS_DIR%\performFEM"  "%APP_DIR%\applications\performFEM\" >nul
xcopy /s /e /i /q "%APPLICATIONS_DIR%\performUQ"   "%APP_DIR%\applications\performUQ\" >nul
xcopy /s /e /i /q "%APPLICATIONS_DIR%\Workflow"    "%APP_DIR%\applications\Workflow\" >nul
xcopy /s /e /i /q "%APPLICATIONS_DIR%\common"      "%APP_DIR%\applications\common\" >nul
xcopy /s /e /i /q "%pathToOpenSees%"               "%APP_DIR%\applications\opensees\" >nul
xcopy /s /e /i /q "%pathToDakota%"                 "%APP_DIR%\applications\dakota\" >nul


:: ---- Extract embedded Python and install SimCenter packages ----

echo.
echo Extracting Python and installing nheri-simcenter...
echo.

"%SEVENZIP%" x "%pathToPython%" -o"%APP_DIR%\applications\" -y
if errorlevel 1 (
    echo FAIL: Python extraction failed.
    exit /b 1
)

set "PYTHON=%APP_DIR%\applications\python\python.exe"
if not exist "%PYTHON%" (
    echo FAIL: python.exe not found at %PYTHON% after extraction.
    exit /b 1
)

"%PYTHON%" -m pip install pip --upgrade
"%PYTHON%" -m pip install nheri-simcenter[quofem] --upgrade


:: ---- Clean up __pycache__ directories ----

for /d /r "%APP_DIR%" %%d in (__pycache__) do (
    if exist "%%d" rmdir /s /q "%%d"
)

echo Cleanup complete.

:: ---- Zip the release bundle ----

echo.
echo Creating release zip...
echo.

pushd "%BUILD_DIR%"
set "RELEASE_ZIP=%PROJECT_NAME%_Windows_Download.zip"
if exist "%RELEASE_ZIP%" del "%RELEASE_ZIP%"
"%SEVENZIP%" a -tzip "%RELEASE_ZIP%" "%PROJECT_NAME%_Windows_Download"
if errorlevel 1 (
    popd
    echo FAIL: 7-Zip packaging failed.
    exit /b 1
)
popd

echo.
echo Release build complete: %BUILD_DIR%\%PROJECT_NAME%_Windows_Download.zip
echo.
exit /b 0


