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
#include <iterator>
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

template<typename T>
concept is_not = !std::is_same_v<T, void>;

template<typename T>
concept referencable = is_not<T>;

template<typename It>
concept legacy_iterator = requires(It it) {
    { *it } -> referencable;
    { ++it } -> std::same_as<It&>;
    { *it++ } -> referencable;
} && std::copyable<It>;

template<typename It>
concept legacy_input_iterator = requires(It it) {
    typename std::incrementable_traits<It>::difference_type;
    typename std::indirectly_readable_traits<It>::value_type;
    typename std::common_reference_t<std::iter_reference_t<It>&&,
        typename std::indirectly_readable_traits<It>::value_type&>;
    *it++;
    typename std::common_reference_t<decltype(*it++)&&, typename std::indirectly_readable_traits<It>::value_type&>;
    requires std::signed_integral<typename std::incrementable_traits<It>::difference_type>;
} && std::equality_comparable<It> && legacy_iterator<It>;

template<typename It>
concept legacy_forward_iterator = requires(It it) {
    { it++ } -> std::convertible_to<const It&>;
    { *it++ } -> std::same_as<std::iter_reference_t<It>>;
} && legacy_input_iterator<It> && std::constructible_from<It> && std::is_reference_v<std::iter_reference_t<It>>
    && std::same_as<std::remove_cvref_t<std::iter_reference_t<It>>,
        typename std::indirectly_readable_traits<It>::value_type>;

template<typename It>
concept legacy_bidirectional_iterator = requires(It it) {
    { --it } -> std::same_as<It&>;
    { it-- } -> std::convertible_to<const It&>;
    { *it-- } -> std::same_as<std::iter_reference_t<It>>;
} && legacy_forward_iterator<It>;

template<typename It>
concept legacy_random_access_iterator = requires(It it, typename std::incrementable_traits<It>::difference_type n) {
    { it += n } -> std::same_as<It&>;
    { it -= n } -> std::same_as<It&>;
    { it + n } -> std::same_as<It>;
    { n + it } -> std::same_as<It>;
    { it - n } -> std::same_as<It>;
    { it - it } -> std::same_as<decltype(n)>;
    { it[n] } -> std::convertible_to<std::iter_reference_t<It>>;
} && legacy_bidirectional_iterator<It> && std::totally_ordered<It>;
} // namespace rtl::typing

#endif // #ifndef RTL_CONCEPTS_HPP
