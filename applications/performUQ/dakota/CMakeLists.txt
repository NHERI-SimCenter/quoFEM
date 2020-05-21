simcenter_add_executable(NAME dpreproSimCenter
                         FILES simCenterDprepro.cpp)
simcenter_add_executable(NAME preprocessDakota
                         FILES preprocessDakota.cpp dakotaProcedures.cpp
                         DEPENDS CONAN_PKG::jansson)
simcenter_add_python_script(SCRIPT parseDAKOTA.py)

