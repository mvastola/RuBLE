# frozen_string_literal: true

module RuBLE
  module Build
    module GithubDep
      class Boost < Base
        GITHUB_REPO = 'boostorg/boost'
        OS_ARCHIVE_EXT = {
          # ** = TODO: confirm standard tools on this OS support these
          /\Alinux/                => 'tar.xz',
          /\A(macos|darwin)/       => 'tar.xz', # **
          /\A(ms)?win(32|64|dows)/ => '7z',     # **
          /\Amingw|msys/           => 'tar.gz', # **
          :fallback                => 'tar.gz', # **
        }.freeze

        def initialize(**kwargs)
          super(**kwargs)
          raise "#{self.class.name} does not support precompiled libraries yet." if precompiled?
        end

        memoize def asset_extension
          OS_ARCHIVE_EXT.detect do |_k, v|
            OS.host_os === v # rubocop:disable Style/CaseEquality
          end&.first || OS_ARCHIVE_EXT.fetch(:fallback)
        end

        memoize def asset_filename = "#{release_name}.#{asset_extension}".freeze
        memoize def matching_asset = assets.fetch(asset_filename).freeze
      end
    end
  end
end