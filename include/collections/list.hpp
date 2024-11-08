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

#ifndef RTL_LIST_HPP
#define RTL_LIST_HPP

#include "typing/concepts.hpp"
#include "utilities/assertions.hpp"
#include "utilities/option.hpp"
#include "utilities/reference.hpp"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <format>
#include <iterator>
#include <memory>
#include <type_traits>

namespace rtl::collections {
template<typename T, typing::simple_allocator Allocator = std::allocator<T>>
class list {
private:
    static constexpr bool s_is_default_constructible = std::is_default_constructible_v<T>;
    static constexpr bool s_is_nothrow_default_constructible = std::is_nothrow_default_constructible_v<T>;
    static constexpr bool s_is_copy_constructible = std::is_copy_constructible_v<T>;
    static constexpr bool s_is_nothrow_copy_constructible = std::is_nothrow_copy_constructible_v<T>;
    static constexpr bool s_is_move_constructible = std::is_move_constructible_v<T>;
    static constexpr bool s_is_nothrow_move_constructible = std::is_nothrow_move_constructible_v<T>;

public:
    using allocator_type = Allocator;
    using allocator_traits = std::allocator_traits<allocator_type>;

    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using rvalue_reference = value_type&&;
    using pointer = std::allocator_traits<allocator_type>::pointer;
    using const_pointer = std::allocator_traits<allocator_type>::const_pointer;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using optional_ref = utilities::option<utilities::reference<T>>;
    using optional_const_ref = utilities::option<utilities::reference<const T>>;

    // construction

    constexpr list() noexcept(noexcept(allocator_type{})) = default;

    explicit constexpr list(const allocator_type& allocator) noexcept(noexcept(allocator_type{allocator}))
        : m_allocator{allocator} {

    }

    constexpr auto get_allocator() const noexcept(noexcept(allocator_type{m_allocator})) {
        return m_allocator;
    }

    // access
    constexpr auto operator[](size_type index) const noexcept -> optional_const_ref {
        return at(index);
    }

    constexpr auto operator[](size_type index) noexcept -> optional_ref {
        return at(index);
    }

    constexpr auto at(size_type index) const noexcept -> optional_const_ref {
        if (index < m_size) {
            return m_array[index];
        }

        return utilities::nullopt;
    }

    constexpr auto at(size_type index) noexcept -> optional_ref {
        if (index < m_size) {
            return m_array[index];
        }

        return utilities::nullopt;
    }

    constexpr auto at_unchecked(size_type index) const noexcept -> const T& {
        RTL_ASSERT(index < m_size,
                std::format("Index out of range: the index is {} but the size is {}", index, m_size));
        return m_array[index];
    }

    constexpr auto at_unchecked(size_type index) noexcept -> T& {
        RTL_ASSERT(index < m_size,
                std::format("Index out of range: the index is {} but the size is {}", index, m_size));
        return m_array[index];
    }

    constexpr auto front() const noexcept -> optional_const_ref {
        if (empty()) {
            return utilities::nullopt;
        }

        return at(0);
    }

    constexpr auto front() noexcept -> optional_ref {
        if (empty()) {
            return utilities::nullopt;
        }

        return at(0);
    }

    constexpr auto front_unchecked() const noexcept -> const T& {
        return at_unchecked(0);
    }

    constexpr auto front_unchecked() noexcept -> T& {
        return at_unchecked(0);
    }

    constexpr auto back() const noexcept -> optional_const_ref {
        if (empty()) {
            return utilities::nullopt;
        }

        return at(m_size - 1);
    }

    constexpr auto back() noexcept -> optional_ref {
        if (empty()) {
            return utilities::nullopt;
        }

        return at(m_size - 1);
    }

    constexpr auto back_unchecked() const noexcept -> const T& {
        return at_unchecked(m_size - 1);
    }

    constexpr auto back_unchecked() noexcept -> T& {
        return at_unchecked(m_size - 1);
    }

    // iterators
    template<typename Ty>
    class raw_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Ty;
        using pointer = value_type*;
        using reference = value_type&;
        using difference_type = difference_type;

        constexpr raw_iterator(pointer p) : m_pointer{p} {

        }

        constexpr auto operator*() const noexcept -> Ty& {
            return *m_pointer;
        }

        constexpr auto operator->() const noexcept -> Ty* {
            return m_pointer;
        }

