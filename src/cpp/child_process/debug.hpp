#ifndef _CHILD_PROCESS_DEBUG_HPP
#define _CHILD_PROCESS_DEBUG_HPP

#include <iostream>

#define expect(expr, args...)                                                  \
    (static_cast<bool>(expr) ? void(0)                                         \
                             : Fuu::debug.panic(#expr, __FILE__, __LINE__,     \
                                                __ASSERT_FUNCTION, args))

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
    template <typename... Types>
    void panic(const char *assertion, const char *file, unsigned int line,
               const char *function, const Types &...args) const;
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
template <typename... Types>
void Debug::panic(const char *assertion, const char *file, unsigned int line,
                  const char *function, const Types &...args) const {
    std::cout
        << "\u001b[1m\u001b[31m[PANIC][child_process]\u001b[39m\u001b[22m:\n";
    std::cout << "  [" << file << ":" << line << "]: " << function << std::endl;
    std::cout << "    断言 `" << assertion << "` 失败, ";
    std::initializer_list<int>{([&args] { std::cout << args; }(), 0)...};
    std::cout << std::endl;
    exit(-1);
}

inline Debug debug;

} // namespace Fuu
#endif