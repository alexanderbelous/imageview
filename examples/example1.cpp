#include "image_processing.h"
#include "mock_image_loaders.h"

#include <imageview/ImageViewUtils.h>

#include <memory>
#include <utility>

namespace {

using ::imageview::ContinuousImageView;
using ::imageview::crop;
using ::imageview::ImageView;
using ::imageview::PixelFormatRGB24;
using ::imageview::RGB24;
using ::imageview::examples::computeMeanColor;
using ::imageview::examples::loadImageRGB24;
using ::imageview::examples::replaceColor;

class CStyleDeleter {
 public:
  void operator()(void* pointer) const { std::free(pointer); }
};

}  // namespace

int main() {
  // Load the image via C-style API.
  unsigned int width = 0;
  unsigned int height = 0;
  unsigned char* data = nullptr;
  if (loadImageRGB24("image.jpg", &data, &width, &height)) {
    return -1;
  }
  // You are using smart pointers, right?
  std::unique_ptr<unsigned char, CStyleDeleter> data_uptr(std::exchange(data, nullptr));
  // loadImageRGB24() guarantees that the size of the returned data is width * height * 3.
  const std::size_t data_size = width * height * 3;

  // Construct an immutable ContinuousImageView into the loaded image.
  ContinuousImageView<PixelFormatRGB24, true> image(height, width,
                                                    gsl::as_writable_bytes(gsl::span{data_uptr.get(), data_size}));
  // Replace green pixels with black.
  replaceColor(image, RGB24(0, 255, 0), RGB24(0, 0, 0));
  // Select a rectangular area within the image.
  const ImageView<PixelFormatRGB24> subimage = crop(image, 10, 10, 50, 50);
  // Compute the mean color within the selected area.
  const RGB24 mean_color = computeMeanColor(subimage);

  return 0;
}