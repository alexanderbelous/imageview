#pragma once

#include <cstddef>
#include <span>
#include <type_traits>

namespace imageview {
namespace detail {

template <class T, typename Enable = void>
class HasColorTypeTypedef : public std::false_type {};

template <class T>
class HasColorTypeTypedef<T, std::void_t<typename T::color_type>> : public std::true_type {};

template <class T, typename Enable = void>
class HasKBytesPerPixelConstant : public std::false_type {};

template <class T>
class HasKBytesPerPixelConstant<T, std::enable_if_t<std::is_integral_v<decltype(T::kBytesPerPixel)> &&
                                                    std::is_const_v<decltype(T::kBytesPerPixel)>>>
    : public std::true_type {};

template <class T, class Enable = void>
class HasRead : public std::false_type {};

template <class T>
class HasRead<T, std::enable_if_t<std::is_same_v<typename T::color_type,
                                                 decltype(std::declval<const T&>().read(
                                                     std::declval<std::span<const std::byte, T::kBytesPerPixel>>()))>>>
    : public std::true_type {};

template <class T, class Enable = void>
class HasWrite : public std::false_type {};

template <class T>
class HasWrite<T, std::enable_if_t<std::is_same_v<void, decltype(std::declval<const T&>().write(
                                                            std::declval<const typename T::color_type&>(),
                                                            std::declval<std::span<std::byte, T::kBytesPerPixel>>()))>>>
    : public std::true_type {};

}  // namespace detail

template <class T>
class IsPixelFormat : public std::conjunction<
                          // Has a member typedef 'color_type'.
                          detail::HasColorTypeTypedef<T>,
                          // Has an integral static member constant 'kBytesPerPixel'.
                          detail::HasKBytesPerPixelConstant<T>,
                          // Has a member function read() with the signature equivalent to
                          //   color_type read(std::span<const std::byte, kBytesPerPixel>) const;
                          detail::HasRead<T>,
                          // Has a member function write() with the signature equivalent to
                          //   void write(const color_type&, std::span<std::byte, kBytesPerPixel>) const;
                          detail::HasWrite<T>> {};

}  // namespace imageview
