require 'shellwords'
require_relative 'lib/ruble/version'

Gem::Specification.new do |spec|
  spec.name = 'ruble'
  spec.version = RuBLE::VERSION
  spec.authors = [ 'Mike Vastola' ]
  spec.email = %w[Mike@Vasto.la]

  spec.summary = 'Ru(by Simple)BLE library'
  spec.description = 'RuBLE is a ruby interface to the SimpleBLE library, providing '\
                     'a cross-platform DSL for interacting with Bluetooth Low Energy (BLE) '\
                     'devices.'
  spec.homepage = 'https://github.com/mvastola/RuBLE'
  spec.license = 'MIT'
  spec.required_ruby_version = '>= 3.2.0'

  # spec.metadata["allowed_push_host"] = "TODO: Set to your gem server 'https://example.com'"
  spec.metadata['homepage_uri'] = spec.homepage
  spec.metadata['bug_tracker_uri'] = 'https://github.com/mvastola/RuBLE/issues'
  spec.metadata['source_code_uri'] = 'https://github.com/mvastola/RuBLE'
  spec.metadata['changelog_uri'] = 'https://github.com/mvastola/RuBLE/blob/master/CHANGELOG.md'
  spec.metadata['documentation_uri'] = 'https://rubydoc.info/gems/RuBLE'
  spec.metadata['rubygems_mfa_required'] = 'true' # Why does this need to be a string??

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files = `git -C #{__dir__.shellescape} ls-files -z`.split("\x0").reject do |f|
    ignore_paths = %w[
      bin/
      contrib/
      test/
      spec/
      features/
      tasks/dev
      .ruby-version
      .envrc
      .idea
      .run
      .github
      .gitmodules
      CMakeLists.txt
    ]
    File.expand_path(f) == __FILE__ || f.start_with?(*ignore_paths)
  end
  spec.bindir = 'exe'
  spec.executables = spec.files.grep(%r{\Aexe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']
  spec.extensions = ['ext/ruble/extconf.rb']
  spec.extra_rdoc_files = Dir['{.,pages}/*.md']

  spec.add_runtime_dependency 'zeitwerk', '~> 2.6.12'

  spec.add_development_dependency 'rice', '~> 4.1'

  spec.add_development_dependency 'faraday', '~> 2.7.11'
  spec.add_development_dependency 'faraday-follow_redirects', '~> 0.3.0'
  spec.add_development_dependency 'faraday-retry', '~> 2.2.0'

  # spec.add_development_dependency 'rake-compiler'

  # Support Libraries
  spec.add_development_dependency 'activesupport', '~> 7.1.2'
  spec.add_development_dependency 'memery', '~> 1.5'
  spec.add_development_dependency 'steep', '~> 1.6.0'
  spec.add_development_dependency 'typeprof', '~> 0.21.11'

  # TODO: I _think_ c++20 will work too, but need to check
  spec.requirements << 'C++ compiler supporting standard c++23 (e.g. g++-13)'
  # TODO: we can likely lower this. Need to find minimum working version though
  spec.requirements << 'CMake version 3.25.1 or higher'

end
