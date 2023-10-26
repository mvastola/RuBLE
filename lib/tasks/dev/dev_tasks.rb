# frozen_string_literal: true

# require_relative './lock_guard'
require 'json'
require_relative './pager'

class DevTasks
  HASHER = OpenSSL::Digest.new('SHA256')
  EXTENSION_NAME = 'SimpleRbBLE'
  ROOT_DIR = (Pathname.new(__dir__) / '../../..').cleanpath(false)
  TMP_DIR = ROOT_DIR / 'tmp'
  EXT_DIR = ROOT_DIR / 'ext' / EXTENSION_NAME
  TEST_DIR = ROOT_DIR / 'test'
  LIB_FILE = ROOT_DIR / 'lib' / EXTENSION_NAME / "#{EXTENSION_NAME}.so"
  TMP_BUILD_DIR = TMP_DIR / RbConfig::MAKEFILE_CONFIG['arch'] /
    EXTENSION_NAME / RbConfig::CONFIG['RUBY_PROGRAM_VERSION']
  TMP_MAKEFILE_PATH = TMP_BUILD_DIR / 'Makefile'
  REL_TMP_MAKEFILE_PATH = TMP_MAKEFILE_PATH.relative_path_from(ROOT_DIR)
  HASH_FILE = EXT_DIR / '.state_hash'
  SRC_GLOB = "{**/,}*.{{h,c}{,pp},ipp}"

  class << self
    def hash_file(path)
      File.open(path.to_s, 'rb') do |f|
        HASHER << f.readpartial(8192) until f.eof?
      end
      HASHER.hexdigest
    rescue => e
      # TODO: catch file not found errors
      raise e
    ensure
      HASHER.reset
    end

    def state_hash
      fields = []
      fields << EXT_DIR.glob("**/*.{{h,c}{,pp},ipp}").sort - %w[extconf.h]
      fields << [
        EXT_DIR / 'extconf.rb',
        EXT_DIR / 'Makefile',
        *EXT_DIR.glob("**/*.rb"),
        ROOT_DIR / 'Gemfile',
        ROOT_DIR / 'Gemfile.lock',
        ROOT_DIR / 'Rakefile',
        ROOT_DIR / '.ruby-version',
        TEST_DIR / 'Makefile',
        *ROOT_DIR.glob("*.gemspec"),
        *(EXT_DIR / 'helpers').glob(SRC_GLOB)
      ].select(&:file?).map(&method(:hash_file))

      fields << LIB_FILE.symlink? ? LIB_FILE.readlink : LIB_FILE.exist?
      JSON.pretty_generate(fields)
      # HASHER.tap { _1 << fields.to_json }.digest
    ensure
      HASHER.reset
    end
  end

  attr_reader :pager
  def initialize
    @pager = Pager.instance
    @pager.start
  end

  def write_hash_file! = HASH_FILE.write(self.class.state_hash)

  def pristine!
    HASH_FILE.unlink if HASH_FILE.exist?
    LIB_FILE.unlink if LIB_FILE.exist?
    TMP_DIR.rmtree if TMP_DIR.exist?
    [ EXT_DIR, TEST_DIR ].each do |dir|
      pager.exec_paginated!('make clean', chdir: dir) if (dir / 'Makefile').exist?
    end
    Rake::Task['clobber'].tap(&:reenable).invoke
    Rake::Task['clean'].tap(&:reenable).invoke
  end

  def reconfigure!
    pristine!
    pager.exec_paginated!(%W[bundle exec rake #{REL_TMP_MAKEFILE_PATH}].shelljoin, chdir: ROOT_DIR)
    # CLion needs a makefile in EXT_DIR to be happy :-\
    pager.exec_paginated!(%W[bundle exec ruby extconf.rb -- --with-debug].shelljoin, chdir: EXT_DIR)

    # from LIB_FILE.parent directory, to make proper symlink
    relative_library_path = (TMP_BUILD_DIR / LIB_FILE.basename).relative_path_from(LIB_FILE.parent)
    LIB_FILE.make_symlink(relative_library_path)
    write_hash_file!
  end

  def needs_reconfigure? = !HASH_FILE.exist? || HASH_FILE.read.strip != self.class.state_hash
  def maybe_reconfigure! = needs_reconfigure? && reconfigure!

  def build!(*flags)
    verbose = flags.include?(:silent) ? 0 : 1
    num_jobs = flags.include?(:single) ? 1 : Etc.nprocessors
    maybe_reconfigure!
    [ TMP_BUILD_DIR, TEST_DIR ].each do |dir|
      pager.exec_paginated!(%W[make -j#{num_jobs} V=#{verbose} CXX=g++-13], chdir: dir)
    end
  end
end

