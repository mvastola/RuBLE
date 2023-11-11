# frozen_string_literal: true

require_relative 'version'
require_relative 'build/core_ext'

module RuBLE
  module Build
    class << self
      include Memery
      memoize def zeitwerk
        require 'zeitwerk'
        Zeitwerk::Loader.new.tap do |loader|
          loader.inflector.inflect(
            'cmake'     => 'CMake',
            'ruble'     => 'RuBLE',
            'simpleble' => 'SimpleBLE',
            'os'        => 'OS',
          )
          loader.push_dir("#{__dir__}/build", namespace: RuBLE::Build)
          loader.ignore("#{__dir__}/build/core_ext.rb")
        end
      end
      private :zeitwerk
    end
  end
end

RuBLE::Build.send(:zeitwerk).setup
