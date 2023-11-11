# From https://stackoverflow.com/a/51987470
# Get all properties that cmake supports
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

function(dump_variables)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endfunction()

macro(build_boost)
    set(REPO "https://github.com/boostorg/boost")
    set(TAG boost-1.83.0)
    cmake_path(SET CACHE_LOCATION NORMALIZE "${CMAKE_SOURCE_DIR}/../../tmp/${TAG}")

    set(ADDITIONAL_DEPENDENCIES core;stacktrace)
    set(HEADER_ONLY_LIBS headers;describe)
    set(LINKED_LIBS exception;endian;algorithm) # + detected stacktrace backend
    set(STACKTRACE_BACKEND_PRIORITIES backtrace;addr2line;windgb_cached;windbg;basic;noop)

    set(ALL_LIBS ${HEADER_ONLY_LIBS};${LINKED_LIBS})
    set(BUILD_DEPS ${ALL_LIBS})
    set(BOOST_INCLUDE_LIBRARIES ${ADDITIONAL_DEPENDENCIES};${BUILD_DEPS})

    list(TRANSFORM HEADER_ONLY_LIBS PREPEND "Boost::"
            OUTPUT_VARIABLE HEADER_ONLY_NAMESPACES)
    list(TRANSFORM LINKED_LIBS PREPEND "Boost::"
            OUTPUT_VARIABLE LINKED_NAMESPACES)
    list(TRANSFORM BUILD_DEPS PREPEND "boost_"
            OUTPUT_VARIABLE PREFIXED_BUILD_DEPS)
