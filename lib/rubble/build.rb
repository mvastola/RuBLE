# frozen_string_literal: true

module RubBLE
  module Build
  end
end

begin
  RubBLE::VERSION
rescue LoadError
  require_relative 'version'
end

require 'zeitwerk'
loader = Zeitwerk::Loader.new
loader.inflector.inflect(
  'rubble'    => 'RubBLE',
  'simpleble' => 'SimpleBLE',
  'os'        => 'OS',
)
loader.push_dir("#{__dir__}/build", namespace: RubBLE::Build)
loader.setup
