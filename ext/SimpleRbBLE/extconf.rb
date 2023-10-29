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

# TODO: invoke mini_portile2 for SimpleBLE

OUT_FILE = Pathname.new(__dir__) / 'build-config.json'
File.write(OUT_FILE, JSON.pretty_generate(config_data))
