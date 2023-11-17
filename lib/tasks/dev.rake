namespace :dev do
  task :initialize, %i[flags] do |_task, args|
    # Bundler.require
    Bundler.setup
    require_relative 'dev/dev_tasks'

    flags = (args.flags || '').split(/,\s*/).each(&:strip!).reject(&:empty?).map(&:to_sym).to_set
    @dev_tasks = DevTasks.new(*flags)
  end
  desc 'Cleans/Clobbers all generated files'
  task :pristine, %i[flags] do |_task, args|
    Rake::Task['dev:initialize'].invoke(args.flags)
    @dev_tasks.pristine!
  end

  desc 'Cleans/Clobbers, generates Makefiles/etc before build'
  task :reconfigure, %i[flags] do |_task, args|
    Rake::Task['dev:initialize'].invoke(args.flags)
    @dev_tasks.reconfigure!
  end

  desc 'Reconfigures only if needed'
  task :maybe_reconfigure, %i[flags] do |_task, args|
    Rake::Task['dev:initialize'].invoke(args.flags)
    @dev_tasks.maybe_reconfigure!
  end

  desc 'Compiles code for debugging'
  task :build, %i[flags] do |_task, args|
    Rake::Task['dev:initialize'].invoke(args.flags)
    @dev_tasks.build!
  end

  desc 'Audit missing symbols in built object files'
  task :audit_objfile_symbols, %i[flags]  do |_task, args|
    Rake::Task['dev:initialize'].invoke(args.flags)
    @dev_tasks.audit_objfile_symbols!
  end
end
task(:dev, %i[flags]) { |task, args| Rake::Task['dev:build'].invoke(args.flags) }

task audit_objfile_symbols: %i[dev:audit_objfile_symbols]
