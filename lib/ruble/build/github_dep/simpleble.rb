# frozen_string_literal: true

module RuBLE
  module Build
    module GithubDep
      class SimpleBLE < Base
        GITHUB_REPO = 'OpenBluetoothToolbox/SimpleBLE'

        def initialize(**kwargs)
          super(**kwargs)

          raise "#{self.class.name} currently only supports precompiled libraries" unless precompiled?
        end

        def supports_precompiled? = true
        def default_use_precompiled = true

        # Static makes sense for distribution if we can do it
        memoize def asset_prefix = static? ? 'simpleble_static' : 'simpleble_shared'
        memoize def asset_name
          if System.target_mac? && System.target_64bit? # FIXME: this regexp is a total guess
            'macos-x86_64'
          elsif System.target_mac? && System.target_arm?
            'macos-arm64'
          elsif System.target_linux? && System.target_arm? # TODO: see if this is even worth supporting (totally untested)
            warn "Precompiled versions of #{name} for ARM cpus on linux are experimental and untested."
            'linux-armv6'
          elsif System.target_linux? && System.target_64bit? # FIXME: test this
            'linux-x64'
          elsif System.target_linux? # FIXME: test this
            'linux-x86'
          elsif System.target_win? && System.target_64bit? # FIXME: test this
            'windows-x64'
          elsif System.target_win? # FIXME: test this
            'windows-Win32'
          else
            raise "Precompiled version of #{name} unavailable on #{System.target}"
          end
        end
        memoize def asset_filename = "#{asset_prefix}_#{asset_name}.zip".freeze
        memoize def matching_asset = assets.fetch(asset_filename).freeze

        memoize def extract_src_path
          os_map = {
            macos:   'Users/runner/work/SimpleBLE/SimpleBLE/build/install',
            windows: 'install',
            linux:   'SimpleBLE/SimpleBLE/build_simpleble/install',
          }
          os_map.fetch(System.target_os.to_sym).freeze
        end

        memoize def build_config
          super.dup.tap do |data|
            data[:github][:archive_path] = extract_src_path if github?
          end.freeze
        end
      end
    end
  end
end