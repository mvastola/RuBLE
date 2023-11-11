# frozen_string_literal: true

module RubBLE
  module Build
    class Extconf
      CONFIG_FORMATS = %i[json cmake].freeze

      include Memery

      attr_reader :config_data
      def initialize
        ENV["#{gem_name.upcase}_DEBUG_ON"] = 'on' if debug_mode?
        @config_data = {
          bundler:      bundler_data,
          extconf:      extension_data,
          rb:           rbconfig_data,
          rb_os:        os_data,
          rb_simpleble: simpleble.config_data,
          rice:         rice_data,
        }
      end

      include RubBLE::Build::Data::OS
      include RubBLE::Build::Data::Bundler
      include RubBLE::Build::Data::Rice
      include RubBLE::Build::Data::Extension
      include RubBLE::Build::CMake

      memoize def debug_mode? = with_config('debug') || ENV["#{gem_name.upcase}_DEBUG_ON"]

      memoize def rbconfig_data
        RbConfig::CONFIG.select do |_k, v|
          v && !v.strip.empty?
        end
      end

      memoize def simpleble
        # TODO: accept custom path
        # TODO: choose shared vs static
        # TODO: choose build vs precompiled
        # TODO: we should probably fix a known-working version come release time
        static = enable_config('simpleble-static', true)
        precompiled = enable_config('precompiled-simpleble', true)
        tag = with_config('simpleble-release').freeze || 'latest'
        SimpleBLE.new(tag:, static:, precompiled:)
      end

      def write_build_config(path:)
        path.unlink if path.file?
        cmake_generate(config_data, path:)
      end
    end
  end
end
