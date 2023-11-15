# frozen_string_literal: true
require 'dry/logic/predicates'

module RuBLE
  module Build
    module DrySettings
      # Dry::Types.load_extensions(:maybe)
      # Dry::Schema.load_extensions(:hints)
      Dry::Schema.load_extensions(:info)
      Dry::Validation.load_extensions(:predicates_as_macros)
      TypeContainer = Dry::Schema::TypeContainer.new

      module Types
        include Dry::Logic
        include Dry.Types()
        extend Dry::Logic::Builder

        StrippedString = Types::String.constructor(&:strip)
        Path = Types.Constructor(Pathname).meta(info: 'foo')

        Predicates.predicate(:exist?) { |input| Pathname.new(input).exist? }
        Predicates.predicate(:directory?) { |input| Pathname.new(input).directory? }
        Predicates.predicate(:file?) { |input| Pathname.new(input).file? }
        ExistingPath = Path.constrained(exist: true)
        Directory = Path.constrained(directory: true)
        File = Path.constrained(file: true)

        TypeContainer.register('stripped_string', StrippedString)
        TypeContainer.register('path', Path)
        TypeContainer.register('existing_path', ExistingPath)
        TypeContainer.register('directory', Directory)
        TypeContainer.register('file', File)
      end

        # flag :"#{name}_static" do
        #   optional
        #   convert :bool
        #   long "--with-static-#{name}"
        # end

        # attr_reader *%i[name_underscored name_dashed name_titleized]
        # def initialize(name)
        #   @name_underscored = name.to_s.underscore
        #   @name_dashed = name_underscored.gsub('_', '-')
        #   @name_titleized = name_underscored.titleize
        # end

      # TypeContainer.register('github_dependency', GithubDependencyContract)

      class ExtconfContract < Dry::Validation::Contract
        config.types = TypeContainer
        GithubDependency = Dry::Schema.define do
          config.types = TypeContainer
          optional(:local_path).value(Types::Directory | Types::True)
          optional(:static).value(:bool)
          optional(:precompiled).value(:bool)
          optional(:release).maybe(Types::StrippedString)
        end

        schema do
          optional(:debug).value(:bool) #.meta(info: { flags: %w[-d --[no-]debug],     desc: 'Build in debug mode' })
          optional(:verbose).value(:bool) #     .info(meta: { flags: %w[-v --[no-]verbose],   desc: 'Output commands during build' })
          optional(:development).value(:bool) # .info(meta: { flags: %w[--dev --development], desc: 'Use latest releases of libraries' })
          optional(:release).value(:bool) #     .info(meta: { flags: %w[-r --release],        desc: 'Build extension for distribution on rubygems' })

          optional(:boost).value(GithubDependency)
          optional(:simpleble).value(GithubDependency)
        end

      end
    end
  end
end
