#include "common.h"
#include "utils.h"

namespace SimpleRbBLE {
    namespace Utils {
        template<>
        std::string human_type_name(const std::type_info &type) {
            int status;
            std::string original_name = type.name();
            std::string result;
            char *result_ptr = abi::__cxa_demangle(original_name.c_str(), nullptr, nullptr, &status);

            if (result_ptr == nullptr || status) {
                std::cerr << "Warning. Demangling of type name " << original_name;
                std::cerr << " failed with status " << status << std::endl;
                result = original_name;
            } else {
                result = result_ptr;
            }
            if (result_ptr != nullptr) std::free(result_ptr);
            return result;
        }

        template<>
        std::string human_type_name(const std::any &any) { return human_type_name(any.type()); }


        std::string inspect_object(const Object &o) {
            return o.inspect().str();
        }

        std::string inspect_object(VALUE val) {
            return inspect_object(Object(val));
        }
    }
}