#pragma once

#include "types/declarations.hpp"
#include "types/ruby.hpp"

namespace Rice {
    template<> constexpr void ruby_mark<RubBLE::Callback>(RubBLE::Callback*);
    template<> void ruby_mark<RubBLE::Adapter>(RubBLE::Adapter *);
    template<> void ruby_mark<RubBLE::AdapterRegistry>(RubBLE::AdapterRegistry*);
    template<> void ruby_mark<RubBLE::AdapterRegistryFactory>(RubBLE::AdapterRegistryFactory*);
    template<> void ruby_mark<RubBLE::ByteArray>(RubBLE::ByteArray*);
    template<> void ruby_mark<RubBLE::Peripheral>(RubBLE::Peripheral*);
    template<> void ruby_mark<RubBLE::PeripheralRegistry>(RubBLE::PeripheralRegistry*);
    template<> void ruby_mark<RubBLE::PeripheralRegistryFactory>(RubBLE::PeripheralRegistryFactory*);
    template<> void ruby_mark<RubBLE::Service>(RubBLE::Service*);
    template<> void ruby_mark<RubBLE::ServiceRegistry>(RubBLE::ServiceRegistry*);
    template<> void ruby_mark<RubBLE::ServiceRegistryFactory>(RubBLE::ServiceRegistryFactory*);
    template<> void ruby_mark<RubBLE::Characteristic>(RubBLE::Characteristic*);
    template<> void ruby_mark<RubBLE::CharacteristicRegistry>(RubBLE::CharacteristicRegistry*);
    template<> void ruby_mark<RubBLE::CharacteristicRegistryFactory>(RubBLE::CharacteristicRegistryFactory*);
}