        constexpr auto operator++() noexcept -> raw_iterator& {
            ++m_pointer;
            return *this;
        }

        constexpr auto operator++(int) noexcept -> raw_iterator {
            auto temp = *this;
            ++(*this);
            return temp;
        }

        constexpr auto operator--() noexcept -> raw_iterator& {
            --m_pointer;
            return *this;
        }

        constexpr auto operator--(int) noexcept -> raw_iterator {
            auto temp = *this;
            --(*this);
            return temp;
        }

        constexpr auto operator+=(difference_type n) noexcept -> raw_iterator& {
            m_pointer += n;
            return *this;
        }

        constexpr auto operator+(difference_type n) const noexcept -> raw_iterator {
            return m_pointer + n;
        }

        constexpr auto operator-=(difference_type n) noexcept -> raw_iterator& {
            m_pointer -= n;
            return *this;
        }

        constexpr auto operator-(difference_type n) const noexcept -> raw_iterator {
            return m_pointer - n;
        }

        constexpr auto operator-(const raw_iterator& other) const noexcept -> difference_type {
            return m_pointer - other.m_pointer;
        }

        constexpr auto operator[](difference_type n) const noexcept -> Ty& {
            return *(*this + n);
        }

        constexpr friend auto operator<(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return b - a > 0;
        }

        constexpr friend auto operator>(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return b < a;
        }

        constexpr friend auto operator<=(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return !(a < b);
        }

        constexpr friend auto operator>=(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return !(a > b);
        }

        constexpr friend auto operator==(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return a.m_pointer == b.m_pointer;
        }

        constexpr friend auto operator!=(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return a.m_pointer != b.m_pointer;
        }

    private:
        pointer m_pointer;
    }; // class raw_iterator

    using iterator = raw_iterator<value_type>;
    using const_iterator = raw_iterator<const value_type>;

    constexpr auto begin() noexcept -> iterator {
        if (empty()) {
            return end();
        }

        return m_array;
    }

    constexpr auto begin() const noexcept -> const_iterator {
        if (empty()) {
            return end();
        }

        return m_array;
    }

    constexpr auto cbegin() const noexcept -> const_iterator {
        if (empty()) {
            return cend();
        }

        return m_array;
    }

    constexpr auto end() noexcept -> iterator {
        return m_array + m_size;
    }

    constexpr auto end() const noexcept -> const_iterator {
        return m_array + m_size;
    }

    constexpr auto cend() const noexcept -> const_iterator {
        return m_array + m_size;
    }

