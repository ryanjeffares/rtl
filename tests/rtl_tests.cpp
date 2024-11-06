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

    auto foo() const -> void {}
};

int main() {
    const utilities::option<std::string> o{std::in_place, {'a', 'b', 'c'}};
}
