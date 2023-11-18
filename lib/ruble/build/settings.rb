# frozen_string_literal: true

require 'optparse'

module RuBLE
  module Build
    class Settings
      TRUE_VALUES = %w[1 on true yes y t].freeze
      FALSE_VALUES = %w[0 off false no n f].freeze
      GLOBAL_FLAGS = %i[debug verbose developer release].freeze
      GLOBAL_FLAG_INQUIRERS = GLOBAL_FLAGS.map { :"#{_1}?" }.freeze

      class << self
        def str_true?(val)  = val.is_a?(String) && TRUE_VALUES.include?(val.strip.downcase)
        def str_false?(val) = val.is_a?(String) && FALSE_VALUES.include?(val.strip.downcase)
      end
      include Memery
      attr_reader *%i[remaining_args]
      def initialize(argv = ARGV, raise_on_unknown: true)
        @argv = argv.dup
        @raise_on_unknown = raise_on_unknown
      end

      GLOBAL_FLAGS.each do |flag|
        define_method(:"#{flag}?") { merged_config.fetch(flag, false) }
        memoize :"#{flag}?"
      end
      def irb! = @irb = true
      def irb? = @irb.present?

      def validate!
        if release? && developer?
          raise ArgumentError, 'The --release and --developer flags are mutually exclusive'
        elsif raise_on_unknown? && remaining_args&.any?
          puts parser.help
          raise ArgumentError, "Unrecognized option(s) found on command line: #{remaining_args.inspect}"
        end
      end
      memoize def config = merged_config.tap { validate! }

      def raise_on_unknown? = @raise_on_unknown

    private
      delegate :gem_name, :gem_version, to: RuBLE::Build::Environment::Extension

      memoize def merged_config = env_config.deep_merge(cli_settings).freeze

      memoize def env_config
        env_prefix = "#{gem_name.upcase}_"
        env_config = {}
        all_envs = ENV.select { _1.start_with?(env_prefix) }
                      .transform_keys { _1.to_s.delete_prefix(env_prefix).downcase }
        all_envs.transform_values! do |v|
          next true if self.class.str_true?(v)
          next false if self.class.str_false?(v)
          next nil if v.blank?

          v
        end
        all_envs.compact!

        github_deps.each do |dep|
          dep_prefix = "#{dep}_".downcase
          dep_settings = env_config[dep.to_s] ||= {}
          all_envs.keys.select { _1.start_with?(dep_prefix) }.each do |k|
            dep_settings[k.delete_prefix(dep_prefix)] = all_envs.delete(k)
          end
        end
        env_config.reverse_merge(all_envs).deep_symbolize_keys
      end

      memoize def cli_settings = (@cli_config ||= {}).tap { parse_cli! }

      memoize def github_deps = GithubDep.deps.keys.freeze

      memoize def parse_cli!
        @parsed_args = {}
        @remaining_args = @argv.dup
        parser.order!(@remaining_args, into: @parsed_args)
        @remaining_args.freeze
        @parsed_args.freeze
      end

      memoize def parser
        ::OptionParser.new do |parser|
          parser.banner = "Compile the C++ extension for the "\
                          "#{gem_name} gem\n"\
                          "Usage: ruby extconf.rb [options]\n"
          parser.separator "\nCommon options:"
          add_global_flags(parser:)
          github_deps.each do |dep_name|
            parser.separator "\n#{dep_name.to_s.camelize(:upper)} library:\n"
            add_github_dep_flags(dep_name:, parser:)
          end
        end
      end

      def add_github_dep_flags(dep_name:, parser:)
        dashed_name = dep_name.to_s.underscore.gsub('_', '-')

        @cli_config ||= {}
        dep_config = @cli_config[dep_name.to_s.underscore.to_sym] ||= {}
        parser.on("--[no-]static-#{dashed_name}", "Link #{dep_name} statically") do |v|
          dep_config[:static] = v.present?
        end

        parser.on("--with-local-#{dashed_name} [LIB_PATH]",
                  "Use local (pre-built) copy of #{dep_name} (in LIB_PATH if specified, else auto-detected)") do |v|
          if dep_config.key?(:tag)
            warn "Argument --with-local-#{dashed_name} given, but --with-#{dashed_name}-release already present. " \
                 "These are mutually exclusive. Will ignore previous --with-#{dashed_name}-release."
            dep_config.delete(:tag)
          end

          if dep_config.key?(:precompiled) && dep_config[:precompiled].blank?
            # TODO: change this?
            warn "--no-use-precompiled-#{dashed_name} is ignored when --with-local-#{dashed_name} " \
                 'is given (local libraries are expected be pre-built)'
            dep_config.delete(:precompiled)
          end
          dep_config[:local] = v
        end

        parser.on("--with-#{dashed_name}-release [GIT_RELEASE_TAG]",
                  "Fetch #{dep_name} from, optionally at the given tag and compile it (unless " \
                  "--use-precompiled-#{dashed_name} is set). Defaults to the latest stable release " \
                  '(in developer mode) or latest version officially supported by this gem') do |v|
          if dep_config.key?(:local)
            warn "Argument --with-#{dashed_name}-release given, but --with-local-#{dashed_name} already present. " \
                 "These are mutually exclusive. Will ignore previous --with-local-#{dashed_name}."
            dep_config.delete(:local)
          end

          dep_config[:tag] = v == true ? 'default' : v
        end

        parser.on("--[no-]use-precompiled-#{dashed_name}", "Link precompiled version of #{dep_name}") do |v|
          if dep_config.key?(:local) && v.blank?
            # TODO: change this?
            warn "--no-use-precompiled-#{dashed_name} is ignored when --with-local-#{dashed_name} is given " \
                 '(local libraries are expected be pre-built)'
            next
          end

          dep_config[:precompiled] = v.present?
        end
      end

      def add_global_flags(parser:)
        @cli_config ||= {}

        parser.on('-d', '--[no-]debug', 'Build in debug mode') do |v|
          @cli_config[:debug] = !!v
        end

        parser.on('-v', '--[no-]verbose', 'Output extra information (incl. executed commands) during build') do |v|
          @cli_config[:verbose] = !!v
        end

        parser.on('-l', '--[no-]developer', 'Use latest releases of libraries and other build settings ' \
                                            "useful for developers of #{gem_name} when working locally") do |v|
          @cli_config[:developer] = !!v
        end

        parser.on('-r', '--release', 'Compile for packaging and distribution on rubygems, etc') do |v|
          @cli_config[:release] = !!v
        end

        parser.on('--irb') { irb! }

        parser.on('-h', '--help', 'Show usage information') do
          puts parser.help
          exit 0
        end

        parser.on('--version', 'Show gem version') do
          puts "#{gem_name} version #{gem_version}"
          exit 0
        end
      end
    end
  end
end
