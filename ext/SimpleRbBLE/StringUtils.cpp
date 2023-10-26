//
// Created by mvastola on 10/25/23.
//

#include "utils.hpp"
#include "StringUtils.hpp"

namespace SimpleRbBLE {
    namespace Utils {
        namespace Strings {
            std::string ruby_quote_string(const std::string &str) {
                return ensure_ruby<std::string>([&str]() -> std::string  {
                    Object obj = Object(rb_cString).call("new", str);
                    return ruby_inspect(obj);
                });
            }

            std::string ruby_inspect(Object obj) {
                auto fn = [&obj]() -> std::string {
                    VALUE rb_str = obj.call("inspect").value();
                    return Rice::String(rb_str).str();
                };
                return ensure_ruby<std::string>(fn);
            }
        }
    } // SimpleRbBLE
} // Utils