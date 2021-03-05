############################################################################################################################
##
## A SimCenter main cmake file
##
## Contents:
##   - Definition of build options
##   - Compiler variables ( c++ standard, warnings etc. )
##     the include paths are set, and the libraries are added to variable SERVICE_LIBS
##   - Subdirectory cmake lists are called
##       -> performUQ/          Contains all modules for uncertainty quantification
##
############################################################################################################################


############################################################################################################################
##
## Project name, version, Custom CMake functions
##
############################################################################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(quoFEM_BackendApplications)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_VERBOSE_MAKEFILE ON)

include(SimCenterFunctions)
include(CMakeParseArguments)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OF)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
endif (NOT CMAKE_BUILD_TYPE)

############################################################################################################################
##
## Definition of build options
##
############################################################################################################################
# Currently no tests to build, so turn this off
option(BUILD_TESTING "Enable testing for stochastic module" OFF)

# Every folder that is listed here can contain modules or tests
# this can be extended by applications to have own modules
list(APPEND SIMCENTER_MODULE_DIRS "${PROJECT_SOURCE_DIR}/performUQ")
list(APPEND SIMCENTER_MODULE_DIRS "${PROJECT_SOURCE_DIR}/performSIM")
list(REMOVE_DUPLICATES  SIMCENTER_MODULE_DIRS )
set(SIMCENTER_MODULE_DIRS  ${SIMCENTER_MODULE_DIRS} CACHE INTERNAL "All folders that contain modules or tests" )

# Bring in dependencies from Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)


############################################################################################################################
##
## Subdirectories
##
############################################################################################################################

# All include paths are specified relative to ./ directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/performUQ)

# Sources
add_subdirectory(performUQ)
add_subdirectory(performSIM)
simcenter_add_file(NAME WorkflowApplications.json)

# Tests (CURRENT NO TESTS, SO COMMENTED OUT...)
# if(BUILD_TESTING)
#     add_subdirectory(tests)
# else()
#     add_subdirectory(tests EXCLUDE_FROM_ALL)
# endif()
