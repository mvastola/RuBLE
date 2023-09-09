# frozen_string_literal: true

require_relative "SimpleRbBLE/version"
require_relative "SimpleRbBLE/SimpleRbBLE.so"
require_relative "SimpleRbBLE/callback_holder"
require_relative "SimpleRbBLE/callback_concern"
require_relative "SimpleRbBLE/adapter"

# TODO: do I require the shared library from here(?)
# TODO: if there end up being more ruby files, might simplify things to add zeitwerk

module SimpleRbBLE
  class Error < StandardError; end
  # Your code goes here...
end
