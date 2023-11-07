#pragma once

#include "types/declarations.hpp"
#include "types/ruby.hpp"

namespace Rice {
    template<> constexpr void ruby_mark<Rubble::Callback>(Rubble::Callback*);
    template<> void ruby_mark<Rubble::Adapter>(Rubble::Adapter *);
    template<> void ruby_mark<Rubble::AdapterRegistry>(Rubble::AdapterRegistry*);
    template<> void ruby_mark<Rubble::AdapterRegistryFactory>(Rubble::AdapterRegistryFactory*);
    template<> void ruby_mark<Rubble::ByteArray>(Rubble::ByteArray*);
    template<> void ruby_mark<Rubble::Peripheral>(Rubble::Peripheral*);
    template<> void ruby_mark<Rubble::PeripheralRegistry>(Rubble::PeripheralRegistry*);
    template<> void ruby_mark<Rubble::PeripheralRegistryFactory>(Rubble::PeripheralRegistryFactory*);
    template<> void ruby_mark<Rubble::Service>(Rubble::Service*);
    template<> void ruby_mark<Rubble::ServiceRegistry>(Rubble::ServiceRegistry*);
    template<> void ruby_mark<Rubble::ServiceRegistryFactory>(Rubble::ServiceRegistryFactory*);
    template<> void ruby_mark<Rubble::Characteristic>(Rubble::Characteristic*);
    template<> void ruby_mark<Rubble::CharacteristicRegistry>(Rubble::CharacteristicRegistry*);
    template<> void ruby_mark<Rubble::CharacteristicRegistryFactory>(Rubble::CharacteristicRegistryFactory*);
}

