simcenter_add_executable(NAME simCenterDprepro2
                         FILES simCenterDprepro2.cpp)
simcenter_add_executable(NAME preprocessSimCenterUQ
                         FILES preprocessSimCenterUQ.cpp SimCenterUQProcedures.cpp
                         DEPENDS CONAN_PKG::jansson)
simcenter_add_python_script(SCRIPT parseSimCenterUQ.py)

