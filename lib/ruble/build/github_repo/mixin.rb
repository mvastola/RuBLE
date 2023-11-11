# frozen_string_literal: true

module RuBLE::Build
  module GithubRepo
    module Mixin
      GITHUB_REPO_DEFAULTS = {
        static:      true,
        path:        false,
        precompiled: false,
        tag:         'default'
      }.freeze

      extend ActiveSupport::Concern

      class_methods do
        def add_github_repo_data(name, **defaults)
          klass_name = RuBLE::Build.zeitwerk.inflector.camelize(name.to_s, __dir__).to_sym
          klass = RuBLE::Build::GithubRepo.const_get(klass_name)
          add_github_repo_config_opts(name, **GITHUB_REPO_DEFAULTS, **defaults)

          define_method name.to_sym do
            # TODO: accept custom path
            # TODO: choose shared vs static
            # TODO: choose build vs precompiled vs system
            kwargs = Settings.config[name].to_h.slice(*%i[static path precompiled tag])
            klass.new(**kwargs)
          end
          memoize name.to_sym
          klass
        end

        def add_github_repo_config_opts(name, **defaults)
          defaults.symbolize_keys!
          # TODO: this should be enhanced a bit. Specifically, it should be possible to alert to incompatible
          #   flags, as well as to -- for example -- turn off static by default if building for local dev
          cli = Settings::CLISource
          cli.add_flag("#{name}.static", "--[no-]static-#{name}", type: TrueClass,
                       initial: defaults.fetch(:static), default: true,
                       desc: "Link #{name} statically")
          cli.add_flag("#{name}.path", "--#{name}-path=[PATH]",
                       initial: defaults.fetch(:path),
                       default: 'SYSTEM', desc: "Use existing local copy of #{name}")
          cli.add_flag("#{name}.precompiled", "--[no-]precompiled-#{name}",
                       initial: defaults.fetch(:precompiled),
                       default: true, desc: "Download a precompiled version of #{name}")
          cli.add_flag("#{name}.tag", "--#{name}-release RELEASE",
                       type:    String,
                       initial: defaults.fetch(:tag),
                       desc:    "Fetch release #{name} from github with the given tag. " \
                                "Defaults to: 'latest' (development mode) or the version " \
                                'set in version.rb') do |val, source|
            source.set_result("#{name}.path", false)
            val
          end

          cli.add_flag_alias("#{name}.path", "--system-#{name}", 'SYSTEM',
                             desc: "Link system #{name}") do |val, source|
            source.set_result("#{name}.static", false, force: false)
            val
          end
        end
      end
    end
  end
end
