cmake_minimum_required(VERSION 3.25.1) # TODO: see if this can/should be lowered
project(RuBLE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23) # TODO: see if this will compile with C++20 (I think it will)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LIBRARY_PREFIX  "") # So target file isn't libruble.so
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_TLS_VERIFY ON)

# relies on extconf.rb having been run
cmake_path(SET INSTALL_DIR NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/ruble")

include("${CMAKE_CURRENT_SOURCE_DIR}/build-config.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/debug.cmake")

set(RUBY_ARCH "${rb_target_cpu}-${rb_target_os}")
set(RICE_INCLUDE_PATH "${rice_include_dir}")
set(RICE_VERSION "${rice_version}")
set(EXTENSION_NAME "${extconf_name}")
set(EXTENSION_VERSION "${extconf_version}")
set(DEBUG_MODE ${build_mode_debug})
set(DEV_MODE ${build_mode_developmentd})
set(VERBOSE_MODE ${build_mode_verbose})
set(RELEASE_MODE ${build_mode_release})
#cmake_host_system_information(RESULT NPROC QUERY NUMBER_OF_PHYSICAL_CORES)

# I was having trouble with this not finding my personal rvm. we could use build-config.json here
find_path(VALGRIND_HEADER_DIR valgrind/valgrind.h)
set(HAVE_VALGRIND ${VALGRIND_HEADER_DIR})


include_directories(.)
# For config.h
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library("${EXTENSION_NAME}" SHARED)

target_compile_options("${EXTENSION_NAME}" PUBLIC
        -ggdb3 # TODO: strip debug info into separate file (don't necessarily need to package it with distributable version)
        -fthreadsafe-statics
)

target_link_options("${EXTENSION_NAME}" PUBLIC
    LINKER:-z,defs
    LINKER:-fPIC
    LINKER:--dynamic-list-cpp-typeinfo
    LINKER:--export-dynamic
    LINKER:--demangle
)

# TODO: should this be debug only? (need to figure out what normal install paths are and where they come from)
cmake_path(SET CMAKE_INSTALL_PREFIX NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/ruble")

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR DEBUG_MODE)
    set(RUBLE_DEBUG true)
    set(CMAKE_VERBOSE_MAKEFILE ${build_mode_verbose})
    # TODO: very few of these are likely needed
    target_compile_options("${EXTENSION_NAME}" PUBLIC
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
    target_link_options("${EXTENSION_NAME}" PUBLIC
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

configure_file(config.h.in config.h)

target_precompile_headers("${EXTENSION_NAME}" PRIVATE
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
target_sources("${EXTENSION_NAME}" PRIVATE
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
target_sources("${EXTENSION_NAME}" PUBLIC
    ./bindings/RuBLE.cpp # The only source file that defines exported functions
)

# TODO: `install(EXPORT...` to generate .cmake files for import
install(TARGETS "${EXTENSION_NAME}" DESTINATION "${INSTALL_DIR}" CONFIGURATIONS Debug RENAME ruble.so)

#print_target_properties(ruble)
#print_target_properties(Boost::exception)
#print_target_properties(simpleble::simpleble)
#dump_variables()
