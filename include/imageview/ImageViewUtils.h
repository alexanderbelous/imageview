#pragma once

#include <imageview/ContinuousImageView.h>
#include <imageview/ImageView.h>

#include <stdexcept>

namespace imageview {

template <class PixelFormat, bool Mutable>
constexpr ImageView<PixelFormat, Mutable> crop(ImageView<PixelFormat, Mutable> image, unsigned int first_row,
                                               unsigned int first_column, unsigned int num_rows,
                                               unsigned int num_columns) {
  if (first_row > image.height())
  {
    throw std::invalid_argument("imageview::crop(): first_row must be less than or equal to image.height().");
  }
  if (first_column > image.width())
  {
    throw std::invalid_argument("imageview::crop(): first_column must be less than or equal to image.width().");
  }
  if (first_row + num_rows > image.height())
  {
    throw std::invalid_argument("imageview::crop(): first_row + num_rows "
                                "must be less than or equal to image.height().");
  }
  if (first_column + num_columns > image.width())
  {
    throw std::invalid_argument("imageview::crop(): first_column + num_columns "
                                "must be less than or equal to image.width().");
  }
  const std::size_t data_offset = (first_row * image.stride() + first_column) * PixelFormat::kBytesPerPixel;
  const std::size_t new_data_size =
      (num_rows == 0) ? 0 : ((num_rows - 1) * image.stride() + num_columns) * PixelFormat::kBytesPerPixel;
  const auto data_new = image.data().subspan(data_offset, new_data_size);
  return ImageView<PixelFormat>(num_rows, num_columns, image.stride(), data_new, image.pixelFormat());
}

template <class PixelFormat, bool Mutable>
constexpr ImageView<PixelFormat, Mutable> crop(ContinuousImageView<PixelFormat, Mutable> image, unsigned int first_row,
                                               unsigned int first_column, unsigned int num_rows,
                                               unsigned int num_columns) {
  return crop(ImageView<PixelFormat, Mutable>(image), first_column, first_column, num_rows, num_columns);
}

}  // namespace imageview
