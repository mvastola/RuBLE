# frozen_string_literal: true

module RuBLE::Build
  module GithubDep
    module Mixin
      include RuBLE::Build::Settings::GithubDep
      extend ActiveSupport::Concern

      class_methods do
        def github_repo_dependency(name, **defaults)
          klass_name = RuBLE::Build.zeitwerk.inflector.camelize(name.to_s, __dir__).to_sym
          klass = RuBLE::Build::GithubDep.const_get(klass_name)
          RuBLE::Build::CLI::GithubDep.add_config_opts(name, **defaults)
          define_method name.to_sym do
            # TODO: accept custom path
            # TODO: choose shared vs static
            # TODO: choose build vs precompiled vs system
            klass.new(**Settings.config[name].to_h)
          end
          memoize name.to_sym
          klass
        end
      end
    end
  end
end
