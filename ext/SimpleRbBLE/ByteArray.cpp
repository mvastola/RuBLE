#include "common.hpp"
#include "ByteArray.hpp"

#include "DebugUtils.hpp"
#include "StringUtils.hpp"

namespace SimpleRbBLE {
    ByteArray_DT rb_cByteArray;

    ByteArray::ByteArray(Object obj) { // NOLINT(*-unnecessary-value-param)
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

    ByteArray ByteArray::from_ruby(Object obj) { // NOLINT(*-unnecessary-value-param)
        return { std::move(obj) };
    }

    std::string ByteArray::to_s() const {
        std::ostringstream oss;
        oss << basic_object_inspect_start(*this);
        oss << to_hex_data(_data);
//        oss << " (" << std::quoted(data()) << ")";
        oss << " (" << ruby_quote_string(_data) << ")>";
        return oss.str();
    }

    std::string ByteArray::inspect() const { return to_s(); }

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

namespace Rice::detail {
    bool From_Ruby<SimpleRbBLE::ByteArray>::can_convert_from_ruby(VALUE obj) {
        static constexpr const auto supported_ruby_types =
                std::to_array({T_BIGNUM, T_FIXNUM, T_SYMBOL, T_STRING, T_NIL, T_TRUE, T_FALSE});
        return std::find(supported_ruby_types.cbegin(),
                         supported_ruby_types.cend(),
                         rb_type(obj)) != supported_ruby_types.cend();
    }
}
std::ostream &operator<<(std::ostream &os, const SimpleRbBLE::ByteArray &cba) {
    return os << cba.data();
}