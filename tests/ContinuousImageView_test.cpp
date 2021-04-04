#include <imageview/ContinuousImageView.h>
#include <imageview/pixel_formats/PixelFormatRGB24.h>

#include <gtest/gtest.h>

namespace imageview {
namespace {

TEST(ContinuousImageView, DefaultConstructor) {
  static_assert(std::is_nothrow_default_constructible_v<ContinuousImageView<PixelFormatRGB24>>,
                "ContinuousImageView<PixelFormatRGB24> must have a noexcept default constructor.");
  constexpr ContinuousImageView<PixelFormatRGB24> image;
  static_assert(image.height() == 0, "Height of the default-constructed ContinuousImageView must be 0.");
  static_assert(image.width() == 0, "Width of the default-constructed ContinuousImageView must be 0.");
  static_assert(image.data().empty(), "Data of the default-constructed ContinuousImageView must be empty.");
}

TEST(ContinuousImageView, ConstructWithoutPixelFormat) {
  constexpr unsigned int kHeight = 3;
  constexpr unsigned int kWidth = 2;
  constexpr std::size_t kDataSize = kHeight * kWidth * PixelFormatRGB24::kBytesPerPixel;
  static constexpr std::array<std::byte, kDataSize> data{};
  static_assert(noexcept(ContinuousImageView<PixelFormatRGB24>(kHeight, kWidth, data)),
                "This constructor must be noexcept.");
  constexpr ContinuousImageView<PixelFormatRGB24> image(kHeight, kWidth, data);
  static_assert(image.height() == kHeight, "Invalid height.");
  static_assert(image.width() == kWidth, "Invalid width.");
  static_assert(image.data().size() == data.size(), "Invalid size of the bitmap.");
  static_assert(image.data().data() == data.data(), "Invalid pointer to th the bitmap.");
}

TEST(ContinuousImageView, SetPixel) {
  constexpr RGB24 kBlack(0, 0, 0);
  constexpr RGB24 kGreen(0, 255, 0);
  constexpr unsigned int kHeight = 3;
  constexpr unsigned int kWidth = 2;
  constexpr std::size_t kDataSize = kHeight * kWidth * PixelFormatRGB24::kBytesPerPixel;
  std::array<std::byte, kDataSize> data{};
  ContinuousImageView<PixelFormatRGB24, true> image(kHeight, kWidth, data);
  // Pixel (1, 1) must be black because data is zero-initialized.
  EXPECT_EQ(image(1, 1), kBlack);
  // Set pixel (1, 1) to green.
  image(1, 1) = kGreen;
  EXPECT_EQ(image(1, 1), kGreen);
}

}  // namespace
}  // namespace imageview
