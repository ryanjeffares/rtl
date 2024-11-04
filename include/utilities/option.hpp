/*
 * Copyright 2024 Ryan Jeffares
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef RTL_OPTION_HPP
#define RTL_OPTION_HPP

#include <functional>
#include <type_traits>

namespace rtl::utilities {
namespace detail {
struct dummy {
    constexpr dummy() noexcept {

    }
};
} // namespace detail

struct nullopt_t {
    constexpr explicit nullopt_t(int) {

    }
};

constexpr inline auto nullopt = nullopt_t{0};

template<typename T>
class option {
private:
    static constexpr bool s_is_move_constructible = std::is_move_constructible_v<T>;
    static constexpr bool s_is_nothrow_move_constructible = std::is_nothrow_move_constructible_v<T>;

public:
    constexpr option() noexcept
        : m_dummy{}
        , m_has_value{false} {
    }

    constexpr option(nullopt_t) noexcept
        : m_dummy{}
        , m_has_value{false} {
    }

    template<typename... Args>
    constexpr option(Args&&... args) noexcept(noexcept(std::is_nothrow_constructible_v<T, Args...>))
        : m_value{std::forward<Args>(args)...}
        , m_has_value{true} {
    }

    template<typename U = T>
    constexpr option(U&& value) noexcept(noexcept(std::is_nothrow_constructible_v<T, U>))
        : m_value{std::forward<U>(value)}
        , m_has_value{true} {
    }

    constexpr ~option() noexcept {
        if (m_has_value) {
            m_value.~T();
        }
    }

    constexpr auto operator->() const noexcept -> const T* {
        return &value();
    }

    constexpr auto operator->() noexcept -> T* {
        return &value();
    }

    constexpr auto operator*() const noexcept -> const T& {
        return value();
    }

    constexpr auto operator*() noexcept -> T& {
        return value();
    }

    constexpr auto value() const noexcept -> T& {
        return m_value;
    }

    constexpr auto value() noexcept -> T& {
        return m_value;
    }

    constexpr explicit operator bool() const noexcept  {
        return m_has_value;
    }

    [[nodiscard]] constexpr auto has_value() const noexcept -> bool {
        return m_has_value;
    }

    template<typename F> requires(requires(F function) {
        std::invocable<F, T&>;
    } && typing::is_specialisation_v<std::invoke_result_t<F, T&>, option>)
    constexpr auto and_then(F&& function) noexcept(noexcept(function(value()))) {
        if (m_has_value) {
            return std::invoke(std::forward<F>(function), value());
        }

        return std::remove_cvref_t<std::invoke_result_t<F, T&>>{};
    }

    template<typename F> requires(requires(F function) {
        std::invocable<F>;
        { function() } -> std::same_as<option<T>>;
    })
    constexpr auto or_else(F&& function)
        noexcept(noexcept(std::is_nothrow_copy_constructible_v<option<T>>) && noexcept(function()))
        -> option<T> {
        if (m_has_value) {
            return *this;
        }

        return std::forward<F>(function)();
    }

    template<typename U, typename F> requires(requires(F function, T value) {
        std::invocable<F, T>;
        { function(value) } -> std::constructible_from<option<U>, decltype(function())>;
    })
    constexpr auto map() -> option<U> {
        if (m_has_value) {
            return std::forward<F>()(value());
        }

        return nullopt;
    }

    constexpr auto unwrap() noexcept(s_is_nothrow_move_constructible) -> T requires(s_is_move_constructible) {
        auto value = std::move(m_value);
        m_has_value = false;
        m_value.~T();
        m_dummy = {};
        return value;
    }

    template<typename U>
    constexpr auto unwrap_or(U&& value)
        noexcept(s_is_nothrow_move_constructible && std::is_nothrow_constructible_v<T, U>)
        -> T requires(s_is_move_constructible && std::is_constructible_v<T, U>) {
        if (m_has_value) {
            return unwrap();
        }

        return T{std::forward<U>(value)};
    }

    template<typename F> requires(requires(F function) {
        std::invocable<F>;
        { function() } -> std::constructible_from<T, decltype(function())>;
    })
    constexpr auto unwrap_or_else(F&& function)
        noexcept(noexcept(function()) && std::is_nothrow_constructible_v<T, decltype(function())>) -> T {
        if (m_has_value) {
            return unwrap();
        }

        return std::forward<F>(function)();
    }

private:
    union {
        std::remove_cv_t<T> m_value;
        detail::dummy m_dummy;
    };

    bool m_has_value;
}; // class option

template<typename T>
option(T) -> option<T>;
} // namespace rtl::utilities

#endif // #ifndef RTL_OPTION_HPP
