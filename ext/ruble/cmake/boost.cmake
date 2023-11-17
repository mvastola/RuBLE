function(get_boost_version out_var include_dir)
    set(get_boost_version_src [=[
        #include <iostream>
        #include <algorithm>
        #include <boost/version.hpp>

        int main() {
            std::string boost_version(BOOST_LIB_VERSION);
            std::replace(boost_version.begin(), boost_version.end(), '_', '.');
            std::cout << boost_version;
            return 0;
        }
    ]=])
    try_run(run_result compile_result
        SOURCE_FROM_VAR get_boost_version.cpp get_boost_version_src
        CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${include_dir}"
        COMPILE_OUTPUT_VARIABLE compile_out
        CXX_STANDARD ${CMAKE_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ON
        RUN_OUTPUT_VARIABLE run_out
    )
    if (compile_result AND run_result EQUAL 0)
        set(${out_var} "${run_out}")
        return(PROPAGATE ${out_var})
    endif()

    if(NOT compile_result)
        message(WARNING "Failed to compile boost version checker: ${compile_out}")
    elseif(NOT run_result)
        message(WARNING "Failed to run boost version checker: ${run_out}")
    endif()
    message(FATAL_ERROR "Failed to determine boost version. "
            "(Compiled: ${compile_result}, Ran: ${run_result}")
endfunction()

block(PROPAGATE
        BOOST_STACKTRACE_BACKEND
        BOOST_STACKTRACE_BACKEND_UPPER
        BOOST_STACKTRACE_LINK
        BOOST_STACKTRACE_BACKTRACE_INCLUDE_FILE
        BOOST_STACKTRACE_ADDR2LINE_LOCATION
        BOOST_VERSION_STR
)

    set(STACKTRACE_BACKEND_PRIORITIES backtrace;addr2line;windgb_cached;windbg;basic;noop)
    set(BOOST_ENABLE_PYTHON OFF)
    set(BUILD_SHARED_LIBS ${rb_boost_shared})
    set(Boost_DEBUG OFF) # TODO: does this have anything to do with debug compiler flags?
    set(Boost_VERBOSE ${RUBLE_VERBOSE})
    set(BUILD_TESTING OFF)

    # TODO: handle option to use local boost (in which case we can link dynamically)
    cmake_path(SET CACHE_LOCATION NORMALIZE
            "${CMAKE_CURRENT_SOURCE_DIR}/../../tmp/${rb_boost_github_release_tag}")

    set(ADDITIONAL_DEPENDENCIES core;stacktrace)
    set(HEADER_ONLY_LIBS headers;describe)
    set(LINKED_LIBS exception;endian;algorithm) # + detected stacktrace backend
    set(STACKTRACE_BACKEND_PRIORITIES backtrace;addr2line;windgb_cached;windbg;basic;noop)

    set(ALL_LIBS ${HEADER_ONLY_LIBS} ${LINKED_LIBS})
    set(BUILD_DEPS ${ALL_LIBS})
    set(BOOST_INCLUDE_LIBRARIES ${ADDITIONAL_DEPENDENCIES} ${BUILD_DEPS})

    list(TRANSFORM HEADER_ONLY_LIBS PREPEND "Boost::"
            OUTPUT_VARIABLE HEADER_ONLY_NAMESPACES)
    list(TRANSFORM LINKED_LIBS PREPEND "Boost::"
            OUTPUT_VARIABLE LINKED_NAMESPACES)
    list(TRANSFORM BUILD_DEPS PREPEND "boost_"
            OUTPUT_VARIABLE PREFIXED_BUILD_DEPS)

    include(FetchContent)
    FetchContent_Declare(
            Boost
            SOURCE_DIR "${CACHE_LOCATION}"
            URL "${rb_boost_github_download_url}"
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

    cmake_path(SET boost_config_include_dir NORMALIZE "${boost_config_SOURCE_DIR}/include")
    get_boost_version(BOOST_VERSION_STR "${boost_config_include_dir}")
    message(STATUS "Library Boost version ${BOOST_VERSION_STR} "
                   "linked ${rb_boost_linkage}ly from ${BOOST_SOURCE_DIR}")
    target_link_libraries(${EXTENSION_NAME} INTERFACE ${HEADER_ONLY_NAMESPACES})
    target_link_libraries(${EXTENSION_NAME} PRIVATE ${LINKED_NAMESPACES})
    add_dependencies(${EXTENSION_NAME} ${PREFIXED_BUILD_DEPS})

    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/boost_stacktrace.cmake")
endblock()
set(BOOST_STACKTRACE_USE_${BOOST_STACKTRACE_BACKEND_UPPER} ON)
