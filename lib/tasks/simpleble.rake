desc 'Fetch SimpleBLE release'
namespace :SimpleBLE do
  task :initialize, %i[release] do |task, args|
    opts = { tag: args.release }.compact

    require_relative 'lib/simpleble'
    @simpleble = SimpleBLE.new(**opts)
  end

  task info: %i[initialize] do
    pp @assets = @simpleble.assets
    pp @asset = @simpleble.matching_asset
    pp @url = @simpleble.download_url
    pp @hash = @simpleble.download_md5_checksum
  end

  task download: %i[initialize] do
    @simpleble.download!
  end

  task extract: %i[download] do
    @simpleble.extract!
  end

  task install: %i[extract] do
    @simpleble.install!
  end
end

