# frozen_string_literal: true

module Rubble
  module Build
    module Data
      module Rice
        def rice_spec = @rice_spec ||= Data['rice']
        def rice_full_path = @rice_full_path ||= Pathname.new(rice_spec.full_gem_path).freeze
        def rice_include_dir = @rice_include_dir ||= (rice_full_path / 'include').freeze
        def rice_version = @rice_version ||= rice_spec.version.freeze

        def rice_data
          @rice_data ||= {
            gem_path:    rice_full_path.to_s,
            include_dir: rice_include_dir.to_s,
            version:     rice_version.to_s,
          }.freeze
        end
      end
    end
  end
end
