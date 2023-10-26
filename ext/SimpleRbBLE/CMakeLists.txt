cmake_minimum_required(VERSION 3.25.1)
project(SimpleRbBLE LANGUAGES CXX)

include(FindBacktrace)
include(FindPkgConfig)
include(FindBoost)
include(FindRuby)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)
set(Boost_USE_STATIC_LIBS ON)
set(_Ruby_DEBUG_OUTPUT ON)

#set(RUBY_FIND_VERSION 3.2)
find_package(Ruby 3.2 REQUIRED Ruby_FIND_VIRTUALENV) # TODO: set min version #
find_package(Boost OPTIONAL_COMPONENTS exception stacktrace_backtrace stacktrace_addr2line stacktrace_basic)
find_package(simpleble REQUIRED)
find_path(VALGRIND valgrind/valgrind.h)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES AND EXISTS "${CMAKE_SOURCE_DIR}/../../.git")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build.")
endif()

include_directories(.)
include_directories(helpers)
# For config.h
include_directories("${CMAKE_CURRENT_BINARY_DIR}")


add_library(SimpleRbBLE SHARED)

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0 -fdiagnostics-color=always -fno-inline -fno-eliminate-unused-debug-symbols -fno-eliminate-unused-debug-types -femit-class-debug-always -fno-indirect-inlining -fno-partial-inlining -fno-omit-frame-pointer -ggnu-pubnames -ginline-points -gstatement-frontiers -gvariable-location-views -fdiagnostics-color=always -fconcepts-diagnostics-depth=1")
target_link_options(SimpleRbBLE PUBLIC LINKER:--no-strip-discarded LINKER:--discard-none LINKER:--ld-generated-unwind-info)

target_link_libraries(SimpleRbBLE PRIVATE ruby)
target_include_directories(SimpleRbBLE PRIVATE ${Ruby_INCLUDE_DIRS})

# find rice path:
# Alt command: ${Ruby_EXECUTABLE} -rrubygems -e "puts Gem::Specification.find_by_path('mkmf-rice.rb').gem_dir"
#execute_process(COMMAND rvm-exec ${Ruby_VERSION} gem which mkmf-rice.rb OUTPUT_VARIABLE RICE_PATH OUTPUT_STRIP_TRAILING_WHITESPACE  COMMAND_ERROR_IS_FATAL ANY COMMAND_ECHO STDERR)
#Setting this manually for the moment
set(RICE_PATH /usr/local/rvm/gems/ruby-3.2.2-dbg/gems/rice-4.1.0/lib/mkmf-rice.rb)
cmake_path(SET RICE_INCLUDE_PATH NORMALIZE "${RICE_PATH}/../../include")
message(STATUS Rice Path: ${RICE_INCLUDE_PATH})
target_include_directories(SimpleRbBLE PRIVATE ${RICE_INCLUDE_PATH})


target_link_libraries(SimpleRbBLE PRIVATE simpleble)
include(${simpleble_CONFIG})


if (CMAKE_BUILD_TYPE=="Debug")
    target_compile_definitions(SimpleRbBLE PUBLIC SIMPLERBBLE_DEBUG)
endif ()

if (Boost_exception_FOUND)
    target_link_libraries(SimpleRbBLE INTERFACE boost_exception)
endif()

if (Backtrace_FOUND AND Boost_stacktrace_backtrace_FOUND)
    set(BOOST_STACKTRACE_LIBRARY Boost::stacktrace_backtrace)
    target_link_libraries(SimpleRbBLE PRIVATE Boost::stacktrace_backtrace)
elseif (Boost_stacktrace_addr2line_FOUND AND CMAKE_ADDR2LINE)
    set(BOOST_STACKTRACE_LIBRARY Boost::stacktrace_addr2line)
    target_link_libraries(SimpleRbBLE PRIVATE Boost::stacktrace_addr2line)
elseif (Boost_stacktrace_basic_FOUND)
    set(BOOST_STACKTRACE_LIBRARY Boost::stacktrace_basic)
    target_link_libraries(SimpleRbBLE PRIVATE Boost::stacktrace_basic)
elseif(FALSE) # TODO: figure out how to test for the presence of just the headers
    target_link_libraries(SimpleRbBLE INTERFACE Boost::stacktrace) # does this need to be present for the above as well?
    set(BOOST_STACKTRACE_LIBRARY Boost::stacktrace)
endif()



#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

configure_file(config.h.in config.h)

target_precompile_headers(SimpleRbBLE PRIVATE
        Adapter.hpp
        ByteArray.hpp
        Characteristic.hpp
        common.hpp
        Descriptor.hpp
        helpers/Callback.hpp
        helpers/exceptions.hpp
        helpers/identifiers.hpp
        helpers/InRuby.hpp
        helpers/LambdaProps.hpp
        helpers/metaprogramming.hpp
        helpers/NamedBitSet.hpp
        helpers/NamedBitSet.ipp
        helpers/RegistryFactory.hpp
        helpers/ResourceIdentifiers.ipp
#        helpers/ResourceUniqueIdentifier.hpp
        Peripheral.hpp
        Registry.hpp
        RubyQueue.hpp
        Service.hpp
        types.hpp
        utils.hpp)
target_sources(SimpleRbBLE PUBLIC
        Adapter.cpp
        Adapter.hpp
        ByteArray.cpp
        ByteArray.hpp
        Characteristic.cpp
        Characteristic.hpp
        common.hpp
        Descriptor.cpp
        Descriptor.hpp
        gc.cpp
        helpers/Callback.hpp
        helpers/exceptions.hpp
        helpers/identifiers.hpp
        helpers/InRuby.hpp
        helpers/LambdaProps.hpp
        helpers/metaprogramming.hpp
        helpers/NamedBitSet.hpp
        helpers/NamedBitSet.ipp
        helpers/RegistryFactory.hpp
        helpers/ResourceIdentifiers.ipp
#        helpers/ResourceUniqueIdentifier.hpp
#        helpers/ResourceUniqueIdentifier.ipp
        Peripheral.cpp
        Peripheral.hpp
        Registry.cpp
        Registry.hpp
        RubyQueue.cpp
        RubyQueue.hpp
        Service.cpp
        Service.hpp
        SimpleRbBLE.cpp
        types.hpp
        utils.cpp
        utils.hpp)