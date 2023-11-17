# frozen_string_literal: true

require_relative 'ruble/version'

require 'zeitwerk'
loader = Zeitwerk::Loader.for_gem
loader.inflector.inflect(
  'ruble'     => 'RuBLE',
  'simpleble' => 'SimpleBLE',
)
loader.ignore("#{__dir__}/tasks")
loader.ignore("#{__dir__}/ruble/build")
loader.ignore("#{__dir__}/ruble/build.rb")
loader.ignore("#{__dir__}/RuBLE.rb")
# preloading RuBLE::VERSION manually, to avoid issues if RuBLE::Build
#  and its zeitwerk loader are being used, but this loader can't be used
#  due to lack of library file
loader.ignore("#{__dir__}/ruble/version.rb")
loader.setup

module RuBLE
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
end

begin
  require_relative 'ruble/ruble.so'
rescue LoadError => ex
  if (symbol = ex.message[/\A[^:]+: undefined symbol: (\S+) - /, 1])
    demangled = RuBLE.try_demangle(symbol)
    ex.message << "\n\nDemangled symbol: #{demangled}" if demangled
  end
  raise
end

