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

#ifndef RTL_UNIQUE_PTR_HPP
#define RTL_UNIQUE_PTR_HPP

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

namespace rtl::memory {
namespace detail {
template<typename T>
class compressed_pair_element {
public:
    compressed_pair_element() noexcept(std::is_nothrow_default_constructible_v<T>)
        requires(std::is_default_constructible_v<T>) = default;

    compressed_pair_element(T&& value) noexcept(noexcept(std::is_nothrow_move_constructible_v<T>))
        : m_value{std::move(value)} {

    }

    auto get() noexcept -> T& {
        return m_value;
    }

private:
    T m_value;
};

template<typename T1, typename T2> requires(std::is_empty_v<T2>)
class compressed_pair : private compressed_pair_element<T1>, private compressed_pair_element<T2> {
public:
    compressed_pair()
        noexcept(std::is_nothrow_default_constructible_v<T1> && std::is_nothrow_default_constructible_v<T2>)
        requires(std::is_default_constructible_v<T1> && std::is_default_constructible_v<T2>) = default;

    compressed_pair(T1&& first, T2&& second)
        noexcept(std::is_nothrow_move_constructible_v<T1> && std::is_nothrow_move_constructible_v<T2>)
        : compressed_pair_element<T1>{std::move(first)}
        , compressed_pair_element<T2>{std::move(second)} {

    }

    auto first() noexcept -> T1& {
        return compressed_pair_element<T1>::get();
    }

    auto second() noexcept {
        return compressed_pair_element<T2>::get();
    }
};
} // namespace detail

template<typename T, typename Deleter = std::default_delete<T>> requires(requires(Deleter deleter, T* pointer) {
    { deleter(pointer) } -> std::same_as<void>;
} && std::is_default_constructible_v<Deleter> && std::is_empty_v<Deleter>)
class unique_ptr {
private:
    static constexpr bool s_deleter_nothrow_default_constructible = std::is_nothrow_default_constructible_v<Deleter>;
    static constexpr bool s_deleter_nothrow_move_constructible = std::is_nothrow_move_constructible_v<Deleter>;

public:
    constexpr unique_ptr() noexcept(s_deleter_nothrow_default_constructible) = default;

    constexpr unique_ptr(std::nullptr_t) noexcept(s_deleter_nothrow_default_constructible)
        : m_pair{nullptr, Deleter{}} {

    }

    constexpr unique_ptr(T* pointer) noexcept(s_deleter_nothrow_default_constructible) : m_pair{pointer, Deleter{}} {

    }

    constexpr unique_ptr(unique_ptr&& other) noexcept(s_deleter_nothrow_move_constructible)
        : m_pair{other.get(), std::move(other.get_deleter())} {
        other.m_pair.first() = nullptr;
    }

    constexpr unique_ptr(const unique_ptr&) = delete;

    constexpr ~unique_ptr() {
        reset();
    }

    constexpr auto get() const noexcept -> const T* {
        return m_pair.first();
    }

    constexpr auto get() noexcept -> T* {
        return m_pair.first();
    }

    constexpr auto get_deleter() noexcept -> Deleter& {
        return m_pair.second();
    }

    constexpr auto get_deleter() const noexcept -> const Deleter& {
        return m_pair.second();
    }

    constexpr auto release() noexcept -> T* {
        return std::exchange(m_pair.first(), nullptr);
    }

    constexpr auto reset() noexcept -> void {
        m_pair.second()(release());
    }

    constexpr auto swap(unique_ptr& other) noexcept -> void {
        std::swap(get(), other.get());
    }

private:
    detail::compressed_pair<T, Deleter> m_pair;
}; // class unique_ptr

template<typename T, typename... Args> requires(std::is_constructible_v<T, Args...>)
constexpr inline auto make_unique(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    -> unique_ptr<T> {
    return unique_ptr{new T{std::forward<Args>(args)...}};
}
} // namespace rtl::memory

#endif // #ifndef RTL_UNIQUE_PTR_HPP
