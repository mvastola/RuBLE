#pragma once

#include "types/declarations.hpp"
#include "types/ruby.hpp"

namespace Rice {
    template<> constexpr void ruby_mark<RuBLE::Callback>(RuBLE::Callback*);
    template<> void ruby_mark<RuBLE::Adapter>(RuBLE::Adapter *);
    template<> void ruby_mark<RuBLE::AdapterRegistry>(RuBLE::AdapterRegistry*);
    template<> void ruby_mark<RuBLE::AdapterRegistryFactory>(RuBLE::AdapterRegistryFactory*);
    template<> void ruby_mark<RuBLE::ByteArray>(RuBLE::ByteArray*);
    template<> void ruby_mark<RuBLE::Peripheral>(RuBLE::Peripheral*);
    template<> void ruby_mark<RuBLE::PeripheralRegistry>(RuBLE::PeripheralRegistry*);
    template<> void ruby_mark<RuBLE::PeripheralRegistryFactory>(RuBLE::PeripheralRegistryFactory*);
    template<> void ruby_mark<RuBLE::Service>(RuBLE::Service*);
    template<> void ruby_mark<RuBLE::ServiceRegistry>(RuBLE::ServiceRegistry*);
    template<> void ruby_mark<RuBLE::ServiceRegistryFactory>(RuBLE::ServiceRegistryFactory*);
    template<> void ruby_mark<RuBLE::Characteristic>(RuBLE::Characteristic*);
    template<> void ruby_mark<RuBLE::CharacteristicRegistry>(RuBLE::CharacteristicRegistry*);
    template<> void ruby_mark<RuBLE::CharacteristicRegistryFactory>(RuBLE::CharacteristicRegistryFactory*);
}

