# frozen_string_literal: true

module RuBLE
  module Build
    module Settings
      module Global
        extend ActiveSupport::Concern

        class_methods do
          add_flag(:debug, %w[-d --[no-]debug], desc: 'Build in debug mode', initial: false)
          add_flag(:verbose, %w[-v --[no-]verbose], desc: 'Output commands during build', initial: false)
          add_flag(:development, %w[--dev --development], desc: 'Use latest releases of libraries', initial: false)
          add_flag(:release, %w[-r --release], desc: 'Build extension for distribution on rubygems', initial: false)
        end
      end
    end
  end
end
