# frozen_string_literal: true

module RuBLE
  module Build
    class Extconf
      include Memery

      class << self
        private :new
        def instance = @instance ||= new
        delegate :build_config, :write_build_config, *ExtconfConfig::GLOBAL_FLAG_INQUIRERS, to: :instance

        def github_deps = @github_deps ||= Set.new

        def configure_github_dep(name, **)
          github_deps << name.to_sym
          define_method name.to_sym do
            klass_name = RuBLE::Build.zeitwerk.inflector.camelize(name.to_s, __dir__)
            klass = RuBLE::Build::GithubDep.const_get(klass_name)
            klass.new(**, **config[name.to_sym].to_h)
          end
          memoize name.to_sym
          nil
        end
      end
      delegate :github_deps, to: :class

      include RuBLE::Build::Data::OS
      include RuBLE::Build::Data::Bundler
      include RuBLE::Build::Data::Rice
      include RuBLE::Build::Data::Extension
      include RuBLE::Build::CMake

      configure_github_dep :simpleble, precompiled: true, supports_precompiled: true
      configure_github_dep :boost

      memoize def config = ExtconfConfig.new(ARGV, github_deps:)
      delegate *ExtconfConfig::GLOBAL_FLAG_INQUIRERS, to: :config

      memoize def build_flags
        config.slice(*ExtconfConfig::GLOBAL_FLAGS).select { _2 }.freeze
      end

      memoize def rbconfig_data
        RbConfig::CONFIG.select do |_k, v|
          v && !v.strip.empty?
        end
      end

      memoize def build_config
        data                = {}
        data[:bundler]      = bundler_data
        data[:rb]           = rbconfig_data
        data[:rb_ext]       = extension_data
        data[:rb_os]        = os_data
        data[:build_flags]  = build_flags
        data[:rb_simpleble] = simpleble.build_config
        data[:rb_boost]     = boost.build_config
        data[:rice]         = rice_data
        data.freeze
      end

      def write_build_config(path:)
        path.unlink if path.file?
        cmake_generate(build_config, path:)
      end
    end
  end
end
