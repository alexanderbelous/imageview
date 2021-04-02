#pragma once

#include <gsl/span>

#include <cstddef>

namespace imageview {

// Class representing a color in RGB24 color space.
class RGB24 {
 public:
  constexpr RGB24() noexcept = default;
  // Construct an RGB24 color from the given channel components.
  constexpr RGB24(unsigned char red, unsigned char green, unsigned char blue) noexcept
      : red(red), green(green), blue(blue) {}

  unsigned char red = 0;
  unsigned char green = 0;
  unsigned char blue = 0;
};

// Implementation of the PixelFormat concept for RGB24 pixel format.
// In this pixel format the color is represented via 3 8-bit integers,
// specifying the red, green and blue channels. When serializing to /
// deserializing from  a byte array, the order of the channels is RGB (i.e.
// not BGR).
class PixelFormatRGB24 {
 public:
  using color_type = RGB24;
  static constexpr int kBytesPerPixel = 3;

  constexpr color_type read(gsl::span<const std::byte, kBytesPerPixel> data) const;

  constexpr void write(const color_type& color, gsl::span<std::byte, kBytesPerPixel> data) const;
};

constexpr bool operator==(const RGB24& lhs, const RGB24& rhs) {
  return lhs.red == rhs.red && lhs.green == rhs.green && lhs.blue == rhs.blue;
}

constexpr bool operator!=(const RGB24& lhs, const RGB24& rhs) { return !(lhs == rhs); }

constexpr PixelFormatRGB24::color_type PixelFormatRGB24::read(gsl::span<const std::byte, kBytesPerPixel> data) const {
  return color_type(static_cast<unsigned char>(data[0]), static_cast<unsigned char>(data[1]),
                    static_cast<unsigned char>(data[2]));
}

constexpr void PixelFormatRGB24::write(const color_type& color, gsl::span<std::byte, kBytesPerPixel> data) const {
  data[0] = static_cast<std::byte>(color.red);
  data[1] = static_cast<std::byte>(color.green);
  data[2] = static_cast<std::byte>(color.blue);
}

}  // namespace imageview
