require 'shellwords'
require_relative "lib/SimpleRbBLE/version"

Gem::Specification.new do |spec|
  spec.name = "SimpleRbBLE"
  spec.version = SimpleRbBLE::VERSION
  spec.authors = ["Mike Vastola"]
  spec.email = ["mike@vasto.la"]

  spec.summary = 'Ruby Interface SimpleBLE'
  spec.description = "SimpleRbBle is a ruby interface to the SimpleBLE library, which provides "\
                     "a cross-platform interface for interacting with Bluetooth Low Energy (BLE) "\
                     "devices."
  spec.homepage = 'https://github.com/mvastola/SimpleRbBLE'
  spec.license = "MIT"
  spec.required_ruby_version = ">= 3.2.0"

  #spec.metadata["allowed_push_host"] = "TODO: Set to your gem server 'https://example.com'"

  spec.metadata["homepage_uri"] = spec.homepage
  spec.metadata['bug_tracker_uri'] = "https://github.com/mvastola/SimpleRbBLE/issues"
  spec.metadata['source_code_uri'] = "https://github.com/mvastola/SimpleRbBLE"
  spec.metadata['changelog_uri'] = "https://github.com/mvastola/SimpleRbBLE/blob/master/CHANGELOG.md"
  # TODO: spec.metadata['documentation_uri'] = "https://rubydoc.info/gems/SimpleRbBLE"

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  #spec.files = `git -C #{__dir__.shellescape} ls-files -z`.split("\x0").reject do |f|
  #    (File.expand_path(f) == __FILE__) ||
  #      f.start_with?(*%w[bin/ test/ spec/ features/ .git .circleci appveyor Gemfile])
  #end
  spec.bindir = "exe"
  spec.executables = spec.files.grep(%r{\Aexe/}) { |f| File.basename(f) }
  spec.require_paths = ["lib"]
  spec.extensions = ['ext/SimpleRbBLE/extconf.rb']
  spec.extra_rdoc_files = Dir['{.,pages}/*.md']

  # spec.add_dependency "example-gem", "~> 1.0"
  spec.add_development_dependency "rice"
  spec.add_development_dependency "bundler"
  spec.add_development_dependency "rake"
  spec.add_development_dependency "rake-compiler"
  spec.add_development_dependency "minitest"
  spec.add_development_dependency "faraday"
  spec.add_development_dependency "mini_portile2"
  spec.add_development_dependency "os"
  spec.add_development_dependency "rubyzip"

end
