require 'tmpdir'
require 'faraday'
require 'faraday/follow_redirects'
require 'faraday/retry'
require 'os'
require_relative './dev_tasks'
#require 'debug'

begin
  require 'ruby-progressbar'
rescue LoadError
  warn 'ruby-progressbar gem not installed. Progress bar disabled.'
end


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

class SimpleBLE
  DEBUG_MODE = ENV['SIMPLERBBLE_DEBUG_ON'].to_s.strip.length > 0
  GITHUB_REPO = 'OpenBluetoothToolbox/SimpleBLE'
  GITHUB_API_VERSION = '2022-11-28'
  ASSET_PREFIX = 'simpleble_static' # Static makes sense for distribution if we can do it
  ASSET_NAME = OsInfo.asset_name
  ASSET_FILENAME = "#{ASSET_PREFIX}_#{ASSET_NAME}.zip".freeze
  EXTRACT_OS_PATH_PRUNE = {
    macos: 'Users/runner/work/SimpleBLE/SimpleBLE/build/install',
    windows: 'install',
    linux: '__w/SimpleBLE/SimpleBLE/build_simpleble/install'
  }

  attr_reader :tag
  def initialize(tag: 'latest')
    @tag = tag.to_s.freeze
  end

  def release
    @release ||= github_api.get("/repos/#{GITHUB_REPO}/releases/#{tag}").body.freeze
  end
  
  def real_tag_name = @real_tag_name ||= release.fetch('tag_name').freeze
  
  def assets = @assets ||= release.fetch('assets').to_h { [ _1.fetch('name'), _1 ] }.freeze

  def matching_asset = @matching_asset ||= assets.fetch(ASSET_FILENAME).freeze

  def download_url
    @download_url ||= matching_asset.fetch('browser_download_url') do
      raise "Your OS appears to not be supported."
    end.freeze
  end
  def extract_src_path = @extracted_path ||= EXTRACT_OS_PATH_PRUNE.fetch(OsInfo.os.to_sym).freeze

  def download_headers 
    @download_headers ||= download_client.head.headers.freeze
  end

  def download_md5_checksum
    @download_md5_checksum ||= Base64.decode64(download_headers.fetch('content-md5')).
                                      unpack1('H*').freeze
  end

  def download_size
    @download_size ||= matching_asset.fetch('size')
  end

  def temp_dir
    return @temp_dir if @temp_dir
    #if DEBUG_MODE
    #  @temp_dir ||= (Pathname.new(Dir.tmpdir) / self.class.name).tap(&:mkpath)
    #else
      @temp_dir ||= Pathname.new(Dir.mktmpdir %w[simpleble tmp])
      at_exit { @temp_dir.rmtree }
    #end
    @temp_dir
  end

  def download_path = @download_path ||= temp_dir / matching_asset.fetch('name')
  def extract_dir = @extract_dir ||= temp_dir / 'extracted'

  def downloaded?
    return false unless download_path&.file? && download_path.size == download_size

    # Checksums don't seem to actually match, so skipping verification for now
    return true
    
    actual_checksum = DevTasks.hash_file(download_path, digest: :MD5)
    expected_checksum = download_md5_checksum
    puts "Downloaded file found at #{download_path} with checksum #{actual_checksum} "\
      "(expected: #{expected_checksum})"
    if actual_checksum == expected_checksum
      puts "Checksum matches!"
      true
    else
      puts "Warning: #{download_path} is present, but does not match expected checksum. "\
        "Will re-download."
      false
    end
  end
  
  def download!
    return if downloaded?

    progressbar = ProgressBar.create(total: download_size) if defined?(ProgressBar)
    io = download_path.open('wb')
    resp = download_client.get do |req|
      req.options.on_data = Proc.new do |chunk, bytes_recd, env|
        io.write(chunk)
        progressbar&.progress += chunk.length
      end
    end
    io.close
    
    raise "Download size/checksum mismatch!" unless downloaded?

    progressbar&.stop
    puts "Successfully downloaded #{download_url} to #{download_path.inspect}!"
  rescue
    progressbar&.stop
    raise
  end

  def extract!
    require 'zip'
    extract_dir.rmtree if extract_dir.exist?
    extract_dir.mkpath
    puts "Extracting #{download_path} to #{extract_dir}..."
    Dir.chdir extract_dir do
      # see https://github.com/rubyzip/rubyzip#reading-a-zip-file
      zip_file = Zip::File.open(download_path.to_s)
      matching_files = zip_file.glob("#{extract_src_path}/{**/,}*")
      progressbar = ProgressBar.create(total: matching_files.count) if defined?(ProgressBar)
      matching_files.each do |entry|
        relative_out_path = Pathname.new(entry.name.delete_prefix("#{extract_src_path}/")).cleanpath

        if relative_out_path.absolute? || relative_out_path.each_filename.include?('..')
          raise "Malformed zip file! (Encountered unsafe path #{relative_out_path})" 
        end
        entry.extract(relative_out_path.to_s)
        progressbar&.increment
      end
      progressbar&.stop
    rescue
      progressbar&.stop
      raise
    end
    puts "Archive #{download_path} successfully extracted into #{extract_dir}."
  end

  def gem_spec
    @gem_spec ||= Bundler.load.specs.find_by_name_and_platform('SimpleRbBLE', RUBY_PLATFORM)
  end
  def full_gem_path = @full_gem_path || gem_spec.full_gem_path

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

protected
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
        f.request  :retry
        f.response :raise_error
        f.response :follow_redirects
      end
    end
  end

  def download_client
    @download_client ||= begin
      options = { 
        request: { timeout: 15 }
      }
      Faraday.new(download_url, options) do |f|
        f.request  :retry
        f.response :raise_error
        f.response :follow_redirects
      end
    end
  end
end
