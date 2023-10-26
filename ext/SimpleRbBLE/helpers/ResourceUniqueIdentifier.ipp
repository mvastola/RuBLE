#pragma once

#include "ResourceUniqueIdentifier.hpp"

namespace SimpleRbBLE {
    template<Identifiers::OwnerTypedResource T>
    constexpr ResourceUniqueIdentifier<T>::ResourceUniqueIdentifier(const T *res) { // NOLINT(*-explicit-constructor)
        std::remove_cvref_t<ThisType> &&thisIdentifier = res->this_resource_identifier();
        //        ThisTuple thisTuple = std::make_tuple<ThisType>(thisIdentifier);
        if constexpr (Identifiers::IDTypes::is_toplevel<T>::value) {
            _identifiers = ThisTuple(std::forward_as_tuple(thisIdentifier));
        } else {
            typename ResourceUniqueIdentifier<typename T::Owner>::FullTuple ownerIdentifiers =
                    ResourceUniqueIdentifier<typename T::Owner>(res->owner()).get();
            auto &&result = std::tuple_cat(
                    std::forward_as_tuple(ownerIdentifiers),
                    ThisTuple(std::forward_as_tuple(thisIdentifier))
            );
            _identifiers = std::make_from_tuple<FullTuple>(result);
        }
    }

    template<Identifiers::OwnerTypedResource T>
    constexpr ResourceUniqueIdentifier<T>::ResourceUniqueIdentifier(const ResourceUniqueIdentifier::FullTuple &tuple) : _identifiers(tuple) { }

    template<Identifiers::OwnerTypedResource T>
    constexpr std::size_t ResourceUniqueIdentifier<T>::hash() const {
        std::size_t h1 = typeid(*this).hash_code();
        static constexpr auto hash_items = [&h1] <std::size_t... I>
                (FullTuple &&tuple, std::index_sequence<I...> = std::make_index_sequence<N>{}) {
            ((h1 ^= Hasher<I>(std::get<I>(tuple))), ...);
        };
        hash_items();
        return h1;
    }

    template<Identifiers::OwnerTypedResource T>
    template<Identifiers::OwnerTypedResource U>
    constexpr std::strong_ordering ResourceUniqueIdentifier<T>::operator<=>(const ResourceUniqueIdentifier<U> &other) {
        static_assert(std::is_constant_evaluated());
        return hash() <=> other.hash();
    }

    template<Identifiers::OwnerTypedResource T>
    constexpr ResourceUniqueIdentifier<T>::FullTuple ResourceUniqueIdentifier<T>::get() const {
        return std::move(FullTuple(_identifiers));
    }

    template<Identifiers::OwnerTypedResource T>
    constexpr ResourceUniqueIdentifier<T>::operator FullTuple () const {
        return std::move(get());
    }
};

template<SimpleRbBLE::Identifiers::OwnerTypedResource T>
constexpr std::size_t std::hash<SimpleRbBLE::ResourceUniqueIdentifier<T>>::operator()(
        const SimpleRbBLE::ResourceUniqueIdentifier<T> &rui) const noexcept {
    return rui.hash();
}