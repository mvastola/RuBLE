#pragma once

#include "types.h"
#include "utils.h"
#include <map>
#include <memory>
#include <functional>
#include <exception>
#include <typeinfo>
#include <mutex>

namespace SimpleRbBLE {

//template<typename T>
//concept Unwrappable = requires(T::DataObject tdo) { typename T::DataObject; } &&
//requires(std::shared_ptr<typename T::DataObject> ptr) { std::static_pointer_cast<T>(ptr); };

    template<typename Key, class ProxyClass, class Value>
    class Registry {
        VALUE _self;
        std::shared_ptr<std::mutex> _mtx = nullptr;
    public:
        using Self = Registry<Key, ProxyClass, Value>;
        using Owner = ProxyClass::Owner;
        using Factory = RegistryFactory<Self>;
        using KeyType = Key;
        using ProxyClassType = ProxyClass;
        using ValueType = Value;
        using DataObject = Data_Object<ProxyClass>;
        using Registry_DO = Data_Object<Self>;
        using ProxyPtr = std::shared_ptr<ProxyClass>;
//    using ProxyRefVector = std::vector<ProxyRef>;
        using ProxyPtrVector = std::vector<ProxyPtr>;
        using Collection = std::map<Key, ProxyPtr>;

        using ForEachFn = std::function<void(typename Collection::mapped_type &)>;
        using ForEachFnConst = std::function<void(typename Collection::mapped_type &)>;
        static constexpr const bool is_owned = !std::is_same_v<Owner, nullptr_t>;

        ~Registry() {
//            std::cout << "Destructing " << SimpleRbBLE::human_type_name<ProxyClass>() << " registry at address ";
//            std::cout << "0x" << std::hex << reinterpret_cast<uint64_t>(this) << std::dec << std::endl;
        }
    private:
        Owner *_owner;
        mutable std::shared_ptr<Collection> _registry;
    protected:
        explicit Registry(Owner *owner) :
                     _owner(owner),
                     _registry(std::make_shared<Collection>()),
                     _mtx(std::make_shared<std::mutex>()),
                     _self(Registry_DO(*this)) {
//            std::cout << "Constructing new " << SimpleRbBLE::human_type_name<ProxyClass>() << " registry at address ";
//            std::cout << "0x" << std::hex << reinterpret_cast<uint64_t>(this) << std::dec;
//            if constexpr (is_owned) std::cout << " owned by " << human_type_name(*_owner); // _owner->to_s();
//            std::cout << std::endl;
        }
        Registry() requires std::is_same_v<Owner, nullptr_t> : Registry(nullptr) {}

        Object self() const { return _self; }
        const Owner *owner() const { return _owner; }
        Owner *owner() { return _owner; }

        void configure_proxy_instance(const ProxyPtr &) const {}

        void configure_proxy_instance(ProxyPtr &result) const requires CallbackSetuppable<ProxyClass> {}

        Key key_from_value(const Value &) const {
            std::cerr << "Type of class: " << SimpleRbBLE::human_type_name(*this) << std::endl;
            throw std::invalid_argument(
                    "key_from_value not implemented without .address() returning a BluetoothAddress");
        }

        BluetoothAddress key_from_value(const Value &v) const requires BluetoothAddressable<Value> {
            return const_cast<Value &>(v).address();
        }

        BluetoothUUID key_from_value(const Value &v) const requires UUIDable<Value> {
            return const_cast<Value &>(v).uuid();
        }

        void ruby_mark() const {
            rb_gc_mark(_self);
            Rice::ruby_mark<Collection>(_registry.get());
            for_each([](const ProxyPtr &ref) -> void {
                Rice::ruby_mark<ProxyClass>(ref.get());
            });
        }

    public:
        Collection::size_type size() const { return _registry->size(); }
        bool contains(const auto &key) const { return _registry->contains(key); }
        ProxyPtr &operator[](const Key &addr) { return _registry->at(addr); }
        const ProxyPtr &operator[](const Key &addr) const { return _registry->at(addr); }
        const ProxyPtr &at(const Key &addr) const { return _registry->at(addr); }
        ProxyPtr &at(const Key &addr) { return _registry->at(addr); }
        constexpr const Collection &data() const { return *_registry; }

        ProxyPtr fetch(const Value &value) const {
            const std::lock_guard<std::mutex> lock(*_mtx); // needed to avoid race condition

            Key key = key_from_value(value);
            typename decltype(_registry)::element_type::iterator found = _registry->find(key);
            if (found != _registry->end()) return found->second;

            if (rb_during_gc()) {
                std::ostringstream oss;
                oss << "Warning: attempt to create new " << human_type_name<ProxyClass>();
                oss << " at key " << key << " during garbage collection.";
                throw std::runtime_error(oss.str());
            }

            if constexpr (is_owned) {
                _registry->emplace(key, new ProxyClass(value, _owner));
            } else {
                _registry->emplace(key, new ProxyClass(value));
            }
            this->configure_proxy_instance(_registry->at(key));
            return _registry->at(key);
        };

        void clear() {
            const std::lock_guard<std::mutex> lock(*_mtx); // needed to avoid race condition
            _registry->clear();
        }

        ProxyPtrVector map_to_objects(const std::vector<Value> &unwrappedValues) const {
            ProxyPtrVector result{};
            std::ranges::transform(unwrappedValues, back_inserter(result), [this](const Value &v) -> ProxyPtr {
                return this->fetch(v);
            });
            return result;
        }

        void for_each(auto fn) {
            std::ranges::for_each(*_registry, [&fn](typename Collection::value_type &pair) -> void {
                auto &[key, value] = pair;
                fn(value);
            });
        }

        void for_each(auto fn) const {
            std::ranges::for_each(*_registry, [&fn](const typename Collection::value_type &pair) -> void {
                const auto &[key, value] = pair;
                fn(value);
            });
        }

        [[nodiscard]] std::string to_s() const {
            std::ostringstream os;
            os << "Registry for " << typeid(ProxyClass).name() << " (";
            os << SimpleRbBLE::human_type_name(*this) << "): "; // << getSelf().inspect();
            if constexpr (is_owned) os << " owned by " << _owner->to_s();
            return os.str();
        }

        friend void Init_Registries();
        template<typename> friend class RegistryFactory;

        friend class AdapterRegistrySingleton;

        template<typename T, typename...Arg_Ts> friend
        class Rice::Constructor;

        template<typename T>
        friend void Rice::ruby_mark(T *);

        friend class std::hash<Registry<Key, ProxyClass, Value>>;
    };

    extern std::shared_ptr<AdapterRegistry> adapterRegistry;
}

// TODO: Use this in the registry map, but don't put in std::hash
//template<typename Key, class ProxyClass, class Value>
//struct std::hash<SimpleRbBLE::Registry<Key, ProxyClass, Value>> {
//    std::size_t operator()(const SimpleRbBLE::Registry<Key, ProxyClass, Value>& r) const noexcept {
//        std::size_t h1 = typeid(r).hash_code();
//        using RegistryOwner = SimpleRbBLE::Registry<Key, ProxyClass, Value>::Owner;
//        static auto hasher = std::hash<SimpleBLE::BluetoothAddress>{};
//        return h1 ^ (hasher(r._registry) << 1);
//    }
//};

template<typename Key, class ProxyClass, class Value>
std::ostream &operator<<(std::ostream &os, const SimpleRbBLE::Registry<Key, ProxyClass, Value> &reg) {
    return os << reg.to_s();
}