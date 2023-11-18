# frozen_string_literal: true

module RuBLE::Build
  module Environment
    module System
      BUILD_CONFIG_KEY      = :rb
      REQUIRED_KEYS         = %w[CFLAGS LDFLAGS target_os target_cpu].freeze

      CPU_64BIT_REGEX       = /[x_-]64(\b|[^0-9])/i
      CPU_ARM_REGEX         = /\A(arm|aarch)(64)?(v[0-9]+)?(e[lb])?([sh]f)?(\b|[^a-z])/i
      OS_LINUX_REGEX        = /\Alinux(\b|_)/
      OS_MAC_REGEX          = /\Adarwin(\b|[^a-z])/
      OS_WIN_REGEX          = /\A(ms)?win(dows)?(\b|[^a-z])/i
      OS_WIN_MING_ISH_REGEX = /\A(cygwin|msvc|musl|mingw?|msys)(\b|[^a-z])/

      class << self
        include Memery

        memoize def info
          config = full_info? ? full_info.dup : full_info.slice(*REQUIRED_KEYS).freeze
          config['target_64bit'] = System.target_64bit?
          config['target_arm']   = System.target_arm?
          config['target_linux'] = System.target_linux?
          config['target_mac']   = System.target_mac?
          config['target_win']   = System.target_win?
          config
        end
        memoize def full_info? = true # Extconf.verbose? || Extconf.developer?

        memoize def full_info
          ::RbConfig::CONFIG.select do |_k, v|
            v && !v.strip.empty?
          end.transform_values do |v|
            next true if Settings.str_true?(v)
            next false if Settings.str_false?(v)

            v
          end.freeze
        end

        memoize def target_os = ActiveSupport::StringInquirer.new(full_info['target_os'])

        # NOTE: these are all educated guesses rather than confirmed from a given source
        #   there has to be an official spec for this we can rely on. One issue though is I'm
        #   not sure if this uses the OS target triplet, or something else
        memoize def target_64bit? = target_cpu.match?(CPU_64BIT_REGEX)
        memoize def target_arm?   = target_cpu.match?(CPU_ARM_REGEX)
        memoize def target_linux? = target_os.match?(OS_LINUX_REGEX)
        memoize def target_mac?   = target_os.match?(OS_MAC_REGEX)
        memoize def target_win?   = target_os.match?(OS_WIN_REGEX) || target_ming?
        memoize def target_ming?  = target_os.match?(OS_WIN_MING_ISH_REGEX)

        def [](field) = full_info[field.to_s]
        def key?(field) = full_info.key?(field.to_s)

        def respond_to_missing?(name, _) = super || key?(name)

      private

        def method_missing(name, *args) = key?(name) ? self[name] : super

      end
    end
  end
end