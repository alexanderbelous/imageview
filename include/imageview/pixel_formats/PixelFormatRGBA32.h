#pragma once

#include <gsl/span>

#include <cstddef>

namespace imageview {

class RGBA32 {
 public:
  constexpr RGBA32() noexcept = default;

  constexpr RGBA32(unsigned char red, unsigned char green, unsigned char blue,
                   unsigned char alpha) noexcept
      : red(red), green(green), blue(blue), alpha(alpha) {}

  unsigned char red = 0;
  unsigned char green = 0;
  unsigned char blue = 0;
  unsigned char alpha = 0;
};

class PixelFormatRGBA32 {
 public:
  using color_type = RGBA32;
  static constexpr int kBytesPerPixel = 4;

  constexpr static color_type read(
      gsl::span<const std::byte, kBytesPerPixel> data);

  constexpr static void write(const color_type& color,
                              gsl::span<std::byte, kBytesPerPixel> data);
};

constexpr PixelFormatRGBA32::color_type PixelFormatRGBA32::read(
    gsl::span<const std::byte, kBytesPerPixel> data) {
  return color_type(
      static_cast<unsigned char>(data[0]), static_cast<unsigned char>(data[1]),
      static_cast<unsigned char>(data[2]), static_cast<unsigned char>(data[3]));
}

constexpr void PixelFormatRGBA32::write(
    const color_type& color, gsl::span<std::byte, kBytesPerPixel> data) {
  data[0] = static_cast<std::byte>(color.red);
  data[1] = static_cast<std::byte>(color.green);
  data[2] = static_cast<std::byte>(color.blue);
  data[3] = static_cast<std::byte>(color.alpha);
}

}  // namespace imageview
