# frozen_string_literal: true

require 'open3'

module RuBLE::Build
  module Data
    module Extension
      include Memery # TODO: use ActiveSupport::Concern so this doesn't load 10x

      extend ActiveSupport::Concern

      included do
        memoize def spec = Data.this_gem
        memoize def root_dir = Data.root_dir
        # memoize def gem_full_path = Pathname.new(gem_spec.full_gem_path).freeze
        def full_path = root_dir

        memoize def name = spec.name.freeze
        memoize def version = spec.version.freeze


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

        memoize def debug_asset_name = "#{name}.#{target_os}-#{target_cpu}.so.debug"
        memoize def debug_info_url   = github_release_asset(asset: debug_asset_name)

        memoize def target_os = RbConfig::CONFIG['target_os']
        memoize def target_cpu = RbConfig::CONFIG['target_cpu']

        # library metadata (such as version/commit hash, etc) to be passed to
        #   `ld --package-metadata` (https://systemd.io/ELF_PACKAGE_METADATA/)
        memoize def linker_package_metadata
          {
            type:         'gem',
            name:         name,
            version:      version,
            dirty:        git_dirty?,
            commit:       git_commit_str,
            release:      git_tag,
            os:           target_os,
            sysname:      Etc.uname.fetch(:sysname),
            os_rel:       Etc.uname.fetch(:release),
            os_ver:       Etc.uname.fetch(:version),
            machine:      Etc.uname.fetch(:machine),
            arch:         target_cpu,
            build_os:     RbConfig::CONFIG['build_os'],
            arch_os:      RbConfig::CONFIG['build_cpu'],
            libc:         (Etc.confstr(Etc::CS_GNU_LIBC_VERSION) rescue nil),
            pthread:      (Etc.confstr(Etc::CS_GNU_LIBPTHREAD_VERSION) rescue nil),
            debugInfoUrl: debug_info_url,
          }.compact.transform_keys(&:to_s).freeze
        end

        memoize def extension_data
          {
            name:    name,
            version: version.to_s,
            path:    full_path.to_s,
            package: {
              linker_metadata: linker_package_metadata.to_json,
            },
          }.freeze
        end

      private

        def run_cmd(*args, chdir: __dir__, path: false, **kwargs)
          return nil if chdir.nil?

          out, status = ::Open3.capture2(*args, chdir:, err: :close, stdin_data: '', **kwargs)
          return nil unless status.success?

          out.strip!
          return out.empty? ? nil : Pathname.new(out).cleanpath(false) if path

          out
        end
      end
    end
  end
end