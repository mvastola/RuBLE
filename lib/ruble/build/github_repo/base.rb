# frozen_string_literal: true

module RuBLE
  module Build
    module GithubRepo
      class Base
        include Memery
        include Data::Extension

        GITHUB_API_VERSION = '2022-11-28'

        class << self
          def github_repo = const_get(:GITHUB_REPO)
          def github_repo_url = "https://github.com/#{github_repo}"
          def github_api_base_url = "https://api.github.com/repos/#{github_repo}"
        end

        attr_reader *%i[requested_tag static precompiled path supported_release_tag]
        def initialize(**config)
          raise "Cannot instantiate #{self.class} directly. Must subclass." if self.class == GithubRepo

          config.deep_symbolize_keys!
          @requested_tag = config.fetch(:tag).to_s.freeze
          @static = config.fetch(:static)
          @path = config.fetch(:path)
          @precompiled = config.fetch(:precompiled)
          @supported_release_tag = config.fetch(:supported_release_tag)

          raise ArgumentError, "Option #{name}.path is not yet supported." if path
        end


        memoize def gem_spec = Extconf.instance.spec
        memoize def github_repo = self.class.github_repo
        memoize def github_repo_url = self.class.github_repo_url
        memoize def github_api_base_url = self.class.github_api_base_url

        memoize def static? = static
        memoize def dynamic = !static
        memoize def dynamic? = dynamic
        memoize def linkage = (static? ? 'static' : 'dynamic')
        memoize def precompiled? = precompiled
        memoize def build? = !precompiled?

        memoize def default_release_tag
          Extconf.instance.development? ? latest_release_tag : supported_release_tag
        end

        memoize def latest_release_tag(include_prereleases: false)
          resp = github_api.get('releases').body.each(&:deep_symbolize_keys!)
          resp.detect do |release|
            is_draft = release.fetch(:draft)
            is_prerelease = release.fetch(:prerelease)
            !is_draft && (include_prereleases || !is_prerelease)
          end.fetch(:tag_name)
        end

        memoize def release
          tag = requested_tag.to_s
          tag = default_release_tag if tag == 'default'

          github_api.get("releases/tags/#{tag}").body.tap(&:deep_symbolize_keys!).freeze
        end

        memoize def release_name = release.fetch(:name).freeze

        memoize def tag_from_gemspec
          class_name = self.class.name
          raise "Can't determine name of anonymous class" unless class_name

          gem_spec.metadata.fetch("#{class_name.downcase}_library_release_tag")
        end

        memoize def real_tag_name = release.fetch(:tag_name).freeze

        memoize def ref_hash(refspec)
          github_api.get("commits/#{refspec}").body.fetch('sha').freeze
        end

        memoize def commit_hash = ref_hash(real_tag_name)

        memoize def assets = release.fetch(:assets).to_h { [ _1.fetch(:name), _1 ] }.freeze

        def matching_asset
          raise NotImplementedError, 'This function must be defined in the subclass'
        end

        memoize def download_size
          matching_asset.fetch(:size)
        end


        memoize def config_data
          release_data = release.dup.tap { _1.delete(:assets) }
          release_data[:author]&.reject! { |k, _v| k.to_s.end_with?('_url') }

          asset_data = matching_asset.dup.tap { _1.delete(:uploader) }

          {
            **filter_github_metadata(release_data),
            linkage:,
            static:,
            dynamic:,
            precompiled:,
            requested_tag:,
            commit_hash:,
            local_path:    path,
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
            f.response :logger
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
end