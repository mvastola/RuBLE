# FIXME: This won't work because extensions aren't built from inside the gem dir when installed
#ENV["BUNDLE_GEMFILE"] ||= File.expand_path("../../Gemfile", __dir__)
#require "bundler/setup" # Set up gems listed in the Gemfile.

require 'mkmf-rice'
require 'pathname'
require 'json'
require 'os'

EXTENSION_NAME = 'SimpleRbBLE'
DEBUG_MODE = with_config("debug") || ENV["#{EXTENSION_NAME.upcase}_DEBUG_ON"]
ENV["#{EXTENSION_NAME.upcase}_DEBUG_ON"] = 'on' if DEBUG_MODE

@bundler_data = Bundler.load
@this_spec = @bundler_data.specs.find_by_name_and_platform('SimpleRbBLE', RUBY_PLATFORM)
@rice_spec = @bundler_data.specs.find_by_name_and_platform('rice', RbConfig::CONFIG['target'])
@rice_full_path = Pathname.new(@rice_spec.full_gem_path)
@rice_include_dir = @rice_full_path / 'include'
@rice_version = @rice_spec.version

OS_FIELDS = %i[dev_null cpu_count host posix? linux? host_os iron_ruby? bits host_cpu java? windows? mac? jruby? osx? x? freebsd? cygwin?]
@os_data = OS_FIELDS.to_h { |name| [ name.to_s.delete_suffix('?'), OS.send(name) ] }
@os_data['os_release'] = OS.parse_os_release rescue nil

# TODO: Add library metadata (such as version/commit hash, etc) to be passed to ld --package-metadata (https://systemd.io/ELF_PACKAGE_METADATA/)
 
config_data = {
  extension: {
    name: EXTENSION_NAME,
    version: @this_spec.version.to_s,
    debug_build: DEBUG_MODE
  },
  rbconfig: RbConfig::CONFIG.select { |k, v| v && !v.strip.empty? },
  rice: {
    gem_path: @rice_full_path.to_s,
    include_dir: @rice_include_dir.to_s,
    version: @rice_version.to_s
  },
  # Based on https://www.rubydoc.info/gems/bundler/Bundler%2FEnv%2Eenvironment
  bundler: {
    platform: Gem.platforms.join(', '),
    config_dir: Pathname.new(Gem::ConfigFile::SYSTEM_WIDE_CONFIG_FILE).dirname,
    rubygems_version: Gem::VERSION,
    gem_home: Gem.dir,
    gem_path: Gem.path.join(File::PATH_SEPARATOR),
    user_home: Gem.user_dir,
    bindir: Gem.bindir,
  },
  os: @os_data.compact,
}
#config_data.flat_join! # to make cmake happy

# Now download SimpleBLE
# TODO: can we get away with the require_relative? The issue is I think RVM, rubygems, and/or bundler
#   separates the extension dir (though it may just be the install dir) from the gem's root dir
#   so I'm not sure if the relative path from the extconf.rb to lib/tasks/lib is always the same.
#   If that's the case, we need something like the following:
# $:.unshift (Pathname.new(@this_spec.full_gem_path) / 'lib' / 'tasks' / 'lib').to_s
# require 'simpleble'
require_relative '../../lib/tasks/lib/simpleble'

# TODO: we should probably fix a known-working version come release time
@simpleble_release = with_config('simpleble-release') || 'latest'
@simpleble = SimpleBLE.new(tag: @simpleble_release) 
@simpleble.download!
@simpleble.extract!
@simpleble.install!

config_data[:simpleble] = {
  requested_tag: @simpleble_release,
  release_tag: @simpleble.real_tag_name,
  release: @simpleble.release.dup.tap { _1.delete('assets') },
  asset: @simpleble.matching_asset.dup.tap { _1.delete('uploader') },
  install_location: @simpleble.install_dir
}


OUT_FILE = Pathname.new(__dir__) / 'build-config.json'
File.write(OUT_FILE, JSON.pretty_generate(config_data))
