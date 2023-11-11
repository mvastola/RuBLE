# frozen_string_literal: true

module RuBLE
  module Build
    class Extconf
      include Memery

      GITHUB_REPO_DEFAULTS = {
        static:      true,
        path:        false,
        precompiled: false,
        tag:         'default'
      }.freeze

      class << self
        private :new
        def instance = @instance ||= new

        def add_github_repo_data(name, **defaults)
          klass_name = RuBLE::Build.zeitwerk.inflector.camelize(name.to_s, __dir__).to_sym
          klass = RuBLE::Build::GithubRepo.const_get(klass_name)
          add_github_repo_config_opts(name, **GITHUB_REPO_DEFAULTS, **defaults)

          define_method name.to_sym do
            # TODO: accept custom path
            # TODO: choose shared vs static
            # TODO: choose build vs precompiled vs system
            kwargs = Settings[name].slice(*%i[static path precompiled tag])
            klass.new(**kwargs)
          end
          memoize name.to_sym
          klass
        end
      end

      include RuBLE::Build::Data::OS
      include RuBLE::Build::Data::Bundler
      include RuBLE::Build::Data::Rice
      include RuBLE::Build::Data::Extension
      include RuBLE::Build::CMake

      include RuBLE::Build::GithubRepo::Mixin
      add_github_repo_data :simpleble, precompiled: true
      add_github_repo_data :boost, precompiled: false

      memoize def config = Settings.config
      memoize def debug? = config.debug

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
