#pragma once

#include "identifiers.hpp"
namespace SimpleRbBLE {

    template<Identifiers::OwnerTypedResource T>
    class ResourceUniqueIdentifier;
}

template<SimpleRbBLE::Identifiers::OwnerTypedResource T>
struct std::hash<SimpleRbBLE::ResourceUniqueIdentifier<T>> {
    constexpr std::size_t operator()(const SimpleRbBLE::ResourceUniqueIdentifier<T> &rui) const noexcept;
};

namespace SimpleRbBLE {
    template<Identifiers::OwnerTypedResource T>
    class ResourceUniqueIdentifier {
    public:
        using FullTuple = Identifiers::IDTypes::FullTuple<T>;
        using ThisType = Identifiers::IDTypes::ThisType<T>;
        using ThisTuple = Identifiers::IDTypes::ThisTuple<T>;
        using OwnerFullTuple = Identifiers::IDTypes::Full<T>::OwnerTuple;
        constexpr static std::size_t N = std::tuple_size_v<FullTuple>;
    protected:
        FullTuple _identifiers;
    public:
        template<std::size_t TupleIdx>
        using ElementType = std::tuple_element_t<TupleIdx, FullTuple>;
        template<std::size_t TupleIdx>
        static constexpr std::hash<ElementType<TupleIdx>> Hasher {};

        constexpr ResourceUniqueIdentifier(const FullTuple &tuple); // NOLINT(*-explicit-constructor)
        constexpr ResourceUniqueIdentifier(const T *res); // NOLINT(*-explicit-constructor)
        constexpr FullTuple get() const;
        constexpr operator FullTuple() const;

        [[nodiscard]] constexpr std::size_t hash() const;

        template<Identifiers::OwnerTypedResource U>
        constexpr std::strong_ordering operator<=>(const ResourceUniqueIdentifier<U> &other);
    };

} // SimpleRbBLE

#include "ResourceUniqueIdentifier.ipp"
