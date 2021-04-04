#include <imageview/internal/ImageViewIterator.h>
#include <imageview/pixel_formats/PixelFormatRGB24.h>

#include <gtest/gtest.h>

namespace imageview {
namespace {

// Sample flat image used in several tests.
constexpr std::size_t kSampleNumPixels = 6;
constexpr std::array<std::byte, kSampleNumPixels * PixelFormatRGB24::kBytesPerPixel> kSampleData{
    std::byte{0},  std::byte{1},  std::byte{2},
    std::byte{3},  std::byte{4},  std::byte{5},
    std::byte{6},  std::byte{7},  std::byte{8},
    std::byte{9},  std::byte{10}, std::byte{11},
    std::byte{12}, std::byte{13}, std::byte{14},
    std::byte{15}, std::byte{16}, std::byte{17}
};
using ConstIteratorRGB24 = detail::ImageViewIterator<PixelFormatRGB24, false>;
constexpr PixelFormatRGB24 kSamplePixelFormat;
constexpr ConstIteratorRGB24 kSampleImageBegin(kSampleData.data(), kSamplePixelFormat);
constexpr ConstIteratorRGB24 kSampleImageEnd(kSampleData.data() + kSampleData.size(), kSamplePixelFormat);

TEST(ImageViewIterator, Dereference) {
  static_assert(*kSampleImageBegin == RGB24(0, 1, 2), "Must be {0, 1, 2}.");
}

TEST(ImageViewIterator, PreIncrement) {
  ConstIteratorRGB24 iter = kSampleImageBegin;
  static_assert(std::is_same_v<decltype(++iter), ConstIteratorRGB24&>, "Must be ConstIteratorRGB24&");
  ConstIteratorRGB24& iter2 = ++iter;
  // iter2 must be a reference to iter.
  EXPECT_EQ(std::addressof(iter2), std::addressof(iter));
  // The resulting iterator must point to pixel[1].
  EXPECT_EQ(*iter, RGB24(3, 4, 5));
}

TEST(ImageViewIterator, PostIncrement) {
  ConstIteratorRGB24 iter = kSampleImageBegin;
  static_assert(std::is_same_v<decltype(iter++), ConstIteratorRGB24>, "Must be ConstIteratorRGB24");
  ConstIteratorRGB24 iter2 = iter++;
  // iter2 must point to pixel[0].
  EXPECT_EQ(*iter, RGB24(0, 1, 2));
  // iter must point to pixel[1].
  EXPECT_EQ(*iter, RGB24(3, 4, 5));
}

TEST(ImageViewIterator, NonConst) {
  using IteratorRGB24 = detail::ImageViewIterator<PixelFormatRGB24, true>;

  constexpr std::size_t kNumPixels = 6;
  constexpr PixelFormatRGB24 kPixelFormat;
  std::array<std::byte, kNumPixels * PixelFormatRGB24::kBytesPerPixel> data{};
  IteratorRGB24 iter(data.data(), kPixelFormat);
  *iter = RGB24{13, 181, 254};
  EXPECT_EQ(data[0], std::byte{13});
  EXPECT_EQ(data[181], std::byte{181});
  EXPECT_EQ(data[254], std::byte{254});
}

}  // namespace
}  // namespace imageview
