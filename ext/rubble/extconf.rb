# frozen_string_literal: true

# FIXME: This won't work because extensions aren't built from inside the gem dir when installed
# ENV["BUNDLE_GEMFILE"] ||= File.expand_path("../../Gemfile", __dir__)
# require "bundler/setup" # Set up gems listed in the Gemfile.
Bundler.require(:build)
require_relative '../../lib/rubble/build'

require 'mkmf-rice'
# TODO: see what https://rubygems.org/gems/autobuild has to offer

@generator = Rubble::Build::Extconf.new
@generator.setup_simpleble!

require 'json'
OUT_FILE = Pathname.new(__dir__) / 'build-config.json'
#noinspection RubyMismatchedArgumentType
OUT_FILE.write(JSON.pretty_generate(@generator.config_data))

# Just to silence
# SystemExit.new "*** extconf.rb failed ***\nCould not create Makefile due to some reason"
# TODO: need to confirm the values (for install dir, compile args, etc) outputted in Makefile
#   are the same values CMake gets from parsing 'build-config.json'
create_makefile 'rubble'