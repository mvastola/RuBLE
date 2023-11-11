# TODO: see if I can bypass needing to prefix commands with `bundle exec` via this:
ENV["BUNDLE_GEMFILE"] ||= File.expand_path("../Gemfile", __dir__)
require "bundler/setup" # Set up gems listed in the Gemfile.
#Bundler.require

require 'rubble'
@cb = ->(a, b=2, *c, d:, e: 3, **f) { [a,b,c,d,e,f].inspect }
puts @result = RubBLE::Utils::get_props_array(@cb)

require "irb"
IRB.start(__FILE__)
