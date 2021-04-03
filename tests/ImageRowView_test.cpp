#include <imageview/ImageRowView.h>
#include <imageview/pixel_formats/PixelFormatRGB24.h>

#include <gtest/gtest.h>

#include <array>

namespace imageview {
namespace {

template <std::size_t N>
constexpr std::array<std::byte, N - 1> makeByteArray(const char (&string_literal)[N]) {
  static_assert(N >= 1, "N must be greater than or equal to 1");
  std::array<std::byte, N - 1> result{};
  for (std::ptrdiff_t i = 0; i < N - 1; ++i) {
    result[i] = static_cast<std::byte>(string_literal[i]);
  }
  return result;
}

TEST(ImageRowView, Iterators) {
  static_assert(PixelFormatRGB24::kBytesPerPixel == 3,
                "In this test we assume that PixelFormatRGB24 has 3 bytes per pixel.");
  constexpr unsigned int kNumPixels = 3;
  constexpr std::size_t kDataSize = kNumPixels * PixelFormatRGB24::kBytesPerPixel;
  static constexpr std::array<std::byte, kDataSize> kData = makeByteArray(
      "\x40\x00\x00"
      "\x00\x80\x00"
      "\x00\x00\xff");

  constexpr ImageRowView<PixelFormatRGB24> image_row(kData, kNumPixels);
  constexpr auto first = image_row.begin();
  constexpr auto last = image_row.end();
  static_assert(first != last, "The range must not be empty.");
  static_assert(last - first == kNumPixels, "The range must have 3 elements.");
  static_assert(first[0] == RGB24(0x40, 0, 0), "The first element must be RGB24(64, 0, 0).");
  static_assert(first[1] == RGB24(0, 0x80, 0), "The second element must be RGB24(0, 128, 0).");
  static_assert(first[2] == RGB24(0, 0, 0xFF), "The third element must be RGB24(0, 0, 255).");

  // Check that range-based for loop compiles.
  for (const RGB24& color : image_row) {
    (void)color;
  }
}

}  // namespace
}  // namespace imageview
