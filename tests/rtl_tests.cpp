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
    collections::list<std::string> strings;

    auto print_all = [&strings] {
        for (std::size_t i = 0; i < strings.size(); i++) {
            std::println("{}", strings[i]->get());
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
    strings.insert(2, 4, "Ryan8");
    print_all();
}
