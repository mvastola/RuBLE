#include "types/declarations.hpp"
#include "containers/ByteArray.hpp"

#include "utils/containers.hpp"

namespace SimpleRbBLE {

    ByteArray::ByteArray(Object obj) : ByteArray() {
        if (obj.is_a(rb_cByteArray)) {
            *this = { *ByteArray_DO(obj).get() };
        } else if(obj.is_a(rb_cNumeric)) {
            *this = { rb_num2ulong(obj) };
        } else if (obj.is_a(rb_cString) || obj.is_a(rb_cSymbol)) {
            *this = {Rice::String(obj).str()};
        } else if(obj.is_a(rb_cNilClass) || obj.is_a(rb_cTrueClass) || obj.is_a(rb_cFalseClass)) {
            *this = { obj.test() };
        } else {
            throw std::invalid_argument("Cannot convert "s + obj.inspect().str() + " to a ByteArray");
        }
    }

    ByteArray ByteArray::from_ruby(Object obj) {
        return { std::move(obj) };
    }

    std::string ByteArray::to_s() const {
        std::ostringstream oss;
        oss << Utils::basic_object_inspect_start(*this) << " ";
        oss << Utils::to_hex_data(_data);
        oss << " (" << Utils::ruby_quote_string(_data) << ")>";
        return oss.str();
    }

    std::string ByteArray::inspect() const { return to_s(); }

    void ByteArray::ruby_mark() const {
        Rubyable::ruby_mark();
    }

    void Init_ByteArray() {
        rb_cByteArray = define_class_under<ByteArray>(rb_mSimpleRbBLE, "ByteArray")
                .define_singleton_function("new", &ByteArray::from_ruby)
                .define_method("get", &ByteArray::to_string)
                .define_method("to_s", &ByteArray::to_s)
                .define_method("inspect", &ByteArray::inspect)
                .define_method("data", &ByteArray::data)
                .define_method("to_i", &ByteArray::to_i<>)
                .define_method("%", &ByteArray::operator%<>)
                .define_method("&", &ByteArray::operator&<>)
                .define_method("|", &ByteArray::operator|<>)
                .define_method("~", &ByteArray::operator~)
                .define_method("!", &ByteArray::operator!)
                .define_method("<=>", &ByteArray::operator<=><>)
                ;
    }
}

//namespace Rice::detail {
//    bool From_Ruby<SimpleRbBLE::ByteArray>::can_convert_from_ruby(VALUE obj) {
//        return std::find(supported_ruby_types.cbegin(),
//                         supported_ruby_types.cend(),
//                         rb_type(obj)) != supported_ruby_types.cend();
//    }
//}
