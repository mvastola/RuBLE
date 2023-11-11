
function(detect_stacktrace_backend outvar)
    foreach(backend_lower IN LISTS STACKTRACE_BACKEND_PRIORITIES)
        string(TOUPPER "${backend_lower}" backend_upper)

        message(VERBOSE "Testing for stacktrace backend ${backend_lower}")
        message(VERBOSE "BOOST_STACKTRACE_ENABLE_${backend_lower}: ${BOOST_STACKTRACE_ENABLE_${possible_backend_upper}}")
        message(VERBOSE "BOOST_STACKTRACE_HAS_${possible_backend_upper}: ${BOOST_STACKTRACE_HAS_${possible_backend_upper}}")
        if(BOOST_STACKTRACE_ENABLE_${possible_backend_upper} AND
                BOOST_STACKTRACE_HAS_${possible_backend_upper})
            set(${outvar} "${backend_lower}" PARENT_SCOPE)
            set(${outvar}_UPPER "${backend_upper}" PARENT_SCOPE)
            break()
        endif()
    endforeach ()
endfunction()

function(disable_other_stacktrace_backends backend_enabled)
    set(UNUSED_BACKENDS ${STACKTRACE_BACKEND_PRIORITIES})
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
        target_link_libraries(${build_target} PRIVATE ${Backtrace_LIBRARY})
    endif()

    if(Backtrace_HEADER)
        set(BACKTRACE_HEADER_PATH "${Backtrace_INCLUDE_DIR}/${Backtrace_HEADER}")
        set(BOOST_STACKTRACE_BACKTRACE_INCLUDE_FILE "${BACKTRACE_HEADER_PATH}" PARENT_SCOPE)
    endif()
endmacro()

macro(setup_stacktrace_windows)
    find_library(OLE32 ole32)
    find_library(WINDBGENG dbgeng)
    target_link_libraries(${build_target} PUBLIC ole32 dbgeng)
endmacro()

macro(setup_stacktrace_addr2line)
    # This really only makes sense to use if this is being used on the build machine
    if (RUBLE_DEV_BUILD)
        set(BOOST_STACKTRACE_ADDR2LINE_LOCATION "${CMAKE_ADDR2LINE}")
    endif ()
endmacro()

detect_stacktrace_backend(STACKTRACE_BACKEND)

if (NOT STACKTRACE_BACKEND)
    message(FATAL_ERROR "Could not find supported Boost::stacktrace backend")
endif()

list(TRANSFORM STACKTRACE_BACKEND_PRIORITIES
        PREPEND "stacktrace_"
        OUTPUT_VARIABLE STACKTRACE_BACKENDS)

message(STATUS "Boost::stacktrace backend: ${STACKTRACE_BACKEND}")

set(BOOST_STACKTRACE_LINK ON PARENT_SCOPE)
set(BOOST_STACKTRACE_USE_${possible_backend_upper} ON PARENT_SCOPE)

target_link_libraries(${build_target} PRIVATE Boost::stacktrace_${STACKTRACE_BACKEND})
add_dependencies(${build_target} boost_stacktrace_${STACKTRACE_BACKEND})

target_link_libraries(${build_target} PRIVATE Boost::stacktrace_${STACKTRACE_BACKEND})

string(REGEX MATCH "^windbg_" BACKEND_IS_WINDBG "${STACKTRACE_BACKEND}")

if(STACKTRACE_BACKEND STREQUAL "backtrace")
    setup_stacktrace_backtrace()
elseif(STACKTRACE_BACKEND STREQUAL "addr2line")
    setup_stacktrace_addr2line()
elseif(WINDOWS AND STACKTRACE_BACKEND_IS_WINDBG)
    setup_stacktrace_windows()
endif()

