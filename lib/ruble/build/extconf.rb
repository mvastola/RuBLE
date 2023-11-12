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

      delegate :config, to: Settings
      delegate *SETTINGS, to: :config

      SETTINGS.each do |sym|
        define_method(:"#{sym}?") { send(sym) }
      end

      include RuBLE::Build::GithubRepo::Mixin
      add_github_repo_data :simpleble, precompiled: true
      add_github_repo_data :boost

      memoize def build_mode
        SETTINGS.to_h { [ _1, send(_1) ] }
      end

      memoize def rbconfig_data
        RbConfig::CONFIG.select do |_k, v|
          v && !v.strip.empty?
        end
      end

      memoize def config_data
        @config_data = {}
        @config_data[:bundler]      = bundler_data
        @config_data[:rb]           = rbconfig_data
        @config_data[:rb_ext]       = extension_data
        @config_data[:rb_os]        = os_data
        @config_data[:build_mode]   = build_mode
        @config_data[:rb_simpleble] = simpleble.config_data
        @config_data[:rb_boost]     = boost.config_data
        @config_data[:rice]         = rice_data
        @config_data.freeze
      end

      def write_build_config(path:)
        path.unlink if path.file?
        cmake_generate(config_data, path:)
      end
    end
  end
end
