# frozen_string_literal: true

require_relative 'version'

require 'active_support/all'
require_relative 'build/core_ext'

ActiveSupport::Inflector.inflections(:en) do |inflect|
  inflect.acronym 'CMake'
  inflect.acronym 'RuBLE'
  inflect.acronym 'SimpleBLE'
  inflect.acronym 'OS'
end

module RuBLE
  module Build
    UNSET = Object.new.tap do |unset|
      unset.define_singleton_method(:empty?) { true }
    end.freeze

    class << self
      include Memery
      memoize def zeitwerk
        require 'zeitwerk'
        Zeitwerk::Loader.new.tap do |loader|
          loader.inflector = ActiveSupport::Inflector
          loader.push_dir("#{__dir__}/build", namespace: RuBLE::Build)
          loader.ignore("#{__dir__}/build/core_ext.rb")
        end
      end
      private :zeitwerk
    end
  end
end

RuBLE::Build.send(:zeitwerk).setup
