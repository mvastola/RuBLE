# frozen_string_literal: true

module SimpleRbBLE
  class Adapter
    # void set_callback_on_scan_start(std::function<void()> on_scan_start)
    # void set_callback_on_scan_stop(std::function<void()> on_scan_stop)
    # void set_callback_on_scan_updated(std::function<void(Peripheral)> on_scan_updated)
    # void set_callback_on_scan_found(std::function<void(Peripheral)> on_scan_found)
    # store_callbacks_named *%i[on_start on_stop on_updated on_found]
    include SimpleRbBLE::CallbackConcern
    store_callbacks_named *%i[on_scan_start on_scan_stop on_scan_updated on_scan_found]

  end
end
