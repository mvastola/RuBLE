# frozen_string_literal: true

module Rubble
  module Build
    module Data
      # noinspection RubyClassModuleNamingConvention
      module OS
        OS_FIELDS = %i[dev_null cpu_count host posix? linux? host_os iron_ruby? bits host_cpu
                     java? windows? mac? jruby? osx? x? freebsd? cygwin?].freeze

        def os_data
          return @os_data if @os_data

          @os_data = OS_FIELDS.to_h { |name| [name.to_s.delete_suffix('?'), ::OS.send(name)] }
          @os_data['os_release'] = OS.parse_os_release rescue nil
          @os_data.compact!.freeze
        end
      end
    end
  end
end
