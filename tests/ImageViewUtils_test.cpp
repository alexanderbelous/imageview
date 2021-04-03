#include <imageview/ImageViewUtils.h>
#include <imageview/pixel_formats/PixelFormatRGB24.h>

#include <gtest/gtest.h>

#include <array>

namespace imageview {
namespace {

TEST(cropImageView, EmptyInputImage) {
  constexpr ImageView<PixelFormatRGB24> image;
  constexpr ImageView<PixelFormatRGB24> result = crop(image, 0, 0, 0, 0);
  static_assert(result.height() == 0, "Should be 0.");
  static_assert(result.width() == 0, "Should be 0.");
  static_assert(result.data().empty(), "Should be empty.");
}

TEST(cropImageView, EmptyOutputImage) {
  constexpr unsigned int kHeight = 3;
  constexpr unsigned int kWidth = 2;
  constexpr std::size_t kDataSize = kHeight * kWidth * PixelFormatRGB24::kBytesPerPixel;
  static constexpr std::array<std::byte, kDataSize> kData{};
  constexpr ImageView<PixelFormatRGB24> image(kHeight, kWidth, kWidth, kData);
  constexpr ImageView<PixelFormatRGB24> result = crop(image, 0, 0, 0, 0);
  static_assert(result.height() == 0, "Should be 0.");
  static_assert(result.width() == 0, "Should be 0.");
  static_assert(result.data().empty(), "Should be empty.");
}

TEST(cropImageView, CropContinuous) {
  //   0123         0123
  //  +----+       +----+
  // 0|....|      0|????|
  // 1|....|  ->  1|?..?|
  // 2|....|      2|?..?|
  // 3|....|      3|?..?|
  // 4|....|      4|????|
  constexpr unsigned int kHeight = 5;
  constexpr unsigned int kWidth = 4;
  constexpr unsigned int kStride = kWidth;
  constexpr std::size_t kDataSize = kHeight * kWidth * PixelFormatRGB24::kBytesPerPixel;
  static constexpr std::array<std::byte, kDataSize> kData{};
  constexpr ContinuousImageView<PixelFormatRGB24> image(kHeight, kWidth, kData);
  constexpr unsigned int kFirstRow = 1;
  constexpr unsigned int kFirstColumn = 1;
  constexpr unsigned int kNewHeight = 3;
  constexpr unsigned int kNewWidth = 2;
  constexpr ImageView<PixelFormatRGB24> result = crop(image, 1, 1, kNewHeight, kNewWidth);
  constexpr std::ptrdiff_t kExpectedOffset = (kFirstRow * kStride + kFirstColumn) * PixelFormatRGB24::kBytesPerPixel;
  constexpr std::size_t kExpectedSize = ((kNewHeight - 1) * kStride + kNewWidth) * PixelFormatRGB24::kBytesPerPixel;
  static_assert(result.height() == kNewHeight, "Should be 3.");
  static_assert(result.width() == kNewWidth, "Should be 2.");
  static_assert(result.stride() == image.width(), "Stride should be equal to the width of the original image.");
  static_assert(result.data().size() == kExpectedSize, "Should be 30 bytes.");
  static_assert(result.data().data() == kData.data() + kExpectedOffset, "data() should point to the element (1, 1) in the original image.");
}

TEST(cropImageView, CropStrided) {
  //   012345         012345
  //  +------+       +------+
  // 0|....??|      0|??????|
  // 1|....??|  ->  1|?..???|
  // 2|....??|      2|?..???|
  // 3|....??|      3|?..???|
  // 4|....??|      4|??????|
  constexpr unsigned int kHeight = 5;
  constexpr unsigned int kWidth = 4;
  constexpr unsigned int kStride = 6;
  constexpr std::size_t kDataSize = ((kHeight - 1) * kStride + kWidth) * PixelFormatRGB24::kBytesPerPixel;
  static constexpr std::array<std::byte, kDataSize> kData{};
  constexpr ImageView<PixelFormatRGB24> image(kHeight, kWidth, kStride, kData);
  constexpr unsigned int kFirstRow = 1;
  constexpr unsigned int kFirstColumn = 1;
  constexpr unsigned int kNewHeight = 3;
  constexpr unsigned int kNewWidth = 2;
  constexpr ImageView<PixelFormatRGB24> result = crop(image, 1, 1, kNewHeight, kNewWidth);
  constexpr std::ptrdiff_t kExpectedOffset = (kFirstRow * kStride + kFirstColumn) * PixelFormatRGB24::kBytesPerPixel;
  constexpr std::size_t kExpectedSize = ((kNewHeight - 1) * kStride + kNewWidth) * PixelFormatRGB24::kBytesPerPixel;
  static_assert(result.height() == kNewHeight, "Should be 3.");
  static_assert(result.width() == kNewWidth, "Should be 2.");
  static_assert(result.stride() == image.stride(), "Should have the same stride as the original image.");
  static_assert(result.data().size() == kExpectedSize, "Should be 42 bytes.");
  static_assert(result.data().data() == kData.data() + kExpectedOffset,
                "data() should point to the element (1, 1) in the original image.");
}

}
}
