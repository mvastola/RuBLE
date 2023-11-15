# frozen_string_literal: true

module RuBLE
  module Build
    module Data
      class << self
        include Memery

        memoize def bundler = ::Bundler.load
        memoize def specs = bundler.specs

        memoize def spec
          specs.find { |gem| gem.full_gem_path == bundler.root.to_s }
        end
        alias_method :this_gem, :spec

        memoize def root_dir = bundler.root.cleanpath(false)
        memoize def ext_dir = (root_dir / spec.extensions.first).parent
        memoize def gem_name = spec.name.freeze
        memoize def gem_version = spec.version.freeze

        memoize def [](name)
          specs.find_by_name_and_platform(name, RbConfig::CONFIG['target'])
        end
      end
    end
  end
end
