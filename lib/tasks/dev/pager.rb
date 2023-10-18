# frozen_string_literal: true

require 'openssl'
require 'shellwords'
require 'pathname'
require 'concurrent-ruby'

Thread.report_on_exception = true

class Pager
  # INSTANCE_LOCK = Mutex.new
  LOCK_LOCAL = Concurrent::ThreadLocalVar.new
  private_constant :LOCK_LOCAL
  #noinspection RubyArgCount
  INSTANCE_USE_COUNT = Concurrent::Semaphore.new(1)
  CMD = %W[less -FXeR]

  class << self
    def reset_fds!
      STDERR.reopen REAL_STDERR
      STDOUT.reopen REAL_STDOUT
    end

    def atom
      @atom ||= Concurrent::AtomicReference.new
    end

    private :new
    def instance
      return atom.get if atom.get

      atom.update do |current|
        next current if current

        @instance = new
      end
    end

    def reset = atom.update { false }
  end

  attr_reader *%i[cmdline pid status started_at finished_at duration waiter _lock]

  # def set_as_singleton_instance!(...) =
  # def unset_as_singleton_instance!(...) = @lock.unlock!(...)

  def initialize
    # @lock = Concurrent::ReentrantReadWriteLock.new
    @mtx = Mutex.new
    # @guard = LockGuard.new
    # @_lock = Concurrent::ReentrantReadWriteLock.new
    # @guards = Concurrent::ThreadLocalVar.new
    @cmdline = CMD.shelljoin.freeze
    @status = nil
  end

  # def guard
  #   @guard
  #   # @guards.value ||= LockGuard.new(@_lock, type: :write)
  # end

  # def lock
  #   LOCK_LOCAL.value ||= LockGuard.new(INSTANCE_LOCK)
  # end
  def io = @in
  def started? = !!started_at
  def finished? = !!finished_at
  def alive? = pid && Process.kill(0, pid)
  def status? = !!status

  def start
    return
    raise "Pager already stopped. You need to create a new instance" if finished?
    return self if @waiter

    @mtx.synchronize do
      return self if @waiter

      @out, @in = IO.pipe
      [@out, @in].each { _1.sync = true }
      @waiter ||= Thread.new { _pager! }
    end
    self
  end

  def wait!
    return status if status?
    raise "Pager is not running. Nothing to wait for." unless @waiter

    @waiter.join
    status
  end

  def close!(ignore_repeats: false)
    # lck = guard.lock(ignore_repeats: ignore_repeats)
    @mtx.synchronize do
      self.class.reset_fds!
      io&.close unless io&.closed?
    end
  end

  # def stop!(wait: true)
  #   close!
  #   Process.kill(:SIGTERM, pid) if alive? # TODO: kill if we don't shutdown cleanly?
  #   wait! if wait
  # end

  def use(&block)
    start
    @mtx.synchronize(&block)
  rescue
    # stop!
    raise
  end

  def eval_paginated(&block)
    use do
      @real_stdout = STDOUT.dup
      @real_stderr = STDERR.dup
      STDOUT.reopen io
      STDERR.reopen io
      block.yield self
    ensure
      STDOUT.reopen @real_stdout
      STDERR.reopen @real_stderr
      self.class.reset_fds!
    end
  end

  def exec_paginated!(*args, chdir: ROOT_PATH, **kwargs)
    status = nil
    use do
      Bundler.with_unbundled_env do
        args[-1] = args[-1].shelljoin if args[-1].is_a?(Array)
        puts "Running: #{args.inspect} in directory #{chdir}"
        pid = Process.spawn(*args, **kwargs, chdir: chdir.to_s) #, out: io, err: io)
        _, status = Process.wait2(pid)
        unless status.success?
          raise "Execution of #{args.inspect} failed with code #{status.inspect}"
        end
      end
    end
    status
  end

  private
  def _pager!
    Thread.current.abort_on_exception = true
    Thread.current.report_on_exception = true

    @pid = Process.fork

    unless @pid
      @in.close
      STDIN.reopen(@out)
      Process.exec(cmdline)
    end

    @out.close
    @started_at ||= Time.now
    at_exit { wait! }
    _, @status = Process.wait2(@pid)

    @finished_at = Time.now
    @duration = @finished_at - @started_at
    status
  end
end
