# frozen_string_literal: true

module RuBLE::Build
  module Environment
    module OS
      BUILD_CONFIG_KEY = :rb_os
      OS_FIELDS = %i[dev_null cpu_count host posix? linux? host_os iron_ruby? bits host_cpu
                   java? windows? mac? jruby? osx? x? freebsd? cygwin?].freeze

      class << self
        include Memery

        memoize def info
          return unless Extconf.verbose?

          result = OS_FIELDS.to_h { |name| [name.to_s.delete_suffix('?'), ::OS.send(name)] }
          result['os_release'] = ::OS.parse_os_release rescue nil
          result.compact.freeze
        end
      end
    end
  end
end