# frozen_string_literal: true

module RuBLE::Build
  module Environment
    module Bundler
      BUILD_CONFIG_KEY = :bundler

      class << self
        include Memery
        # Based on https://www.rubydoc.info/gems/bundler/Bundler%2FEnv%2Eenvironment

        memoize def info
          return unless Extconf.verbose?

          {
            platform:         Gem.platforms.map(&:to_s),
            config_dir:       Pathname.new(Gem::ConfigFile::SYSTEM_WIDE_CONFIG_FILE).dirname,
            rubygems_version: Gem::VERSION,
            gem_home:         Pathname.new(Gem.dir),
            gem_path:         Gem.path.join(File::PATH_SEPARATOR),
            user_home:        Pathname.new(Gem.user_dir),
            bindir:           Pathname.new(Gem.bindir),
          }.freeze
        end
      end
    end
  end
end