#!/usr/bin/env ruby
# frozen_string_literal: true

# TODO: see if I can bypass needing to prefix commands with `bundle exec` via this:
ENV["BUNDLE_GEMFILE"] ||= File.expand_path("../Gemfile", __dir__)
require "bundler/setup" # Set up gems listed in the Gemfile.
#Bundler.require

require 'ruBLE'
require 'securerandom'

#require 'rainbow/ext/string'
#require_relative 'tracepoint_manager'

@ba1 = RuBLE::ByteArray.new(1234)
@ba2 = RuBLE::ByteArray.new('abc')
@ba3 = RuBLE::ByteArray.new(0xf0f0f0f0f0)
@ba4 = RuBLE::ByteArray.new(SecureRandom.uuid)
require "irb"
IRB.start(__FILE__)
