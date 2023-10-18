#!/usr/bin/env ruby
# frozen_string_literal: true

# TODO: see if I can bypass needing to prefix commands with `bundle exec` via this:
ENV["BUNDLE_GEMFILE"] ||= File.expand_path("../Gemfile", __dir__)
require "bundler/setup" # Set up gems listed in the Gemfile.
#Bundler.require

require 'SimpleRbBLE'
require 'securerandom'

#require 'rainbow/ext/string'
#require_relative 'tracepoint_manager'

@ba1 = SimpleRbBLE::ByteArray.new(1234)
@ba2 = SimpleRbBLE::ByteArray.new('abc')
@ba3 = SimpleRbBLE::ByteArray.new(0xf0f0f0f0f0)
@ba4 = SimpleRbBLE::ByteArray.new(SecureRandom.uuid)
require "irb"
IRB.start(__FILE__)
