#pragma once

#include <memory>
#include <iostream>
#include <ranges>

#include "types/declarations.hpp"
#include "types/helpers.hpp"
#include "types/ruby.hpp"
#include "utils/ruby_context.hpp"
#include "utils/human_type_names.hpp"
#include "utils/garbage_collection.hpp"

namespace ranges = std::ranges;
namespace views = ranges::views;

namespace RuBLE {
    template<typename Key, class ProxyClass, class Value>
    class Registry {
        VALUE _self = Qnil;
        std::shared_ptr<std::mutex> _mtx = nullptr;
    public:
        using Self [[maybe_unused]] = Registry<Key, ProxyClass, Value>;
        using Owner = ProxyClass::Owner;
        using Factory = RegistryFactory<Self>;
        using KeyType = Key;
        using ProxyClassType = ProxyClass;
        using ProxyRubyObjType = ToRubyObjectResult<ProxyClass>;
        using ValueType = Value;
        using DataObject = Data_Object<ProxyClass>;
        using Registry_DO = Data_Object<Self>;
        using ProxyPtr = std::shared_ptr<ProxyClass>;
//    using ProxyRefVector = std::vector<ProxyRef>;
        using ProxyPtrVector = std::vector<ProxyPtr>;
        using Collection = std::map<Key, ProxyPtr>;
        using CollectionIterator = typename Collection::iterator;

//        using ForEachFn = std::function<void(typename Collection::mapped_type &)>;
//        using ForEachFnConst = std::function<void(typename Collection::mapped_type &)>;
        static constexpr const bool is_owned = !std::is_same_v<Owner, nullptr_t>;

        void ruby_mark() const {
            rb_gc_mark(_self);
            Rice::ruby_mark<Collection>(_registry.get());
            for_each([](const ProxyPtr &ref) -> void {
                Rice::ruby_mark<ProxyClass>(ref.get());
            });
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
//            std::cout << "Constructing new " << RuBLE::human_type_name<ProxyClass>() << " registry at address ";
//            std::cout << to_hex_addr(this);
//            if constexpr (is_owned) std::cout << " owned by " << human_type_name(*_owner); // _owner->to_s();
//            std::cout << std::endl;
        }
        // FIXME: why am I getting a warning that _self is uninitialized with the constructor below?
        Registry() requires std::is_same_v<Owner, nullptr_t> : Registry(nullptr) {}

        [[nodiscard]] Object self() const { return _self; }
        constexpr const Owner *owner() const { return _owner; }
        constexpr Owner *owner() { return _owner; }

        [[maybe_unused]] Key key_from_value(const Value &) const {
            std::cerr << "Type of class: " << RuBLE::Utils::human_type_name(*this) << std::endl;
            throw std::invalid_argument(
                    "key_from_value not implemented without .address() returning a BluetoothAddress");
        }

        [[maybe_unused]] BluetoothAddress key_from_value(const Value &v) const requires BluetoothAddressable<Value> {
            return const_cast<Value &>(v).address();
        }

        [[maybe_unused]] BluetoothUUID key_from_value(const Value &v) const requires UUIDable<Value> {
            return const_cast<Value &>(v).uuid();
        }

    public:
        Collection::size_type size() const { return _registry->size(); }
        bool contains(const auto &key) const { return _registry->contains(key); }
        ProxyPtr &operator[](const Key &addr) { return _registry->at(addr); }
        const ProxyPtr &operator[](const Key &addr) const { return _registry->at(addr); }
        const ProxyPtr &at(const Key &addr) const { return _registry->at(addr); }
        ProxyPtr &at(const Key &addr) { return _registry->at(addr); }
        std::shared_ptr<Collection> &data() const { return _registry; }

        ProxyPtr fetch(const Value &value) const {
            const std::lock_guard<std::mutex> lock(*_mtx); // needed to avoid race condition

            Key key = key_from_value(value);
            CollectionIterator found = _registry->find(key);
            if (found != _registry->end()) return found->second;

            if (rb_during_gc()) {
                std::ostringstream oss;
                oss << "Warning: attempt to create new " << Utils::human_type_name<ProxyClass>();
                oss << " at key " << key << " during garbage collection.";
                throw std::runtime_error(oss.str());
            }

            if constexpr (is_owned) {
                _registry->emplace(key, new ProxyClass(value, _owner));
            } else {
                _registry->emplace(key, new ProxyClass(value));
            }
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

        Rice::Array map_to_ruby_objects(const auto &arg) const requires HasRubyObject<ProxyClass> {
            std::vector<ProxyPtr> proxyVec = map_to_objects(arg);
            auto xform = [](const ProxyPtr &obj) -> Rice::Object { return obj->self(); };
            auto objs = proxyVec | views::transform(xform);
            Rice::Array result(objs.begin(), objs.end());
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
            os << "Registry for " << Utils::human_type_name<ProxyClass>() << " (";
            os << Utils::human_type_name(*this) << "): "; // << getSelf().inspect();
            if constexpr (is_owned) os << " owned by " << _owner->to_s();
            return os.str();
        }

        friend void Init_Registries();
        template<typename> friend class RegistryFactory;

        friend class std::hash<Registry<Key, ProxyClass, Value>>;
    };

//    template<typename Key, class ProxyClass, class Value>
//    constexpr std::shared_ptr<Registry<Key, ProxyClass, Value>::Collection> &Registry<Key, ProxyClass, Value>::data() { return *_registry; }

    extern std::shared_ptr<AdapterRegistry> adapterRegistry;
}


