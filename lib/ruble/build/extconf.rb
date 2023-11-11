# frozen_string_literal: true

module RuBLE
  module Build
    class Extconf
      include Memery
      SETTINGS = %i[debug verbose development release].freeze

      class << self
        private :new
        def instance = @instance ||= new
      end

      include RuBLE::Build::Data::OS
      include RuBLE::Build::Data::Bundler
      include RuBLE::Build::Data::Rice
      include RuBLE::Build::Data::Extension
      include RuBLE::Build::CMake

      include RuBLE::Build::GithubRepo::Mixin
      add_github_repo_data :simpleble, precompiled: true
      add_github_repo_data :boost

      memoize def config = Settings.config
      delegate *SETTINGS, to: :config
      SETTINGS.each { memoize _1 }.each { alias_method :"#{_1}?", _1 }

      memoize def build_mode
        SETTINGS.to_h { [ _1, send(:"#{_1}?") ] }
      end

      memoize def default_library_version(name:)
        gem_spec.metadata.fetch("#{name}_library_version")
      end

      memoize def rbconfig_data
        RbConfig::CONFIG.select do |_k, v|
          v && !v.strip.empty?
        end
      end

      memoize def config_data
        @config_data = {
          build_mode:,
          bundler:      bundler_data,
          extconf:      extension_data,
          rb:           rbconfig_data,
          rb_os:        os_data,
          rb_simpleble: simpleble.config_data,
          rb_boost:     boost.config_data,
          rice:         rice_data,
        }.each_value(&:freeze).freeze
      end

      def write_build_config(path:)
        path.unlink if path.file?
        cmake_generate(config_data, path:)
      end
    end
  end
end
