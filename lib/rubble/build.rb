# frozen_string_literal: true

require_relative 'version'
require_relative 'build/core_ext'

module RubBLE
  module Build
  end
end

require 'zeitwerk'
loader = Zeitwerk::Loader.new
loader.inflector.inflect(
  'cmake'     => 'CMake',
  'rubble'    => 'RubBLE',
  'simpleble' => 'SimpleBLE',
  'os'        => 'OS',
)
loader.push_dir("#{__dir__}/build", namespace: RubBLE::Build)
loader.ignore("#{__dir__}/build/core_ext.rb")
loader.setup
