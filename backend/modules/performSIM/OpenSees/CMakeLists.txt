simcenter_add_executable(NAME createOpenSeesDriver
                         FILES createOpenSeesDriver.cpp processInput.cpp
                         DEPENDS CONAN_PKG::jansson)

set_property(TARGET createOpenSeesDriver PROPERTY CXX_STANDARD 17)

