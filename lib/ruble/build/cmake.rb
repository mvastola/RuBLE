# frozen_string_literal: true

require 'debug'
module RuBLE::Build
  module CMake
    CMAKE_LITERALS = {
      nil   => 'NULL',
      true  => 'ON',
      false => 'OFF',
    }.freeze
    CMAKE_KEY_SEPARATOR = '_'
    STRINGY_TYPES = [ String, Symbol, Pathname ].freeze

    extend ActiveSupport::Concern

    included do
      def cmake_generate(data, path: nil)
        enum = cmake_serialized_enumerator(data)
        return enum.reduce(String.new) do |memo, obj|
          memo << "\n" unless memo.empty?
          memo << obj
        end unless path

        path = Pathname.new(path)
        path.open(File::WRONLY|File::CREAT) do |f|
          # f.sync = true
          enum.each { |field| f.puts(field) }
        end
        path
      end

      def cmake_serialize(value)
        if value.is_a?(Array)
          array = cmake_serialize_array(value)
          array.map { cmake_escape(_1, quote: true) }.join(' ')
        elsif value.is_any_of?(*STRINGY_TYPES)
          cmake_escape(value.to_s)
        elsif CMAKE_LITERALS.key?(value)
          CMAKE_LITERALS[value]
        elsif value.is_a?(Numeric)
          value.to_s # no escaping necessary
        else
          raise ArgumentError, "Unrecognized value #{value.inspect}"
        end
      end

      def cmake_serializable?(value)
        value.is_any_of?(Array, Hash, *STRINGY_TYPES, Numeric) || CMAKE_LITERALS.key?(value)
      end

    private

      def cmake_escape(str, quote: true)
        result = str.gsub(/[\\"$]/) do |match|
          %{\\#{match[0]}}
        end
        quote ? %{"#{result}"} : result
      end

      def cmake_serialized_enumerator(data)
        cmake_flatten_hash(data).lazy.map do |k, v|
          %{set(#{k} #{cmake_serialize(v)})}
        end
      end

      def cmake_serialize_array(array)
        elements = array.compact
        unless elements.all? { _1.is_any_of?(*STRINGY_TYPES, Numeric) }
          debugger if defined?(debugger)
          raise ArgumentError, 'Array must contain only String-y elements'
        end
        elements
      end

      def cmake_flatten_hash(hash)
        final = {}
        hash = hash.dup if hash.frozen?
        until hash.empty?
          hash.transform_keys!(&:to_s)

          # Join arrays and make stringy values `String`s
          hash.transform_values! do |value|
            next value unless value.is_any_of?(Array, *STRINGY_TYPES)
            next value.to_s unless value.is_a?(Array) # if stringy, just make it a string

            value.map(&:to_s)
          end
          hash.select { _2.is_a?(Array) }.keys.each do |k|
            if final.key?(k)
              warn "Duplicate hash key #{k.inspect}. Ignoring new value"
              next
            end
            final[k] = hash.delete(k)
          end

          unrecognized_keys = hash.reject { |_k, v| cmake_serializable?(v) }.keys
          if unrecognized_keys.any?
            raise ArgumentError, "Unrecognized values in hash at keys #{unrecognized_keys}"
          end

          # Flatten hashes
          hash_keys = hash.select { |_k, v| v.is_a?(Hash) }.keys
          hash_keys.each do |k|
            subhash = hash.delete(k)
            subhash = subhash.dup if subhash.frozen?

            flat_subhash = cmake_flatten_hash(subhash)
            flat_subhash.transform_keys! { [k, _1].join(CMAKE_KEY_SEPARATOR) }
            hash.merge!(flat_subhash) do |k, old_val, _new_val|
              warn "Duplicate hash key #{k.inspect}. Ignoring new value"

              old_val
            end
          end

          # Move completed keys into `final` hash
          completed_keys = hash.select do |_k, v|
            v.is_any_of?(String, Numeric) || CMAKE_LITERALS.include?(v)
          end.keys

          completed_keys.each do |k|
            v = hash.delete(k)
            if final.key?(k)
              warn "Duplicate hash key #{k.inspect}. Ignoring new value"
              next
            end

            final[k] = v
          end
        end

        final
      end
    end
  end
end
