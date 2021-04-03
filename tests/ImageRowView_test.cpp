#include <imageview/ImageRowView.h>
#include <imageview/pixel_formats/PixelFormatRGB24.h>

#include <gtest/gtest.h>

#include <array>

namespace imageview {
namespace {

TEST(ImageRowView, Iterators) {
  constexpr unsigned int kNumPixels = 3;
  constexpr std::size_t kDataSize = kNumPixels * PixelFormatRGB24::kBytesPerPixel;
  static constexpr std::array<std::byte, kDataSize> kData{};
  constexpr ImageRowView<PixelFormatRGB24> image_row(kData, kNumPixels);
  static_assert(image_row.begin() == detail::ImageViewIterator(kData.data(), image_row.pixelFormat()));
  static_assert(image_row.end() == detail::ImageViewIterator(kData.data() + kDataSize, image_row.pixelFormat()));
  // Check that range-based for loop compiles.
  for (const RGB24& color : image_row) {
    (void)color;
  }
}

}  // namespace
}  // namespace imageview
