#include "ConvertableByteArray.h"
#include "common.h"

namespace SimpleRbBLE {
    ConvertableByteArray_DT rb_cConvertableByteArray;

    std::string ConvertableByteArray::to_s() const { return _data; }

    const ByteArray &ConvertableByteArray::data() const { return _data; }

    ConvertableByteArray::ConvertableByteArray() : _data() {}

    void Init_ConvertableByteArray() {
        rb_cConvertableByteArray = define_class_under<ConvertableByteArray>(rb_mSimpleRbBLE, "ByteArray")
                .define_constructor(Constructor<ConvertableByteArray>())
                .define_method("to_i", &ConvertableByteArray::to_i<>)
                .define_method("to_int", &ConvertableByteArray::to_i<>)
                .define_method("to_uint8", &ConvertableByteArray::to_i<uint8_t>)
                .define_method("to_uint16", &ConvertableByteArray::to_i<uint16_t>)
                .define_method("to_uint32", &ConvertableByteArray::to_i<uint32_t>)
                .define_method("to_uint64", &ConvertableByteArray::to_i<uint64_t>)
                .define_method("to_int8", &ConvertableByteArray::to_i<int8_t>)
                .define_method("to_int16", &ConvertableByteArray::to_i<int16_t>)
                .define_method("to_int32", &ConvertableByteArray::to_i<int32_t>)
                .define_method("to_int64", &ConvertableByteArray::to_i<int64_t>)
                .define_method("to_str", &ConvertableByteArray::to_s)
                .define_method("to_s", &ConvertableByteArray::to_s)
                .define_method("+", &ConvertableByteArray::operator+<>)
                .define_method("-", &ConvertableByteArray::operator-<>)
                .define_method("*", &ConvertableByteArray::operator*<>)
                .define_method("/", &ConvertableByteArray::operator/<>)
                .define_method("%", &ConvertableByteArray::operator%<>)
                .define_method("&", &ConvertableByteArray::operator&<>)
                .define_method("|", &ConvertableByteArray::operator|<>)
                .define_method("~", &ConvertableByteArray::operator~)
                .define_method("<=>", &ConvertableByteArray::operator<=><>)
                ;
    }
}
std::ostream &operator<<(std::ostream &os, const SimpleRbBLE::ConvertableByteArray &cba) {
    return os << cba.data();
}