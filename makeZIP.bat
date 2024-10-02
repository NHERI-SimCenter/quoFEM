
REM ## make the exe

cd build
conan install .. --build missing
qmake QMAKE_CXXFLAGS+=-D_SC_RELEASE ..\quoFEM.pro
nmake

REM ## copy application into folder and winddeployqt it

copy quoFEM.exe .\quoFEM_Windows_Download
cd quoFEM_Windows_Download
windeployqt quoFEM.exe

REM ## copy examples

rmdir /s /q .\Examples
mkdir .\Examples
xcopy /s /e /Q ..\..\Examples .\Examples

REM ## delete applications folder and copy new stuff

rmdir /s /q .\applications
mkdir .\applications
mkdir .\applications\performFEM
mkdir .\applications\performUQ
mkdir .\applications\Workflow
mkdir .\applications\opensees
mkdir .\applications\dakota
mkdir .\applications\python
mkdir .\applications\common
xcopy /s /e    ..\..\..\SimCenterBackendApplications\applications\performFEM  .\applications\performFEM
xcopy /s /e    ..\..\..\SimCenterBackendApplications\applications\performUQ   .\applications\performUQ
xcopy /s /e    ..\..\..\SimCenterBackendApplications\applications\Workflow    .\applications\Workflow
xcopy /s /e    ..\..\..\SimCenterBackendApplications\applications\OpenSees    .\applications\opensees
xcopy /s /e    ..\..\..\SimCenterBackendApplications\applications\dakota      .\applications\dakota
xcopy /s /e    ..\..\..\SimCenterBackendApplications\applications\python      .\applications\python
xcopy /s /e    ..\..\..\SimCenterBackendApplications\applications\common      .\applications\common



