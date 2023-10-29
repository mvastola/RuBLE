#pragma once

#include "types/declarations.hpp"
#include "types/ruby.hpp"

namespace Rice {
    template<> constexpr void ruby_mark<SimpleRbBLE::Callback>(SimpleRbBLE::Callback*);
    template<> void ruby_mark<SimpleRbBLE::Adapter>(SimpleRbBLE::Adapter *);
    template<> void ruby_mark<SimpleRbBLE::AdapterRegistry>(SimpleRbBLE::AdapterRegistry*);
    template<> void ruby_mark<SimpleRbBLE::AdapterRegistryFactory>(SimpleRbBLE::AdapterRegistryFactory*);
    template<> void ruby_mark<SimpleRbBLE::Peripheral>(SimpleRbBLE::Peripheral*);
    template<> void ruby_mark<SimpleRbBLE::PeripheralRegistry>(SimpleRbBLE::PeripheralRegistry*);
    template<> void ruby_mark<SimpleRbBLE::PeripheralRegistryFactory>(SimpleRbBLE::PeripheralRegistryFactory*);
    template<> void ruby_mark<SimpleRbBLE::Service>(SimpleRbBLE::Service*);
    template<> void ruby_mark<SimpleRbBLE::ServiceRegistry>(SimpleRbBLE::ServiceRegistry*);
    template<> void ruby_mark<SimpleRbBLE::ServiceRegistryFactory>(SimpleRbBLE::ServiceRegistryFactory*);
    template<> void ruby_mark<SimpleRbBLE::Characteristic>(SimpleRbBLE::Characteristic*);
    template<> void ruby_mark<SimpleRbBLE::CharacteristicRegistry>(SimpleRbBLE::CharacteristicRegistry*);
    template<> void ruby_mark<SimpleRbBLE::CharacteristicRegistryFactory>(SimpleRbBLE::CharacteristicRegistryFactory*);
}

