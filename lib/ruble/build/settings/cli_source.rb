# frozen_string_literal: true

module RuBLE
  module Build
    module Settings
      class CLISource
        class << self
          def flags = @flags ||= Set.new
          def add_flag(...) = Settings::CLIFlag.new(...).tap { flags << _1 }
          def add_flag_alias(...) = Settings::CLIFlag.alias(...).tap { flags << _1 }
          def clear! = flags.clear
        end

        add_flag(:debug, %w[-d --[no-]debug], desc: 'Build in debug mode', type: FalseClass)
        add_flag(:verbose, %w[-v --[no-]verbose], desc: 'Output commands during build', type: FalseClass)
        add_flag(:development, %w[--dev --development], desc: 'Use latest releases of libraries', type: FalseClass)
        add_flag(:release, %w[-r --release], desc: 'Build extension for distribution on rubygems', type: FalseClass)

        attr_reader *%i[original_opts parsed_opts remaining_opts result parser raise_on_unknown]

        def initialize(*opts, raise_on_unknown: true)
          @original_opts = opts.freeze
          @parsed_opts = nil
          @result = nil
          @raise_on_unknown = raise_on_unknown
        end

        def remaining_opts? = !!remaining_opts&.any?

        def load
          return @result if @result

          @result = {}
          @parser = OptionParser.new
          self.class.flags.each do |flag|
            flag.configure(self)
          end

          @parsed_opts = {}
          @remaining_opts = original_opts.dup
          parser.order!(remaining_opts, into: parsed_opts)
          if raise_on_unknown && remaining_opts&.any?
            puts @parser.help
            raise ArgumentError, "Unrecognized options found on command line: #{remaining_opts.inspect}"
          end

          self.class.flags.each do |flag|
            flag.set_result_fallback(self)
          end
          @result
        end

        def set_result(path, value, force: true)
          path = CLIFlag.format_key(path)

          return UNSET if value.equal?(UNSET)

          result_hash = path[0..-2].reduce(result) { |memo, obj| memo[obj.to_s] ||= {} }
          if force || !result_hash.key?(path.last) || result_hash[path.last].equal?(UNSET)
            result_hash[path.last] = value
          else
            result_hash[path.last] # return old value if not overwriting
          end
        end
      end
    end
  end
end
