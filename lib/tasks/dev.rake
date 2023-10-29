

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
  task :build, %i[flags] do |task, args|
    flags = args.fetch(flags, '').split(/,\s*/).uniq.each(&:strip!).reject(&:empty?).to_set
    @devTasks.build!(flags)
  end
  task build: %i[initialize]

  desc 'Audit missing symbols in built object files'
  task audit_objfile_symbols: %i[initialize] do |task, args|
    @devTasks.audit_objfile_symbols!
  end
end
task(:dev, %i[flags]) { |task, args| Rake::Task['dev:build'].invoke(args[:flags]) }

task audit_objfile_symbols: %i[dev:audit_objfile_symbols]