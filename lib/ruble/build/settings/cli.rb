# frozen_string_literal: true

module RuBLE
  module Build
    module Settings
      class CLI
        attr_reader *%i[remaining_args raise_on_unknown github_deps]
        def initialize(argv = ARGV, github_deps: [], raise_on_unknown: true)
          @argv = argv.dup
          @github_deps = Set.new(github_deps.map(&:to_s)).freeze
          @raise_on_unknown = raise_on_unknown
        end

        memoize def config
          env_settings.deep_merge(cli_settings)
        end

        %i[debug verbose development release help version].each do |flag|
          define_method(:"#{flag}?") { config.fetch(flag, false) }
          memoize :"#{flag}?"
        end

      private
        
        memoize def env_settings
          env_prefix = "#{Data.gem_name.upcase}_"
          settings = {}
          all_envs = ENV.select { |k| k.start_with?(env_prefix) }.
            transform_keys { k.to_s.delete_prefix(env_prefix).downcase }
          
          github_deps.each do |dep|
            dep_prefix = "#{dep}_".downcase
            dep_settings = settings[dep.to_s] ||= {}
            all.keys.select { _1.start_with?(dep_prefix) }.each do |k|
              dep_settings[k.delete_prefix(dep_prefix)] = all_envs.delete(k)
            end
          end
          settings.reverse_merge(all_envs).deep_symbolize_keys
        end
        
        memoize def cli_settings
          @cli_settings ||= {}
          parse_cli!
          @cli_settings
        end

        memoize def parse_cli!
          @parsed_args = {}
          @remaining_args = @argv.dup
          parser.order!(@remaining_args, into: @parsed_args)
          @remaining_args.freeze
          @parsed_args.freeze
          if raise_on_unknown? && remaining_args&.any?
            puts parser.help
            raise ArgumentError, "Unrecognized options found on command line: #{remaining_opts.inspect}"
          end
        end

        memoize def parser = OptionParser.new do |parser|
          parser.banner = "Usage: ruby extconf.rb [options]\n" \
            "Compile the C++ extension for the #{Data.gem_name} gem"
          add_global_flags(parser:)
          github_deps.each do |dep_name|
            add_github_dep_flags(dep_name:, parser:)
          end
        end

        def add_github_dep_flags(dep_name:, parser:)
          dashed_name = dep_name.to_s.underscore.gsub('_', '-')
          
          @cli_settings ||= {}
          dep_config = @cli_settings[dep_name.to_s.underscore.to_sym] ||= {}
          parser.on("--[no-]static-#{dashed_name}", "Link #{name} statically") do |v|
            dep_config[:static] = !!v
          end
          
          parser.on("--with-local-#{dashed_name} [PATH]",
                    "Use local copy of #{name} in PATH if specified (else autodetected)") do |v|
            dep_config[:local] = v
          end
          
          parser.on("--with-#{dashed_name}-release [GIT_RELEASE_TAG]",
                    "Fetch #{name} from, optionally at the given tag and compile it (unless 'precompiled' is set)."\
                    "Defaults to the latest stable release (development mode) or latest version officially "\
                    "supported by this gem") do |v|
            dep_config[:tag] = v == true ? 'default' : v
          end
          
          parser.on("--[no-]-use-precompiled-#{dashed_name}", 
                    "Link precompiled version of #{name}") do |v|
            dep_config[:precompiled] = !!v
          end
        end
          
        def add_global_flags(parser:)
          @cli_settings ||= {}
          parser.on_head('-d', '--[no-]debug', desc: 'Build in debug mode') do |v|
            @cli_settings[:debug] = !!v
          end

          parser.on_head('-v', '--[no-]verbose', "Output extra information (incl. executed commands) during build") do |v|
            @cli_settings[:verbose] = !!v
          end

          parser.on_head('-l', '--[no-]development', 'Use latest releases of libraries (compatability not assured)') do |v|
            @cli_settings[:development] = !!v
          end

          parser.on_head('-r', '--release', desc: 'Compile for packaging and distribution on rubygems, etc') do |v|
            @cli_settings[:release] = !!v
          end

          parser.on_tail('-h', '--help', desc: 'Show usage information') do |v|
            @cli_settings[:help] = !!v
          end

          parser.on_tail('--version', desc: 'Show gem version') do |v|
            @cli_settings[:version] = true
          end
        end
      end
    end
  end
end
