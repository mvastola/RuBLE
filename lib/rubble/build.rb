# frozen_string_literal: true

module Rubble
  module Build
  end
end

begin
  Rubble::VERSION
rescue LoadError
  require_relative 'version'
end

require 'zeitwerk'
loader = Zeitwerk::Loader.new
loader.inflector.inflect(
  'simpleble' => 'SimpleBLE',
  'os'        => 'OS',
)
loader.push_dir("#{__dir__}/build", namespace: Rubble::Build)
loader.setup
