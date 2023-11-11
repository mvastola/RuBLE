# frozen_string_literal: true

module RubBLE
  module Build
    class GithubRepo
      include Memery
      include Data::Extension

      GITHUB_API_VERSION = '2022-11-28'

      class << self
        def github_repo = const_get(:GITHUB_REPO)
        def github_repo_url = "https://github.com/#{github_repo}"
        def github_api_base_url = "https://api.github.com/repos/#{github_repo}"
      end

      attr_reader *%i[requested_tag static precompiled]
      def initialize(tag: 'latest', static: true, precompiled: true)
        # rubocop:disable Style/ClassEqualityComparison
        raise "Cannot instantiate #{self.class} directly. Must subclass." if self.class == GithubRepo

        @requested_tag = tag.to_s.freeze
        @static = static
        @precompiled = precompiled
      end

      memoize def github_repo = self.class.github_repo
      memoize def github_repo_url = self.class.github_repo_url
      memoize def github_api_base_url = self.class.github_api_base_url

      memoize def static? = static
      memoize def linkage = (static? ? 'static' : 'dynamic')
      memoize def precompiled? = precompiled
      memoize def build? = !precompiled?

      memoize def latest_release(include_prereleases: false)
        github_api.get('releases').body.detect do |release|
          is_draft = release.fetch('draft')
          is_prerelease = release.fetch('prerelease')
          !is_draft && (include_prereleases || !is_prerelease)
        end.fetch('id')
      end

      memoize def release
        # TODO: when requested_tag = 'latest', we need to filter through
        #   the most recent releases and ignore those where one or more of
        #   the keys 'draft' and 'prerelease' are set to true
        tag = requested_tag.to_s == 'latest' ? latest_release : requested_tag.to_s
        github_api.get("releases/#{tag}").body.freeze
      end

      memoize def real_tag_name = release.fetch('tag_name').freeze

      memoize def commit_hash
        github_api.get("commits/#{real_tag_name}").body.fetch('sha').freeze
      end

      memoize def assets = release.fetch('assets').to_h { [ _1.fetch('name'), _1 ] }.freeze
      
      def matching_asset
        raise NotImplementedError, "This function must be defined in the subclass"
      end

      memoize def download_size
        matching_asset.fetch('size')
      end

      memoize def gem_spec
        Bundler.load.specs.find_by_name_and_platform('RubBLE', RUBY_PLATFORM)
      end

      alias_method :full_gem_path, :gem_full_path

      memoize def config_data
        release_data = release.dup.tap { _1.delete('assets') }
        release_data['author']&.reject! { |k, _v| k.to_s.end_with?('_url') }
        asset_data   = matching_asset.dup.tap { _1.delete('uploader') }

        {
          **filter_github_metadata(release_data),
          linkage:,
          requested_tag:,
          commit_hash:,
          release_tag:   real_tag_name,
          asset:         filter_github_metadata(asset_data),
          repo_url:      github_repo_url,
        }.freeze
      end

    protected

      memoize def github_api
        options = {
          headers: {
            'X-GitHub-Api-Version' => GITHUB_API_VERSION,
            'Accept'               => 'application/vnd.github+json',
          },
          request: { timeout: 15 },
        }
        Faraday.new(github_api_base_url, options) do |f|
          f.response :json
          f.request  :retry
          f.response :raise_error
          f.response :follow_redirects
        end
      end

      def filter_github_metadata(data)
        data.reject! do |k, v|
          next true if v == ''

          k.to_s.match?(/(\A|_)(mentions|reactions|node_id|body|gravatar)(\z|_)/)
        end
        data.transform_values! { _1.is_a?(Hash) ? filter_github_metadata(_1) : _1 }
      end
    end
  end
end
