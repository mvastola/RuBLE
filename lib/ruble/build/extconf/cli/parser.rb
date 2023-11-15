# frozen_string_literal: true

module RuBLUE
  module Build
    module Extconf
      module CLI
        class Parser
          class << self
            def cli = @cli ||= new
            private :new

            def parse(*args)
              return nil if @result

              cli.parse(args, raise_on_parse_error: false, check_invalid_params: true)
              params = cli.params
              return params if params.errors.none?

              params.errors.each do |error|
                STDERR.puts "Error: #{error.inspect}"
              end

              cli.exit_with('usage_error', params.errors.summary)
            rescue TTY::Option::ParseError => err
              puts "At #{__FILE__}:#{__LINE__}"
              raise err
            end
          end

          include TTY::Option
          include TTY::Exit

          usage do

            # example "Some example how to use foo",
            #         " $ foo bar"
          end

          
          flag :help do
            short '-h'
            long  '--help'
            desc 'Print usage'
          end

          def help!(exit_code: nil)
            print help(width: TTY::Screen.width, indent: 2)

            exit_code = 0 if exit_code == true
            exit(exit_code) unless exit_code.nil?
          end

          def run
            help!(exit_code: 0) if params[:help]
            # TODO: should alert (or at least warn) about incompatible flags

            if params.valid?
              pp "Params: #{params.inspect}"
              puts 'Valid!'
            else
              pp "Params: #{params.inspect}"
              puts "Invalid!"
            end
            params
          end
        end
      end
    end
  end
end
