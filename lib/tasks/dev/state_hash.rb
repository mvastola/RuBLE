# frozen_string_literal: true
require_relative 'paths'

module StateHash
  READ_BUFFER_SIZE = 8192
  
  def hash_file(path, digest: 'SHA256')
    hasher = OpenSSL::Digest.new(digest.to_s)
    File.open(path.to_s, 'rb') do |f|
      hasher << f.readpartial(READ_BUFFER_SIZE) until f.eof?
    end
    hasher.hexdigest
  rescue RefError # TODO: catch file not found errors (this is placeholder)
    nil
  end

  def watched_files = [
    *basic_gem_files,
    *build_libs_files,
    *cmake_files,
    *build_output_files,
  # TODO(?): Include lib/tasks/dev/*.rb
  ].select(&:file?)

  def watched_file_hashes = watched_files.to_h do
    [_1, hash_file(_1)]
  end

  def state_hash
    fields = [
      watched_file_hashes
    ]

    JSON.pretty_generate(fields)
  end

private

  def basic_gem_files = [
    Paths::ROOT_DIR / '.ruby-version',
    Paths::ROOT_DIR / 'Gemfile',
    Paths::ROOT_DIR / 'Gemfile.lock',
    *Paths::ROOT_DIR.glob('*.gemspec'),
    Paths::ROOT_DIR / 'Rakefile',
  ]

  def build_libs_files = [
    Paths::BUILD_LIBS_DIR.sub_ext('.rb'),
    *Paths::BUILD_LIBS_DIR.glob('*.rb'),
    *Paths::BUILD_LIBS_DIR.glob('**/*.rb')
  ]

  def cmake_files = [
    Paths::EXT_DIR / 'extconf.rb',
    Paths::EXT_DIR / 'CMakeLists.txt',
    *Paths::EXT_DIR.glob('*.cmake'),
  ]

  def build_output_files = [
    Paths::BUILD_INFO_FILE,
    Paths::NINJA_FILE,
    Paths::BUILD_DIR / 'CMakeCache.txt',
    Paths::BUILD_DIR / 'config.h',
  ]
end
