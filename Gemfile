# frozen_string_literal: true

source 'https://rubygems.org'

ruby '3.2.2'
gemspec development_group: :build

gem 'bundler', '~> 2.4.21'

group :build do
  gem 'rice', '~> 4.1'

  gem 'faraday', '~> 2.7.11'
  gem 'faraday-follow_redirects', '~> 0.3.0'
  gem 'faraday-retry', '~> 2.2.0'
  gem 'rubyzip', '~> 2.3.2'

  gem 'os', '~> 1.1'
  gem 'rainbow', '~> 3.1'
  gem 'ruby-progressbar', '~> 1.13.0'
end

gem 'rake', '~> 13.0'

group :dev do
  gem 'debug'
  gem 'concurrent-ruby'
  gem 'rubocop', '~> 1.57.2', require: false
end
