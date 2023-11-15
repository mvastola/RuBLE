# frozen_string_literal: true

module RuBLE
  module Build
    module GithubDep
      class SimpleBLE < Base
        GITHUB_REPO = 'OpenBluetoothToolbox/SimpleBLE'
        EXTRACT_OS_PATH_PRUNE = {
          macos:   'Users/runner/work/SimpleBLE/SimpleBLE/build/install',
          windows: 'install',
          linux:   'SimpleBLE/SimpleBLE/build_simpleble/install',
        }.freeze

        class << self
          def os_arch
            if OS.mac?
              OS.host_cpu == 'x86_64' ? 'macos-x86_64' : 'macos-arm64'
            elsif OS.linux?
              return 'armv6' if OS.host_cpu.start_with?('arm')

              OS.bits == 32 ? 'x86' : 'x64'
            elsif OS.windows?
              OS.bits == 32 ? 'Win32' : 'x64'
            else
              raise 'Unsupported OS'
            end
          end

          def os
            return 'macos' if OS.mac?
            return 'windows' if OS.windows?
            return 'linux' if OS.linux?

            raise 'Unsupported OS'
          end

          def os_asset_name = "#{os}-#{os_arch}"
        end

        def initialize(**kwargs)
          super(**kwargs)
          raise "#{self.class.name} currently only supports precompiled libraries" unless precompiled?
        end

        # Static makes sense for distribution if we can do it
        memoize def asset_prefix = (static? ? 'simpleble_static' : 'simpleble_shared')
        memoize def asset_name = self.class.os_asset_name
        memoize def asset_filename = "#{asset_prefix}_#{asset_name}.zip".freeze
        memoize def matching_asset = assets.fetch(asset_filename).freeze

        memoize def extract_src_path
          EXTRACT_OS_PATH_PRUNE.fetch(self.class.os.to_sym).freeze
        end

        memoize def build_config
          super.dup.tap do |data|
            data[:archive_path] = extract_src_path
          end.freeze
        end
      end
    end
  end
end