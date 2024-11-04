#include "rtl.hpp"

#include <print>
#include <string>

using namespace rtl;

struct S {
    S() noexcept {
        std::println("Default");
    }

    S(int) noexcept {
        std::println("Parameterised");
    }

    S(const S&) noexcept {
        std::println("Copy");
    }

    S(S&&) noexcept {
        std::println("Move");
    }

    S& operator=(const S&) noexcept {
        std::println("Copy assign");
        return *this;
    }

    S& operator=(S&&) noexcept {
        std::println("Move assign");
        return *this;
    }
};

int main() {
    // std::optional<S> s{{1}};
    std::optional<const char*> o = "hello";
    std::optional<std::string> s = o;
    std::println("{}", *s);

   /* auto val = o.and_then([] (const int& i) {
        return rtl::utilities::option<std::string>{std::to_string(i)};
    }).and_then([] (std::string_view sv) {
        return rtl::utilities::option<std::size_t>(sv.size());
    }).unwrap_or(std::size_t{});*/

    // std::println("{}", val);
    /*rtl::collections::list<std::string> list;

    for (auto i = 10; i < 20; i++) {
        list.add(std::to_string(i));
    }

    for (std::size_t i = 0; i < 15; i++) {
        auto el = list[i];

        if (el.has_value()) {
            std::println("{} {}", i, el->get());
        }
    }*/
}
