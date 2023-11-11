require_relative '../lib/rubble/build'
require 'pathname'

CMake = Class.new { include RubBLE::Build::CMake }.new

def flatten_hash(...) = CMake.cmake_flatten_hash(...)
def generate(...)     = CMake.cmake_generate(...)

test = {
  a: [1,2,3, Pathname.new(__FILE__), :sym],
  b: {
    c: Pathname.new(__dir__),
    d: true,
    e: false,
    f: nil,
    g: 10,
    h: 12.2
  },
  i: nil,
  j: 'foo',
  k: :bar,
  l: %i[m n o p],
  m: '$"fa\x',
  n: ('a'..'z').to_a.join * 20
}

#pp test
#pp flatten_hash(test)

generate(test)
