# frozen_string_literal: true

# TODO: see if I can bypass needing to prefix commands with `bundle exec` via this:
#ENV["BUNDLE_GEMFILE"] ||= File.expand_path("./Gemfile", __dir__)
require "bundler/setup" # Set up gems listed in the Gemfile.
Bundler.require(*%i[dev build])

require "bundler/gem_tasks"

#noinspection RubyMismatchedArgumentType
Pathname.new(__dir__).glob('lib/tasks/*.rake').each { |r| import r }

begin
  require "rubocop/rake_task"
  RuboCop::RakeTask.new
rescue LoadError
  STDERR.puts "Rubocop not installed. Skipping associated tasks."
end

#require 'rake/extensiontask'
# TODO: figure out how to build/install the gem with precompiled binaries
# ...or at least debug why systemwide installation can be finnicky
# EXTENSION_NAME = 'rubble'
#Rake::ExtensionTask.new(EXTENSION_NAME) do |ext|
#  ext.lib_dir = "lib/#{EXTENSION_NAME}"
#end


# TODO: configure rubocop
task default: :rubocop

