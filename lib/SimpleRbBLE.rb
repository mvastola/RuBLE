# frozen_string_literal: true

require 'zeitwerk'
loader = Zeitwerk::Loader.for_gem
loader.ignore("#{__dir__}/build")
loader.ignore("#{__dir__}/build.rb")
loader.setup

module SimpleRbBLE
  # TODO: is this used? (should it be?)
  class Error < StandardError; end
  class << self
    def try_demangle(symbol)
      require 'open3'
      demangled, status = Open3.capture2('c++filt', stdin_data: symbol)
      demangled if status.success?
    rescue Exception # rubocop:disable Lint/RescueException
      nil
    end
  end

  # Your code goes here...
end

begin
  require_relative 'SimpleRbBLE/SimpleRbBLE.so'
rescue LoadError => ex
  if (symbol = ex.message[/\A[^:]+: undefined symbol: (\S+) - /, 1])
    demangled = SimpleRbBLE.try_demangle(symbol)
    ex.message << "\n\nDemangled symbol: #{demangled}" if demangled
  end
  raise
end

