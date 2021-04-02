#include <imageview/ImageView.h>
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

TEST(ImageView, DefaultConstructor) {
  constexpr ImageView<PixelFormatRGB24> image;
  static_assert(image.height() == 0, "Default-constructed ImageView should have zero height.");
  static_assert(image.width() == 0, "Default-constructed ImageView should have zero width.");
  static_assert(image.data().empty(), "Default-constructed ImageView should have empty data.");
}

TEST(ImageView, RawParamsConstructor) {
  constexpr unsigned int kHeight = 3;
  constexpr unsigned int kWidth = 2;
  constexpr unsigned int kBytesPerPixel = PixelFormatRGB24::kBytesPerPixel;
  constexpr std::size_t kDataSize = kHeight * kWidth * kBytesPerPixel;
  static constexpr std::array<std::byte, kDataSize> bitmap{};
  constexpr ImageView<PixelFormatRGB24> image(kHeight, kWidth, kWidth, bitmap);
  static_assert(image.height() == kHeight, "Height should equal kHeight.");
  static_assert(image.width() == kWidth, "Width should equal kWidth.");
  static_assert(image.stride() == kWidth, "Stride should equal kWidth.");
  static_assert(image.data().size() == kDataSize, "data() should return a span of kDataSize bytes.");
  static_assert(image.data().data() == bitmap.data(), "data() should refer to the input bitmap.");
}

TEST(ImageView, ConstructReadOnlyFromMutable) {
  constexpr unsigned int kHeight = 3;
  constexpr unsigned int kWidth = 2;
  std::array<std::byte, kHeight * kWidth * PixelFormatRGB24::kBytesPerPixel> kData{};
  const ImageView<PixelFormatRGB24, true> image_mutable(kHeight, kWidth, kWidth, kData);
  const ImageView<PixelFormatRGB24> image_const(image_mutable);
  EXPECT_EQ(image_const.height(), image_mutable.height());
  EXPECT_EQ(image_const.width(), image_mutable.width());
  EXPECT_EQ(image_const.stride(), image_mutable.stride());
  EXPECT_EQ(image_const.data().size(), image_mutable.data().size());
  EXPECT_EQ(image_const.data().data(), image_mutable.data().data());
}

TEST(ImageView, ReadElement) {
  constexpr unsigned int kHeight = 2;
  constexpr unsigned int kWidth = 2;
  static_assert(PixelFormatRGB24::kBytesPerPixel == 3,
                "In this test we assume that PixelFormatRGB24 has 3 bytes per pixel.");
  constexpr std::size_t kDataSize = kHeight * kWidth * PixelFormatRGB24::kBytesPerPixel;
  static constexpr std::array<std::byte, kDataSize> data = makeByteArray(
      "\x40\x00\x00"   "\x00\x80\x00"
      "\x00\x00\xff"   "\x00\x00\x00");
  constexpr ImageView<PixelFormatRGB24> image(kHeight, kWidth, kWidth, data);
  static_assert(image(0, 0) == RGB24(0x40, 0, 0), "Should be {64, 0, 0}.");
  static_assert(image(0, 1) == RGB24(0, 0x80, 0), "Should be {0, 128, 0}.");
  static_assert(image(1, 0) == RGB24(0, 0, 0xFF), "Must be {0, 0, 255}.");
  static_assert(image(1, 1) == RGB24(0, 0, 0), "Must be {0, 0, 0}.");
}

}  // namespace
}  // namespace imageview
