

namespace :dev do
  task :initialize do
    # Bundler.require
    Bundler.setup
    require_relative './dev/dev_tasks'
    @devTasks = DevTasks.new
  end
  desc 'Cleans/Clobbers all generated files'
  task(pristine: %i[initialize]) { @devTasks.pristine! }

  desc 'Cleans/Clobbers, generates Makefiles/etc before build'
  task(reconfigure: %i[initialize]) { @devTasks.reconfigure! }

  desc 'Reconfigures only if needed'
  task(maybe_reconfigure: %i[initialize]) { @devTasks.maybe_reconfigure! }

  desc 'Compiles code for debugging'
  task(build: %i[initialize]) { @devTasks.build! }
end
task dev: %w[dev:build]
