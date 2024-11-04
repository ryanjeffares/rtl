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

#ifndef RTL_CONCEPTS_HPP
#define RTL_CONCEPTS_HPP

#include <concepts>
#include <cstddef>
#include <type_traits>

namespace rtl::typing
{
#if __cplusplus <= 202302L
template<typename Allocator>
concept simple_allocator = requires(Allocator allocator, std::size_t n) {
    { *allocator.allocate(n) } -> std::same_as<typename Allocator::value_type&>;
    { allocator.deallocate(allocator.allocate(n), n) };
} && std::copy_constructible<Allocator> && std::equality_comparable<Allocator>;
#else // #if __cplusplus <= 202302L
// TODO: use C++26 allocator concept
#endif // #if __cplusplus <= 202302L

template<typename T, template<typename...> typename Template>
inline constexpr bool is_specialisation_v = false;

template<template<typename...> typename Template, typename... Ts>
inline constexpr bool is_specialisation_v<Template<Ts...>, Template> = true;
} // namespace rtl::typing

#endif // #ifndef RTL_CONCEPTS_HPP
