# TODO: is there a way to use the rice version depended on by the gem to build?

# FIXME: This won't work because extensions aren't built from inside the gem dir when installed
#ENV["BUNDLE_GEMFILE"] ||= File.expand_path("../../Gemfile", __dir__)
#require "bundler/setup" # Set up gems listed in the Gemfile.

require 'mkmf-rice'

EXTENSION_NAME = 'SimpleRbBLE'
DEBUG_MODE = with_config("debug") || ENV["#{EXTENSION_NAME.upcase}_DEBUG_ON"]
#
# Remove any builtin CXX standards version
$CXXFLAGS.gsub!(/(\s+|\b)-?-std=c\+\+[A-Za-z0-9-]+(\s+|\b)/, ' ')
#$CXXFLAGS += ' -fPIC '
append_cppflags	' -std=c++23 '

# TODO: preferably link statically
pkg_config 'simpleble'
lib_name = DEBUG_MODE ? 'simpleble-debug' : 'simpleble'
$LOCAL_LIBS += " -l#{lib_name} "
$libs.sub!(/(\b|\s+)-l#{Regexp.escape lib_name}(\b|\s+)/, ' ')
#have_library('dbus-1')

require_relative 'includes/boost_stacktrace'
BoostStacktrace.configure!(required: DEBUG_MODE)

if DEBUG_MODE
  have_header 'valgrind/valgrind.h'
  # FIXME: getting error that this isn't supported
  append_ldflags ' -Wl,--no-strip-discarded -Wl,--discard-none -Wl,--ld-generated-unwind-info '
  #--gc-keep-exported --no-gc-sections 

  $defs.push("-DSIMPLERBBLE_DEBUG") unless $defs.include? "-DSIMPLERBBLE_DEBUG"

  # just tooling around here for now
  append_cppflags %w[
    -ggdb3
    -O0
    -fno-inline
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
  ].join(' ').prepend(' ')
	#-fopt-info
	#-ffunction-cse
	#-fipa-reference
	#-fipa-reference-addressable
end

MakeMakefile::HDR_EXT << 'ipp'
$LOCAL_LIBS = "-Wl,-Bstatic #{$LOCAL_LIBS} -Wl,-Bdynamic " unless DEBUG_MODE
$preload ||= []
#$preload << "\nLDSHAREDXX += -Wl,-fPIC\n" 
create_header
create_makefile(EXTENSION_NAME)
