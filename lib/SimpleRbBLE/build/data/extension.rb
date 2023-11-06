# frozen_string_literal: true

module SimpleRbBLE
  module Build
    module Data
      module Extension
        def extension_spec = @extension_spec ||= Data.this_gem
        def extension_name = @extension_name ||= extension_spec.name.freeze
        def extension_full_path = @extension_full_path ||= extension_spec.full_gem_path.freeze
        def extension_version = @extension_version ||= extension_spec.version.freeze

        # TODO: Add library metadata (such as version/commit hash, etc) to be passed to
        #   `ld --package-metadata` (https://systemd.io/ELF_PACKAGE_METADATA/)
        def extension_data
          @extension_data ||= {
            name:    extension_name,
            version: extension_version.to_s,
            path:    extension_full_path.to_s,
          }.freeze
        end
      end
    end
  end
end
