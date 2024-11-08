#include "rtl.hpp"

#include <print>
#include <string>
#include <vector>

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

    auto foo() const -> void {}
};

int main() {
    collections::list<std::string> strings;

    auto print_all = [&strings] {
        for (const auto& string : strings) {
            std::println("{}", string);
        }
        std::println("===");
    };

    strings.add("Ryan1");
    strings.add("Ryan2");
    strings.add("Ryan3");
    strings.add("Ryan4");
    strings.add("Ryan5");
    print_all();
    strings.insert(0, "Ryan6");
    print_all();
    strings.insert(5, "Ryan7");
    print_all();

    const collections::list<std::string> const_strings;
    for (auto it = strings.cbegin(); it != const_strings.cend(); ++it) {
        std::println("{}, {}", *it, it->size());
    }

    std::vector<int> v;
    auto it = typename decltype(v)::iterator{};
}
