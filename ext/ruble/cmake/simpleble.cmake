macro(ConfigureCompiledSimpleBLE)
    FetchContent_Declare(
            simpleble
            CMAKE_ARGS "-DBUILD_SHARED_LIBS:BOOL=${rb_simpleble_shared}" "-DSIMPLEBLE_PLAIN:BOOL=OFF"
                       "-DSIMPLEBLE_SANITIZE:STRING=Address" "-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}"
            SOURCE_SUBDIR simpleble
            URL "${rb_simpleble_github_download_url}"
            DOWNLOAD_EXTRACT_TIMESTAMP ON
            OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(simpleble)
    find_package(simpleble REQUIRED NO_DEFAULT_PATH EXCLUDE_FROM_ALL ON)
    cmake_path(SET SIMPLEBLE_PC_DIR NORMALIZE "${simpleble_BINARY_DIR}")

    dump_variables()
endmacro()

macro(ConfigurePrecompiledSimpleBLE)
    FetchContent_Declare(
            simpleble
            URL "${rb_simpleble_github_download_url}"
            DOWNLOAD_EXTRACT_TIMESTAMP ON
    )
    FetchContent_MakeAvailable(simpleble)
    cmake_path(SET simpleble_DIR NORMALIZE
            "${simpleble_SOURCE_DIR}/${rb_simpleble_github_archive_path}/lib/cmake/simpleble")
#    message(STATUS "SimpleBLE Dir: ${simpleble_DIR}")
    find_package(simpleble REQUIRED CONFIG PATHS "${simpleble_DIR}" NO_DEFAULT_PATH)
    cmake_path(SET SIMPLEBLE_PC_DIR NORMALIZE "${simpleble_DIR}/../../pkgconfig")
endmacro()

include(FetchContent)
message(STATUS "Downloading ${rb_simpleble_download_type} SimpleBLE from ${rb_simpleble_github_download_url}")

if(${rb_simpleble_precompiled})
    ConfigurePrecompiledSimpleBLE()
else()
    ConfigureCompiledSimpleBLE()
#    message(FATAL_ERROR "Non-precompiled SimpleBLE not yet supported")
endif()

message(STATUS "SimpleBLE PC Dir: ${SIMPLEBLE_PC_DIR}")
get_pkg_config_lib_version(SIMPLEBLE_VERSION simpleble "${SIMPLEBLE_PC_DIR}")

target_link_libraries(${EXTENSION_NAME} PUBLIC simpleble::simpleble)
include("${simpleble_CONFIG}")

message(STATUS "Library SimpleBLE version ${SIMPLEBLE_VERSION} "
               "linked ${rb_simpleble_linkage}ly from ${simpleble_DIR}")
