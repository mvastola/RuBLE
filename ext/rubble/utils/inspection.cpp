#include "inspection.hpp"

namespace RubBLE::Utils::Inspection {
    [[maybe_unused]] std::string basic_object_inspect_start(const Rice::Object &o) {
            std::ostringstream oss;
            oss << "#<" << o.class_name();
            oss << ":" << to_hex_string(reinterpret_cast<uint64_t>(o.value()));
            return oss.str();
        }
        std::string ruby_quote_string(const std::string &str) {
            return ensure_ruby<std::string>([&str]() -> std::string {
                Object obj = Object(rb_cString).call("new", str);
                return ruby_inspect(obj);
            });
        }

        std::string ruby_inspect(Rice::Object obj) {
            auto fn = [&obj]() -> std::string {
                VALUE rb_str = obj.call("inspect").value();
                return Rice::String(rb_str).str();
            };
            return ensure_ruby<std::string>(fn);
        }
}