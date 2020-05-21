######################################################################################################################
#   PLEASE NOTE:
#   The functionality below is taken from the CMake configuration in waLBerla (https://www.walberla.net/)
#   and has been modified and simplified to build SimCenter backend applications.
######################################################################################################################

# include(SimCenterHelperFunctions)
include(SimCenterModuleDependencies)

set(SIMCENTER_GLOB_FILES *.cpp
                         *.h
			 CACHE INTERNAL "File endings to glob for source files" )

#######################################################################################################################
#
# Creates a SimCenter module library
#
#
# Keywords:
#   DEPENDS [required]   list of modules that this module depends on
#   FILES   [optional]   list of all source and header files belonging to that module
#                        if this is not given, all source and header files in the directory are added.
#                        Careful: when file was added or deleted, cmake has to be run again
#   EXCLUDE [optional]   Files that should not be included in the module (but are located in module directory).
#                        This makes only sense if FILES was not specified, and all files have been added automatically.
#   BUILD_ONLY_IF_FOUND  Before building the module test if all libraries specified here are availbable.
#   [optional]           This is done using the ${arg}_FOUND variable.
#                        Example: simcenter_add_module(DEPENDS someModule BUILD_ONLY_IF_FOUND something)
#                                 This module is only built if SOMETHING_FOUND is true.
#
#######################################################################################################################

function(simcenter_add_module)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs DEPENDS EXCLUDE FILES BUILD_ONLY_IF_FOUND)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Module name is the directory relative to SIMCENTER_MODULE_DIRS
    get_current_module_name(moduleName)
    get_module_library_name(moduleLibraryName ${moduleName})

    # Test if all required libraries are available
    # this is detected using the _FOUND variable
    foreach(externalName ${ARG_BUILD_ONLY_IF_FOUND})
        string(TOUPPER ${externalName} externalNameUpper)
        if (NOT ${externalNameUpper}_FOUND)
            message(STATUS "Module ${moduleName} is not built, because ${externalName} not available")
            return()
        endif()
    endforeach()

    # Take source files either from parameter or search all source files
    file (GLOB_RECURSE allFiles "*")  # Find all files
    if(ARG_FILES)
        foreach(sourceFile ${ARG_FILES})
           get_filename_component(sourceFile ${sourceFile} ABSOLUTE)
           list(APPEND sourceFiles ${sourceFile})
        endforeach()
    else()
        file(GLOB_RECURSE sourceFiles ${SIMCENTER_GLOB_FILES})  # Find all source files
    endif()

    # Remove exclude files from the sources
    if (ARG_EXCLUDE)
        foreach (fileToExclude ${ARG_EXCLUDE})
            get_filename_component(fileToExclude ${fileToExclude} ABSOLUTE)
            list(REMOVE_ITEM sourceFiles ${fileToExclude})
        endforeach()
    endif()

    list_minus(otherFiles LIST1 ${allFiles} LIST2 ${sourceFiles})
    set_source_files_properties(${otherFiles} PROPERTIES HEADER_FILE_ONLY ON)

    # Dependency Check
    check_dependencies(missingDeps additionalDeps FILES ${sourceFiles} EXPECTED_DEPS ${ARG_DEPENDS} ${moduleName})
    if (missingDeps)
        message (WARNING "The module ${moduleName} depends on ${missingDeps} which are not listed as dependencies!")
    endif()

    set(hasSourceFiles FALSE)
    foreach (sourceFile ${sourceFiles})
      if (${sourceFile} MATCHES "\\.(c|cpp|cc)")
 	set(hasSourceFiles TRUE)
      endif()
    endforeach()

    if (hasSourceFiles)
      set( generatedSourceFiles )
      set( generatorSourceFiles )
      
      add_library(${moduleLibraryName} STATIC ${sourceFiles} ${generatedSourceFiles} ${generatorSourceFiles} ${otherFiles})
      set_source_files_properties(${generatedSourceFiles} PROPERTIES GENERATED TRUE)
    else( )
      add_custom_target(${moduleLibraryName} SOURCES ${sourceFiles} ${generatedSourceFiles} ${otherFiles})  # dummy IDE target
    endif( )
    
    simcenter_register_dependency(${moduleName} ${ARG_DEPENDS})
    set_property(TARGET ${moduleName} PROPERTY CXX_STANDARD 14)

    # Install rule for library
    # get_target_property(module_type ${moduleLibraryName} TYPE)
    # if (${module_type} MATCHES LIBRARY)
    #    install(TARGETS ${moduleLibraryName} RUNTIME DESTINATION bin
    #                                         LIBRARY DESTINATION lib
    #                                         ARCHIVE DESTINATION lib)
    # endif()

endfunction(simcenter_add_module)
#######################################################################################################################

#######################################################################################################################
#
# Compiles an application either from given source files, or otherwise globs all files in the current folder.
# The application is linked against all SimCenter modules that are listed after DEPENDS
#
#
#   NAME    [required]    Name of application
#   GROUP   [optional]    IDE group name (e.g. VS)
#   DEPENDS [required]    list of modules, that this application depends on
#   FILES   [optional]    list of all source and header files belonging to that application
#                         if this is not given, all source and header files in the directory are added.
#                         Careful: when file was added or deleted, cmake has to be run again
#
#  Example:
#     simcenter_add_executable(NAME myApp DEPENDS external/smelt Common)
#######################################################################################################################

