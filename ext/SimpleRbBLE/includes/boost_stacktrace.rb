require 'mkmf'
require 'memery'

class BoostStacktrace
  HEADER_FILE = 'boost/stacktrace.hpp'
  CLASS_NAME ='boost::stacktrace'
  LIBRARY_BACKENDS = %i[backtrace addr2line basic windbg windbg_cached]
  NON_LIBRARY_BACKENDS = %i[header_only]
  ALL_BACKENDS = LIBRARY_BACKENDS + NON_LIBRARY_BACKENDS
  TARGET_OS = RbConfig::CONFIG['target_os'].to_sym

  include Memery

  class Unsupported < RuntimeError
    def initialize(msg = nil, *args, **kwargs)
      msg = "Can't use boost::stacktrace library, as it is not installed/accessible."\
        "\n#{msg}".strip
      super(msg, *args, **kwargs)
    end
  end

  class UnsupportedBackend < RuntimeError
    def initialize(msg = nil, *args, backend:, **kwargs)
      backend_str = Array.wrap(backend).map(&:to_s).join(',')
      msg = "Can't use #{backend_str} as #{CLASS_NAME} backend. "\
        "Will try others, per availability.\n#{msg}".strip
      super(msg, *args, **kwargs)
    end
  end

  class << self
    def instance = @instance ||= new
    private :new

    def configure!(required: true)
      instance.configure!
    rescue => ex
      warn Unsupported.new("Dependencies required to support stacktraces (via #{CLASS_NAME}) "\
           "by #{EXTENSION_NAME} could not be found.")
      raise ex if required
      warn ex unless required
    end
  end

  memoize def test_have_library(...)
    old_libs = $libs
    begin
      return have_library(...)
    ensure
      $libs = old_libs
    end
  end

  memoize def header_available?
    have_macro('BOOST_STACKTRACE_STACKTRACE_HPP', HEADER_FILE)
    have_header('boost/exception/all.hpp')

  end

  memoize def addr2line_path = find_executable('addr2line')

  memoize def libbacktrace_header? = have_header('backtrace.h')
  memoize def libbacktrace_library? = test_have_library('backtrace', nil, 'backtrace.h')
  
  memoize def dbgeng_header? = have_header('dbgeng.h')
  memoize def dbeng_library? = test_have_library('dbeng', nil, 'dbgeng.h')
  
  memoize def backend_library_present?(backend_name)
    test_have_library("boost_stacktrace_#{backend_name}", nil, HEADER_FILE)
  end

  memoize def permitted_backends
    permitted_backend_map = {
      nil => ALL_BACKENDS,
      true => LIBRARY_BACKENDS,
      false => NON_LIBRARY_BACKENDS
    }

    requested_backends = with_config('boost-stacktrace-backend')
    if permitted_backend_map.key?(requested_backends) 
      return permitted_backend_map[requested_backends] 
    end

    # Otherwise, a specific backend (or backends) were given
    candidates = requested_backends.
        gsub(/-/, '_').
        split(/(\s*,|\s)\s*/).
        each(&:strip!).
        reject(&:empty?).
        map(&:to_sym).
        uniq

    unsupported_candidates = candidates - ALL_BACKENDS
    unless unsupported_candidates.empty?
      warn UnsupportedBackend.new("Warning: One or more of the requested #{CLASS_NAME} "\
                                  "backends is unrecognized.", backend: unsupported_candidates)
    end
    candidates & ALL_BACKENDS
  end

  memoize def library_backends_deselected?
    (permitted_backends & LIBRARY_BACKENDS).empty?
  end

  memoize def available_backends
    raise Unsupported, "libboost-stacktrace could not be found" unless header_available?

    result = permitted_backends.select { send(:"supports_#{_1}_backend?") }
    puts "Available #{EXTENSION_NAME} #{CLASS_NAME} backends: "\
      "#{result.map(&:to_s).join(', ')}"

    return result unless result.empty?

    raise UnsupportedBackend, "None of the candidate #{CLASS_NAME} backends "\
      "are supported on this system. (Tried #{candidate_backends.map(&:to_s).join(', ')}"
  end

  # Requires -lboost_stacktrace_addr2line -ldl AND /usr/bin/addr2line
  memoize def supports_addr2line_backend?
    header_available? && 
      backend_library_present?(:addr2line) && 
      addr2line_path && 
      File.executable?(addr2line_path)
  end
  
  # Requires -lboost_stacktrace_backtrace -ldl -lbacktrace, and backtrace.h header
  memoize def supports_backtrace_backend?
    header_available? && 
      backend_library_present?(:backtrace) && 
      libbacktrace_header? && 
      libbacktrace_library?
  end

  memoize def supports_windbg_backend?
    return false if TARGET_OS == :linux # FIXME what is the value on windows?

    warn UnsupportedBackend.new("Windows is not yet supported for #{CLASS_NAME} "\
                                 "by #{EXTENSION_NAME}", backend: :windbg)
    return false

    header_available? && 
      backend_library_present?(:windbg) && 
      dbgeng_header? &&
      dbeng_library?
  end

  memoize def supports_windbg_cached_backend?
    return false if TARGET_OS == :linux # FIXME what is the value on windows?

    warn UnsupportedBackend.new("Windows is not yet supported for #{CLASS_NAME} "\
                                 "by #{EXTENSION_NAME}", backend: :windbg_cached)
    header_available? && 
      backend_library_present?(:windbg_cached) && 
      dbgeng_header? &&
      dbeng_library?
  end

  memoize def supports_basic_backend?
    header_available? && backend_library_present?(:basic)  
  end
  alias_method :supports_header_only_backend?, :header_available?
    
  memoize def backend
    backend = available_backends.detect do |be|
      result = send(:"configure_#{be}_backend!")
      !(result === false)
    end
    
    if NON_LIBRARY_BACKENDS.include?(backend) && !library_backends_deselected?
      warn UnsupportedBackend, "None of the library-based #{CLASS_NAME} backends "\
        "are supported on this system. Falling back to headers-only."
    end
    backend
  end

  memoize def configure!
    puts "#{EXTENSION_NAME} #{CLASS_NAME} backend chosen: #{backend}"
    
    #$CXXFLAGS << ' -Wl,-fPIC '
    $defs.push('-DBOOST_ENABLE_ASSERT_DEBUG_HANDLER')
    $defs.push('-DBOOST_STACKTRACE_LINK') unless NON_LIBRARY_BACKENDS.include?(backend)
    $defs.push('-DBOOST_STACKTRACE_SUPPORT')
  end


