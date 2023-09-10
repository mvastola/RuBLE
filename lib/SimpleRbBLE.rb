# frozen_string_literal: true

require_relative "SimpleRbBLE/version"
require_relative "SimpleRbBLE/SimpleRbBLE.so"

# TODO: do I require the shared library from here(?)
# TODO: if there end up being more ruby files, might simplify things to add zeitwerk

module SimpleRbBLE
  class Error < StandardError; end
  # Your code goes here...
end
