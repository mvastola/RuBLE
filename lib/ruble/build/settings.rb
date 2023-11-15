# frozen_string_literal: true

require 'optparse'
module RuBLE::Build::Settings
  class << self
    include Memery
    memoize def config

      build_data = ::RuBLE::Build::Data

      ::Config.setup do |config|
        config.const_name = 'BuildConfig'
        config.use_env = true
        config.env_prefix = build_data.gem_name
        config.env_parse_values = true
        config.env_separator = '_'
        config.env_converter = :downcase
        config.fail_on_missing = true
      end

      ::Config.load_and_set_settings
      ::BuildConfig.tap do |config|
        config.add_source!(cli_settings)
        ext_dir = (build_data.root_dir / build_data.spec.extensions.first).parent
        config.add_source!(ext_dir / 'dependencies.yml')
        config.tap(&:reload!)
      end
    end

  private

    memoize def cli_settings = RuBLE::Build::Settings::CLISource.new(*ARGV)

  end
end