# frozen_string_literal: true

module RuBLE
  module Build
    module Extconf
      module CLI
        class GithubDepArgs
          DEFAULTS = {
            static:      true,
            path:        false,
            precompiled: false,
            tag:         'default',
          }.freeze
          LINKAGES = %i[static shared].freeze

          include Memery

          memoize def supports_precompiled?(linkage)
            raise NotImplementedError, 'Must implement in subclass'
          end

          memoize def supports_source_build?(linkage)
            raise NotImplementedError, 'Must implement in subclass'
          end

          memoize def default_linkage = development? ? :shared : :static

          # def git_needed?
          #   path.blank?
          # end

          def verify_settings!(parsed)
            conflicting_linkages = linkage_opts.length > 1
            if parsed.development == parsed.release
              raise ArgumentError, "Development and release modes are mutually exclusive"
            end
            if [ parsed.shared, parsed.static ].count(&:itself) > 1
              raise ArgumentError, "Shared and static linkages are mutually exclusive"
            end
          end

          attr_reader :repo_defaults
          def initialize(name, **repo_defaults)
            name_underscored = name.to_s.underscore
            name_dashed = name_underscored.gsub('_', '-')
            name_titleized = name_underscored.titleize

            @user_defaults = user_defaults.to_h.tap(&:deep_symbolize_keys!).freeze

            flag :"#{name}_static" do
              optional
              convert :bool
              long "--with-static-#{name}"
              desc "Link #{name} statically"
              initial defaults.fetch(:static)

              default do
                development? ? false
              end
            end
            cli.add_flag("#{name}.static", "--[no-]static-#{name}", type: TrueClass, initial: defaults.fetch(:static),
                         default: true, desc: "Link #{name} statically")
            cli.add_flag("#{name}.local", "--local-#{name}=[PATH]", initial: defaults.fetch(:local),
                         default: 'AUTODETECT',
                         desc:    "Use local copy of #{name}, optionally with specific directory. "\
                           "(auto-detects location by default)"
            )
            cli.add_flag(
              "#{name}.tag",
              "--#{name}-release=[GIT_RELEASE_TAG]",
              type:    String,
              initial: defaults.fetch(:tag),
              desc:    "Fetch release #{name} from github with the given tag. "\
                "Defaults to the latest stable release (development mode only) "\
                "or, else, the latest version supported") do |val, source|
              val.tap { source.set_result("#{name}.path", false) }
            end
            cli.add_flag(
              "#{name}.precompiled",
              "--[no-]-use-#{name}-binary-release",
              initial: defaults.fetch(:precompiled),
              default: true,
              desc: "Download a precompiled version of #{name}"
            )
            cli.add_flag_alias("#{name}.path", "--system-#{name}", 'SYSTEM',
                               desc: "Link system #{name}") do |val, source|
              source.set_result("#{name}.static", false, force: false)
              val
            end
          end

          memoize def repo_defaults = {}
          memoize def merged_defaults = DEFAULTS.deep_dup.deep_merge!(repo_defaults || {})

        end
      end

    end
  end
end
