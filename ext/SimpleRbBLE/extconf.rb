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

$CXXFLAGS << ' -ggdb3 -O0 -std=c++23 -fno-inline'
#$CPPFLAGS << ' '
$libs << ' -L/usr/local/lib -lsimpleble-debug'


# I think there might be an issue in rice's override of pkg_config.. getting some intermittent issues I need to trace
#pkg_config('simpleble')
#dir_config('simpleble', File.expand_path('../../contrib/SimpleBLE/build_simpleble/install', __dir__))
create_makefile('SimpleRbBLE')
