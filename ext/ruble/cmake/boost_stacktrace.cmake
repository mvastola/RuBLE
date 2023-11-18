
function(detect_stacktrace_backend outvar)
    foreach(backend_lower IN LISTS STACKTRACE_BACKEND_PRIORITIES)
        string(TOUPPER "${backend_lower}" backend_upper)

        message(VERBOSE "Testing for stacktrace backend ${backend_lower}")
        message(VERBOSE "BOOST_STACKTRACE_ENABLE_${backend_lower}: ${BOOST_STACKTRACE_ENABLE_${backend_upper}}")
        message(VERBOSE "BOOST_STACKTRACE_HAS_${backend_upper}: ${BOOST_STACKTRACE_HAS_${backend_upper}}")
        if(BOOST_STACKTRACE_ENABLE_${backend_upper} AND
                BOOST_STACKTRACE_HAS_${backend_upper})
            set(${outvar} "${backend_lower}" PARENT_SCOPE)
            set(${outvar}_UPPER "${backend_upper}" PARENT_SCOPE)
            set(BOOST_STACKTRACE_BACKEND "${backend_lower}" CACHE 
              STRING "The backend used by boost::stacktrace")
            break()
        endif()
    endforeach()
endfunction()

function(disable_other_stacktrace_backends backend_enabled)
    set(UNUSED_BACKENDS ${STACKTRACE_BACKENDS_SHORT})
    list(REMOVE_ITEM UNUSED_BACKENDS "${backend_enabled}")
    message(VERBOSE "Disabling unused Boost::stacktrace backends: ${UNUSED_BACKENDS}")
    foreach(unused_backend IN LISTS UNUSED_BACKENDS)
        string(TOUPPER "${unused_backend}" unused_backend_upper)
        if(BOOST_STACKTRACE_ENABLE_${unused_backend_upper})
            set_target_properties(boost_stacktrace_${unused_backend}
                    PROPERTIES EXCLUDE_FROM_ALL ON)
            set(BOOST_STACKTRACE_ENABLE_${unused_backend_upper} OFF)
        endif()
    endforeach()
endfunction()

macro(setup_stacktrace_backtrace)
    include(FindBacktrace)
    if(Backtrace_LIBRARY)
        target_link_libraries(${EXTENSION_NAME} PRIVATE ${Backtrace_LIBRARY})
    endif()

    if(Backtrace_HEADER)
        set(BACKTRACE_HEADER_PATH "${Backtrace_INCLUDE_DIR}/${Backtrace_HEADER}")
        set(BOOST_STACKTRACE_BACKTRACE_INCLUDE_FILE "${BACKTRACE_HEADER_PATH}")
    endif()
endmacro()

macro(setup_stacktrace_windows)
    find_library(OLE32 ole32)
    find_library(WINDBGENG dbgeng)
    find_path(DBGENG_H_PATH dbgeng.h REQUIRED)
    cmake_path(SET BOOST_STACKTRACE_DBGENG_H_FULL_PATH NORMALIZE "${DBGENG_H_PATH}/dbgeng.h")
    #message(STATUS BOOST_STACKTRACE_DBGENG_H_FULL_PATH "${BOOST_STACKTRACE_DBGENG_H_FULL_PATH}")

    # TODO see if this works for others
    target_link_libraries(${EXTENSION_NAME} PUBLIC ole32 dbgeng)
endmacro()

macro(setup_stacktrace_addr2line)
    # This really only makes sense to use if this is being used on the build machine
    if (RUBLE_DEVELOPER)
        set(BOOST_STACKTRACE_ADDR2LINE_LOCATION "${CMAKE_ADDR2LINE}")
    endif ()
endmacro()

detect_stacktrace_backend(STACKTRACE_BACKEND)

if (NOT STACKTRACE_BACKEND)
    message(FATAL_ERROR "Could not find supported Boost::stacktrace backend")
endif()

message(STATUS "Boost::stacktrace backend: ${STACKTRACE_BACKEND}")

set(BOOST_STACKTRACE_BACKEND ${STACKTRACE_BACKEND})
set(BOOST_STACKTRACE_BACKEND_UPPER ${STACKTRACE_BACKEND_UPPER})
set(BOOST_STACKTRACE_LINK ON)

target_link_libraries(${EXTENSION_NAME} PRIVATE Boost::stacktrace_${STACKTRACE_BACKEND})
add_dependencies(${EXTENSION_NAME} boost_stacktrace_${STACKTRACE_BACKEND})
target_link_libraries(${EXTENSION_NAME} PRIVATE Boost::stacktrace_${STACKTRACE_BACKEND})
disable_other_stacktrace_backends(${STACKTRACE_BACKEND})

string(REGEX MATCH "^windbg" STACKTRACE_BACKEND_IS_WINDBG "${STACKTRACE_BACKEND}")

if(STACKTRACE_BACKEND STREQUAL "backtrace")
    setup_stacktrace_backtrace()
elseif(STACKTRACE_BACKEND STREQUAL "addr2line")
    setup_stacktrace_addr2line()
elseif(STACKTRACE_BACKEND_IS_WINDBG)
    # FIXME: Alert to https://github.com/boostorg/stacktrace/issues/133 somehow
    # Test if mingw includes this change https://github.com/mirror/mingw-w64/commit/ce5a9f624dfc691082dad2ea2af7b1985e3476b5
    setup_stacktrace_windows()
endif()

