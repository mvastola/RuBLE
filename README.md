# Rubble (Pre-Alpa)

Preliminary attempt to implement an interface to the easy-to-use [SimpleBLE](https://github.com/OpenBluetoothToolbox/SimpleBLE) (Bluetooth Low Energy) library in Ruby using the very nifty [rice](https://github.com/jasonroelofs/rice) gem.

I'm reasonably far along now (actually, theoretically, this is fully-functional) and I'm only aware of one bug (which I'm pretty sure is in SimpleBLE). It's definitely not tested though, and I'm not happy yet with the code quality, nor confident in the feature set. I should publish it soon though to get feedback.

## TODOs

- See [Known Issues](#known-issues) just below
- Make tests (/ figure out how to mock bluetooth devices!)
- Create gem on rubygems, and automate building/packaging for different OSes
    - Implement `rake-compiler`-compatible rake tasks for compiling gem 
    - Forward all relevant configuration options to cmake somehow
- See lots of tiny todos in the comments throughout my code (`grep -rIn TODO` should flag them all)
- Documentation
- RBS file

## Known Issues
- For some reason, if ruby crashes (e.g. segfaults) while a device is connected, the device seems to be invisible to ruby once restarting, until one disconnects the bluetooth adapter from the device through other means
- This won't package correctly if you try to turn it into a gem.  Namely, you'll have to build the extension manually by doing the following (starting in the base directory):
  
    ```bash
      $ bundle install
      $ cd ext/rubble
      $ bundle exec ruby extconf.rb
      $ mkdir build
      $ cd build
      $ cmake ..
      $ make
      $ make install
    ```

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

Bug reports and pull requests are welcome on GitHub at https://github.com/mvastola/rubble. This project is intended to be a safe, welcoming space for collaboration, and contributors are expected to adhere to the [code of conduct](https://github.com/mvastola/rubble/blob/master/CODE_OF_CONDUCT.md).

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).

## Code of Conduct

Everyone interacting in the rubble project's codebases, issue trackers, chat rooms and mailing lists is expected to follow the [code of conduct](https://github.com/mvastola/rubble/blob/master/CODE_OF_CONDUCT.md).
