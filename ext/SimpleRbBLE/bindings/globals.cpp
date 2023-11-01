#include "types/declarations.hpp"
#include "types/ruby.hpp"
#include "types/SimpleBLE.hpp"
#include "common.hpp"
#include "utils/ruby_context.hpp"
#include "utils/hash.hpp"
#include "utils/garbage_collection.hpp"
#include "utils/exception_handling.hpp"
#include "bindings/Characteristic.hpp"
#include "management/Registry.hpp"
#include "bindings/Peripheral.hpp"
#include "bindings/Adapter.hpp"
#include "bindings/Characteristic.hpp"
#include <memory>

namespace SimpleRbBLE {
    thread_local InRuby in_ruby {};

    std::shared_ptr<RubyQueue> rubyQueue;
    Rice::Module rb_mSimpleRbBLE;
    Rice::Module rb_mSimpleRbBLEUnderlying;
    Rice::Module rb_mUtils;
    [[maybe_unused]] BluetoothAddressType_DT rb_cBluetoothAddressType;
    Characteristic_DT rb_cCharacteristic;
    [[maybe_unused]] CharacteristicCapabilityType_DT rb_cCharacteristicCapabilityType;
    [[maybe_unused]] Descriptor_DT rb_cDescriptor;
    Service_DT rb_cService;
    [[maybe_unused]] ServiceRegistry_DT rb_cServiceRegistry;
    AdapterRegistry_DT rb_cAdapterRegistry;
    [[maybe_unused]] PeripheralRegistry_DT rb_cPeripheralRegistry;
    [[maybe_unused]] CharacteristicRegistry_DT rb_cCharacteristicRegistry;
    ByteArray_DT rb_cByteArray;
    Peripheral_DT rb_cPeripheral;
    Adapter_DT rb_cAdapter;

    std::shared_ptr<AdapterRegistryFactory> adapterRegistryFactory;
    std::shared_ptr<AdapterRegistry> adapterRegistry;
    std::shared_ptr<PeripheralRegistryFactory> peripheralRegistryFactory;
    std::shared_ptr<ServiceRegistryFactory> serviceRegistryFactory;
    std::shared_ptr<CharacteristicRegistryFactory> characteristicRegistryFactory;

    bool Peripheral::unpair_all_on_exit = true;
}
