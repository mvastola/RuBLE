# frozen_string_literal: true

module RuBLE::Build
  module Environment
    module System
      BUILD_CONFIG_KEY = :rb
      REQUIRED_KEYS = %w[CFLAGS LDFLAGS target_os target_cpu].freeze

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
            true if Settings.str_true?(v)
            false if Settings.str_false?(v)

            v
          end.freeze
        end

        memoize def target_os = ActiveSupport::StringInquirer.new(full_info['target_os'])

        # NOTE: these are all educated guesses rather than confirmed from a given source
        #   there has to be an official spec for this we can rely on. One issue though is I'm
        #   not sure if this uses the OS target triplet, or something else
        memoize def target_64bit? = target_cpu.match?(/[x_-]64(\b|[^0-9])/i)
        memoize def target_arm?   = target_cpu.match?(/\Aarm(64)?(v[0-9]+)?(e[lb])?([sh]f)?(\b|[^a-z])/i)
        memoize def target_linux? = target_os.match?(/\Alinux(\b|_)/)
        memoize def target_mac?   = target_os.match?(/\A(mac(os)?|darwin)(\b|[^a-z])/)
        memoize def target_win?   = target_os.match?(/\Awin(dows)?(\b|[^a-z])/i) || target_ming?
        memoize def target_ming?  = target_os.match?(/\A(msvc|musl|mingw?|msys)[^a-z]/)

        def [](field) = full_info[field.to_s]
        def key?(field) = full_info.key?(field.to_s)

        def respond_to_missing?(name, _) = super || key?(name)

      private

        def method_missing(name, *args) = key?(name) ? self[name] : super

      end
    end
  end
end