# frozen_string_literal: true

module SimpleRbBLE
  module Build
    module Extconf
      attr_reader :config_data
      def initialize
        ENV["#{extension_name.upcase}_DEBUG_ON"] = 'on' if debug_mode?
        @config_data             = {}
        @config_data[:rbconfig]  = rbconfig_data
        @config_data[:rice]      = rice_data
        @config_data[:bundler]   = bundler_data
        @config_data[:extension] = extension_data
        @config_data[:os]        = os_data
      end

      include SimpleRbBLE::Build::Data::OS
      include SimpleRbBLE::Build::Data::Bundler
      include SimpleRbBLE::Build::Data::Rice
      include SimpleRbBLE::Build::Data::Extension

      def debug_mode?
        return @debug_mode if defined?(@debug_mode)

        @debug_mode = with_config("debug") || ENV["#{extension_name.upcase}_DEBUG_ON"]
      end

      def rbconfig_data = @rbconfig_data ||= RbConfig::CONFIG.select { |k, v| v && !v.strip.empty? }

      def setup_simpleble!
        return if @simpleble # Already done

        # Now download SimpleBLE
        # TODO: we should probably fix a known-working version come release time
        tag = with_config('simpleble-release').freeze || 'latest'
        @simpleble = SimpleBLE.new(tag:)
        @simpleble.setup!
        @config_data[:simpleble] = @simpleble.config_data
      end
    end
  end
end
