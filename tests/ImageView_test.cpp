#include <imageview/ImageView.h>
#include <imageview/pixel_formats/PixelFormatRGB24.h>

#include <gtest/gtest.h>

namespace imageview {
namespace {

TEST(ImageView, DefaultConstructor) {
  ImageView<PixelFormatRGB24> image;
  EXPECT_EQ(image.height(), 0);
  EXPECT_EQ(image.width(), 0);
  EXPECT_TRUE(image.data().empty());
}

}  // namespace
}  // namespace imageview