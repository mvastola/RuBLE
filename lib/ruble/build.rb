# frozen_string_literal: true

require_relative 'version'

require 'active_support/all'
require_relative 'build/core_ext'

ActiveSupport::Inflector.inflections(:en) do |inflect|
  inflect.acronym 'CMake'
  inflect.acronym 'RuBLE'
  inflect.acronym 'SimpleBLE'
end

module RuBLE
  module Build
    class << self
      include Memery
      memoize def zeitwerk
        require 'zeitwerk'
        Zeitwerk::Loader.new.tap do |loader|
          loader.inflector = ActiveSupport::Inflector
          loader.push_dir("#{__dir__}/build", namespace: RuBLE::Build)
          loader.ignore("#{__dir__}/build/core_ext.rb")
        end
      end
      private :zeitwerk

      def run_cmd(*args, chdir: __dir__, path: false, **kwargs)
        return nil if chdir.nil?

        out, status = ::Open3.capture2(*args, chdir:, stdin_data: '', **kwargs)
        return nil unless status.success?

        out.strip!
        return out.empty? ? nil : Pathname.new(out).cleanpath(false) if path

        out
      end
    end
  end
end

RuBLE::Build.send(:zeitwerk).setup
