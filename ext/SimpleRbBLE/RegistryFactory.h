#pragma once

#include "common.h"
#include <unordered_map>


namespace SimpleRbBLE {
// there is always a single singleton instance of this class,
// but each instance holds a registry of registries (so to speak)
// the keys of the meta-registry are hashes of Owner
    template<class RegistryType>
    class RegistryFactory {
    public:
        using Self = RegistryFactory<RegistryType>;

        using RegistryOwner = RegistryType::Owner;
        using RegistryMapKey = HashResultType<RegistryOwner>;
        using RegistryMapValue = std::shared_ptr<RegistryType>;
        using RegistryMapType = std::unordered_map<RegistryMapKey, RegistryMapValue>;

        // no moving. only copying
        RegistryFactory() : _registries(std::make_shared<RegistryMapType>()) {}
        RegistryFactory(RegistryFactory &&) = delete;
        RegistryFactory &operator=(RegistryFactory &&) = delete;
        ~RegistryFactory() = default;

    protected:
        static constexpr HasherType<RegistryOwner> RegistryKeyHasher {};
        inline static std::shared_ptr<Self> _instance {};
        const std::shared_ptr<RegistryMapType> _registries {};


    public:
        static const std::shared_ptr<RegistryFactory> &instance() {
            static std::mutex _static_mtx;
            if (!_instance) {
                std::lock_guard<std::mutex> _lock(_static_mtx);
                if (!_instance) _instance = std::shared_ptr<Self>(new Self());
            }
            return _instance;
        }

        const std::shared_ptr<RegistryType> &registry(RegistryOwner *owner) const {
            RegistryMapKey index;
            if constexpr (RegistryType::is_owned) {
                index = RegistryKeyHasher(*owner);
            } else {
                index = RegistryKeyHasher(nullptr);
            }
            // happy path is registry at index already exists
            if (_registries->contains(index)) return (*_registries)[index];

            // using new RegistryType instead of std::make_shared allows keeping the constructor private
            // will go out of scope later if try_emplace fails due to race condition
            std::shared_ptr<RegistryType> newRegistry;
            if constexpr (RegistryType::is_owned) {
                newRegistry = std::shared_ptr<RegistryType>(new RegistryType(owner));
            } else {
                newRegistry = std::shared_ptr<RegistryType>(new RegistryType());
            }
            const auto &[registry_it, inserted] = _registries->try_emplace(index, std::move(newRegistry));
            return registry_it->second;
        }

        const std::shared_ptr<RegistryType> &registry() const requires std::same_as<RegistryOwner, nullptr_t> {
            return this->registry(nullptr);
        }

        template<class... Types>
        static const std::shared_ptr<RegistryType> &getRegistry(const Types &... args) {
            return instance()->registry(args...);
        }

        void ruby_mark() const {
            Rice::ruby_mark(_registries.get());
            static constexpr auto mark_each_value = [](const RegistryMapValue &value) {
                Rice::ruby_mark<RegistryType>(value.get());
                rb_gc_mark(value->_self);
            };
            for (const auto &pair : *_registries) mark_each_value(pair.second);
        };

        template<typename T>
        friend void Rice::ruby_mark(T *);
    };

    extern std::shared_ptr<AdapterRegistryFactory> adapterRegistryFactory;
    extern std::shared_ptr<PeripheralRegistryFactory> peripheralRegistryFactory;
    extern std::shared_ptr<ServiceRegistryFactory> serviceRegistryFactory;
    extern std::shared_ptr<CharacteristicRegistryFactory> characteristicRegistryFactory;
}
