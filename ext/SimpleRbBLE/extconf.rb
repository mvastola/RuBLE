# TODO: is there a way to use the rice version depended on by the gem to build?

# FIXME: This won't work because extensions aren't built from inside the gem dir when installed
#ENV["BUNDLE_GEMFILE"] ||= File.expand_path("../../Gemfile", __dir__)
#require "bundler/setup" # Set up gems listed in the Gemfile.

require 'mkmf-rice'

# FIXME: hardcoded for now, but need to figure out best way to implement this
# Ideally (at least in dev), it should clone the git repo for SimpleBLE (a git submodule in ./contrib),
# build it, and statically link in the library so it's not a runtime dependency to be installed system wide.
# Users should be able to specify a custom library path though.

# Remove any builtin CXX standards version
$CXXFLAGS.gsub!(/(\s+|\b)-?-std=c\+\+[A-Za-z0-9-]+(\s+|\b)/, ' ')

# just tooling around here for now
new_cppflags = %w[
  -ggdb3
	-O0
	-std=c++23
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
	-DDEBUG
]

	#-fopt-info
	#-ffunction-cse
	#-fipa-reference
	#-fipa-reference-addressable
append_cppflags " #{new_cppflags.join(' ')}"
pkg_config 'simpleble'
append_ldflags ' -Wl,--no-strip-discarded -Wl,--discard-none -Wl,--ld-generated-unwind-info'
#--gc-keep-exported --no-gc-sections 
append_library 'simpleble-debug', 'simpleble'

# Mkmf doesn't natively support subdirs
#$srcs = Dir[File.join(__dir__, "**/*.{#{SRC_EXT.join(%q{,})}}")].sort
#$objs = $srcs.map { _1.sub(/\.#{Regexp.union SRC_EXT}\z/, ".#{$OBJEXT}") }

# I think there might be an issue in rice's override of pkg_config.. getting some intermittent issues I need to trace
#pkg_config('simpleble')
#dir_config('simpleble', File.expand_path('../../contrib/SimpleBLE/build_simpleble/install', __dir__))
create_makefile('SimpleRbBLE')
