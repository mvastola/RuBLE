#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once

#include "types.hpp"
#include "DebugUtils.hpp"
#include "helpers/Callback.hpp"
#include "Registry.hpp"

#include <unordered_set>

namespace SimpleRbBLE {
    namespace Rice = ::Rice;

    template<Identifiers::OwnerTypedResource T>
    class ResourceUniqueIdentifier;

    class Adapter {
    public:
        using Owner = nullptr_t;
        using DataObject = Data_Object<Adapter>;

    protected:
        VALUE _self;
        std::shared_ptr<SimpleBLE::Adapter> _adapter;
        BluetoothAddress _addr;
        std::shared_ptr<Callback> _on_scan_start;
        std::shared_ptr<Callback> _on_scan_stop;
        std::shared_ptr<Callback> _on_scan_update;
        std::shared_ptr<Callback> _on_scan_find;
        std::shared_ptr<PeripheralRegistry> _peripheral_registry;

        Adapter() = delete;
    public:
        Adapter(const SimpleBLE::Adapter &adapter);

        ~Adapter() = default;

        Object self() const;

        static bool bluetooth_enabled();

        static std::vector<std::shared_ptr<Adapter>> get_adapters();

        virtual bool initialized() const;

        SimpleBLE::Adapter &get();

        const SimpleBLE::Adapter &get() const;

        std::string identifier();

        SimpleBLE::BluetoothAddress address() const;

        void scan_start();

        void scan_stop();

        void scan_for(int timeout_ms);

        bool scan_is_active() const;

        void on_scan_start(Object on_scan_start);
        void on_scan_stop(Object on_scan_stop);
        void on_scan_update(Object on_scan_updated);
        void on_scan_find(Object on_scan_found);

        std::vector<std::shared_ptr<Peripheral>> scan_get_results();

        std::vector<std::shared_ptr<Peripheral>> get_paired_peripherals();

        bool operator==(const SimpleBLE::BluetoothAddress &addr) const;

        bool operator==(const Adapter &ap) const;

        std::unordered_set<std::string> status_flags() const;
        std::string to_s() const;

        constexpr ResourceUniqueIdentifier<Adapter> full_resource_identifier() const;
        constexpr BluetoothAddress this_resource_identifier() const;

        template<typename Key, class ProxyClass, class Value>
        friend class Registry;
        template<typename T>
        friend void ::Rice::ruby_mark(T *);

        friend void Init_Adapter();
        friend void Init_Registries();
        friend void DeInit(VALUE);
    };

    static inline Adapter_DT rb_cAdapter;
}

#pragma clang diagnostic pop
