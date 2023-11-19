# frozen_string_literal: true

module RuBLE
  module Build
    module GithubDep
      class Base
        include Memery

        System = Environment::System
        HTTP_TIMEOUT = 15
        # Github's limit is 60 requests/hr, by setting the cache timeout to 15 mins
        #   we need to be under 15 requests per run of extconf (reality should be ~6/run max)
        #   this might need to be tweaked if we have lots of people developing on shared IPs,
        #   since limits are per-IP
        HTTP_CACHE_TIMEOUT = 15.minutes
        GITHUB_API_VERSION = '2022-11-28'
        GITHUB_METADATA_FILTER_REGEX = /(\A|_)(mentions|reactions|node_id|body|gravatar)(\z|_)/

        class << self
          include Memery
          def github_repo = const_get(:GITHUB_REPO)
          def github_repo_url = "https://github.com/#{github_repo}"
          def github_api_base_url = "https://api.github.com/repos/#{github_repo}"

          memoize def dependency_versions
            YAML.load_file(Environment::Extension.ext_dir / 'dependencies.yml')
                .tap(&:deep_symbolize_keys!).freeze
          end

          memoize def faraday_cache
            return unless faraday_cache?

            cache_path = (Environment::Extension.root_dir / 'tmp/cache/faraday').tap(&:mkpath)
            ActiveSupport.cache_format_version = 7.1
            ActiveSupport::Cache::FileStore.new(cache_path, expires_in: HTTP_CACHE_TIMEOUT).tap(&:cleanup)
          end

          memoize def faraday_cache?
            require 'faraday-http-cache'
            true
          rescue LoadError
            false
          end
        end

        memoize def default_local_path = false
        memoize def default_link_statically = !local? || Extconf.release? # !Extconf.developer?)
        memoize def default_use_precompiled = false
        memoize def include_prereleases? = false
        memoize def supports_precompiled? = false
        memoize def supports_local_path? = false
        memoize def supports_shared_link? = Extconf.developer?

        attr_reader *%i[requested_tag static precompiled local local_path]
        def initialize(**config)
          raise "Cannot instantiate #{self.class} directly. Must subclass." if instance_of?(GithubDep::Base)

          config.deep_symbolize_keys!
          @requested_tag = config.fetch(:tag, default_release_tag).to_s.freeze
          @static = config.fetch(:static, default_link_statically)
          @precompiled = config.fetch(:precompiled, default_use_precompiled)
          @local_path = config.fetch(:local, default_local_path)
          @local = @local_path.present?

          if local?
            @local_path = Pathname.new(@local_path).expand_path(Dir.pwd).cleanpath(false)

            unless @local_path.directory?
              raise ArgumentError, "Local path given for #{self.class} (#{@local_path}) must exist!"
            end
          end
          validate_config!
        end

        memoize def validate_config!
          if shared? && !supports_shared_link?
            raise ArgumentError, "#{name} can only be linked as a shared library "\
                                 'with current settings (developer mode likely required).' 
          elsif precompiled? && !supports_precompiled?
            raise ArgumentError, "Precompiled releases of #{name} are not available."
          elsif local?
            raise ArgumentError, "Using a prebuilt local copy of #{name} is not yet supported." 
          end
        end

        memoize def name = self.class.name.demodulize.freeze || 'ANONYMOUS'
        memoize def github_repo = self.class.github_repo
        memoize def github_repo_url = self.class.github_repo_url
        memoize def github_api_base_url = self.class.github_api_base_url
        delegate :target_cpu, :target_os, to: System

        alias_method :local?, :local
        memoize def github? = !local?
        memoize def source = (local? ? 'local' : 'github')
        memoize def local_path? = local_path.present?
        memoize def local_path_autodetect? = local? && !local_path?
        alias_method :local_path_autodetect, :local_path_autodetect?

        memoize def shared = !static?
        alias_method :static?, :static
        alias_method :shared?, :shared
        memoize def linkage = (static? ? 'static' : 'shared')
        memoize def precompiled? = precompiled
        memoize def download_type
          precompiled? ? 'precompiled' : 'source' unless local?
        end

        memoize def default_release_tag
          Extconf.developer? ? latest_release_tag : supported_release_tag
        end

        memoize def supported_release_tag
          GithubDep::Base.dependency_versions
            .fetch(name.underscore.to_sym)
            .fetch(:supported_release_tag)
        end

        memoize def latest_release_tag
          resp = github_api.get('releases').body.each(&:deep_symbolize_keys!)
          resp.detect do |release|
            is_draft = release.fetch(:draft)
            is_prerelease = release.fetch(:prerelease)
            !is_draft && (include_prereleases? || !is_prerelease)
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
          {
            source:,
            linkage:,
            static:,
            shared:,
            precompiled:,
            download_type:,
            github_repo_url:,
            "#{source}": send(:"#{source}_build_config"), # rubocop:disable Layout/HashAlignment
          }
        end

        memoize def precompiled_binary_url
          matching_asset.fetch(:browser_download_url) if supports_precompiled?
        end
        memoize def source_tarball_url = release.fetch(:tarball_url)
        memoize def download_url = precompiled? ? precompiled_binary_url : source_tarball_url

      protected

        memoize def local_build_config
          raise "This function shouldn't be called outside of local mode" unless local?

          { enabled: true, local_path:, local_path_autodetect: }
        end

        memoize def github_build_config
          raise "This function shouldn't be called in local mode" if local?

          release_data = release.dup.tap { _1.delete(:assets) }
          release_data[:author]&.reject! { |k, _v| k.to_s =~ /(?<!tarball)_url/ }
          asset_data = matching_asset.dup.tap { _1.delete(:uploader) }

          config = {
            commit_hash:,
            requested_tag:,
            download_url:,
            precompiled_binary_url:,
            source_tarball_url:,
            release_tag: real_tag_name, # rubocop:disable Layout/HashAlignment
          }.compact

          if Extconf.verbose?
            config.reverse_merge! filter_github_metadata(release_data)
            config[:asset] = filter_github_metadata(asset_data).compact
          end
          config
        end

        memoize def github_api
          raise "This function shouldn't be called in local mode" if local?

          options = {
            headers: {
              'X-GitHub-Api-Version' => GITHUB_API_VERSION,
              'Accept'               => 'application/vnd.github+json',
            },
            request: { timeout: HTTP_TIMEOUT },
          }
          Faraday.new(github_api_base_url, options) do |f|
            f.response :json
            f.request  :retry
            f.response :raise_error
            f.response :follow_redirects
            # prevent running over rate limit in developer mode
            if GithubDep::Base.faraday_cache?
              f.use :http_cache,
                    store:        GithubDep::Base.faraday_cache,
                    serializer:   Marshal,
                    compressor:   false,
                    shared_cache: false
            end
            # f.response :logger
          end
        end

        def filter_github_metadata(data)
          data.reject! do |k, v|
            v == '' || k.to_s.match?(GITHUB_METADATA_FILTER_REGEX)
          end
          data.transform_values! { _1.is_a?(Hash) ? filter_github_metadata(_1) : _1 }
        end
      end
    end
  end
end
