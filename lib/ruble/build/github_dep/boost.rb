# frozen_string_literal: true

module RuBLE
  module Build
    module GithubDep
      class Boost < Base
        GITHUB_REPO = 'boostorg/boost'

        def initialize(**kwargs)
          super(**kwargs)
          raise "#{self.class.name} does not support precompiled libraries yet." if precompiled?
        end

        memoize def asset_extension
          case System.host_os
          when System.target_linux? || System.target_mac? # (confirm builtin support for .tar.xz on mac)
            'tar.xz'
          when System.target_win? && !System.target_ming? # (confirm builtin support for .7z on win)
            '7z'
          else
            'tar.gz'
          end
        end
        memoize def asset_filename = "#{release_name}.#{asset_extension}".freeze
        memoize def matching_asset = assets.fetch(asset_filename).freeze
      end
    end
  end
end