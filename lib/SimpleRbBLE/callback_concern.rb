require 'active_support/all'

module SimpleRbBLE
  module CallbackConcern
    UNSET = CallbackHolder::UNSET
    extend ActiveSupport::Concern

    class << self
      def parse_cb_setter_args(proc = UNSET, &block)
        {
          block: block_given? ? block : UNSET,
          proc: proc
        }.reject { |_, v| v == UNSET }
      end
    end

    class_methods do
      def store_callbacks_named(*cb_names)
        new_names = cb_names.reject { simplerbble_callbacks.key?(_1.to_sym) }
        new_names.each do |m|
          iv_sym = :"@#{m}"
          setter_sym = :"#{m}="
          setter_sym2 = :"set_#{m}"
          fire_sym = :"fire_#{m}"
          instance_variable_set(iv_sym, UNSET)

          define_method m do |*args, **kwargs, &block|
            parsed_args = CallbackConcern.parse_cb_setter_args(*args, **kwargs, &block)
            return instance_variable_get(iv_sym) if parsed_args.count.zero?

            val = send(setter_sym, *args, **kwargs, &block)
            val unless UNSET === val
          end

          define_method setter_sym do |*args, **kwargs, &block|
            parsed_args = CallbackConcern.parse_cb_setter_args(*args, **kwargs, &block)
            raise ArgumentError, "EXACTLY ONE of a block or a proc can be given." unless parsed_args.count == 1

            new_val = parsed_args.values.first || UNSET
            unless [Proc, NilClass].any? { _1 === new_val }
              raise ArgumentError, "Invalid or missing callback (#{new_val.inspect})"
            end

            new_ch = CallbackHolder.new
            new_ch.set(new_val) unless new_val.nil?

            instance_variable_set(iv_sym, new_ch)
          end
          alias_method setter_sym2, setter_sym

          define_method fire_sym do |*args, **kwargs, &block|
            cb = instance_variable_get(iv_sym)
            return false if !instance_variable_defined?(iv_sym) || UNSET === cb

            cb.fire(*args, **kwargs, &block)
          end
        end
      end

    protected
      def simplerbble_callbacks = @simplerbble_callbacks ||= Hash.new(UNSET)
    end
  end
end

