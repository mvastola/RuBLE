#pragma once

#include "metaprogramming.hpp"
#include "types.hpp"

namespace SimpleRbBLE {
    namespace Identifiers {}

    namespace Identifiers {
        namespace IDTypes {
            using namespace Identifiers;
    template<OwnerTypedResource T> struct is_toplevel {};
    template<TopLevelResource T> struct is_toplevel<T> {
        static constexpr bool value = true;
    };
    template<OwnedResource T> struct is_toplevel<T> {
        static constexpr bool value = false;
    };
    template<OwnerTypedResource T>
    constexpr bool is_toplevel_v = is_toplevel<T>::value;

    template<PartiallyIdentifiableResource T>
    using ThisType = PartialIdentifierType<T>;
    template<PartiallyIdentifiableResource T>
    using ThisTuple = std::tuple<ThisType<T>>;

    template<PartiallyIdentifiableResource T, bool is_top = is_toplevel_v<T>>
    struct Full {};

    template<TopLevelResource T>
    struct Full<T, true> {
        using OwnerTuple = std::tuple<>;
        using tuple = ThisTuple<T>;
    };

    template<PartiallyIdentifiableResource T>
    struct Full<T, false> {
        using OwnerTuple = typename IDTypes::Full<typename T::Owner>::tuple;
        using tuple = CombinedTupleType<OwnerTuple, ThisTuple<T>>;
    };

    template<PartiallyIdentifiableResource T, bool is_top = is_toplevel<T>::value>
    using FullTuple = Full<T, is_top>::tuple;

//            namespace IDTypes = SimpleRbBLE::Identifiers::IDTypes;
//            using FullType = IDTypes::FullTuple<Service>;
//            static constexpr const std::type_info &full_tuple_type = typeid(FullType);
//            std::cout << human_type_name<FullType>() << std::endl;
};
}
}