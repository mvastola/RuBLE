# frozen_string_literal: true

module RubBLE
  module Build
    module Data
      module Bundler
        # Based on https://www.rubydoc.info/gems/bundler/Bundler%2FEnv%2Eenvironment
        def bundler_data
          @to_h ||= {
            platform:         Gem.platforms.join(', '),
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
