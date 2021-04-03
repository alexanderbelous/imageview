#include <imageview/pixel_formats/PixelFormatRGB24.h>
#include <imageview/IsPixelFormat.h>

#include <gtest/gtest.h>

#include <array>

namespace imageview {
namespace {

static_assert(IsPixelFormat<PixelFormatRGB24>::value, "PixelFormatRGB24 must be a valid PixelFormat.");
static_assert(PixelFormatRGB24::kBytesPerPixel == 3, "Color depth of PixelFormatRGB24 must be 24 bpp.");

TEST(PixelFormatRGB24, Read) {
  static constexpr std::array<const std::byte, 3> kPixelData{std::byte{123}, std::byte{215}, std::byte{7}};
  constexpr PixelFormatRGB24 pixel_format;
  static_assert(pixel_format.read(kPixelData) == RGB24(123, 215, 7), "Must be {123, 215, 7}.");
}

TEST(PixelFormatRGB24, Write) {
  static constexpr PixelFormatRGB24 pixel_format;
  static constexpr RGB24 color(123, 215, 7);
  constexpr std::array<std::byte, 3> pixel_data = [] {
    std::array<std::byte, 3> pixel_data{};
    pixel_format.write(color, pixel_data);
    return pixel_data;
  }();
  static_assert(pixel_data[0] == std::byte{123}, "Must be 123.");
  static_assert(pixel_data[1] == std::byte{215}, "Must be 215.");
  static_assert(pixel_data[2] == std::byte{7}, "Must be 7.");
}

}  // namespace
}  // namespace imageview