private
  memoize def link_library_backend!(backend)
    $LOCAL_LIBS += " -lboost_stacktrace_#{backend} "
  end

  memoize def configure_addr2line_backend!
    raise UnsupportedBackend.new(backend: :addr2line) unless supports_addr2line_backend?

    $defs.push('-DBOOST_STACKTRACE_USE_ADDR2LINE')
    $defs.push('-DBOOST_STACKTRACE_ADDR2LINE_LOCATION=' + addr2line_path)
    link_library_backend!(:addr2line)
  end

  memoize def configure_backtrace_backend!
    raise UnsupportedBackend.new(backend: :windbg) unless supports_backtrace_backend?

    $defs.push('-DBOOST_STACKTRACE_USE_BACKTRACE')
    # Can't use this apparently because find_header doesn't return a path
    # $defs.push('-DBOOST_STACKTRACE_BACKTRACE_INCLUDE_FILE=' + backtrace_header_path)
    link_library_backend!(:backtrace)
    $LOCAL_LIBS += " -lbacktrace "
    #$libs << ' -ldl '
  end

  memoize def configure_windbg_backend!
    raise UnsupportedBackend.new(backend: :windbg) unless supports_windbg_backend?
    
    # TODO: configure!
    link_library_backend!(:windbg)
    $libs = append_library($libs, 'ole32')
    $libs = append_library($libs, 'dbgeng')
  end

  memoize def configure_windbg_cached_backend!
    raise UnsupportedBackend.new(backend: :windbg_cached) unless supports_windbg_cached_backend?
    
    # TODO: configure!
    link_library_backend!(:windbg_cached)
    $libs = append_library($libs, 'ole32')
    $libs = append_library($libs, 'dbgeng')
  end

  memoize def configure_basic_backend!
    raise UnsupportedBackend.new(backend: :basic) unless supports_basic_backend?

    link_library_backend!(:basic)
  end

  memoize def configure_headers_only_backend!
    raise UnsupportedBackend.new(backend: :headers_only) unless supports_headers_only_backend?
  end
end
    
