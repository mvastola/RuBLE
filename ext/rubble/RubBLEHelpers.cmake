# From https://stackoverflow.com/a/51987470
# Get all propreties that cmake supports
execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)

# Convert command output into a CMake list
STRING(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
STRING(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
# Fix https://stackoverflow.com/questions/32197663/how-can-i-remove-the-the-location-property-may-not-be-read-from-target-error-i
list(FILTER CMAKE_PROPERTY_LIST EXCLUDE REGEX "^LOCATION$|^LOCATION_|_LOCATION$")
# For some reason, "TYPE" shows up twice - others might too?
list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)

# build whitelist by filtering down from CMAKE_PROPERTY_LIST in case cmake is
# a different version, and one of our hardcoded whitelisted properties
# doesn't exist!
unset(CMAKE_WHITELISTED_PROPERTY_LIST)
foreach(prop ${CMAKE_PROPERTY_LIST})
    if(prop MATCHES "^(INTERFACE|[_a-z]|IMPORTED_LIBNAME_|MAP_IMPORTED_CONFIG_)|^(COMPATIBLE_INTERFACE_(BOOL|NUMBER_MAX|NUMBER_MIN|STRING)|EXPORT_NAME|IMPORTED(_GLOBAL|_CONFIGURATIONS|_LIBNAME)?|NAME|TYPE|NO_SYSTEM_FROM_IMPORTED)$")
        list(APPEND CMAKE_WHITELISTED_PROPERTY_LIST ${prop})
    endif()
endforeach(prop)

function(print_properties)
    message ("CMAKE_PROPERTY_LIST = ${CMAKE_PROPERTY_LIST}")
endfunction(print_properties)

function(print_whitelisted_properties)
    message ("CMAKE_WHITELISTED_PROPERTY_LIST = ${CMAKE_WHITELISTED_PROPERTY_LIST}")
endfunction(print_whitelisted_properties)

function(print_target_properties tgt)
    if(NOT TARGET ${tgt})
        message("There is no target named '${tgt}'")
        return()
    endif()

    get_target_property(target_type ${tgt} TYPE)
    if(target_type STREQUAL "INTERFACE_LIBRARY")
        set(PROP_LIST ${CMAKE_WHITELISTED_PROPERTY_LIST})
    else()
        set(PROP_LIST ${CMAKE_PROPERTY_LIST})
    endif()

    foreach (prop ${PROP_LIST})
        string(REPLACE "<CONFIG>" "${CMAKE_BUILD_TYPE}" prop ${prop})
        # message ("Checking ${prop}")
        get_property(propval TARGET ${tgt} PROPERTY ${prop} SET)
        if (propval)
            get_target_property(propval ${tgt} ${prop})
            message ("${tgt} ${prop} = ${propval}")
        endif()
    endforeach(prop)
endfunction(print_target_properties)

macro(dump_variables)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endmacro()

# TODO: Should we fall back with this?
# include(FetchContent)
# FetchContent_Declare(
#   Boost
#   URL https://github.com/boostorg/boost/releases/download/boost-1.81.0/boost-1.81.0.tar.xz
#   URL_MD5 6cf0cdd797bca685910d527ae3c08cb3
#   DOWNLOAD_EXTRACT_TIMESTAMP ON
# )
# FetchContent_MakeAvailable(Boost)
macro(setup_boost build_target)
#    find_package(Boost OPTIONAL_COMPONENTS headers exception)
    get_target_property(BOOST_HEADER_DIRS Boost::headers INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(${build_target} INTERFACE ${BOOST_HEADER_DIRS})

    find_path(EXCEPTION_HEADER_DIR boost/exception/all.hpp PATHS ${BOOST_HEADER_DIRS})
    if (EXCEPTION_HEADER_DIR)
        set(HAVE_BOOST_EXCEPTION ON)
        target_link_libraries(${build_target} INTERFACE Boost::exception)
    endif()

    find_path(STACKTRACE_HEADER_DIR boost/stacktrace.hpp PATHS ${BOOST_HEADER_DIRS})
    if(STACKTRACE_HEADER_DIR)
        set(HAVE_BOOST_STACKTRACE ON)
        foreach(stacktrace_backend IN LISTS STACKTRACE_BACKEND_PRIORITIES)
            message(STATUS "Testing for stacktrace backend ${stacktrace_backend}")
#            find_package(Boost COMPONENTS stacktrace_${stacktrace_backend})
            if(NOT Boost_stacktrace_${stacktrace_backend}_FOUND)
                continue()
            endif()

            string(REGEX MATCH "^windbg_" WINDBG_STACKTRACE_BACKEND "${stacktrace_backend}")
            set(STACKTRACE_BACKEND "${stacktrace_backend}")
            string(TOUPPER "${stacktrace_backend}" stacktrace_backend_upper)
            set(BOOST_STACKTRACE_LINK ON)
            set(BOOST_STACKTRACE_USE_${stacktrace_backend_upper} ON)

            target_link_libraries(${build_target} PRIVATE Boost::stacktrace_${stacktrace_backend})
            if(stacktrace_backend STREQUAL "backtrace")
                set(BOOST_STACKTRACE_BACKTRACE_INCLUDE_FILE "${Backtrace_INCLUDE_DIR}/${Backtrace_HEADER}")
                if(Backtrace_LIBRARY)
                    target_link_libraries(${build_target} PRIVATE ${Backtrace_LIBRARY})
                endif()
            elseif(stacktrace_backend STREQUAL "addr2line")
                set(BOOST_STACKTRACE_ADDR2LINE_LOCATION "${CMAKE_ADDR2LINE}")
            elseif(WINDOWS AND WINDBG_STACKTRACE_BACKEND)
                find_library(OLE32 ole32)
                find_library(WINDBGENG dbgeng)
                target_link_libraries(${build_target} PUBLIC ole32 dbgeng)
            endif()
            break()
        endforeach()
        if(STACKTRACE_BACKEND AND NOT STACKTRACE_BACKEND)
            target_link_libraries(${build_target} PUBLIC Boost::stacktrace)
        endif()
    endif()
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "STACKTRACE_BACKEND: ${STACKTRACE_BACKEND}")
    message(STATUS "HAVE_BOOST_EXCEPTION: ${HAVE_BOOST_EXCEPTION}")
endmacro()

macro(setup_simpleble build_target)
    find_package(simpleble REQUIRED CONFIG PATHS "${SIMPLEBLE_EXTRACT_DIR}" NO_DEFAULT_PATH)
    target_link_libraries("${build_target}" PUBLIC simpleble::simpleble)
    include(${simpleble_CONFIG})
    message(STATUS "SimpleBLE found in ${simpleble_DIR}")
endmacro()

macro(setup_ruby build_target)
    set(RUBY_FIND_VERSION 3.2)
    find_package(Ruby 3.2 REQUIRED Ruby_FIND_VIRTUALENV)
    target_link_libraries("${build_target}" PUBLIC ${Ruby_LIBRARIES})
    target_include_directories("${build_target}" PUBLIC ${Ruby_INCLUDE_DIRS})
    target_include_directories("${build_target}" PRIVATE ${RICE_INCLUDE_PATH})
    set_target_properties("${build_target}" PROPERTIES INSTALL_RPATH_USE_LINK_PATH ON)
    message(STATUS "libruby ${Ruby_VERSION} found at ${Ruby_LIBRARY}")
    message(STATUS "Rice ${RICE_VERSION} found in ${RICE_INCLUDE_PATH}")
endmacro()