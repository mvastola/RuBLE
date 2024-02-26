cmake_minimum_required(VERSION 3.25.1) # TODO: see if this can/should be lowered
project(RuBLE LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23) # TODO: see if this will compile with C++20 (I think it will)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_SHARED_LIBRARY_PREFIX  "") # So target file isn't libruble.so
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_TLS_VERIFY ON)

include("${CMAKE_CURRENT_SOURCE_DIR}/build-config.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/helpers.cmake")

set(RUBY_ARCH "${rb_target_cpu}-${rb_target_os}")
set(RICE_INCLUDE_PATH "${rice_include_dir}")
set(RICE_VERSION "${rice_version}")
set(EXTENSION_NAME "${rb_ext_name}")
set(EXTENSION_VERSION "${rb_ext_version}")

if (NOT DEFINED RUBLE_DEBUG)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(RUBLE_DEBUG true)
  else()
    set(RUBLE_DEBUG ${build_flags_debug})
  endif()
endif()

if (NOT DEFINED RUBLE_DEVELOPER)
  set(RUBLE_DEVELOPER ${build_flags_developer})
endif()

if (NOT DEFINED RUBLE_VERBOSE)
  set(RUBLE_VERBOSE ${build_flags_verbose})
endif()

if (NOT DEFINED RUBLE_RELEASE)
  set(RUBLE_RELEASE ${build_flags_release})
endif()
#cmake_host_system_information(RESULT NPROC QUERY NUMBER_OF_PHYSICAL_CORES)

#if(RUBLE_DEBUG OR RUBLE_DEVELOPER)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/debug.cmake")
#endif()

if(RUBLE_VERBOSE)
  set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

find_path(VALGRIND_HEADER_DIR valgrind/valgrind.h)
set(HAVE_VALGRIND "${VALGRIND_HEADER_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")


# For config.h
message("Current bin dir: ${CMAKE_BINARY_DIR}")
include_directories("${CMAKE_BINARY_DIR}")

add_library(${EXTENSION_NAME} SHARED)

# TODO: these REPLACEs will likely cause problems if there are any pathnames with spaces in the RbConfig vars
string(REPLACE " " ";" rb_CFLAGS ${rb_CFLAGS})
list(REMOVE_ITEM rb_CFLAGS "-Wold-style-definition" "-Wimplicit-int" "-Wimplicit-function-declaration" "-Wno-self-assign" "-Wno-parentheses-equality" "-Wno-constant-logical-operand")
# TODO: make target non-specific

target_compile_options(${EXTENSION_NAME} PUBLIC
        ${rb_CFLAGS}
        -ggdb3 # TODO: strip debug info into separate file (don't necessarily need to package it with distributable version)
        -fthreadsafe-statics
)

# TODO: these REPLACEs will likely cause problems if there are any pathnames with spaces in the RbConfig vars
string(REPLACE " " ";" rb_LDFLAGS ${rb_LDFLAGS})
string(REPLACE " " ";" rb_DLDFLAGS ${rb_DLDFLAGS})
# TODO: make target non-specific
target_link_options(${EXTENSION_NAME} PUBLIC
    ${rb_LDFLAGS}
    ${rb_DLDFLAGS}
    LINKER:-fPIC
    LINKER:--dynamic-list-cpp-typeinfo
    #    LINKER:--export-dynamic
    #LINKER:--demangle
)

