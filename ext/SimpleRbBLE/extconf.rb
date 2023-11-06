# frozen_string_literal: true

# FIXME: This won't work because extensions aren't built from inside the gem dir when installed
# ENV["BUNDLE_GEMFILE"] ||= File.expand_path("../../Gemfile", __dir__)
# require "bundler/setup" # Set up gems listed in the Gemfile.
Bundler.require(:build)

require 'mkmf-rice'
# TODO: see what https://rubygems.org/gems/autobuild has to offer

@generator = SimpleRbBLE::Build::Extconf.new
@generator.setup_simpleble!

require 'json'
OUT_FILE = Pathname.new(__dir__) / 'build-config.json'
File.write(OUT_FILE, JSON.pretty_generate(@generator.config_data))
