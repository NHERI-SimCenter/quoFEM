simcenter_add_executable(NAME createFeapDriver
                         FILES createFeapDriver.cpp processInput.cpp
                         DEPENDS CONAN_PKG::jansson)

set_property(TARGET createFeapDriver PROPERTY CXX_STANDARD 17)

