#include "mock_image_loaders.h"

#include <memory>

namespace imageview {
namespace examples {

int loadImageRGB24(const char* path, unsigned char** data, unsigned int* width, unsigned int* height) {
  if (!path || !data || !width || !height) {
    return -1;
  }
  const unsigned int kWidth = 40;
  const unsigned int kHeight = 20;
  *width = kWidth;
  *height = kHeight;
  *data = (unsigned char*)std::malloc(kWidth * kHeight * 3 * sizeof(unsigned char));
  if (*data == nullptr) {
    return -1;
  }
  unsigned char* pixel_ptr = *data;
  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      pixel_ptr[0] = static_cast<unsigned char>(y);
      pixel_ptr[1] = static_cast<unsigned char>(x);
      pixel_ptr[2] = 0;
      pixel_ptr += 3;
    }
  }
  return 0;
}

}  // namespace examples
}  // namespace imageview
