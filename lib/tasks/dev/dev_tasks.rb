# frozen_string_literal: true

require 'json'
require_relative './pager'
require_relative './state_hash'
require_relative './paths'

class DevTasks
  include Paths
  include StateHash

  attr_reader :pager
  def initialize(*flags)
    @flags = flags.map(&:to_sym).to_set
    @pager = Pager.instance
    @pager.start
  end

  def flag?(name) = @flags.include?(name.to_sym)
  def quiet? = flag?(:quiet)
  def verbose? = !quiet?
  def single? = flag?(:single)
  def nproc = single? ? 1 : Etc.nprocessors

  def write_hash_file! = HASH_FILE.write(state_hash)

  def ninja_cmd(target: nil)
    verbose_flag = verbose? ? '-v' : nil
    [ 'ninja', "-j#{nproc}", verbose_flag, target].compact
  end

  def make_cmd(target: nil)
    verbose_val = verbose? ? 1 : 0
    ['make', "-j#{nproc}", "V=#{verbose_val}", 'CXX=g++-13', target].compact
  end


  def pristine!
    HASH_FILE.unlink if HASH_FILE.exist?
    pager.exec_paginated!(ninja_cmd(target: 'clean'), chdir: BUILD_DIR) if NINJA_FILE.exist?
    pager.exec_paginated!(make_cmd(target: 'clean'), chdir: TEST_DIR) if (TEST_DIR / 'Makefile').exist?
    LIB_FILE.unlink if LIB_FILE.exist?
    BUILD_INFO_FILE.unlink if BUILD_INFO_FILE.exist?
    BUILD_DIR.rmtree if BUILD_DIR.exist?
  end

  def reconfigure!
    pristine!
    pager.exec_paginated!(%W[bundle exec ruby extconf.rb -- --with-debug].shelljoin, chdir: EXT_DIR)


    BUILD_DIR.mkpath
    cmd = %W[
      cmake -G Ninja
      -DCMAKE_CXX_COMPILER=#{CMAKE_CXX_COMPILER}
      -DCMAKE_BUILD_TYPE=Debug
      -DCMAKE_INSTALL_CONFIG_NAME=Debug
      #{EXT_DIR_FROM_BUILD_DIR}
    ]
    pager.exec_paginated!(cmd, chdir: BUILD_DIR)
    write_hash_file!
  end

  def needs_reconfigure? = !HASH_FILE.exist? || HASH_FILE.read.strip != state_hash
  def maybe_reconfigure! = needs_reconfigure? && reconfigure!

  def build!
    maybe_reconfigure!
    pager.exec_paginated!(make_cmd, chdir: TEST_DIR)

    pager.exec_paginated!(ninja_cmd, chdir: BUILD_DIR)
    pager.exec_paginated!(ninja_cmd(target: 'install'), chdir: BUILD_DIR)
  end

  Symbol = Struct.new(:Symbol, *%i[address name file flag defined], keyword_init: true) do
    include Comparable

    def defined? = self.defined

    def ignore?
      return true if name =~ /\A(typeinfo|vtable|TLS init function) for /

      false
    end

    def err_if_undefined?
      name =~ /Simple(Rb)?BLE/
    end

    def <=>(other) = name <=> other.name
  end

  def audit_objfile_symbols!
    defined_symbols = Set.new
    undefined_symbols = Hash.new
    objfiles = OBJFILES_DIR.glob('{**/,}*.o')
    filenames = objfiles.map(&:to_s)
    cmd = %W[nm -A -C --defined-only] + filenames
    _result, out, _err = pager.exec_unpaginated!(cmd, chdir: BUILD_DIR)
    out.strip.split(/[\n\r]+/).each do |line|
      file, _, line = line.partition(/:\s*/)
      addr_str, flag, name = line.strip.split(/\s+/, 3)
      address = addr_str.to_i(16)
      sym = Symbol.new(address:, name:, flag:, file:, defined: true)
      defined_symbols << sym
    end

    cmd = %W[nm -A -C --undefined-only] + filenames
    _result, out, _err = pager.exec_unpaginated!(cmd, chdir: BUILD_DIR)
    out.strip.split(/[\n\r]+/).each do |line|
      file, line = line.split(/:\s*/,2)
      flag, name = line.strip.split(/\s+/, 2)
      sym = Symbol.new(name:, flag:, file:, defined: false)
      next if sym.ignore?

      undefined_symbols[name] ||= []
      undefined_symbols[name] << sym
    end

    defined_symbol_names = defined_symbols.map(&:name).to_set
    undefined_symbol_names = undefined_symbols.keys.to_set
    missing_symbol_names = undefined_symbol_names - defined_symbol_names
    missing_symbols = undefined_symbols.values_at(*missing_symbol_names).flatten
    puts 'The following symbols are not defined in the object files, but should be:'
    missing_symbols.select!(&:err_if_undefined?)

    missing_symbols.map(&:name).uniq.each { puts _1 }
  end
end

