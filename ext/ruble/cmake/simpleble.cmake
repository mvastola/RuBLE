macro(ConfigurePrecompiledSimpleBLE)
    include(FetchContent)
    # TODO: would it be easier/better to use git and the commit hash
    #   and build SimpleBLE from source? The main downside is more
    #   dependencies. The upside is vastly simplifying configuring
    #   simpleble_DIR, as well as choosing between shared/static linking.
    message(STATUS "Downloading precompiled SimpleBLE from ${rb_simpleble_github_download_url}")
    FetchContent_Declare(
            simpleble
            URL "${rb_simpleble_github_download_url}"
            DOWNLOAD_EXTRACT_TIMESTAMP ON
            #            OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(simpleble)
    set(simpleble_DIR ${simpleble_SOURCE_DIR}/${rb_simpleble_github_archive_path})
    find_package(simpleble REQUIRED CONFIG PATHS "${simpleble_DIR}" NO_DEFAULT_PATH)
    cmake_path(SET SIMPLEBLE_PC_DIR NORMALIZE "${simpleble_DIR}/../../pkgconfig")
    get_pkg_config_lib_version(SIMPLEBLE_VERSION simpleble "${SIMPLEBLE_PC_DIR}")
endmacro()

if(${rb_simpleble_precompiled})
    ConfigurePrecompiledSimpleBLE()
else()
    message(FATAL_ERROR "Non-precompiled SimpleBLE not yet supported")
endif()
target_link_libraries(${EXTENSION_NAME} PUBLIC simpleble::simpleble)
include("${simpleble_CONFIG}")

message(STATUS "Library SimpleBLE version ${SIMPLEBLE_VERSION} "
               "linked ${rb_simpleble_linkage}ly from ${simpleble_DIR}")