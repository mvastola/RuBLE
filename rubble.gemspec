require 'shellwords'
require_relative 'lib/rubble/version'

Gem::Specification.new do |spec|
  spec.name = 'rubble'
  spec.version = Rubble::VERSION
  spec.authors = ['Mike Vastola']
  spec.email = ['mike@vasto.la']

  spec.summary = 'Ruby Interface for SimpleBLE (BLE library)'
  spec.description = 'RubBLE is a ruby interface to the SimpleBLE library, which provides '\
                     'a cross-platform interface for interacting with Bluetooth Low Energy (BLE) '\
                     'devices.'
  spec.homepage = 'https://github.com/mvastola/rubble'
  spec.license = 'MIT'
  spec.required_ruby_version = '>= 3.2.0'

  # spec.metadata["allowed_push_host"] = "TODO: Set to your gem server 'https://example.com'"
  spec.metadata['homepage_uri'] = spec.homepage
  spec.metadata['bug_tracker_uri'] = 'https://github.com/mvastola/rubble/issues'
  spec.metadata['source_code_uri'] = 'https://github.com/mvastola/rubble'
  spec.metadata['changelog_uri'] = 'https://github.com/mvastola/rubble/blob/master/CHANGELOG.md'
  spec.metadata['documentation_uri'] = 'https://rubydoc.info/gems/rubble'
  spec.metadata['rubygems_mfa_required'] = 'true' # Why does this need to be a string??

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files = `git -C #{__dir__.shellescape} ls-files -z`.split("\x0").reject do |f|
      (File.expand_path(f) == __FILE__) ||
        f.start_with?(*%w[bin contrib test/ spec/ features/ tmp .idea .run .git lib/rubble/build])
  end
  spec.bindir = 'exe'
  spec.executables = spec.files.grep(%r{\Aexe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']
  spec.extensions = ['ext/rubble/extconf.rb']
  spec.extra_rdoc_files = Dir['{.,pages}/*.md']

  spec.add_runtime_dependency 'zeitwerk', '~> 2.6.12'

  spec.add_development_dependency 'rice'

  spec.add_development_dependency 'rake'

  # TODO: Gemspec/DevelopmentDependencies cop is complaining
  #   (figure out where best to put these)
  # spec.add_development_dependency 'rake-compiler'
  spec.add_development_dependency 'faraday'
  spec.add_development_dependency 'faraday-follow_redirects'
  spec.add_development_dependency 'faraday-retry'
  spec.add_development_dependency 'os'
  spec.add_development_dependency 'rubyzip'
end
