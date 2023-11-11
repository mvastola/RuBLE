# frozen_string_literal: true

module RuBLE
  module Build
    module Data
      module Bundler
        # Based on https://www.rubydoc.info/gems/bundler/Bundler%2FEnv%2Eenvironment
        include Memery

        memoize def bundler_data
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
