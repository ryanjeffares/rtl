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

#ifndef RTL_REFERENCE_HPP
#define RTL_REFERENCE_HPP

#include <memory>
#include <type_traits>

namespace rtl::utilities {
template<typename T>
class reference {
public:
    constexpr reference(T& value) noexcept
        : m_ptr{std::addressof(value)} {

    }

    constexpr reference(const reference&) noexcept = default;
    constexpr auto operator=(const reference&) noexcept -> reference& = default;

    constexpr explicit operator T&() const noexcept {
        return *m_ptr;
    }

    constexpr auto get() const noexcept -> T& {
        return *m_ptr;
    }

    constexpr auto operator==(reference other) const -> bool {
        return get() == other.get();
    }

    constexpr auto operator==(reference<const T> other) const -> bool requires(!std::is_const_v<T>) {
        return get() == other.get();
    }

    constexpr auto operator==(const T& other) const -> bool {
        return get() == other;
    }

    template<typename... Args> requires(std::invocable<T, Args...>)
    constexpr auto operator()(Args&&... args)
        const noexcept(noexcept(std::invoke(get(), std::forward<Args>(args)...)))
        -> std::invoke_result_t<T, Args...> {
        return std::invoke(get(), std::forward<Args>(args)...);
    }

private:
    T* m_ptr;
};
}// namespace rtl::utilities

#endif// #ifndef RTL_REFERENCE_HPP
