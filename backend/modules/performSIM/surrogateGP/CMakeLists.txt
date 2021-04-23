simcenter_add_executable(NAME createGpDriver
                         FILES createGpDriver.cpp processInput.cpp
                         DEPENDS CONAN_PKG::jansson)

set_property(TARGET createGpDriver PROPERTY CXX_STANDARD 17)

simcenter_add_python_script(SCRIPT gpPredict.py)
