#pragma once

#include <iostream>
#include <rice/rice.hpp>
#include <rice/stl.hpp>
#include <simpleble/SimpleBLE.h>
#include <simpleble/Adapter.h>

using namespace Rice;
using namespace std::placeholders;
using SimpleBLE::Adapter,
        SimpleBLE::Peripheral,
        SimpleBLE::Service,
        SimpleBLE::ByteArray,
        SimpleBLE::BluetoothUUID,
        SimpleBLE::Characteristic,
        SimpleBLE::Descriptor,
        SimpleBLE::BluetoothAddressType;

struct CallbackHolder {
    Rice::Object callback_;

    CallbackHolder() = default;
    CallbackHolder(CallbackHolder&&) = default;
    CallbackHolder(const CallbackHolder&) = default;

    inline void set(const Rice::Object &cb) { callback_ = cb; }
    template<class... Types>
    Object operator()(Types... args) const {
        return callback_.call("call", args...);
    }

    Object fire() const {
        return (*this)();
    }
//    template<class... Types>
//    Object fire(Types... args) const {
//        return (*this)(args...);
//    }
};