# frozen_string_literal: true

module RuBLE
  module Build
    module Settings
      class CLIFlag
        class << self
          def alias(key, flags, value, desc: UNSET, &block)
            new(key, flags, default: value, desc:, &block)
          end
        end

        attr_reader *%i[key flags args type desc initial default block]
        def initialize(key, flags, *args, type: UNSET, desc: UNSET,
                       initial: UNSET, default: UNSET, &block)
          @key = key.to_s.split('.')
          @flags = [flags].flatten
          @args = args
          @type = type
          @desc = desc
          @initial = initial
          @default = default
          if [TrueClass, FalseClass].include?(@type)
            @initial = @type == TrueClass unless initial?
            @default = true unless default?
          end
          @block = block_given? ? block : UNSET
        end

        def type? = !type.equal?(UNSET)
        def desc? = !desc.equal?(UNSET)
        def initial? = !initial.equal?(UNSET)
        def default? = !default.equal?(UNSET)
        def block? = !block.equal?(UNSET)

        def set_result_fallback(source)
          source.set_result(key, initial, force: false) if initial?
        end

        def configure(source)
          parser_args = [*flags, type, desc, *args].flatten.compact
          source.parser.on(*parser_args) do |val = UNSET|
            val = default if val.equal?(UNSET) && default?
            val = block? ? block.call(val, key, source) : val

            raise "Attempted to set value of setting #{key} to UNSET" if val.equal?(UNSET)

            source.set_result(key, val)
            val
          end
        end
      end
    end
  end
end
