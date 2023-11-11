cmake_minimum_required(VERSION 3.25.1) # TODO: see if this can/should be lowered
project(RubBLE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23) # TODO: see if this will compile with C++20 (I think it will)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LIBRARY_PREFIX  "") # So target file isn't librubble.so
set(STACKTRACE_BACKEND_PRIORITIES backtrace;addr2line;windgb_cached;windbg;basic)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# TODO: why can't I enable this without a compile error(?)

# relies on extconf.rb having been run
cmake_path(SET INSTALL_DIR NORMALIZE "${CMAKE_SOURCE_DIR}/../../lib/rubble")

include(${CMAKE_CURRENT_SOURCE_DIR}/build-config.cmake)
set(RUBY_ARCH ${rb_target_cpu}-${rb_target_os})
set(RICE_INCLUDE_PATH ${rice_include_dir})
set(RICE_VERSION ${rice_version})
set(EXTENSION_NAME ${extconf_name})
set(EXTENSION_VERSION ${extconf_version})
set(DEBUG_MODE ${extconf_debug_build})
#set(SIMPLEBLE_EXTRACT_DIR ${rb_simpleble_install_location})

include(RubBLEHelpers.cmake)

# I was having trouble with this not finding my personal rvm. we could use build-config.json here
find_path(VALGRIND_HEADER_DIR valgrind/valgrind.h)
set(HAVE_VALGRIND ${VALGRIND_HEADER_DIR})


include_directories(.)
# For config.h
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(rubble SHARED)

target_compile_options(rubble PUBLIC
        -ggdb3 # TODO: strip debug info into separate file (don't necessarily need to package it with distributable version)
        -fthreadsafe-statics
)

target_link_options(rubble PUBLIC
    LINKER:-z,defs
    LINKER:-fPIC
    LINKER:--dynamic-list-cpp-typeinfo
    LINKER:--export-dynamic
    LINKER:--demangle
)

# TODO: should this be debug only? (need to figure out what normal install paths are and where they come from)
cmake_path(SET CMAKE_INSTALL_PREFIX NORMALIZE "${CMAKE_SOURCE_DIR}/../../lib/rubble")

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR DEBUG_MODE)
    set(RUBBLE_DEBUG true)
#    set(CMAKE_VERBOSE_MAKEFILE ON)
    target_compile_options(rubble PUBLIC
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
    target_link_options(rubble PUBLIC
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

setup_ruby(rubble)
setup_boost(rubble)
setup_simpleble(rubble)
configure_file(config.h.in config.h)

target_precompile_headers(rubble PRIVATE
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
target_sources(rubble PRIVATE
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
target_sources(rubble PUBLIC
    ./bindings/RubBLE.cpp # The only source file that defines exported functions
)

# TODO: `install(EXPORT...` to generate .cmake files for import
install(TARGETS rubble DESTINATION "${INSTALL_DIR}" CONFIGURATIONS Debug RENAME rubble.so)

#print_target_properties(rubble)
#print_target_properties(Boost::exception)
#print_target_properties(simpleble::simpleble)
#dump_variables()
