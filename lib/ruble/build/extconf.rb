# frozen_string_literal: true

module RuBLE
  module Build
    class Extconf
      include Memery

      class << self
        private :new
        def instance = @instance ||= new

        def add_github_repo_data(name)
          klass_name = RuBLE::Build.zeitwerk.inflector.camelize(name.to_s, __dir__).to_sym
          klass = RuBLE::Build.const_get(klass_name)
          define_method name.to_sym do
            # TODO: accept custom path
            # TODO: choose shared vs static
            # TODO: choose build vs precompiled

            static = enable_config("#{name}-static", true)
            precompiled = enable_config("precompiled-#{name}", true)

            tag = with_config("#{name}-release")&.freeze || 'default'
            klass.new(tag:, static:, precompiled:)
          end
          memoize name.to_sym
          klass
        end
      end

      def initialize
        ENV[debug_env_var] = 'on' if debug_mode?
      end

      include RuBLE::Build::Data::OS
      include RuBLE::Build::Data::Bundler
      include RuBLE::Build::Data::Rice
      include RuBLE::Build::Data::Extension
      include RuBLE::Build::CMake

      memoize def debug_env_var = "#{gem_name.upcase}_DEBUG_ON"

      memoize def debug_mode? = with_config('debug') || ENV[debug_env_var] # rubocop:disable Style/FetchEnvVar

      memoize def default_library_version(name:)
        gem_spec.metadata.fetch("#{name}_library_version")
      end

      memoize def rbconfig_data
        RbConfig::CONFIG.select do |_k, v|
          v && !v.strip.empty?
        end
      end

      add_github_repo_data :simpleble
      add_github_repo_data :boost

      memoize def config_data
        @config_data = {
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
