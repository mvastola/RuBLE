# frozen_string_literal: true

module RuBLE
  module Build
    class ExtConfig
      GLOBAL_FLAGS = %i[debug verbose development release].freeze
      GLOBAL_FLAG_INQUIRERS = GLOBAL_FLAGS.map { :"#{_1}?" }.freeze

      attr_reader *%i[remaining_args github_deps]
      def initialize(argv = ARGV, github_deps: [], raise_on_unknown: true)
        @argv = argv.dup
        @github_deps = Set.new(github_deps.map(&:to_s)).freeze
        @raise_on_unknown = raise_on_unknown
      end

      GLOBAL_FLAGS.each do |flag|
        define_method(:"#{flag}?") { merged_config.fetch(flag, false) }
        memoize :"#{flag}?"
      end

      def validate!
        if release? && development?
          raise ArgumentError, 'The --release and --development flags are mutually exclusive'
        end
        if raise_on_unknown? && remaining_args&.any?
          puts parser.help
          raise ArgumentError, "Unrecognized option(s) found on command line: #{remaining_args.inspect}"
        end
      end
      memoize def config = merged_config.tap { validate! }

      def raise_on_unknown? = raise_on_unknown

    private
      memoize def merged_config = env_config.deep_merge(cli_settings).freeze

      memoize def env_config
        env_prefix = "#{Data.gem_name.upcase}_"
        env_config = {}
        all_envs = ENV.select { |k| k.start_with?(env_prefix) }.
                   transform_keys { k.to_s.delete_prefix(env_prefix).downcase }

        github_deps.each do |dep|
          dep_prefix = "#{dep}_".downcase
          dep_settings = env_config[dep.to_s] ||= {}
          all.keys.select { _1.start_with?(dep_prefix) }.each do |k|
            dep_settings[k.delete_prefix(dep_prefix)] = all_envs.delete(k)
          end
        end
        env_config.reverse_merge(all_envs).deep_symbolize_keys
      end

      memoize def cli_settings = (@cli_config ||= {}).tap { parse_cli! }

      memoize def parse_cli!
        @parsed_args = {}
        @remaining_args = @argv.dup
        parser.order!(@remaining_args, into: @parsed_args)
        @remaining_args.freeze
        @parsed_args.freeze
      end

      memoize def parser
        OptionParser.new do |parser|
          parser.banner = "Usage: ruby extconf.rb [options]\n\n" \
                          "Compile the C++ extension for the #{Data.gem_name} gem"
          add_global_flags(parser:)
          github_deps.each { |dep_name| add_github_dep_flags(dep_name:, parser:) }
        end
      end

      def add_github_dep_flags(dep_name:, parser:)
        dashed_name = dep_name.to_s.underscore.gsub('_', '-')

        @cli_config ||= {}
        dep_config = @cli_config[dep_name.to_s.underscore.to_sym] ||= {}
        parser.on("--[no-]static-#{dashed_name}", "Link #{name} statically") do |v|
          dep_config[:static] = v.present?
        end

        parser.on("--with-local-#{dashed_name} [LIB_PATH]",
                  "Use local (pre-built) copy of #{name} (in LIB_PATH if specified, else auto-detected)") do |v|
          if dep_config.key?(:tag)
            warn "Argument --with-local-#{dashed_name} given, but --with-#{dashed_name}-release already present. "\
                 "These are mutually exclusive. Will ignore previous --with-#{dashed_name}-release."
            dep_config.delete(:tag)
          end

          if dep_config.key?(:precompiled)
            # TODO: change this?
            warn "--[no-]-use-precompiled-#{dashed_name} is ignored when --with-local-#{dashed_name} "\
                 'is given (local libraries are expected be already built)'
            dep_config.delete(:precompiled)
          end
          dep_config[:local] = v == true ? 'AUTODETECT' : v
        end

        parser.on("--with-#{dashed_name}-release [GIT_RELEASE_TAG]",
                  "Fetch #{name} from, optionally at the given tag and compile it (unless 'precompiled' is set)."\
                  'Defaults to the latest stable release (development mode) or latest version officially '\
                  'supported by this gem') do |v|
          if dep_config.key?(:local)
            warn "Argument --with-#{dashed_name}-release given, but --with-local-#{dashed_name} already present. "\
                 "These are mutually exclusive. Will ignore previous --with-local-#{dashed_name}."
            dep_config.delete(:local)
          end

          dep_config[:tag] = v == true ? 'default' : v
        end

        parser.on("--[no-]-use-precompiled-#{dashed_name}", "Link precompiled version of #{name}") do |v|
          if dep_config.key?(:local)
            # TODO: change this?
            warn "--[no-]-use-precompiled-#{dashed_name} is ignored when --with-local-#{dashed_name} is given "\
                   '(local libraries are expected be already built)'
            next
          end

          dep_config[:precompiled] = v.present?
        end
      end

      def add_global_flags(parser:)
        @cli_config ||= {}
        parser.on_head('-d', '--[no-]debug', desc: 'Build in debug mode') do |v|
          @cli_config[:debug] = !!v
        end

        parser.on_head('-v', '--[no-]verbose', 'Output extra information (incl. executed commands) during build') do |v|
          @cli_config[:verbose] = !!v
        end

        parser.on_head('-l', '--[no-]development', 'Use latest releases of libraries (compatability not assured)') do |v|
          @cli_config[:development] = !!v
        end

        parser.on_head('-r', '--release', desc: 'Compile for packaging and distribution on rubygems, etc') do |v|
          @cli_config[:release] = !!v
        end

        parser.on_tail('-h', '--help', desc: 'Show usage information') do
          parser.help
          exit 0
        end

        parser.on_tail('--version', desc: 'Show gem version') do
          puts "#{Data.gem_name} version #{Data.gem_version}"
          exit 0
        end
      end
    end
  end
end
