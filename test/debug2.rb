#!/usr/bin/env ruby
# frozen_string_literal: true

# TODO: see if I can bypass needing to prefix commands with `bundle exec` via this:
ENV["BUNDLE_GEMFILE"] ||= File.expand_path("../Gemfile", __dir__)
require "bundler/setup" # Set up gems listed in the Gemfile.
#Bundler.require

require 'rubble'

q = Rubble::RubyQueue.instance
queue_thread = Thread.new do
  Rubble.c_debug
  q.run
  puts "Queue thread shutting down"
end

@cb = Rubble::Callback.new
prok = ->() { puts 'Howdy!' }
@cb.set(prok)
puts @cb

#sleep 3
q.push(@cb)
puts 'Pushed'
#sleep 3
@cb.fire

puts q
puts queue_thread

sleep 3
Rubble.c_debug
q.stop
queue_thread.join



