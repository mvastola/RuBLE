block(PROPAGATE ruble_boost_setup BOOST_STACKTRACE_BACKEND
        BOOST_STACKTRACE_BACKTRACE_INCLUDE_FILE )

    set(STACKTRACE_BACKEND_PRIORITIES backtrace;addr2line;windgb_cached;windbg;basic;noop)
    set(BOOST_ENABLE_PYTHON OFF)
    set(BUILD_SHARED_LIBS ${rb_boost_dynamic})
    set(Boost_DEBUG OFF)
    set(Boost_VERBOSE OFF)
    set(BUILD_TESTING OFF)

    # TODO: add option to use local boost (in which case we can link dynamically)
    cmake_path(SET CACHE_LOCATION NORMALIZE
            "${CMAKE_CURRENT_SOURCE_DIR}/../../tmp/${rb_boost_release_tag}")

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

    target_link_libraries(${build_target} INTERFACE ${HEADER_ONLY_NAMESPACES})
    target_link_libraries(${build_target} PRIVATE ${LINKED_NAMESPACES})
    add_dependencies(${build_target} ${PREFIXED_BUILD_DEPS})


endblock()