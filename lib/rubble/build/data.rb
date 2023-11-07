# frozen_string_literal: true

module RubBLE
  module Build
    module Data
      class << self
        def bundler = @bundler ||= ::Bundler.load

        def specs = @specs ||= bundler.specs

        def this_gem
          return @this_gem if defined?(@this_gem)

          @this_gem = specs.find { |gem| gem.full_gem_path == bundler.root.to_s }
        end

        def [](name)
          @gem_specs ||= {}
          return @gem_specs[name] if @gem_specs.key?(name)

          @gem_specs[name] = specs.find_by_name_and_platform(name, RbConfig::CONFIG['target'])
        end
      end
    end
  end
end
