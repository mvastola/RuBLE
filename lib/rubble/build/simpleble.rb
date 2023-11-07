# frozen_string_literal: true

# TODO: just use bundler/zeitwerk(?)
require 'tmpdir'

module RubBLE
  module Build
    # noinspection RubyTooManyInstanceVariablesInspection
    class SimpleBLE
      include Data::Extension
      DEBUG_MODE = ENV['RUBBLE_DEBUG_ON'].to_s.strip.length.positive?
      GITHUB_REPO = 'OpenBluetoothToolbox/SimpleBLE'
      GITHUB_API_VERSION = '2022-11-28'
      GITHUB_API_BASE_URL = 'https://api.github.com'
      ASSET_PREFIX = 'simpleble_static' # Static makes sense for distribution if we can do it
      EXTRACT_OS_PATH_PRUNE = {
        macos:   'Users/runner/work/SimpleBLE/SimpleBLE/build/install',
        windows: 'install',
        linux:   '__w/SimpleBLE/SimpleBLE/build_simpleble/install',
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

        def os_asset_name
          "#{os}-#{os_arch}"
        end
      end

      ASSET_NAME = SimpleBLE.os_asset_name
      ASSET_FILENAME = "#{ASSET_PREFIX}_#{ASSET_NAME}.zip".freeze

      attr_reader :requested_tag

      def initialize(tag: 'latest')
        @requested_tag = tag.to_s.freeze
      end

      def release
        @release ||= github_api.get("/repos/#{GITHUB_REPO}/releases/#{requested_tag}").body.freeze
      end

      def real_tag_name = @real_tag_name ||= release.fetch('tag_name').freeze

      def assets = @assets ||= release.fetch('assets').to_h { [ _1.fetch('name'), _1 ] }.freeze

      def matching_asset = @matching_asset ||= assets.fetch(ASSET_FILENAME).freeze

      def download_url
        @download_url ||= matching_asset.fetch('browser_download_url') do
          raise 'Your OS appears to not be supported.'
        end.freeze
      end

      def extract_src_path
        return @extract_src_path if defined?(@extract_src_path)

        @extract_src_path ||= EXTRACT_OS_PATH_PRUNE.fetch(self.class.os.to_sym).freeze
      end

      def download_headers
        @download_headers ||= download_client.head.headers.freeze
      end

      def download_md5_checksum
        @download_md5_checksum ||= Base64.decode64(download_headers.fetch('content-md5'))
                                         .unpack1('H*').freeze
      end

      def download_size
        @download_size ||= matching_asset.fetch('size')
      end

      def temp_dir
        return @temp_dir if @temp_dir

        @temp_dir = Pathname.new(Dir.mktmpdir(%w[simpleble tmp]))
        at_exit { @temp_dir.rmtree }
        @temp_dir
      end

      def download_path = @download_path ||= temp_dir / matching_asset.fetch('name')

      def extract_dir = @extract_dir ||= temp_dir / 'extracted'

      def downloaded?
        return false unless download_path&.file?

        actual_size = download_path.size
        expected_size = download_size
        warn "Downloaded file found at #{download_path} with size #{actual_size} " \
             "(expected: #{expected_size})"
        unless actual_size == expected_size
          warn "Warning: #{download_path} is present, but does not match expected size. " \
               'Will re-download.'
          false
        end

        puts 'Success'
        true
      end

      def download!
        return if downloaded?

        progressbar = ProgressBar.create(total: download_size)
        io = download_path.open('wb')
        _resp = download_client.get do |req|
          # noinspection RubyResolve
          req.options.on_data = ->(chunk, _bytes_recd, _env) do
            io.write(chunk)
            progressbar.progress += chunk.length
          end
        end
        io.close

        progressbar.stop
        raise 'Download size/checksum mismatch!' unless downloaded?

        warn "Successfully downloaded #{download_url} to #{download_path}!"
      rescue StandardError
        # noinspection RubyScope
        progressbar.stop
        raise
      end

      def extract!
        require 'zip'
        extract_dir.rmtree if extract_dir.exist?
        extract_dir.mkpath
        puts "Extracting #{download_path} to #{extract_dir}..."
        progressbar = nil
        Dir.chdir extract_dir.to_s do
          # see https://github.com/rubyzip/rubyzip#reading-a-zip-file
          zip_file = Zip::File.open(download_path.to_s)
          raise "Could not open zip file #{zip_file}" unless zip_file

          matching_files = zip_file.glob("#{extract_src_path}/{**/,}*")
          progressbar = ProgressBar.create(total: matching_files.count)
          matching_files.each do |entry|
            relative_out_path = Pathname.new(entry.name.delete_prefix("#{extract_src_path}/")).cleanpath

            if relative_out_path.absolute? || relative_out_path.each_filename.include?('..')
              raise "Malformed zip file! (Encountered unsafe path #{relative_out_path})"
            end

            entry.extract(relative_out_path.to_s)
            progressbar.increment
          end
          progressbar.stop
        rescue StandardError
          progressbar&.stop
          raise
        end
        puts "Archive #{download_path} successfully extracted into #{extract_dir}."
      end

      def gem_spec
        @gem_spec ||= Bundler.load.specs.find_by_name_and_platform('RubBLE', RUBY_PLATFORM)
      end

      def full_gem_path = @full_gem_path || extension_full_path

      def install_dir_basename
        @install_dir_basename ||= ASSET_FILENAME.delete_suffix('.zip') + "_#{real_tag_name}"
      end

      def install_dir
        @install_dir ||= Pathname.new(full_gem_path) / 'tmp' / install_dir_basename
      end

      def install!
        install_dir.rmtree if install_dir.exist?
        install_dir.parent.mkpath
        puts "Installing extracted files in #{extract_dir} into #{install_dir}"
        FileUtils.mv extract_dir.to_s, install_dir.to_s, force: false, verbose: true, secure: true
        puts "Downloaded #{self.class.name} successfully installed into #{install_dir}!"
      end

      def setup!
        return if @config_data

        download!
        extract!
        install!

        @config_data = {
          requested_tag:,
          release_tag:      real_tag_name,
          release:          release.dup.tap { _1.delete('assets') },
          asset:            matching_asset.dup.tap { _1.delete('uploader') },
          install_location: install_dir,
        }.freeze
      end

      def config_data
        raise '#setup! must be called prior to this function' unless @config_data

        @config_data
      end

    protected
      def github_api
        return @github_api if @github_api

        options = {
          headers: {
            'X-GitHub-Api-Version' => GITHUB_API_VERSION,
            'Accept'               => 'application/vnd.github+json',
          },
          request: { timeout: 15 },
        }
        @github_api = Faraday.new(GITHUB_API_BASE_URL, options) do |f|
          f.response :json
          f.request :retry
          f.response :raise_error
          f.response :follow_redirects
        end
      end

      def download_client
        return @download_client if @download_client

        options = { request: { timeout: 15 } }
        @download_client = Faraday.new(download_url, options) do |f|
          f.request :retry
          f.response :raise_error
          f.response :follow_redirects
        end
      end
    end
  end
end
