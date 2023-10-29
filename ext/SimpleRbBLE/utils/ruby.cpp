#include "ruby.hpp"

namespace SimpleRbBLE::Utils::Ruby {
    InRuby::Guard InRuby::assert_guard() {
        Guard flag = _state.lock();
        if (!flag) _state = flag = std::make_shared<bool>(true);
        return flag;
    }

    bool InRuby::state() const { return !_state.expired(); }

    InRuby::operator bool() const { return state(); }
}
