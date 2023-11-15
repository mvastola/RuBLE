# frozen_string_literal: true

module RuBLE
  module Build
    module Extconf
      class << self
        include Memery


        memoize def configure
          pp Build::Extconf::CLI.parse
        end

        memoize def build_config
          Build::Extconf::BuildConfig.instance
        end
        delegate :write_build_config, to: :build_config
      end
    end
  end
end
