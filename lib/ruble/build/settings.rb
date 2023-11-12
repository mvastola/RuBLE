# frozen_string_literal: true

require 'optparse'
module RuBLE::Build::Settings
  UNSET = Object.new.tap do |unset|
    unset.define_singleton_method(:empty?) { true }
  end.freeze

  class << self
    attr_reader :cli_settings
    def config
      return @config if @config

      @cli_settings = RuBLE::Build::Settings::CLISource.new(*ARGV)

      build_data = ::RuBLE::Build::Data
      ::Config.setup do |config|
        config.const_name = 'BuildConfig'
        config.use_env = true
        config.env_prefix = 'RUBLE' # TODO: get from gemspec
        config.env_parse_values = true
        config.env_separator = '_'
        config.env_converter = :downcase
        config.fail_on_missing = true
      end
      # debugger
      # FIXME!! This is b0rked (need to look into ::Config to see how values are being lost)
      ::Config.load_and_set_settings
      @config = ::BuildConfig.tap do |config|
        # envconfig = config.to_h
        config.add_source!(cli_settings)
        ext_dir = (build_data.root_dir / build_data.spec.extensions.first).parent
        config.add_source!(ext_dir / 'dependencies.yml')
        # debugger
        config.tap(&:reload!)
      end
    end
  end
end