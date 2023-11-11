# frozen_string_literal: true

module RuBLE
  module Build
    module Data
      # noinspection RubyClassModuleNamingConvention
      module OS
        include Memery

        OS_FIELDS = %i[dev_null cpu_count host posix? linux? host_os iron_ruby? bits host_cpu
                     java? windows? mac? jruby? osx? x? freebsd? cygwin?].freeze

        memoize def os_data
          result = OS_FIELDS.to_h { |name| [name.to_s.delete_suffix('?'), ::OS.send(name)] }
          result['os_release'] = OS.parse_os_release rescue nil
          result.compact.freeze
        end
      end
    end
  end
end
