# frozen_string_literal: true

source 'https://rubygems.org'

ruby '3.2.2'
gemspec development_group: :build

gem 'bundler', '~> 2.4.21'

group :build do
  gem 'rainbow', '~> 3.1' # TODO: use this more
  gem 'terrapin', '~> 0.6.0' # TODO: switch to this
end

gem 'rake', '~> 13.0'

group :dev do
  gem 'debug'
  gem 'rubocop', require: false
  gem 'rubocop-rake', require: false
end