    // capacity/size queries

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return m_size == 0;
    }

    [[nodiscard]] constexpr auto size() const noexcept -> size_type {
        return m_size;
    }

    [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
        return m_capacity;
    }

    constexpr auto reserve(size_type capacity) noexcept(s_is_nothrow_move_constructible)
        -> void requires(s_is_move_constructible) {
        if (capacity <= m_capacity) {
            return;
        }

        std::println("Reserving {}", capacity);
        auto array = m_allocator.allocate(capacity);

        for (size_type i = 0; i < m_size; i++) {
            auto& element = m_array[i];
            allocator_traits::construct(m_allocator, array + i, std::move(element));
            allocator_traits::destroy(m_allocator, m_array + i);
        }

        m_allocator.deallocate(m_array, m_capacity);
        m_array = array;
        m_capacity = capacity;
    }

    constexpr auto shrink_to_fit() noexcept(s_is_nothrow_move_constructible)
        -> void requires(s_is_move_constructible) {
        if (m_size == m_capacity) {
            return;
        }

        auto array = m_allocator.allocate(m_size);

        for (size_type i = 0; i < m_size; i++) {
            auto& element = m_array[i];
            allocator_traits::construct(m_allocator, array + i, std::move(element));
            allocator_traits::destroy(m_allocator, m_array + i);
        }

        m_allocator.deallocate(m_array, m_capacity);
        m_array = array;
        m_capacity = m_size;
    }

    // modification

    // clear, remove, insert, pop

    constexpr auto add(const T& value) noexcept(s_is_nothrow_copy_constructible && s_is_nothrow_move_constructible)
        -> void requires(s_is_copy_constructible && s_is_move_constructible) {
        grow_if_needed();
        allocator_traits::construct(m_allocator, m_array + m_size, value);
        m_size++;
    }

    constexpr auto add(T&& value) noexcept(s_is_nothrow_move_constructible)
        -> void requires(s_is_move_constructible) {
        grow_if_needed();
        allocator_traits::construct(m_allocator, m_array + m_size, std::move(value));
        m_size++;
    }

    template<typename... Args> requires(std::constructible_from<T, Args...>)
    constexpr auto add(Args&& ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) -> void {
        grow_if_needed();
        allocator_traits::construct(m_allocator, m_array + m_size, std::forward<Args>(args)...);
        m_size++;
    }

    constexpr auto insert(size_type index, const T& value)
        noexcept(s_is_nothrow_copy_constructible && s_is_nothrow_move_constructible)
        -> optional_ref requires(s_is_copy_constructible && s_is_move_constructible) {
        if (index >= m_size) {
            return utilities::nullopt;
        }

        grow_if_needed();

        for (size_type i = m_size; i > index; i--) {
            allocator_traits::construct(m_allocator, m_array + i, std::move(m_array[i - 1]));
        }
        
        allocator_traits::construct(m_allocator, m_array + index, value);
        m_size++;
        return at_unchecked(index);
    }

    constexpr auto insert(size_type index, T&& value) noexcept(s_is_nothrow_move_constructible)
        -> optional_ref requires(s_is_move_constructible) {
        if (index >= m_size) {
            return utilities::nullopt;
        }

        grow_if_needed();

        for (size_type i = m_size; i > index; i--) {
            allocator_traits::construct(m_allocator, m_array + i, std::move(m_array[i - 1]));
        }
        
        allocator_traits::construct(m_allocator, m_array + index, std::move(value));
        m_size++;
        return at_unchecked(index);
    }

    template<typename... Args> requires(std::constructible_from<T, Args...>)
    constexpr auto insert(size_type index, Args&&...  args)
        noexcept(s_is_nothrow_move_constructible && std::is_nothrow_constructible_v<T, Args...>)
        -> optional_ref requires(s_is_move_constructible) {
        if (index >= m_size) {
            return utilities::nullopt;
        }

        grow_if_needed();

        for (size_type i = m_size; i > index; i--) {
            allocator_traits::construct(m_allocator, m_array + i, std::move(m_array[i - 1]));
        }
        
        allocator_traits::construct(m_allocator, m_array + index, std::forward<Args>(args)...);
        m_size++;
        return at_unchecked(index);
    }

    constexpr auto pop() noexcept(s_is_nothrow_move_constructible) -> T requires(s_is_move_constructible) {
        auto value = std::move(back());
        resize(m_size - 1);
        return value;
    }

    constexpr auto resize(size_type size)
        noexcept(s_is_nothrow_default_constructible && s_is_nothrow_move_constructible)
        -> void requires(s_is_default_constructible) {
        if (size < m_size) {
            for (auto i = size; i < m_size; i++) {
                allocator_traits::destroy(m_allocator, m_array + i);
            }
        } else if (size > m_size) {
            grow_if_needed(size - m_size);
            for (auto i = m_size; i < size; i++) {
                allocator_traits::construct(m_allocator, m_array + i, T{});
            }
        }

        m_size = size;
    }

    constexpr auto resize(size_type size, const T& value) noexcept(s_is_nothrow_copy_constructible)
        -> void requires(s_is_copy_constructible) {
        if (size < m_size) {
            for (auto i = size; i < m_size; i++) {
                allocator_traits::destroy(m_allocator, m_array + i);
            }
        } else {
            grow_if_needed(size - m_size);
            for (auto i = m_size; i < size; i++) {
                allocator_traits::construct(m_allocator, m_array + i, value);
            }
        }

        m_size = size;
    }

    constexpr auto clear() noexcept -> void {
        for (size_type i = 0; i < m_size; i++) {
            allocator_traits::destroy(m_allocator, m_array + i);
        }

        m_size = 0;
    }

private:
    constexpr auto grow_if_needed(size_type increase = 1) noexcept(noexcept(reserve(0)))
        -> void requires(s_is_move_constructible) {
        if (empty() || m_size == m_capacity || m_capacity - m_size < increase) {
            reserve(std::max(m_capacity == 0 ? 1 : m_capacity * 2, m_size + increase));
        }
    }

    T* m_array{};
    size_type m_size{};
    size_type m_capacity{};
    allocator_type m_allocator{};
}; // class list
} // namespace rtl::collections

#endif // #ifndef RTL_LIST_HPP
