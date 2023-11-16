# frozen_string_literal: true

require 'active_support/all'

module RuBLE
  module Build
    class Extconf
      include Memery
      class << self
        private :new
        def instance = @instance ||= new
        delegate :config, :build_config, :write_build_config, *Settings::GLOBAL_FLAG_INQUIRERS, to: :instance
      end

      Environment.modules.each do |mod|
        key = mod.name.demodulize.underscore.to_sym
        define_method(:"#{key}_info") { mod.info }
      end

      GithubDep.deps.each do |sym, klass|
        define_method(sym) { klass.new(**config[sym].to_h) }
        memoize sym
      end

      memoize def config = config_parser.config
      delegate *Settings::GLOBAL_FLAG_INQUIRERS, to: :config_parser

      memoize def build_flags
        Settings::GLOBAL_FLAGS
          .to_h { [_1, config_parser.send(:"#{_1}?") ] }
          .freeze
      end

      memoize def build_config
        config = { build_flags: }

        Environment.modules.each do |mod|
          key = mod.name.demodulize.underscore.to_sym
          config[mod::BUILD_CONFIG_KEY] = send(:"#{key}_info")
        end
        GithubDep.deps.each_key { config[:"rb_#{_1}"] = send(_1).build_config }

        config.tap(&:compact_blank!).freeze
      end

      def write_build_config(path:)
        path.unlink if path.file?
        pp build_config if verbose?
        CMake.cmake_generate(build_config, path:)
      end

    protected
      memoize def config_parser = Settings.new(ARGV)
    end
  end
end
