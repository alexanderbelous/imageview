#pragma once

#include <cstddef>
#include <span>

namespace imageview {

// Implementation of the PixelFormat concept for the 8-bit grayscale pixel
// format.
class PixelFormatGrayscale8 {
 public:
  using color_type = unsigned char;
  static constexpr int kBytesPerPixel = 1;

  constexpr color_type read(std::span<const std::byte, kBytesPerPixel> data) const;

  constexpr void write(const color_type& color, std::span<std::byte, kBytesPerPixel> data) const;
};

constexpr PixelFormatGrayscale8::color_type PixelFormatGrayscale8::read(
  std::span<const std::byte, kBytesPerPixel> data) const {
  return static_cast<color_type>(data[0]);
}

constexpr void PixelFormatGrayscale8::write(const color_type& color, std::span<std::byte, kBytesPerPixel> data) const {
  data[0] = static_cast<std::byte>(color);
}

}  // namespace imageview
