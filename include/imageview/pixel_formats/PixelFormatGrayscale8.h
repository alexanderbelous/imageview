#pragma once

#include <gsl/span>

#include <cstddef>

namespace imageview {

// Implementation of the PixelFormat concept for the 8-bit grayscale pixel
// format.
class PixelFormatGrayscale8 {
 public:
  using color_type = unsigned char;
  static constexpr int kBytesPerPixel = 1;

  constexpr static color_type read(gsl::span<const std::byte, kBytesPerPixel> data);

  constexpr static void write(const color_type& color, gsl::span<std::byte, kBytesPerPixel> data);
};

constexpr PixelFormatGrayscale8::color_type PixelFormatGrayscale8::read(
    gsl::span<const std::byte, kBytesPerPixel> data) {
  return static_cast<color_type>(data[0]);
}

constexpr void PixelFormatGrayscale8::write(const color_type& color, gsl::span<std::byte, kBytesPerPixel> data) {
  data[0] = static_cast<std::byte>(color);
}

}  // namespace imageview
