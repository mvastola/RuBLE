# frozen_string_literal: true

module RuBLE::Build
  module Environment
    module Ruby
      BUILD_CONFIG_KEY = :rb
      REQUIRED_KEYS = %w[CFLAGS LDFLAGS target_os target_cpu].freeze

      class << self
        include Memery

        memoize def info = full_info? ? full_info : full_info.slice(*REQUIRED_KEYS)
        memoize def full_info? = true # Extconf.verbose? || Extconf.development?

        memoize def full_info
          ::RbConfig::CONFIG.select do |_k, v|
            v && !v.strip.empty?
          end.freeze
        end
      end
    end
  end
end