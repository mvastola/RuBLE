#pragma once
#include <iostream>
#include <concepts>
#include "types/ruby.hpp"
#include "utils/human_type_names.hpp"
#include "utils/ruby_context.hpp"
#include "utils/hexadecimal.hpp"

namespace SimpleRbBLE {
    template <class T>
    class Rubyable {
    protected:
        VALUE _self = Qnil;

        virtual constexpr VALUE &get_or_allocate_rb_object() {
            if (_self != Qnil) return _self;
            if (in_ruby) {
                _self = Rice::Data_Object<T>(*dynamic_cast<T*>(this));
                std::cerr << "Note: allocated ruby object for ";
                std::cerr << Utils::human_type_name(*this) << " (";
                std::cerr << Utils::ptr_to_hex_addr(this) << ") with VALUE ";
                std::cerr << _self << "." << std::endl;
            } else {
                std::cerr << "Warning: could not allocate ruby object for ";
                std::cerr << Utils::human_type_name(*this) << " (";
                std::cerr << Utils::ptr_to_hex_addr(this) << ") because ";
                std::cerr << "inRuby is false." << std::endl;
            }
            return _self;
        }
    public:
        constexpr Rubyable() = default;

        template<class U = Rice::Data_Object<T>> requires std::is_base_of_v<Rice::Object, U>
        U self() const {
            if (_self == Qnil) _self = get_or_allocate_rb_object();
            return { _self };
        }

        virtual void ruby_mark() const {
            if (_self != Qnil) rb_gc_mark(_self);
        }
    };
} // SimpleRbBLE

