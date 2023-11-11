# frozen_string_literal: true

module RuBLE
  module Build
    module Data
      module Rice
        include Memery

        memoize def rice_spec = Data['rice']
        memoize def rice_full_path = Pathname.new(rice_spec.full_gem_path).freeze
        memoize def rice_include_dir = (rice_full_path / 'include').freeze
        memoize def rice_version = rice_spec.version.freeze

        memoize def rice_data
          {
            gem_path:    rice_full_path.to_s,
            include_dir: rice_include_dir.to_s,
            version:     rice_version.to_s,
          }.freeze
        end
      end
    end
  end
end
