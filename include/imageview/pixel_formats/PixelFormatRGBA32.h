#pragma once

#include <cstddef>
#include <span>

namespace imageview {

// Class representing a color in RGBA32 color space.
class RGBA32 {
 public:
  constexpr RGBA32() noexcept = default;

  constexpr RGBA32(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) noexcept
      : red(red), green(green), blue(blue), alpha(alpha) {}

  unsigned char red = 0;
  unsigned char green = 0;
  unsigned char blue = 0;
  unsigned char alpha = 0;
};

// Implementation of the PixelFormat concept for RGBA32 pixel format.
// In this pixel format the color is represented via 4 8-bit integers,
// specifying the red, green, blue and alpha channels. When serializing to /
// deserializing from  a byte array, the order of the channels is RGBA (i.e.
// not BGRA).
class PixelFormatRGBA32 {
 public:
  using color_type = RGBA32;
  static constexpr int kBytesPerPixel = 4;

  constexpr color_type read(std::span<const std::byte, kBytesPerPixel> data) const;

  constexpr void write(const color_type& color, std::span<std::byte, kBytesPerPixel> data) const;
};

constexpr bool operator==(const RGBA32& lhs, const RGBA32& rhs) {
  return lhs.red == rhs.red && lhs.green == rhs.green && lhs.blue == rhs.blue && lhs.alpha == rhs.alpha;
}

constexpr bool operator!=(const RGBA32& lhs, const RGBA32& rhs) { return !(lhs == rhs); }

constexpr PixelFormatRGBA32::color_type PixelFormatRGBA32::read(std::span<const std::byte, kBytesPerPixel> data) const {
  return color_type(static_cast<unsigned char>(data[0]), static_cast<unsigned char>(data[1]),
                    static_cast<unsigned char>(data[2]), static_cast<unsigned char>(data[3]));
}

constexpr void PixelFormatRGBA32::write(const color_type& color, std::span<std::byte, kBytesPerPixel> data) const {
  data[0] = static_cast<std::byte>(color.red);
  data[1] = static_cast<std::byte>(color.green);
  data[2] = static_cast<std::byte>(color.blue);
  data[3] = static_cast<std::byte>(color.alpha);
}

}  // namespace imageview
