#include "rtl.hpp"

#include <print>
#include <string>

int main() {
    rtl::collections::list<std::string> list;

    for (auto i = 10; i < 20; i++) {
        list.add(std::to_string(i));
    }

    for (std::size_t i = 0; i < 15; i++) {
        auto el = list[i];

        if (el.has_value()) {
            std::println("{} {}", i, el->get());
        }
    }
}