# TODO: should this be debug only? (need to figure out what normal install paths are and where they come from)
cmake_path(SET CMAKE_INSTALL_PREFIX
        NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/ruble")

if(RUBLE_DEBUG)
    # TODO: make target non-specific
    # TODO: only a few of these are likely needed
    target_compile_options(${EXTENSION_NAME} PUBLIC
            -ggdb3
            -O0
            -fdiagnostics-color=always
            -fno-eliminate-unused-debug-symbols
            -fno-eliminate-unused-debug-types
            -femit-class-debug-always
            -fno-indirect-inlining
            -fno-partial-inlining
            -fno-omit-frame-pointer
            -ggnu-pubnames
            -ginline-points
            -gstatement-frontiers
            -gvariable-location-views
            -fdiagnostics-color=always
            -fconcepts-diagnostics-depth=1
    )
    # TODO: make target non-specific (so it applies to boost/simpleble)
    target_link_options(${EXTENSION_NAME} PUBLIC
            LINKER:--no-strip-discarded
            LINKER:--discard-none
            LINKER:--ld-generated-unwind-info
            LINKER:--gc-keep-exported
            LINKER:--no-gc-sections
            LINKER:--no-copy-dt-needed-entries
            LINKER:--unresolved-symbols=report-all
            LINKER:--enable-new-dtags
            LINKER:-O0
            LINKER:--ctf-variables
    )
endif()
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ruby.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/boost.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/simpleble.cmake")

configure_file(config.h.in "${CMAKE_BINARY_DIR}/config.h")

target_include_directories(${EXTENSION_NAME} BEFORE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_precompile_headers(${EXTENSION_NAME} PRIVATE
        ${CMAKE_BINARY_DIR}/config.h
        ./management/RegistryFactory.hpp
        ./management/Registry.hpp
        ./management/RubyQueue.hpp
        ./bindings/Characteristic.hpp
        ./bindings/common.hpp
        ./bindings/Service.hpp
        ./bindings/Adapter.hpp
        ./bindings/Descriptor.hpp
        ./bindings/Peripheral.hpp
        ./utils/containers.hpp
        ./utils/hexadecimal.hpp
        ./utils/RubyCallbackTraits.hpp
        ./utils/ruby_context.hpp
        ./utils/ruby.hpp
        ./utils/human_type_names.hpp
        ./utils/inspection.hpp
        ./utils/hash.hpp
        ./utils/garbage_collection.hpp
        ./utils/async.hpp
        ./utils/exception_handling.hpp
        ./containers/ByteArray.hpp
        ./containers/NamedBitSet.hpp
        ./containers/NamedBitSet.ipp
        ./containers/Callback.hpp
        ./types/SimpleBLE.hpp
        ./types/ruby.hpp
        ./types/helpers.hpp
        ./types/stl.hpp
        ./types/declarations.hpp
        ./concerns/CharacteristicValueTracker.hpp
        ./concerns/Rubyable.hpp
)
target_sources(${EXTENSION_NAME} PRIVATE
        ./management/RubyQueue.cpp
        ./management/Registry.cpp
        ./bindings/Service.cpp
        ./bindings/globals.cpp
        ./bindings/Descriptor.cpp
        ./bindings/Adapter.cpp
        ./bindings/Peripheral.cpp
        ./bindings/Characteristic.cpp
        ./utils/hash.cpp
        ./utils/exception_handling.cpp
        ./utils/inspection.cpp
        ./utils/RubyCallbackTraits.cpp
        ./utils/garbage_collection.cpp
        ./utils/async.cpp
        ./containers/ByteArray.cpp
        ./concerns/CharacteristicValueTracker.cpp
        ./concerns/Rubyable.cpp
)

# TODO: does PUBLIC vs PRIVATE in target_sources actually do anything here?
target_sources(${EXTENSION_NAME} PUBLIC
    ./bindings/RuBLE.cpp # The only source file that defines exported functions
)

target_link_libraries(${EXTENSION_NAME} PUBLIC stdc++)
if (RUBLE_DEVELOPER) # This is probably only wanted locally
    cmake_path(SET INSTALL_DIR NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/ruble")
    message(STATUS "Install dir is ${INSTALL_DIR}")
    # TODO(?): `install(EXPORT...` to generate .cmake files for import
    install(TARGETS ${EXTENSION_NAME} DESTINATION "${INSTALL_DIR}" CONFIGURATIONS Debug RENAME ruble.so)
endif()

#print_target_properties(ruble)
#print_target_properties(Boost::exception)
#print_target_properties(simpleble::simpleble)
#dump_variables()
