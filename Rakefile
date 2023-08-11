# frozen_string_literal: true

# TODO: see if I can bypass needing to prefix commands with `bundle exec` via this:
#ENV["BUNDLE_GEMFILE"] ||= File.expand_path("./Gemfile", __dir__)
#require "bundler/setup" # Set up gems listed in the Gemfile.
#Bundler.require

require "bundler/gem_tasks"
require "rubocop/rake_task"

RuboCop::RakeTask.new

require 'rake/extensiontask'
# TODO: figure out how to build/install the gem with precompiled binaries
# ...or at least debug why systemwide installation can be finnicky
Rake::ExtensionTask.new('SimpleRbBLE') do |ext|
  ext.lib_dir = 'lib/SimpleRbBLE'
end

# TODO: configure rubocop
task default: :rubocop

