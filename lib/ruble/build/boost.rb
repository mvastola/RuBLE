# frozen_string_literal: true

module RuBLE
  module Build
    class Boost < GithubRepo
      GITHUB_REPO = 'boostorg/boost'
      OS_ARCHIVE_EXT = {
        # ** = TODO: confirm standard tools on this OS support these
        /\Alinux/                => 'tar.xz',
        /\A(macos|darwin)/       => 'tar.xz', # **
        /\A(ms)?win(32|64|dows)/ => '7z',     # **
        /\Amingw|msys/           => 'tar.gz', # **
        :fallback                => 'tar.gz', # **
      }.freeze

      memoize def asset_extension
        OS_ARCHIVE_EXT.detect do |_k, v|
          OS.host_os === v # rubocop:disable Style/CaseEquality
        end&.first || OS_ARCHIVE_EXT.fetch(:fallback)
      end

      # Static makes sense for distribution if we can do it
      memoize def asset_filename = "#{release_name}.#{asset_extension}".freeze
      memoize def matching_asset = assets.fetch(asset_filename).freeze
      memoize def extract_src_path = '[UNUSED]'

      memoize def config_data
        super.dup.tap do |data|
          data[:archive_path] = extract_src_path
        end.freeze
      end
    end
  end
end
