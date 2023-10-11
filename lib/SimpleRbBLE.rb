# frozen_string_literal: true

require_relative "SimpleRbBLE/version"
begin
    require_relative "SimpleRbBLE/SimpleRbBLE.so"
rescue LoadError => ex
    if (symbol = ex.message[/\A[^:]+: undefined symbol: (\S+) - /, 1])
        require 'open3'
        demangled, status = Open3.capture2('c++filt', stdin_data: symbol)
        ex.message << "\n\nDemangled symbol: #{demangled}" if status.success?
    end
    raise
end

# TODO: do I require the shared library from here(?)
# TODO: if there end up being more ruby files, might simplify things to add zeitwerk

module SimpleRbBLE
  class Error < StandardError; end
  # Your code goes here...
end
