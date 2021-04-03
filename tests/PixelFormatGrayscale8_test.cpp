#include <imageview/IsPixelFormat.h>
#include <imageview/pixel_formats/PixelFormatGrayscale8.h>

#include <gtest/gtest.h>

#include <array>

namespace imageview {
namespace {

static_assert(IsPixelFormat<PixelFormatGrayscale8>::value, "PixelFormatGrayscale8 must be a valid PixelFormat.");
static_assert(PixelFormatGrayscale8::kBytesPerPixel == 1, "Color depth of PixelFormatGrayscale8 must be 8 bpp.");

TEST(PixelFormatGrayscale8, Read) {
  static constexpr std::array<const std::byte, 1> kPixelData{std::byte{123}};
  constexpr PixelFormatGrayscale8 pixel_format;
  static_assert(pixel_format.read(kPixelData) == 123, "Must be 123.");
}

TEST(PixelFormatGrayscale8, Write) {
  static constexpr PixelFormatGrayscale8 pixel_format;
  static constexpr unsigned char color = 123;
  constexpr std::array<std::byte, 1> pixel_data = [] {
    std::array<std::byte, 1> pixel_data{};
    pixel_format.write(color, pixel_data);
    return pixel_data;
  }();
  static_assert(pixel_data[0] == std::byte{123}, "Must be 123.");
}

}  // namespace
}  // namespace imageview
