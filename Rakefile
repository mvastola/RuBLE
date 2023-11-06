# frozen_string_literal: true

# TODO: see if I can bypass needing to prefix commands with `bundle exec` via this:
#ENV["BUNDLE_GEMFILE"] ||= File.expand_path("./Gemfile", __dir__)
require "bundler/setup" # Set up gems listed in the Gemfile.
Bundler.require(*%i[dev build])


EXTENSION_NAME = 'SimpleRbBLE'
#noinspection RubyMismatchedArgumentType
ROOT_DIR = Pathname.new(__dir__)

#require "bundler/gem_tasks"
begin
  require 'rubygems/tasks'
  Gem::Tasks.new
rescue LoadError
  STDERR.puts "rubygems-tasks not installed. Skipping associated tasks."
end

begin
  require "rubocop/rake_task"
  RuboCop::RakeTask.new
rescue LoadError
  STDERR.puts "Rubocop not installed. Skipping associated tasks."
end

ROOT_DIR.glob('lib/tasks/*.rake').each { |r| import r }
#require 'rake/extensiontask'
# TODO: figure out how to build/install the gem with precompiled binaries
# ...or at least debug why systemwide installation can be finnicky
#Rake::ExtensionTask.new(EXTENSION_NAME) do |ext|
#  ext.lib_dir = "lib/#{EXTENSION_NAME}"
#end


# TODO: configure rubocop
task default: :rubocop

