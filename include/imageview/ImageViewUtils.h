#pragma once

#include <imageview/ImageView.h>

#include <gsl/assert>

namespace imageview {

template <class PixelFormat, bool Mutable>
ImageView<PixelFormat, Mutable> crop(ImageView<PixelFormat, Mutable> image, unsigned int first_row,
                                     unsigned int first_column, unsigned int num_rows, unsigned int num_columns) {
  Expects(first_row <= image.height());
  Expects(first_column <= image.width());
  Expects(first_row + num_rows <= image.height());
  Expects(first_column + num_columns <= image.width());
  const std::size_t data_offset = (first_row * image.stride() + first_column) * PixelFormat::kBytesPerPixel;
  const std::size_t new_data_size =
      (num_rows == 0) ? 0 : ((num_rows - 1) * image.stride() + num_columns) * PixelFormat::kBytesPerPixel;
  const auto data_new = image.data().subspan(data_offset, new_data_size);
  return ImageView<PixelFormat>(num_rows, num_columns, data_new, image.stride());
}

}  // namespace imageview
