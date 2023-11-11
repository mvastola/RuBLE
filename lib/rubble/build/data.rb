# frozen_string_literal: true

module RubBLE
  module Build
    module Data
      class << self
        include Memery

        memoize def bundler = ::Bundler.load
        memoize def specs = bundler.specs

        memoize def this_gem
          specs.find { |gem| gem.full_gem_path == bundler.root.to_s }
        end

        memoize def [](name)
          specs.find_by_name_and_platform(name, RbConfig::CONFIG['target'])
        end
      end
    end
  end
end
