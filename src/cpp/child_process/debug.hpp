#ifndef _CHILD_PROCESS_DEBUG_HPP
#define _CHILD_PROCESS_DEBUG_HPP

#include <iostream>

namespace Fuu {

enum DebugLevel {
    Info,
    Warn,
    Error,
    None = Error + 1,
};

class Debug {
  public:
    DebugLevel curr_debug_level = DebugLevel::Info;

    template <typename... Types> void info(const Types &...args) const;
    template <typename... Types> void warn(const Types &...args) const;
    template <typename... Types> void error(const Types &...args) const;
};

template <typename... Types> void Debug::info(const Types &...args) const {
    if (DebugLevel::Info < curr_debug_level) {
        return;
    }
    std::cout << "\u001b[1m\u001b[36m[child_process]\u001b[39m\u001b[22m: ";
    std::initializer_list<int>{([&args] { std::cout << args; }(), 0)...};
    std::cout << std::endl;
}
template <typename... Types> void Debug::warn(const Types &...args) const {
    if (DebugLevel::Warn < curr_debug_level) {
        return;
    }
    std::cout
        << "\u001b[1m\u001b[33m[WARN][child_process]\u001b[39m\u001b[22m: ";
    std::initializer_list<int>{([&args] { std::cout << args; }(), 0)...};
    std::cout << std::endl;
}
template <typename... Types> void Debug::error(const Types &...args) const {
    if (DebugLevel::Error < curr_debug_level) {
        return;
    }
    std::cout
        << "\u001b[1m\u001b[31m[ERROR][child_process]\u001b[39m\u001b[22m: ";
    std::initializer_list<int>{([&args] { std::cout << args; }(), 0)...};
    std::cout << std::endl;
}

extern Debug debug;

} // namespace Fuu
#endif