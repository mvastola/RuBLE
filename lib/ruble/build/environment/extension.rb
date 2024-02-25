# frozen_string_literal: true

require 'open3'

module RuBLE::Build::Environment
  module Extension
    BUILD_CONFIG_KEY = :rb_ext

    class << self
      include Memery
      delegate :run_cmd, to: ::RuBLE::Build
      memoize def bundler = ::Bundler.load
      memoize def specs = bundler.specs
      memoize def spec = specs.find { |gem| gem.full_gem_path == bundler.root.to_s }
      memoize def find_spec(name) = specs.find_by_name_and_platform(name, System.target)

      memoize def gem_name = spec.name.freeze
      memoize def gem_version = spec.version.freeze

      memoize def root_dir = bundler.root.cleanpath(false)
      alias_method :full_path, :root_dir
      memoize def ext_dir = (root_dir / spec.extensions.first).parent

      memoize def git? = run_cmd('git --version')
      memoize def git_root = run_cmd('git rev-parse --show-toplevel', path: true)
      memoize def git_root? = !!git_root
      memoize def git_head = run_cmd('git rev-parse HEAD', chdir: git_root)
      memoize def git_status = run_cmd('git status --porcelain', chdir: git_root)
      memoize def git_dirty? = git_status.nil? || git_status&.empty?
      memoize def git_last_tag = run_cmd('git describe --tags --abbrev=0', chdir: git_root)
      memoize def git_tagged? = git_root? && !git_dirty? && git_last_tag == git_head
      memoize def git_tag = (git_last_tag if git_tagged?)
      memoize def git_commit_str = [git_head, git_dirty? && '-dirty'].select(&:itself).join

      memoize def github_url = spec.metadata['source_code_uri']
      memoize def github_release_asset(asset:, tag: git_tag)
        "#{github_url}/releases/download/#{tag}/#{asset}".freeze if tag
      end

      memoize def my_ruby_home = ENV['MY_RUBY_HOME']

      memoize def debug_asset_name = "#{name}.#{target_os}-#{target_cpu}.so.debug"
      memoize def debug_info_url   = github_release_asset(asset: debug_asset_name)

      delegate :target_os, :target_cpu, :build_os, :build_cpu, to: System

      # library metadata (such as version/commit hash, etc) to be passed to
      #   `ld --package-metadata` (https://systemd.io/ELF_PACKAGE_METADATA/)
      memoize def linker_package_metadata
        {
          type:         'gem',
          name:         gem_name,
          version:      gem_version,
          dirty:        git_dirty?, # TODO: condition these on git being present
          commit:       git_commit_str,
          release:      git_tag,
          os:           target_os,
          sysname:      Etc.uname.fetch(:sysname),
          os_rel:       Etc.uname.fetch(:release),
          os_ver:       Etc.uname.fetch(:version),
          machine:      Etc.uname.fetch(:machine),
          arch:         target_cpu,
          build_os:     System.build_os,
          arch_os:      System.build_cpu,
          libc:         (Etc.confstr(Etc::CS_GNU_LIBC_VERSION) rescue nil),
          pthread:      (Etc.confstr(Etc::CS_GNU_LIBPTHREAD_VERSION) rescue nil),
          debugInfoUrl: debug_info_url,
        }.compact.transform_keys(&:to_s).freeze
      end

      memoize def info
        {
          name:    gem_name,
          version: gem_version.to_s,
          path:    full_path.to_s,
          ext_dir:,
          my_ruby_home:,
          package: {
            linker_metadata: linker_package_metadata.to_json,
          },
        }.freeze
      end
    end
  end
end
