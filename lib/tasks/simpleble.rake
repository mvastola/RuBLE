require 'faraday'
require 'mini_portile2'
require 'os'

class OsInfo
  class << self
    def arch
      if OS.mac?
        OS.host_cpu == 'x86_64' ? 'macos-x86_64' : 'macos-arm64'
      elsif OS.linux?
        return 'armv6' if OS.host_cpu.start_with?('arm')
        OS.bits == 32 ? 'x86' : 'x64'
      elsif OS.windows?
        OS.bits == 32 ? 'Win32' : 'x64'
      else
        raise "Unsupported OS"
      end
    end

    def os
      return 'macos' if OS.mac?
      return 'windows' if OS.windows?
      return 'linux' if OS.linux?
      raise "Unsupported OS"
    end

    def asset_name
      "#{os}-#{arch}"
    end
  end
end

class SimpleBLERecipe < MiniPortile
  GITHUB_REPO = 'OpenBluetoothToolbox/SimpleBLE'
  GITHUB_API_VERSION = '2022-11-28'
  ASSET_PREFIX = 'simpleble_static' # Static makes sense for distribution if we can do it
  ASSET_NAME = OsInfo.asset_name
  EXTRACT_OS_PATH_PRUNE = {
    macos: 'Users/runner/work/SimpleBLE/SimpleBLE/build/install',
    windows: 'install',
    linux: '__w/SimpleBLE/SimpleBLE/build_simpleble/install'
  }

  class << self
    def github_api
      @github_api ||= begin
        options = { 
          headers: { 
            'X-GitHub-Api-Version' => GITHUB_API_VERSION,
            'Accept' => 'application/vnd.github+json'
          }, 
          request: { timeout: 15 }
        }
        Faraday.new('https://api.github.com', options) do |f|
          f.response :json
          f.response :raise_error
        end
      end
    end

    def latest_release
      @latest_release ||= github_api.get("/repos/#{GITHUB_REPO}/releases/latest").body
    end
    
    def latest_version = @latest_version ||= latest_release['tag_name'].delete_prefix('v').freeze
    
    def latest_release_assets = @latest_release_assets ||= latest_release['assets'].to_h { [ _1['name'], _1 ] }.freeze

    def matching_asset
      @matching_asset ||= begin
        data = latest_release_assets.fetch("#{ASSET_PREFIX}_#{ASSET_NAME}.zip")
        OpenStruct.new(data).freeze
      end
    end

    def download_url
      @download_url ||= matching_asset.fetch('browser_download_url') do
        raise "Your OS appears to not be supported."
      end
    end
    def extracted_path = @extracted_path ||= EXTRACT_OS_PATH_PRUNE[OsInfo.os]
  end

  def configure
    raise NotImplementedError
  end

  def configured?
    raise NotImplementedError
  end

  def compile
    raise NotImplementedError
  end

  def install
    raise NotImplementedError
    # see https://github.com/rubyzip/rubyzip#reading-a-zip-file
  end
end

desc 'Fetch SimpleBLE release'
namespace :SimpleBLE do
  task :show do
    pp SimpleBLERecipe.latest_version
    pp @asset = SimpleBLERecipe.matching_asset
    pp @url = SimpleBLERecipe.download_url
  end

  task :initialize do
    @simpleble_install = SimpleBLERecipe.new
  end

  task(configure: %i[initialize]) { @simpleble_install.configure }
  task(compile: %i[configure]) { @simpleble_install.compile }
  task(install: %i[compile]) { @simpleble_install.install }
end

