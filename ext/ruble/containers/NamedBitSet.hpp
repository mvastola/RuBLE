#pragma once

#include <cstddef>
#include <map>
#include <bitset>
#include <utility>
#include <functional>
#include <variant>
#include <string_view>
#include <ranges>
#include "utils/containers.hpp"

#include "types/helpers.hpp"

namespace RuBLE {
    template<const auto &FIELD_NAMES> class NamedBitSet;
}

template<const auto &FIELDS> struct std::hash<RuBLE::NamedBitSet<FIELDS>> {
    constexpr std::size_t operator()(const RuBLE::NamedBitSet<FIELDS> &nbs) const noexcept;
};

namespace RuBLE {
    // this namespace from https://en.cppreference.com/w/cpp/utility/variant/visit
    namespace visit {
        template<class... Ts>
        struct overloaded : Ts... { using Ts::operator()...; };
#if !defined(__cpp_deduction_guides) || __cpp_deduction_guides < 201907L
        // explicit deduction guide (not needed as of C++20)
        template<class... Ts>
        overloaded(Ts...) -> overloaded<Ts...>;
#endif
    }

    namespace FlagCheck {
        using ConstFlag = bool;
        using StaticFn = const std::function<bool()>;
        template<class T> using ConstMemFn = std::function<bool(const std::remove_cvref_t<T>&)>;

        template<class T = nullptr_t>
        using Fn = std::variant<ConstFlag, StaticFn, ConstMemFn<T>>;

        using MapKey = std::string_view;
        template<class T = nullptr_t> using MapType = std::map<MapKey, Fn<T>>;

        template<class R, class T>
        concept Map = requires(const R &r, T &t) {
            { std::get<0>(*r.begin()) } -> std::convertible_to<const MapKey>;
            { std::get<1>(*r.begin()) } -> std::convertible_to<Fn<T>>;
        };

#ifdef NBSDEBUG
        template<class T = nullptr_t> using Pair = std::pair<const MapKey, Fn<T>>;
        template<class T> using MapPairType = FlagCheck::Pair<T>;
        template <class R> using RangeIteratorType = std::result_of_t<decltype(&R::cbegin)(R)>;
        template<class R> using RangeItem = std::result_of_t<decltype(&RangeIteratorType<R>::operator*)(RangeIteratorType<R>)>;
        template<class R> using RangeItemNoRef = std::remove_reference_t<RangeItem<R>>;
        template<class R> using RangeItemKey = RangeItemNoRef<R>::first_type;
        template<class R> using RangeItemValue = RangeItemNoRef<R>::second_type;
        static_assert(std::is_same_v<MapType<int>::value_type, Pair<int>>);
        static_assert(std::is_convertible_v<RangeItem<MapType<int>>, MapPairType<int>>);
        static_assert(std::is_convertible_v<RangeItemKey<MapType<int>>, const MapKey&>);
        static_assert(Map<MapType<int>, int>);
#endif

        template<class T>
        bool test_flag(const FlagCheck::Fn<T> &fn, const auto &obj = nullptr) {
            bool result = std::visit(visit::overloaded{
                    []() -> bool { return false; },
                    [](nullptr_t) -> bool { return false; },
                    [](const FlagCheck::ConstFlag &val) -> bool { return val; },
                    [](const FlagCheck::StaticFn &fn)  -> bool { return fn(); },
                    [&obj](const std::function<bool(const T&)> &fn) -> bool {
                        return fn(obj);
                    },
            }, fn);
            return result;
        }

    }
    template<const auto& FIELD_NAMES>
    class NamedBitSet {
    public:
        static constexpr auto FIELDS = std::to_array<const std::string_view>(FIELD_NAMES);
        static constexpr const std::size_t N = FIELDS.size();
        using BitSet = std::bitset<N>;
        using BitRef = BitSet::reference;
        using PairType = std::pair<std::string_view, std::size_t>;
        static constexpr auto PAIRS = ([]<std::size_t... I>(std::index_sequence<I...>) -> std::array<PairType, N> {
            return {std::make_pair(FIELD_NAMES[I], I)...};
        })(std::make_index_sequence<N>{});

        static constexpr std::size_t index_of(const std::string_view &name);

    private:
        BitSet _bits = 0;

    public:
        constexpr const BitSet &bits() const;

        constexpr BitRef operator[](const std::string_view &name);

        constexpr bool operator[](const std::string_view &name) const;

        // Ensure T is an integral type that can hold all N bits
        template<typename T> requires std::is_integral_v<T> && (std::numeric_limits<T>::max() >= (1 << N) - 1)
                constexpr explicit operator T() const { return static_cast<T>(_bits.to_ullong()); }

        [[nodiscard]] constexpr std::vector<std::string_view> flag_names() const;

        [[nodiscard]] constexpr std::vector<std::string> flag_name_strs() const;

        constexpr NamedBitSet(): _bits(0) {}

        constexpr NamedBitSet(const std::initializer_list<const std::string_view> &list);

        constexpr NamedBitSet(const auto &rangeList);
        constexpr NamedBitSet(const BitSet &bitSet);
        // Takes a map (or equivalent) of 'std::string_view's to one of: boolean literal, static fn, const mem fn, mem fn
        template<typename ObjT, FlagCheck::Map<ObjT> R>
        constexpr NamedBitSet(R &r, ObjT &&obj);

        [[nodiscard]] constexpr std::string to_s() const;
    };



#ifdef RUBLE_DEBUG
    void NamedBitSetDemo();
#endif
}

template<const auto &FIELDS>
constexpr std::size_t std::hash<RuBLE::NamedBitSet<FIELDS>>::operator()(const RuBLE::NamedBitSet<FIELDS> &nbs) const noexcept {
    std::size_t h1 = typeid(nbs).hash_code();
    static auto hasher = std::hash<decltype(RuBLE::NamedBitSet<FIELDS>::BitSet)>{};
    return h1 ^ (hasher(nbs.bits()) << 1);
}

#include "NamedBitSet.ipp"
