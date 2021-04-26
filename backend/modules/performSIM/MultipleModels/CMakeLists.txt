simcenter_add_executable(NAME createMultipleModelsDriver
                         FILES createMultipleModelsDriver.cpp processInput.cpp
                         DEPENDS CONAN_PKG::jansson)

set_property(TARGET createMultipleModelsDriver PROPERTY CXX_STANDARD 17)

