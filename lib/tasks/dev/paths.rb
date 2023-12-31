# frozen_string_literal: true

require 'pathname'
require 'rbconfig'
require_relative '../../ruble/build'

module Paths
  class << self
    include RuBLE::Build::Data::Extension
  end

  GEM_NAME = Paths.gem_name
  EXTENSION_NAME = GEM_NAME.downcase
  ROOT_DIR = Paths.gem_full_path.cleanpath(false)
  TMP_DIR = ROOT_DIR / 'tmp'
  EXT_DIR = ROOT_DIR / 'ext' / EXTENSION_NAME
  TEST_DIR = ROOT_DIR / 'test'
  LIB_DIR = ROOT_DIR / 'lib' / GEM_NAME.downcase
  LIB_FILE = LIB_DIR / "#{EXTENSION_NAME}.so"
  BUILD_LIBS_DIR = LIB_DIR / 'build'
  BUILD_DIR = TMP_DIR / RbConfig::MAKEFILE_CONFIG['arch'] /
    EXTENSION_NAME / RbConfig::CONFIG['RUBY_PROGRAM_VERSION']
  EXT_DIR_FROM_BUILD_DIR = EXT_DIR.relative_path_from(BUILD_DIR)
  BUILD_INFO_FILE = BUILD_DIR / 'build-config.cmake'
  NINJA_FILE = BUILD_DIR / 'build.ninja'
  HASH_FILE = BUILD_DIR / '.state_hash'
  SRC_GLOB = '{**/,}*.{{h,c}{,pp},ipp}'
  OBJFILES_DIR = BUILD_DIR / 'CMakeFiles' / "#{EXTENSION_NAME}.dir"
  CMAKE_CXX_COMPILER = '/usr/bin/g++-13'
end