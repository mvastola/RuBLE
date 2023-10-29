cmake_minimum_required(VERSION 3.25.1)
project(SimpleRbBLE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LIBRARY_PREFIX  "")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_DEBUG ON)
#set(_Ruby_DEBUG_OUTPUT ON)
set(CMAKE_INSTALL_DO_STRIP OFF)
set(STACKTRACE_BACKEND_PRIORITIES backtrace;addr2line;windgb_cached;windbg;basic)
set(RUBY_FIND_VERSION 3.2)
set(BOOST_STACKTRACE_LINK ON)

# relies on extconf.rb having been run
cmake_path(SET INSTALL_DIR NORMALIZE "${CMAKE_SOURCE_DIR}/../../lib/SimpleRbBLE")
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/build-config.json JSON_BUILD_CONFIG)
string(JSON RUBY_ARCH GET "${JSON_BUILD_CONFIG}" rbconfig arch)
string(JSON RICE_INCLUDE_PATH GET "${JSON_BUILD_CONFIG}" rice include_dir)
string(JSON RICE_VERSION GET "${JSON_BUILD_CONFIG}" rice version)
string(JSON EXTENSION_NAME GET "${JSON_BUILD_CONFIG}" extension name)
string(JSON EXTENSION_VERSION GET "${JSON_BUILD_CONFIG}" extension version)
string(JSON DEBUG_MODE GET "${JSON_BUILD_CONFIG}" extension debug_build)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES AND EXISTS "${CMAKE_SOURCE_DIR}/../../.git")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build.")
elseif (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES AND ENV{SIMPLERBBLE_DEBUG_ON} OR "${DEBUG_MODE}")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build.")
endif()

include(SimpleRbBLEHelpers.cmake)
include(FindBacktrace)
#include(FindPkgConfig)
#include(FindBoost)
#include(FindRuby)

# I was having trouble with this not finding my personal rvm. we could use build-config.json here
find_package(Ruby 3.2 REQUIRED Ruby_FIND_VIRTUALENV)
find_package(Boost OPTIONAL_COMPONENTS headers stacktrace_backtrace stacktrace_addr2line stacktrace_basic stacktrace_windbg stacktrace_windbg_cached)
find_path(VALGRIND_HEADER_DIR valgrind/valgrind.h)
set(HAVE_VALGRIND ${VALGRIND_HEADER_DIR})

find_package(simpleble REQUIRED)

include_directories(.)
# For config.h
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(SimpleRbBLE SHARED)

add_compile_options(-fthreadsafe-statics)
#add_compile_options(-fPIC -Wl,-fPIC)
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0 -fdiagnostics-color=always -fno-eliminate-unused-debug-symbols -fno-eliminate-unused-debug-types -femit-class-debug-always -fno-indirect-inlining -fno-partial-inlining -fno-omit-frame-pointer -ggnu-pubnames -ginline-points -gstatement-frontiers -gvariable-location-views -fdiagnostics-color=always -fconcepts-diagnostics-depth=1")
target_link_options(SimpleRbBLE PUBLIC
        LINKER:--no-strip-discarded
        LINKER:--discard-none
#        LINKER:--ld-generated-unwind-info
        LINKER:-z,defs
        LINKER:-z,now
        LINKER:-fPIC
#        LINKER:--embedded-relocs
) # LINKER:--gc-keep-exported LINKER:--no-gc-sections

target_link_libraries(SimpleRbBLE PUBLIC ${Ruby_LIBRARIES})
target_include_directories(SimpleRbBLE PUBLIC ${Ruby_INCLUDE_DIRS})

target_include_directories(SimpleRbBLE PRIVATE ${RICE_INCLUDE_PATH})
target_link_libraries(SimpleRbBLE PUBLIC simpleble::simpleble)
include(${simpleble_CONFIG})

setup_boost(SimpleRbBLE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SIMPLERBBLE_DEBUG true)
    cmake_path(SET CMAKE_INSTALL_PREFIX NORMALIZE "${CMAKE_SOURCE_DIR}/../../lib/SimpleRbBLE")
    set_target_properties(SimpleRbBLE PROPERTIES INSTALL_RPATH_USE_LINK_PATH ON)
endif()

configure_file(config.h.in config.h)

target_precompile_headers(SimpleRbBLE PRIVATE
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
        ./utils/CallableTraits.hpp
        ./utils/ruby.hpp
        ./utils/human_type_names.hpp
        ./utils/inspection.hpp
        ./utils/hash.hpp
        ./utils/endianness.hpp
        ./utils/garbage_collection.hpp
        ./utils/async.hpp
        ./utils/exception_handling.hpp
        ./containers/ByteArray.hpp
        ./containers/NamedBitSet.hpp
        ./containers/NamedBitSet.ipp
        ./containers/Callback.hpp
        ./types/spans.hpp
        ./types/SimpleBLE.hpp
        ./types/ruby.hpp
        ./types/ranges.hpp
        ./types/helpers.hpp
        ./types/chrono.hpp
        ./types/declarations.hpp
)
target_sources(SimpleRbBLE PUBLIC
        ./management/RubyQueue.cpp
        ./management/Registry.cpp
        ./bindings/SimpleRbBLE.cpp
        ./bindings/Service.cpp
        ./bindings/globals.cpp
        ./bindings/Descriptor.cpp
        ./bindings/Adapter.cpp
        ./bindings/Peripheral.cpp
        ./bindings/Characteristic.cpp
        ./utils/hexadecimal.cpp
        ./utils/hash.cpp
        ./utils/exception_handling.cpp
        ./utils/inspection.cpp
        ./utils/CallableTraits.cpp
        ./utils/garbage_collection.cpp
        ./utils/async.cpp
        ./utils/ruby.cpp
        ./containers/ByteArray.cpp
)

# TODO: `install(EXPORT...` to generate .cmake files for import
install(TARGETS SimpleRbBLE DESTINATION "${INSTALL_DIR}" CONFIGURATIONS Debug RENAME SimpleRbBLE.so)
#print_target_properties(SimpleRbBLE)
