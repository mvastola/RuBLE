# frozen_string_literal: true

# FIXME: This won't work because extensions aren't built from inside the gem dir when installed
# ENV["BUNDLE_GEMFILE"] ||= File.expand_path("../../Gemfile", __dir__)
# require "bundler/setup" # Set up gems listed in the Gemfile.

Bundler.require(:build)
# TODO: ideally I'd do this later on, but I'm not sure if this changes RbConfig at all (which could easily get cached)
require 'mkmf-rice'

require_relative '../../lib/ruble/build'
Bundler.require(:dev) if RuBLE::Build::Extconf.developer?
Bundler.require(:release) if RuBLE::Build::Extconf.release?
# TODO(?): see what https://rubygems.org/gems/autobuild has to offer

# noinspection RubyMismatchedArgumentType
THIS_DIR = Pathname.new(__dir__)
debugger if defined?(debugger) && RuBLE::Build::Extconf.irb? # rubocop:disable Lint/Debugger

RuBLE::Build::Extconf.write_build_config(path: THIS_DIR / 'build-config.cmake')

# TODO: need to confirm the values (for install dir, compile args, etc) outputted in Makefile
#   are the same values CMake gets from reading 'build-config.json'

create_makefile 'PLACEHOLDER'
File.open(RuBLE::Build::Environment::Extension.ext_dir / 'Makefile', 'at') do |f|
  f.puts '# include cmake.mk'
end


# TODO: does setting TARGET_ENTRY in makefile do anything? (if so, use it)
