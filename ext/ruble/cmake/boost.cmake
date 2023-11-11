block(PROPAGATE
        BOOST_STACKTRACE_BACKEND
        BOOST_STACKTRACE_BACKEND_UPPER
        BOOST_STACKTRACE_LINK
        BOOST_STACKTRACE_BACKTRACE_INCLUDE_FILE
        BOOST_STACKTRACE_ADDR2LINE_LOCATION)

    set(STACKTRACE_BACKEND_PRIORITIES backtrace;addr2line;windgb_cached;windbg;basic;noop)
    set(BOOST_ENABLE_PYTHON OFF)
    set(BUILD_SHARED_LIBS ${rb_boost_dynamic})
    set(Boost_DEBUG OFF)
    set(Boost_VERBOSE ${build_mode_verbose})
    set(BUILD_TESTING OFF)

    # TODO: handle option to use local boost (in which case we can link dynamically)
    cmake_path(SET CACHE_LOCATION NORMALIZE
            "${CMAKE_CURRENT_SOURCE_DIR}/../../tmp/${rb_boost_release_tag}")

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
    #    dump_variables()
    #    message(FATAL_ERROR "quitting")

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

#    message(STATUS ${BOOST_INCLUDE_LIBRARIES})
#    message(STATUS ${HEADER_ONLY_NAMESPACES})
    message(STATUS "Boost build target: ${EXTENSION_NAME}")
    target_link_libraries(${EXTENSION_NAME} INTERFACE ${HEADER_ONLY_NAMESPACES})
    target_link_libraries(${EXTENSION_NAME} PRIVATE ${LINKED_NAMESPACES})
    add_dependencies(${EXTENSION_NAME} ${PREFIXED_BUILD_DEPS})

    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/boost_stacktrace.cmake")

endblock()
set(BOOST_STACKTRACE_USE_${BOOST_STACKTRACE_BACKEND_UPPER} ON)