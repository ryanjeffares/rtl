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

#include "assertions.hpp"
#include "typing/concepts.hpp"

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

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
    static constexpr bool s_is_copy_constructible = std::is_copy_constructible_v<T>;
    static constexpr bool s_is_nothrow_copy_constructible = std::is_nothrow_copy_constructible_v<T>;

public:
    constexpr option() noexcept
        : m_dummy{}
        , m_has_value{false} {
    }

    constexpr option(nullopt_t) noexcept
        : m_dummy{}
        , m_has_value{false} {
    }

    constexpr option(const option& other) noexcept(s_is_nothrow_copy_constructible) requires(s_is_copy_constructible)
        : m_value{other.value()}
        , m_has_value{true} {

    }

    constexpr option(option&& other) noexcept(s_is_nothrow_move_constructible) requires(s_is_move_constructible)
        : m_value{std::move(other.value())}
        , m_has_value{true} {

    }

    template<typename U> requires(std::is_constructible_v<T, const U&>)
    constexpr explicit(!std::is_convertible_v<const U&, T>) option(const option<U>& other)
        noexcept(std::is_nothrow_constructible_v<T, U>)
        : m_value{other.value()}
        , m_has_value{true} {

    }

    template<typename U> requires(std::is_constructible_v<T, U>)
    constexpr option(option<U>&& other)
        noexcept(std::is_nothrow_constructible_v<T, U>)
        : m_value{std::move(other.value())}
        , m_has_value{true} {

    }

    template<typename... Args>
    constexpr option(std::in_place_t, Args&&... args) noexcept(noexcept(std::is_nothrow_constructible_v<T, Args...>))
        : m_value{std::forward<Args>(args)...}
        , m_has_value{true} {
    }

    template<typename U = T> requires(!typing::is_specialisation_v<std::remove_cvref_t<U>, option>)
    constexpr option(U&& value) noexcept(noexcept(std::is_nothrow_constructible_v<T, U>))
        : m_value{std::forward<U>(value)}
        , m_has_value{true} {
    }

    constexpr ~option() noexcept {
        if (m_has_value) {
            m_value.~T();
        }
    }

    constexpr auto reset() noexcept -> void {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }
    }

    constexpr auto operator=(nullopt_t) noexcept -> option& {
        reset();
    }

    constexpr auto operator=(const option& other) noexcept(s_is_nothrow_copy_constructible)
        -> option& requires(s_is_copy_constructible) {
        if (this == &other) {
            return *this;
        }

        if (m_has_value) {
            if (other.has_value()) {
                m_value = other.m_value;
            } else {
                reset();
            }
        } else if (other.has_value()) {
            m_value = other.m_value;
            m_has_value = true;
        }

        return *this;
    }

    constexpr auto operator=(option&& other) noexcept(s_is_nothrow_move_constructible)
        -> option& requires(s_is_move_constructible) {
        if (this == &other) {
            return *this;
        }

        if (m_has_value) {
            if (other.has_value()) {
                m_value = std::move(other.m_value);
            } else {
                reset();
            }
        } else if (other.has_value()) {
            m_value = std::move(other.m_value);
            m_has_value = true;
        }

        return *this;
    }

    template<typename U = T>
    requires(!typing::is_specialisation_v<std::remove_cvref_t<U>, option>
        && std::is_constructible_v<T, U> && std::is_assignable_v<T&, U>)
    constexpr auto operator=(U&& value)
        noexcept(std::is_nothrow_constructible_v<T, U> && std::is_nothrow_assignable_v<T&, U>) -> option& {
        m_value = std::forward<U>(value);
        m_has_value = true;
        return *this;
    }

    template<typename U>
    requires(std::is_constructible_v<T, U> && std::is_assignable_v<T&, U>)
    constexpr auto operator=(const option<U>& other)
        noexcept(std::is_nothrow_constructible_v<T, U> && std::is_nothrow_assignable_v<T&, U>) -> option& {
        m_value = other.value();
        m_has_value = true;
        return *this;
    }

    template<typename U>
    requires(std::is_constructible_v<T, U> && std::is_assignable_v<T&, U>)
    constexpr auto operator=(option<U>&& other)
        noexcept(std::is_nothrow_constructible_v<T, U> && std::is_nothrow_assignable_v<T&, U>) -> option& {
        m_value = std::move(other.value());
        m_has_value = true;
        return *this;
    }

    constexpr auto operator->() const noexcept -> const T* {
        return std::addressof(value());
    }

    constexpr auto operator->() noexcept -> T* {
        return std::addressof(value());
    }

    constexpr auto operator*() const noexcept -> const T& {
        return value();
    }

    constexpr auto operator*() noexcept -> T& {
        return value();
    }

    constexpr auto value() const noexcept -> const T& {
        RTL_ASSERT(m_has_value, "Trying to access value in empty option");
        return m_value;
    }

    constexpr auto value() noexcept -> T& {
        RTL_ASSERT(m_has_value, "Trying to access value in empty option");
        return m_value;
    }

    template<typename U> requires(std::is_convertible_v<U&&, T>)
    constexpr auto value_or(U&& value) const noexcept(std::is_nothrow_convertible_v<U&&, T>) -> T {
        if (m_has_value) {
            return m_value;
        }

        return std::forward<U>(value);
    }

    constexpr explicit operator bool() const noexcept  {
        return m_has_value;
    }

    [[nodiscard]] constexpr auto has_value() const noexcept -> bool {
        return m_has_value;
    }

    template<typename F>
    requires(std::invocable<F> && std::is_same_v<std::remove_cvref_t<std::invoke_result_t<F>>, option<T>>)
    constexpr auto or_else(F&& function) const
        noexcept(noexcept(std::is_nothrow_copy_constructible_v<option<T>>)
                 && noexcept(std::invoke(std::forward<F>(function))))
        -> option<T> {
        if (m_has_value) {
            return *this;
        }

        return std::invoke(std::forward<F>(function));
    }

    template<typename F>
    requires(std::invocable<F, T&> && typing::is_specialisation_v<std::invoke_result_t<F, T&>, option>)
    constexpr auto and_then(F&& function) noexcept(noexcept(std::invoke(std::forward<F>(function), m_value))) {
        if (m_has_value) {
            return std::invoke(std::forward<F>(function), m_value);
        }

        return std::remove_cvref_t<std::invoke_result_t<F, T&>>{};
    }

    template<typename F>
    requires(std::invocable<F, T&> && typing::is_specialisation_v<std::invoke_result_t<F, T&>, option>)
    constexpr auto and_then(F&& function) const noexcept(noexcept(std::invoke(std::forward<F>(function), m_value))) {
        if (m_has_value) {
            return std::invoke(std::forward<F>(function), m_value);
        }

        return std::remove_cvref_t<std::invoke_result_t<F, T&>>{};
    }

    template<typename F> requires(std::invocable<F, T&>)
    constexpr auto map(F&& function) noexcept(noexcept(std::invoke(std::forward<F>(function), m_value))) {
        using U = std::remove_cvref_t<std::invoke_result_t<F, T&>>;

        if (m_has_value) {
            return option<U>{std::invoke(std::forward<F>(function), m_value)};
        }

        return option<U>{};
    }

    template<typename F> requires(std::invocable<F, const T&>)
    constexpr auto map(F&& function) const noexcept(noexcept(std::invoke(std::forward<F>(function), m_value))) {
        using U = std::remove_cvref_t<std::invoke_result_t<F, const T&>>;

        if (m_has_value) {
            return option<U>{std::invoke(std::forward<F>(function), m_value)};
        }

        return option<U>{};
    }

    constexpr auto unwrap() noexcept(s_is_nothrow_move_constructible) -> T requires(s_is_move_constructible) {
        RTL_ASSERT(m_has_value, "Trying to unwrap empty option");
        auto value = std::move(m_value);
        m_has_value = false;
        m_value.~T();
        m_dummy = {};
        return value;
    }

    template<typename U> requires(s_is_move_constructible && std::is_constructible_v<T, U>)
    constexpr auto unwrap_or(U&& value)
        noexcept(s_is_nothrow_move_constructible && std::is_nothrow_constructible_v<T, U>) -> T {
        if (m_has_value) {
            return unwrap();
        }

        return T{std::forward<U>(value)};
    }

    template<typename F>
    requires(std::invocable<F> && std::constructible_from<T, std::remove_cvref_t<std::invoke_result_t<F>>>)
    constexpr auto unwrap_or_else(F&& function)
        noexcept(s_is_nothrow_move_constructible
                 && noexcept(std::invoke(std::forward<F>(function)))
                 && std::is_nothrow_constructible_v<T, std::remove_cvref_t<std::invoke_result_t<F>>>)
        -> T {
        if (m_has_value) {
            return unwrap();
        }

        return std::invoke(std::forward<F>(function));
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
