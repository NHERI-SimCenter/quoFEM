simcenter_add_executable(NAME createOpenSeesPyDriver
                         FILES createOpenSeesPyDriver.cpp processInput.cpp
                         DEPENDS CONAN_PKG::jansson)

set_property(TARGET createOpenSeesPyDriver PROPERTY CXX_STANDARD 17)

