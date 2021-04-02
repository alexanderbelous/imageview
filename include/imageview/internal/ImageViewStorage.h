#pragma once

#include <cstddef>
#include <type_traits>

namespace imageview {
namespace detail {

// Class that aggregates PixelFormat and a pointer to the bitmap data.
template <class PixelFormat, bool Mutable, class Enable = void>
class ImageViewStorage {
 public:
  using byte_type = std::conditional_t<Mutable, std::byte, const std::byte>;

  template <class Enable = std::enable_if_t<std::is_default_constructible_v<PixelFormat>>>
  constexpr ImageViewStorage() noexcept(noexcept(std::is_nothrow_default_constructible_v<PixelFormat>)) {}

  template <class Enable = std::enable_if_t<std::is_default_constructible_v<PixelFormat>>>
  constexpr ImageViewStorage(byte_type* data) noexcept(noexcept(std::is_nothrow_default_constructible_v<PixelFormat>))
      : data_(data) {}

  constexpr ImageViewStorage(byte_type* data, const PixelFormat& pixel_format) noexcept(
      noexcept(std::is_nothrow_copy_constructible_v<PixelFormat>))
      : pixel_format_(pixel_format), data_(data) {}

  constexpr ImageViewStorage(byte_type* data, PixelFormat&& pixel_format) noexcept(
      noexcept(std::is_nothrow_move_constructible_v<PixelFormat>))
      : pixel_format_(std::move(pixel_format)), data_(data) {}

  constexpr const PixelFormat& pixelFormat() const noexcept { return pixel_format_; }

  constexpr byte_type* data() const noexcept { return data_; }

 private:
  byte_type* data_ = nullptr;
  PixelFormat pixel_format_;
};

// Specialization for PixelFormat types that are eligible for empty base optimization.
template <class PixelFormat, bool Mutable>
class ImageViewStorage<PixelFormat, Mutable,
                       std::enable_if_t<std::is_empty_v<PixelFormat> && !std::is_final_v<PixelFormat>>>
    : private PixelFormat {
 public:
  using byte_type = std::conditional_t<Mutable, std::byte, const std::byte>;

  template <class Enable = std::enable_if_t<std::is_default_constructible_v<PixelFormat>>>
  constexpr ImageViewStorage() noexcept(noexcept(std::is_nothrow_default_constructible_v<PixelFormat>)) {}

  template <class Enable = std::enable_if_t<std::is_default_constructible_v<PixelFormat>>>
  constexpr ImageViewStorage(byte_type* data) noexcept(noexcept(std::is_nothrow_default_constructible_v<PixelFormat>))
      : data_(data) {}

  constexpr ImageViewStorage(byte_type* data, const PixelFormat& pixel_format) noexcept(
      noexcept(std::is_nothrow_copy_constructible_v<PixelFormat>))
      : PixelFormat(pixel_format), data_(data) {}

  constexpr ImageViewStorage(byte_type* data, PixelFormat&& pixel_format) noexcept(
      noexcept(std::is_nothrow_move_constructible_v<PixelFormat>))
      : PixelFormat(std::move(pixel_format)), data_(data) {}

  constexpr const PixelFormat& pixelFormat() const noexcept { return *this; }

  constexpr byte_type* data() const noexcept { return data_; }

 private:
  byte_type* data_ = nullptr;
};

}  // namespace detail
}  // namespace imageview
