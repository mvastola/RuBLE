# frozen_string_literal: true

# TODO: see if I can bypass needing to prefix rake commands with `bundle exec` via this:

# ENV["BUNDLE_GEMFILE"] ||= File.expand_path("./Gemfile", __dir__)
require 'bundler/setup' # Set up gems listed in the Gemfile.
Bundler.require(*%i[dev build])

require 'bundler/gem_tasks'
require 'rubygems/package_task'

# noinspection RubyMismatchedArgumentType
ROOT_DIR = Pathname.new(__dir__)
ROOT_DIR.glob('lib/tasks/*.rake').each { |r| import r }

GEMSPECS = ROOT_DIR.glob('*.gemspec')

GEMSPECS.each do |gemspec|
  spec = Bundler.load_gemspec(gemspec.to_s)
  Gem::PackageTask.new(spec).define
end

begin
  require 'rubocop/rake_task'

  RuboCop::RakeTask.new do |task|
    task.requires << 'rubocop-rake'
  end
  task default: :rubocop
rescue LoadError
  warn 'Rubocop not installed. Skipping reverse-dependent rake tasks.'
end

# require 'rake/extensiontask'
#  TODO: figure out how to build/install the gem with precompiled binaries
#  ...or at least debug why systemwide installation can be finnicky
# Rake::ExtensionTask.new(EXTENSION_NAME) do |ext|
#   ext.lib_dir = "lib/#{EXTENSION_NAME}"
# end



