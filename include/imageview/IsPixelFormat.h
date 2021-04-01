#pragma once

#include <gsl/span>

#include <cstddef>
#include <type_traits>

namespace imageview {
namespace detail {

template <class T, typename Enable = void>
class HasColorTypeTypedef : public std::false_type {};

template <class T>
class HasColorTypeTypedef<T, std::void_t<typename T::color_type>>
    : public std::true_type {};

template <class T, typename Enable = void>
class HasKBytesPerPixelConstant : public std::false_type {};

template <class T>
class HasKBytesPerPixelConstant<
    T, std::enable_if_t<std::is_integral_v<decltype(T::kBytesPerPixel)> &&
                        std::is_const_v<decltype(T::kBytesPerPixel)>>>
    : public std::true_type {};

template <class T, class Enable = void>
class HasRead : public std::false_type {};

template <class T>
class HasRead<
    T,
    std::enable_if_t<std::is_same_v<
        typename T::color_type,
        decltype(T::read(
            std::declval<gsl::span<const std::byte, T::kBytesPerPixel>>()))>>>
    : public std::true_type {};

template <class T, class Enable = void>
class HasWrite : public std::false_type {};

template <class T>
class HasWrite<
    T,
    std::enable_if_t<std::is_same_v<
        void, decltype(T::write(
                  std::declval<const typename T::color_type&>(),
                  std::declval<gsl::span<std::byte, T::kBytesPerPixel>>()))>>>
    : public std::true_type {};

}  // namespace detail

template <class T>
class IsPixelFormat
    : public std::conjunction<
          // Has a member typedef 'color_type'.
          detail::HasColorTypeTypedef<T>,
          // Has an integral static member constant 'kBytesPerPixel'.
          detail::HasKBytesPerPixelConstant<T>,
          // Has a static member function read() with the signature equivalent to
          //   color_type read(gsl::span<const std::byte, kBytesPerPixel>);
          detail::HasRead<T>,
          // Has a static member function write() with the signature equivalent to
          //   void write(const color_type&, gsl::span<std::byte, kBytesPerPixel>);
          detail::HasWrite<T>> {};

}  // namespace imageview
