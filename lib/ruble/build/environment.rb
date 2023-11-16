# frozen_string_literal: true

module RuBLE
  module Build
    module Environment
      class << self
        def modules
          return @modules if @modules

          Build.zeitwerk.eager_load_namespace(self)
          @modules = constants.map(&method(:const_get)).freeze
        end
      end
    end
  end
end
