#!/usr/bin/env ruby
# frozen_string_literal: true

# TODO: see if I can bypass needing to prefix commands with `bundle exec` via this:
ENV["BUNDLE_GEMFILE"] ||= File.expand_path("../Gemfile", __dir__)
#require "bundler/setup" # Set up gems listed in the Gemfile.
#Bundler.require

require 'rubble'

require 'rainbow/ext/string'
require_relative 'tracepoint_manager'

# debugger
SCAN_TIMEOUT = 10
SCAN_INTERVAL = 0.1

class Tester 
  class << self

    def callback(peripheral)
      puts "Found: #{peripheral.inspect}"
      if peripheral.identifier == 'S&B VOLCANO H' && !@dev
        @dev ||= peripheral
        @adapter.scan_stop
      end
    end

    def registry = @registry ||= RubBLE::AdapterRegistry.registry

    def adapters = @adapters ||= RubBLE::Adapter.get_adapters

    def adapter
      return @adapter if @adapter

      @adapter = adapters.first
      #rubble.c_debug
      puts "#{@adapter.inspect} (#{@adapter.object_id}) -- #{@adapter.address}"

      @callback_proc = method(:callback).to_proc
      @adapter.on_scan_find(@callback_proc)
      @adapter
    end

    def device
      return @dev if @dev
      
      # Testing out connecting to a device so I can send a command
      adapter.scan_start
      puts "scan started"
      (SCAN_TIMEOUT / SCAN_INTERVAL).floor.times do
        break if @dev
        break unless adapter.scanning?
        sleep SCAN_INTERVAL
      end
      adapter.scan_stop unless adapter.scanning? # only applies if cb not called
      puts "scan stopped"

      raise "Target device not detected" unless @dev
      @dev.connect
      at_exit { @dev&.disconnect }
      @dev
    end

    def service
      @svc3 ||= device.services.detect do |svc| 
        svc.uuid == '10110000-5354-4f52-5a26-4249434b454c' 
      end
    end

    def characteristics
      @chars ||= service.characteristics.each.to_a
    end

    def characteristic
      @char ||= characteristics.detect { _1.uuid == '10110013-5354-4f52-5a26-4249434b454c' }
    end

    def descriptors
      @descs ||= characteristic.descriptors.each.to_a
    end

    def run!
      puts "Adapter registry is #{registry.inspect}"
      puts "Adapter is #{adapter.inspect}"
      puts "Device is #{device.inspect}"
      puts "Service is #{service.inspect}"
      puts "Characteristics are #{characteristics.inspect}"
      puts "Characteristic is #{characteristic.inspect}"
      puts "Descriptors are #{descriptors.inspect}"
    end
  end
end

if ENV['RUBY_GC_DISABLE']
  puts "Disabling ruby garbage collection"
  GC.disable 
elsif ENV['RUBY_GC_STRESS']
  puts "Enabling ruby garbage collection stress"
  GC.stress = true 
end


#TracePointManager.wrap do
  Tester.run!
#end

#require "irb"
#IRB.start(__FILE__)
