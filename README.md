# SimpleRbBLE (Pre-Alpa)

Preliminary attempt to implement an interface to the easy-to-use [SimpleBLE](https://github.com/OpenBluetoothToolbox/SimpleBL) (Bluetooth Low Energy) library in Ruby using the very nifty [rice](https://github.com/jasonroelofs/rice) gem.

Currently it's just me tooling around, but some basic functionality is working. If this gets to alpha stage, I'll publish on RubyGems.

## Development

I'm currently debugging a few issues and built my own version of ruby-3.2.2 (with better debug info). If you have RVM, it's very easy to reproduce:

1. `cd /usr/local/rvm/src`
2. `cp -a ruby-3.2.2{,-dbg}`
3. `cd ruby-3.2.2-dbg`
4. `./configure -C --prefix=/usr/local/rvm/rubies/ruby-3.2.2-dbg --disable-install-doc --enable-shared --enable-debug-env --with-valgrind --enable-mkmf-verbose --enable-yjit --enable-devel --enable-dtrace STRIP=/bin/true 'CFLAGS=-fno-inline -O0 -ggdb3' 'debugflags=-fno-inline -O0 -ggdb3' # (Customize as desired)`
5. `make all`
6. `make install`

Then, if you simply `rvm list`, you'll see a new version listed as ruby-3.2.2-dbg. To use in this project, just `cd` into the source directory and (assuming RVM is loaded) RVM will autodetect the ruby install it needs to use. From there, make sure to clear all temporary/build files for the C++ code and recompile.

## Installation

TODO: Replace `UPDATE_WITH_YOUR_GEM_NAME_PRIOR_TO_RELEASE_TO_RUBYGEMS_ORG` with your gem name right after releasing it to RubyGems.org. Please do not do it earlier due to security reasons. Alternatively, replace this section with instructions to install your gem from git if you don't plan to release to RubyGems.org.

Install the gem and add to the application's Gemfile by executing:

    $ bundle add UPDATE_WITH_YOUR_GEM_NAME_PRIOR_TO_RELEASE_TO_RUBYGEMS_ORG

If bundler is not being used to manage dependencies, install the gem by executing:

    $ gem install UPDATE_WITH_YOUR_GEM_NAME_PRIOR_TO_RELEASE_TO_RUBYGEMS_ORG

## Usage

TODO: Write usage instructions here

## Development

After checking out the repo, run `bin/setup` to install dependencies. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and the created tag, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/[USERNAME]/SimpleRbBLE. This project is intended to be a safe, welcoming space for collaboration, and contributors are expected to adhere to the [code of conduct](https://github.com/[USERNAME]/SimpleRbBLE/blob/master/CODE_OF_CONDUCT.md).

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).

## Code of Conduct

Everyone interacting in the SimpleRbBLE project's codebases, issue trackers, chat rooms and mailing lists is expected to follow the [code of conduct](https://github.com/[USERNAME]/SimpleRbBLE/blob/master/CODE_OF_CONDUCT.md).
