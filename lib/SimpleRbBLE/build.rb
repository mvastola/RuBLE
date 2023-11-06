# frozen_string_literal: true

module SimpleRbBLE
  module Build
  end
end

begin
  SimpleRbBLE::VERSION
rescue LoadError
  require_relative 'version'
end

require 'zeitwerk'
loader = Zeitwerk::Loader.new
loader.push_dir("#{__dir__}/build", namespace: SimpleRbBLE::Build)
loader.setup
