# frozen_string_literal: true

source 'https://rubygems.org'

ruby '3.2.2'
gemspec development_group: :build

gem 'bundler', '~> 2.5.6'

group :build do # TODO: put these into gemspec if they really get used
  gem 'rainbow', '~> 3.1' # TODO: use this more to show progress messages
  gem 'terrapin', '~> 0.6.0' # TODO: switch to this in RuBLE::Build::Environment
end

gem 'rake', '~> 13.0'

group :dev, :development do
  gem 'debug'
  gem 'faraday-http-cache'
  gem 'rubocop', require: false
  gem 'rubocop-rake', require: false
  gem 'steep', '~> 1.6.0', require: false
end
