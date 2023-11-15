# frozen_string_literal: true

module RuBLE
  module Build
    module Extconf
      class BuildConfig
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

        include RuBLE::Build::GithubDep::Mixin
        github_repo_dependency :simpleble, precompiled: true
        github_repo_dependency :boost

        memoize def build_mode
          SETTINGS.to_h { [ _1, send(_1) ] }
        end

        memoize def rbconfig_data
          RbConfig::CONFIG.select do |_k, v|
            v && !v.strip.empty?
          end
        end

        memoize def config_data
          data                = {}
          data[:bundler]      = bundler_data
          data[:rb]           = rbconfig_data
          data[:rb_ext]       = extension_data
          data[:rb_os]        = os_data
          data[:build_mode]   = build_mode
          data[:rb_simpleble] = simpleble.config_data
          data[:rb_boost]     = boost.config_data
          data[:rice]         = rice_data
          data.freeze
        end

        def write_build_config(path:)
          path.unlink if path.file?
          cmake_generate(config_data, path:)
        end
      end
    end
  end
end