function(simcenter_add_executable)
    set(options)
    set(oneValueArgs NAME GROUP)
    set(multiValueArgs FILES DEPENDS COMPILE_FLAGS EXE_LINKER_FLAGS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if( NOT ARG_NAME )
      message ( FATAL_ERROR "simcenter_add_executable called without a NAME" )
    endif()

    # Skip this app, if it depends on modules that have not been built
    foreach(depMod ${ARG_DEPENDS})
        get_module_library_name(depModLibraryName ${depMod})
        if(NOT TARGET ${depModLibraryName})
            message(STATUS "Skipping ${ARG_NAME} since dependent module ${depMod} was not built")
            #return()
        endif()
    endforeach()

    # Take source files either from parameter or search all source files
    set(sourceFiles)
    if (ARG_FILES)
        foreach(sourceFile ${ARG_FILES})
           get_filename_component(sourceFile ${sourceFile} ABSOLUTE)
           list(APPEND sourceFiles ${sourceFile})
        endforeach()
    else()
        file(GLOB_RECURSE sourceFiles ${SIMCENTER_GLOB_FILES})  # Find all source files
    endif()

    set(generatedSourceFiles)
    set(generatorSourceFiles)       
    add_executable(${ARG_NAME} ${sourceFiles} ${generatedSourceFiles})

    if (ARG_COMPILE_FLAGS)
      message("Passed these:\n" ${ARG_COMPILE_FLAGS})
      set_target_properties(${ARG_NAME} PROPERTIES COMPILE_OPTIONS ${ARG_COMPILE_FLAGS})
    endif()
   
    set_source_files_properties(${generatedSourceFiles} PROPERTIES GENERATED TRUE)
    target_link_modules(${ARG_NAME} ${ARG_DEPENDS})

    if (ARG_EXE_LINKER_FLAGS)
      message("Passed these:\n" ${ARG_EXE_LINKER_FLAGS})      
      target_link_libraries(${ARG_NAME} ${ARG_EXE_LINKER_FLAGS})
    endif()

    target_link_libraries(${ARG_NAME} ${SIMCENTER_LINK_LIBRARIES_KEYWORD} ${SERVICE_LIBS})
    
    set_property(TARGET ${ARG_NAME} PROPERTY CXX_STANDARD 14)
    # Install rule for executable
    get_current_module_dir(moduleDir)
    get_current_module_name(moduleName)    
    install(TARGETS ${ARG_NAME} DESTINATION "${PROJECT_SOURCE_DIR}/applications/${moduleName}")

endfunction(simcenter_add_executable)

#######################################################################################################################
#
# Copies python script and dependencies to applications directory
#
#######################################################################################################################

function(simcenter_add_python_script)
    set(options)
    set(oneValueArgs SCRIPT)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if( NOT ARG_SCRIPT )
      message ( FATAL_ERROR "simcenter_add_python_script called without a NAME" )
    endif()

    get_current_module_name(moduleDir)    
    foreach(f ${ARG_DEPENDS})
      simcenter_add_python_dependency(DEPENDENCY ${f} COPY_LOCATION "${PROJECT_SOURCE_DIR}/applications/${moduleDir}")
    endforeach()
    
    file(COPY ${ARG_SCRIPT} DESTINATION "${PROJECT_SOURCE_DIR}/applications/${moduleDir}")
      
endfunction(simcenter_add_python_script)

#######################################################################################################################
#
# Copies python dependencies to applications directory
#
#######################################################################################################################

function(simcenter_add_python_dependency)
    set(options)
    set(oneValueArgs DEPENDENCY COPY_LOCATION)
    set(multiValueArgs)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    file(COPY ${ARG_DEPENDENCY} DESTINATION "${ARG_COPY_LOCATION}")
    
endfunction(simcenter_add_python_dependency)

#######################################################################################################################
#
# Copies file to applications directory
#
#######################################################################################################################

function(simcenter_add_file)
    set(options)
    set(oneValueArgs NAME)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if( NOT ARG_NAME )
      message ( FATAL_ERROR "simcenter_add_file called without a NAME" )
    endif()

    get_current_module_name(moduleDir)   
    file(COPY ${ARG_NAME} DESTINATION "${PROJECT_SOURCE_DIR}/applications/${moduleDir}")
      
endfunction(simcenter_add_file)


#######################################################################################################################
#
# Links all files in current source dir matching a globbing expression to the build directory
#
# first parameter is glob expression
#
# Typical usage: link all parameter files in same folder as the binary was produced
#                Assuming the parameter files end with prm, write this to your CMakeLists in the
#                app or test folder:
#                simcenter_link_files_to_builddir( "*.prm" )
#
# Symlinking works only under linux, on windows the files are copied. For in-source builds this does nothing.
#
#######################################################################################################################

function(simcenter_link_files_to_builddir globExpression )

    # don't need links for in-source builds
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
        return()
    endif()

    file(GLOB filesToLink RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${globExpression})

    foreach(f ${filesToLink})
        if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
            configure_file(${f} ${f} COPYONLY)
        else()
            execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
                            ${CMAKE_CURRENT_SOURCE_DIR}/${f}
                            ${CMAKE_CURRENT_BINARY_DIR}/${f})
        endif()
    endforeach()

endfunction(simcenter_link_files_to_builddir)

#######################################################################################################################
#
# Subtracts list2 from list 2
# 
# Keywords:
#     LIST1   first list
#     LIST2   second list
# Example:
#     list_minus ( result LIST1 "entry1" "entry2" "entry3" LIST2 "entry1" "entry3" )
#      -> result has a single entry: "entry2"
#     
#######################################################################################################################
function(list_minus resultOut)
    set(option)
    set(oneValueArgs)
    set(multiValueArgs LIST1 LIST2)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
   
    set(result ${ARG_LIST1})
    foreach(secondEntry ${ARG_LIST2})
        list(REMOVE_ITEM result ${secondEntry})
    endforeach()
    
    set(${resultOut} ${result} PARENT_SCOPE)
    
endfunction(list_minus)    
#######################################################################################################################
