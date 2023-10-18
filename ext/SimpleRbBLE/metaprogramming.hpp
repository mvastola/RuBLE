#pragma once
#include <tuple>
#include <string_view>
#include <concepts>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <compare>
#include <string>
#include <future>
#include <optional>

namespace ranges = std::ranges;

namespace SimpleRbBLE {

    template<typename Key>
    using HasherType = std::hash<Key>;

    template<typename Key>
    using HashFnType = std::invoke_result_t<std::hash<Key>()>;

    template<typename Key>
    using HashResultType = std::invoke_result<HashFnType<Key>, const Key &>::type;
    static_assert(std::is_same<std::size_t, HashResultType<std::string>>::value);

    template<typename T>
    concept CallbackSetuppable = requires(T t) {
        { t.setup_callbacks() };
    };

    template<class T, class R>
    concept RangeOf = requires(const R &r, T &t) {
        { *ranges::begin(r) } -> std::convertible_to<T>;
        { *ranges::end(r) } -> std::convertible_to<T>;
    };

    template<ranges::viewable_range RangeT>
    using RangeIteratorType = std::invoke_result_t<decltype(std::ranges::begin), RangeT &&>;
    template<ranges::viewable_range RangeT>
    using RangeElementType = std::invoke_result_t<decltype(RangeIteratorType<RangeT>::operator*), RangeIteratorType<RangeT> &&>;

    template <typename From, typename To, bool is_const = std::is_const_v<From>>
    using MatchConst = std::conditional_t<is_const, const To, std::remove_const_t<To>>;
    template <typename From, bool is_const>
    using ConstIf = std::conditional_t<is_const, const From, From>;
    template <typename From, bool deref>
    using DeRefIf = std::conditional_t<deref, std::remove_reference_t<From>, From>;
    template <typename From>
    using WrapReferences = std::conditional_t<std::is_reference_v<From>, std::reference_wrapper<std::remove_reference_t<From>>, From>;


    namespace Identifiers {
        template<class T, class U>
        concept SameAsSansCVRef = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;
        template<class T>
        using PartialIdentifierType = std::remove_reference_t<std::invoke_result_t<decltype(&T::this_resource_identifier), std::decay_t<const T &>>>;

        template<typename T> concept ClassOrNull = true; //(std::is_class_v<T> || std::same_as<T, std::nullptr_t>);
        template<class T> concept OwnerTypedResource = requires { typename T::Owner; }; // ClassOrNull<typename T::Owner>; //
        template<class T> concept PartiallyIdentifiableResource = std::regular<PartialIdentifierType<T>>;
        template<class T> concept PartiallyIdentifiableOwnerTypedResource = PartiallyIdentifiableResource<T> &&
                                                                            OwnerTypedResource<T>;
        template<class T> concept TopLevelResource = PartiallyIdentifiableOwnerTypedResource<T> &&
                                                     std::same_as<typename T::Owner, std::nullptr_t>;

        template<class T> concept OwnedResource = PartiallyIdentifiableOwnerTypedResource<T> &&
                                                  std::is_class_v<T> && requires(
                const T &t) {{ *t.owner() } -> SameAsSansCVRef<typename T::Owner>; };
        template<class T> concept Resource = TopLevelResource<T> || OwnedResource<T>;

        template<class Tuple1, class Tuple2> using CombinedTupleType =
                std::invoke_result_t<decltype(&std::tuple_cat<Tuple1, Tuple2>), Tuple1, Tuple2>;

        static_assert(std::same_as<CombinedTupleType<std::tuple<int, bool>,
                std::tuple<float, long>>, std::tuple<int, bool,float,long>>);

    }

    template<typename T>
    constexpr bool Nullopt = std::same_as<std::remove_cvref_t<T>, std::nullopt_t>;

    template<typename T>
    struct is_duration {
        constexpr static const bool value = false;
    };

    template<typename ...Types>
    struct is_duration<std::chrono::duration<Types...>> {
        constexpr static const bool value = true;
    };
    template<typename T>
    concept Duration = is_duration<T>::value;

    template<class T>
    concept OptionalDuration = Duration<T> || std::same_as<std::nullopt_t, T>;

    template<typename T>
    struct future_traits {
        static constexpr const bool value = false;
    };

    template<template<typename> class FutureType, typename ValueType>
    struct future_traits<FutureType<ValueType>> {
    private:
        using exact_type = FutureType<ValueType>;
        using exact_value_type = ValueType;
    public:
        using type = std::remove_reference_t<exact_type>;
        using value_type = std::invoke_result_t<decltype(&exact_type::get), exact_type&>;
    private:
        using future_type = std::future<exact_value_type>;
        using shared_future_type = std::shared_future<exact_value_type>;
    public:
        static constexpr const bool shared = std::same_as<std::remove_cvref_t<type>, std::remove_cvref_t<shared_future_type>>;
        static constexpr const bool not_shared = std::same_as<std::remove_cvref_t<type>, std::remove_cvref_t<future_type>>;
        static constexpr const bool value = shared || not_shared;
    };

    template<template<typename> class FutureType, typename ValueType>
    struct future_traits<const FutureType<ValueType>> {
        using type = future_traits<FutureType<ValueType>>::type;
        using value_type = std::remove_reference_t<typename future_traits<FutureType<ValueType>>::value_type>;
        static constexpr const bool shared = future_traits<FutureType<ValueType>>::shared;
        static constexpr const bool not_shared = future_traits<FutureType<ValueType>>::not_shared;
        static constexpr const bool value = future_traits<FutureType<ValueType>>::value;

    };

    template<typename T>
    concept Future = future_traits<T>::value;

}
