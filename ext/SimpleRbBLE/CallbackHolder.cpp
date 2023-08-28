#include "SimpleRbBLE.h"

Data_Type<CallbackHolder> Init_SimpleRbBLECallbackHolder(Module &rb_mSimpleRbBLE) {
    return define_class<CallbackHolder>("CallbackHolder")
            .define_constructor(Constructor<CallbackHolder>())
            .define_method("fire", &CallbackHolder::fire);
}