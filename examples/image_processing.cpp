#include "image_processing.h"

namespace imageview {
namespace examples {

RGB24 computeMeanColor(ImageView<PixelFormatRGB24> image) {
  if (image.empty()) {
    return RGB24(0, 0, 0);
  }
  unsigned int sum_red = 0;
  unsigned int sum_green = 0;
  unsigned int sum_blue = 0;
  // ImageView is not necessarily continuous, so we cannot iterate over it in one loop
  // (well, technically we could, but it would be ugly and likely less efficient).
  for (int y = 0; y < image.height(); ++y) {
    for (const RGB24& color : image.row(y)) {
      sum_red += color.red;
      sum_green += color.green;
      sum_blue += color.blue;
    }
  }
  return RGB24(static_cast<unsigned char>(sum_red / image.area()),
               static_cast<unsigned char>(sum_green / image.area()),
               static_cast<unsigned char>(sum_blue / image.area()));
}

void replaceColor(ContinuousImageView<PixelFormatRGB24, true> image,
                  const RGB24& color_original, const RGB24& color_new) {
  const auto flat_image = flatten(image);
  for (int i = 0; i < flat_image.size(); ++i) {
    if (flat_image[i] == color_original) {
      flat_image.setElement(i, color_new);
    }
  }
}

}  // namespace examples
}  // namespace imageview
