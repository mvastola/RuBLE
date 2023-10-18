#include "common.hpp"
#include "ConvertableByteArray.hpp"


namespace SimpleRbBLE {
    ConvertableByteArray_DT rb_cConvertableByteArray;

    ConvertableByteArray::ConvertableByteArray(Object obj) { // NOLINT(*-unnecessary-value-param)
        if (obj.is_a(rb_cConvertableByteArray)) {
            *this = { *ConvertableByteArray_DO(obj).get() };
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

    ConvertableByteArray ConvertableByteArray::from_ruby(Object obj) { // NOLINT(*-unnecessary-value-param)
        return { std::move(obj) };
    }

    std::string ConvertableByteArray::to_s() const {
        std::cerr << to_i<uint64_t>() << std::endl;
        std::ostringstream oss;
        auto data_span = std::span(_data.begin(), _data.length());
        oss << to_hex_data(data_span);
        oss << " (" << std::quoted(data()) << ")";
        return oss.str();
    }

    std::string ConvertableByteArray::inspect() const {
        std::ostringstream oss;
        oss << basic_object_inspect_start(*this) << " ";
        VALUE inspected = rb_str_inspect(Rice::String(_data));
        oss << "(" << Rice::String(inspected).str() << ") ";
        oss << ">";
        return oss.str();
    }

    void Init_ConvertableByteArray() {
        rb_cConvertableByteArray = define_class_under<ConvertableByteArray>(rb_mSimpleRbBLE, "ByteArray")
                .define_singleton_function("new", &ConvertableByteArray::from_ruby)
                .define_method("get", &ConvertableByteArray::to_string)
                .define_method("to_s", &ConvertableByteArray::to_s)
                .define_method("inspect", &ConvertableByteArray::inspect)
                .define_method("data", &ConvertableByteArray::data)
                .define_method("to_i", &ConvertableByteArray::to_i<>)
//                .define_method("to_int", &ConvertableByteArray::to_i<>)
//                .define_method("to_uint8", &ConvertableByteArray::to_i<uint8_t>)
//                .define_method("to_uint16", &ConvertableByteArray::to_i<uint16_t>)
//                .define_method("to_uint32", &ConvertableByteArray::to_i<uint32_t>)
//                .define_method("to_uint64", &ConvertableByteArray::to_i<uint64_t>)
                .define_method("%", &ConvertableByteArray::operator%<>)
                .define_method("&", &ConvertableByteArray::operator&<>)
                .define_method("|", &ConvertableByteArray::operator|<>)
                .define_method("~", &ConvertableByteArray::operator~)
                .define_method("!", &ConvertableByteArray::operator!)
                .define_method("<=>", &ConvertableByteArray::operator<=><>)
                ;
    }
}

namespace Rice::detail {
    bool From_Ruby<SimpleRbBLE::ConvertableByteArray>::can_convert_from_ruby(VALUE obj) {
        static constexpr const auto supported_ruby_types =
                std::to_array({T_BIGNUM, T_FIXNUM, T_SYMBOL, T_STRING, T_NIL, T_TRUE, T_FALSE});
        return std::find(supported_ruby_types.cbegin(),
                         supported_ruby_types.cend(),
                         rb_type(obj)) != supported_ruby_types.cend();
    }
}
std::ostream &operator<<(std::ostream &os, const SimpleRbBLE::ConvertableByteArray &cba) {
    return os << cba.data();
}