#    dump_variables()
#    message(FATAL_ERROR "quitting")

    set(STACKTRACE_BACKEND_PRIORITIES backtrace;addr2line;windgb_cached;windbg;basic;noop)
    list(TRANSFORM STACKTRACE_BACKEND_PRIORITIES
            PREPEND "stacktrace_"
            OUTPUT_VARIABLE STACKTRACE_BACKENDS)

    set(BOOST_ENABLE_PYTHON OFF)
    set(BUILD_SHARED_LIBS OFF)
    set(Boost_DEBUG OFF)
    set(Boost_VERBOSE OFF)
    set(BUILD_TESTING OFF)

    # TODO: add option to use local boost (in which case we can link dynamically)

        include(FetchContent)
        FetchContent_Declare(
            Boost
            SOURCE_DIR "${CACHE_LOCATION}"
            URL "${rb_boost_asset_browser_download_url}"
            DOWNLOAD_EXTRACT_TIMESTAMP ON
            OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(Boost)

    find_package(
        Boost
        REQUIRED
            ${BOOST_INCLDUE_LIBRARIES}
        OPTIONAL_COMPONENTS
            ${STACKTRACE_BACKENDS}
        PATHS "${CACHE_LOCATION}" "${CACHE_LOCATION}/tools/cmake"
        EXCLUDE_FROM_ALL ON
        NO_DEFAULT_PATH
    )
endmacro()

# TODO: Should we fall back with this?
function(setup_boost build_target)
    build_boost()

    target_link_libraries(${build_target} INTERFACE ${HEADER_ONLY_NAMESPACES}) #Boost::headers Boost::exception Boost::describe Boost::endian)
    target_link_libraries(${build_target} PRIVATE ${LINKED_NAMESPACES})
    add_dependencies(${build_target} ${PREFIXED_BUILD_DEPS})

    foreach(possible_backend IN LISTS STACKTRACE_BACKEND_PRIORITIES)
        string(TOUPPER "${possible_backend}" possible_backend_upper)

        message(STATUS "Testing for stacktrace backend ${possible_backend}")
        message(STATUS "BOOST_STACKTRACE_ENABLE_${possible_backend_upper}: ${BOOST_STACKTRACE_ENABLE_${possible_backend_upper}}")
        message(STATUS "BOOST_STACKTRACE_HAS_${possible_backend_upper}: ${BOOST_STACKTRACE_HAS_${possible_backend_upper}}")
        if(NOT BOOST_STACKTRACE_ENABLE_${possible_backend_upper} OR NOT BOOST_STACKTRACE_HAS_${possible_backend_upper})
            continue()
        endif()

        string(REGEX MATCH "^windbg_" STACKTRACE_BACKEND_IS_WINDBG "${possible_backend}")
        set(STACKTRACE_BACKEND "${possible_backend}")
        set(STACKTRACE_BACKEND "${possible_backend}" PARENT_SCOPE)
        set(BOOST_STACKTRACE_LINK ON)
        set(BOOST_STACKTRACE_LINK ON PARENT_SCOPE)
        set(BOOST_STACKTRACE_USE_${possible_backend_upper} ON PARENT_SCOPE)

        target_link_libraries(${build_target} PRIVATE Boost::stacktrace_${STACKTRACE_BACKEND})
        add_dependencies(${build_target} boost_stacktrace_${STACKTRACE_BACKEND})

        target_link_libraries(${build_target} PRIVATE Boost::stacktrace_${possible_backend})
        if(possible_backend STREQUAL "backtrace")
            include(FindBacktrace)
            if(Backtrace_LIBRARY)
                target_link_libraries(${build_target} PRIVATE ${Backtrace_LIBRARY})
            endif()

            if(Backtrace_HEADER)
                set(BACKTRACE_HEADER_PATH "${Backtrace_INCLUDE_DIR}/${Backtrace_HEADER}")
                set(BOOST_STACKTRACE_BACKTRACE_INCLUDE_FILE "${BACKTRACE_HEADER_PATH}" PARENT_SCOPE)
            endif ()
#        elseif(possible_backend STREQUAL "addr2line")
#            # This really only makes sense to use if this is being built on the computer it's used on
#            set(BOOST_STACKTRACE_ADDR2LINE_LOCATION "${CMAKE_ADDR2LINE}")
        elseif(WINDOWS AND STACKTRACE_BACKEND_IS_WINDBG) # TODO: untested
            find_library(OLE32 ole32)
            find_library(WINDBGENG dbgeng)
            target_link_libraries(${build_target} PUBLIC ole32 dbgeng)
        endif()
        break()
    endforeach()

    if(NOT BOOST_STACKTRACE_LINK)
#        dump_variables()
        message(FATAL_ERROR "Could not find supported Boost::stacktrace backend")
    endif()
    message(STATUS "Boost::stacktrace backend: ${STACKTRACE_BACKEND}")

    set(UNUSED_STACKTRACE_BACKENDS ${STACKTRACE_BACKEND_PRIORITIES})
    list(REMOVE_ITEM UNUSED_STACKTRACE_BACKENDS "${STACKTRACE_BACKEND}")
    message(VERBOSE "Disabling unused Boost::stacktrace backends: ${UNUSED_STACKTRACE_BACKENDS}")
    foreach(unused_backend IN LISTS UNUSED_STACKTRACE_BACKENDS)
        string(TOUPPER "${unused_backend}" unused_backend_upper)
        if(NOT BOOST_STACKTRACE_ENABLE_${unused_backend_upper})
            continue()
        endif()

        set_target_properties(boost_stacktrace_${unused_backend} PROPERTIES EXCLUDE_FROM_ALL ON)
        set(BOOST_STACKTRACE_ENABLE_${unused_backend_upper} OFF)
    endforeach()
endfunction()

# TODO: try changing these into functions
function(setup_simpleble build_target)
    include(FetchContent)
    # TODO: would it be easier/better to use git and the commit hash
    #   and build SimpleBLE from source? The main downside is more
    #   dependencies. The upside is vastly simplifying configuring
    #   simpleble_DIR, as well as choosing between shared/static linking.
    FetchContent_Declare(
            simpleble
            URL "${rb_simpleble_asset_browser_download_url}"
            DOWNLOAD_EXTRACT_TIMESTAMP ON
#            OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(simpleble)
    set(simpleble_DIR ${simpleble_SOURCE_DIR}/${rb_simpleble_archive_path})
    find_package(simpleble REQUIRED CONFIG PATHS "${simpleble_DIR}" NO_DEFAULT_PATH)
    target_link_libraries("${build_target}" PUBLIC simpleble::simpleble)
    include("${simpleble_CONFIG}")
    message(STATUS "SimpleBLE found in ${simpleble_DIR}")
endfunction()


macro(setup_ruby build_target)
    #set(_Ruby_DEBUG_OUTPUT ON)
    set(RUBY_FIND_VERSION 3.2)
    find_package(Ruby 3.2 REQUIRED Ruby_FIND_VIRTUALENV)
    target_link_libraries("${build_target}" PUBLIC ${Ruby_LIBRARIES})
    target_include_directories("${build_target}" PUBLIC ${Ruby_INCLUDE_DIRS})
    target_include_directories("${build_target}" PRIVATE ${RICE_INCLUDE_PATH})
    set_target_properties("${build_target}" PROPERTIES INSTALL_RPATH_USE_LINK_PATH ON)
    message(STATUS "libruby ${Ruby_VERSION} found at ${Ruby_LIBRARY}")
    message(STATUS "Rice ${RICE_VERSION} found in ${RICE_INCLUDE_PATH}")
endmacro()
