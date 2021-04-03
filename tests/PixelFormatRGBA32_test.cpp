#include <imageview/IsPixelFormat.h>
#include <imageview/pixel_formats/PixelFormatRGBA32.h>

#include <gtest/gtest.h>

#include <array>

namespace imageview {
namespace {

static_assert(IsPixelFormat<PixelFormatRGBA32>::value, "PixelFormatRGBA32 must be a valid PixelFormat.");
static_assert(PixelFormatRGBA32::kBytesPerPixel == 4, "Color depth of PixelFormatRGBA32 must be 32 bpp.");

TEST(PixelFormatRGBA32, Read) {
  static constexpr std::array<const std::byte, 4> kPixelData{std::byte{123}, std::byte{215}, std::byte{7},
                                                             std::byte{244}};
  constexpr PixelFormatRGBA32 pixel_format;
  static_assert(pixel_format.read(kPixelData) == RGBA32(123, 215, 7, 244), "Must be {123, 215, 7, 244}.");
}

TEST(PixelFormatRGBA32, Write) {
  static constexpr PixelFormatRGBA32 pixel_format;
  static constexpr RGBA32 color(123, 215, 7, 244);
  constexpr std::array<std::byte, 4> pixel_data = [] {
    std::array<std::byte, 4> pixel_data{};
    pixel_format.write(color, pixel_data);
    return pixel_data;
  }();
  static_assert(pixel_data[0] == std::byte{123}, "Must be 123.");
  static_assert(pixel_data[1] == std::byte{215}, "Must be 215.");
  static_assert(pixel_data[2] == std::byte{7}, "Must be 7.");
  static_assert(pixel_data[3] == std::byte{244}, "Must be 244.");
}

}  // namespace
}  // namespace imageview
