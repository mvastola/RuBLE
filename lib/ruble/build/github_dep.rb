# frozen_string_literal: true

module RuBLE
  module Build
    module GithubDep
      class << self
        def deps
          return @deps if @deps

          Build.zeitwerk.eager_load_namespace(self)
          @deps = GithubDep::Base.subclasses.to_h do |klass|
            [ klass.name.demodulize.underscore.to_sym, klass ]
          end
        end
      end
    end
  end
end
