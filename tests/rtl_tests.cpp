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
    rtl::collections::list<std::string> list;

    for (auto i = 10; i < 20; i++) {
        list.add(std::to_string(i));
    }

    const rtl::collections::list<std::string> l2;

    for (std::size_t i = 0; i < 15; i++) {
        auto el = list.at(i);

        if (el) {
            std::println("{}", el->get());
        }
    }
}
