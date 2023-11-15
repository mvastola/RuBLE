# frozen_string_literal: true

module RuBLE
  module Build
    module GithubDep
      class Base
        include Memery
        include Data::Extension

        GITHUB_API_VERSION = '2022-11-28'

        class << self
          include Memery
          def github_repo = const_get(:GITHUB_REPO)
          def github_repo_url = "https://github.com/#{github_repo}"
          def github_api_base_url = "https://api.github.com/repos/#{github_repo}"

          memoize def dependency_versions
            YAML.laod_file(Data.ext_dir / 'dependencies.yml').tap(&:deep_symbolize_keys!).freeze
          end
        end

        # def cli_flag_prefix = "--#{self.class.name.underscore.gsub('_', '-')}".freeze
        # def env_prefix = "#{Data.gem_name.upcase}_#{self.class.name.underscore}_".freeze

        attr_reader *%i[requested_tag static precompiled local]
        def initialize(supports_precompiled: false, **config)
          raise "Cannot instantiate #{self.class} directly. Must subclass." if self.class == GithubDep::Base

          config.deep_symbolize_keys!
          @requested_tag = config.fetch(:tag, default_release_tag).to_s.freeze
          @local = config.fetch(:local, false)
          @static = config.fetch(:static, !local || Extconf.release?) # !Extconf.development?)
          @precompiled = config.fetch(:precompiled, false)

          if precompiled? && !supports_precompiled
            raise ArgumentError, "Precompiled releases of #{name} are not available."
          end
          raise ArgumentError, "Option 'local' for #{name} is not yet supported." if local
        end

        memoize def gem_spec = Extconf.instance.spec
        memoize def github_repo = self.class.github_repo
        memoize def github_repo_url = self.class.github_repo_url
        memoize def github_api_base_url = self.class.github_api_base_url

        memoize def local? = local
        memoize def static? = static
        memoize def shared = !static?
        memoize def shared? = shared
        memoize def linkage = (static? ? 'static' : 'shared')
        memoize def precompiled? = precompiled
        memoize def build? = !precompiled?

        memoize def default_release_tag
          Extconf.development? ? latest_release_tag : supported_release_tag
        end

        memoize def supported_release_tag
          GithubDep::Base.dependency_versions.fetch(self.class.name.&underscore.to_sym)
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
          tag = latest_release_tag if tag == 'latest'

          github_api.get("releases/tags/#{tag}").body.tap(&:deep_symbolize_keys!).freeze
        end

        memoize def release_name = release.fetch(:name).freeze

        memoize def real_tag_name = release.fetch(:tag_name).freeze

        memoize def ref_hash(refspec)
          github_api.get("commits/#{refspec}").body.fetch('sha').freeze
        end

        memoize def commit_hash = ref_hash(real_tag_name)

        memoize def assets = release.fetch(:assets).to_h { [ _1.fetch(:name), _1 ] }.freeze

        def matching_asset
          raise NotImplementedError, 'This function must be defined in the subclass'
        end

        memoize def download_size = matching_asset.fetch(:size)

        memoize def build_config
          release_data = release.dup.tap { _1.delete(:assets) }
          release_data[:author]&.reject! { |k, _v| k.to_s.end_with?('_url') }

          asset_data = matching_asset.dup.tap { _1.delete(:uploader) }

          {
            **filter_github_metadata(release_data),
            linkage:,
            static:,
            shared:,
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
            # f.response :logger
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