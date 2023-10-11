class TracePointManager
  ROOT_PATH = Pathname.new(__dir__).parent
  TP_EVENT_TYPES = {
    c_call: 'C call',
    call: 'Ruby call',
    b_call: 'Block call'
  }.freeze
  TP_IGNORED_CLASSES = [ IO, Kernel, Pathname, Class, TracePointManager, TracePoint ]
  TP_METHODS = %i[tracepoint_call] # tracepoint_line

  class << self
    def relative_tracepoint_path(path, line, under_root_only: true)
      under_root = path.start_with?("#{ROOT_PATH}/") 
      return nil if path == __FILE__
      return nil unless !under_root_only || under_root

      relpath = under_root ? Pathname.new(path).relative_path_from(ROOT_PATH) : path
      "#{relpath}:#{line}"
    end

    def tracepoint_line
      @tracepoint_line ||= TracePoint.trace(:line) do |tp|
        rel_path = relative_tracepoint_path(tp.path.dup, tp.lineno)
        next unless rel_path
        next if ignored?(tp.defined_class) 

        print_nodup "At ruby line #{rel_path}"
      end
    end

    def tracepoint_call
      @tracepoint_call ||= TracePoint.trace(:a_call) do |tp|
        rel_path = relative_tracepoint_path(tp.path.dup, tp.lineno, under_root_only: false)
        next unless rel_path
        next if ignored?(tp.defined_class) 

        type = TP_EVENT_TYPES.fetch(tp.event, tp.event)
        is_singleton = tp.defined_class&.singleton_class? 
        actual_class = is_singleton ? tp.defined_class.attached_object : tp.defined_class

        msg = "#{type} to "
        if tp.event == :b_call
          msg += "block "
        elsif is_singleton
          msg += "method #{actual_class}.#{tp.method_id} "
        else
          msg += "method #{tp.self.inspect}##{tp.method_id} "
        end
        msg += "(as #{tp.callee_id}) " if tp.method_id != tp.callee_id 
        msg += "@ #{rel_path}"
        print_nodup msg
      end
    end

    def ignored?(klass)
      return true if TP_IGNORED_CLASSES.include?(klass)
      TP_IGNORED_CLASSES.any? { klass == _1.singleton_class }
    end

    def wrap(*args, **kwargs, &block)
      TP_METHODS.reverse.each { |sym| send(sym).enable(*args, **kwargs) }
      yield
    ensure
      TP_METHODS.each { |sym| send(sym).disable }
    end

    def print_nodup(msg)
      return if @last_msg == msg 

      @last_msg = msg
      STDERR.puts msg.color(:dimgray) 
    end
  end
end

