#pragma once

#include <imageview/ContinuousImageView.h>

#include <gsl/span>

#include <cstddef>

namespace imageview {

template <class PixelFormat, bool Mutable = false>
class ImageView {
 public:
  static_assert(IsPixelFormat<PixelFormat>::value, "Not a PixelFormat.");

  using byte_type =
      std::conditional_t<Mutable, std::byte, const std::byte>;
  using color_type = typename PixelFormat::color_type;

  // Construct an empty view.
  constexpr ImageView() = default;
  // Construct a view into an image.
  // \param height - height of the image.
  // \param width - width of the image.
  // \param data - bitmap data.
  // \param stride -
  constexpr ImageView(unsigned int height, unsigned int width,
                      gsl::span<byte_type> data, unsigned int stride);
  // Construct a read-only view from a mutable view.
  template <class Enable = std::enable_if_t<!Mutable>>
  constexpr ImageView(ImageView<PixelFormat, !Mutable> other);

  constexpr ImageView(ContinuousImageView<PixelFormat, Mutable> image);
  // Construct a read-only view from a mutable contiguous view.
  template<class Enable = std::enable_if_t<!Mutable>>
  constexpr ImageView(ContinuousImageView<PixelFormat, !Mutable> image);

  // Return the height of the image
  constexpr unsigned int height() const;

  constexpr unsigned int width() const;

  constexpr unsigned int stride() const;

  constexpr unsigned int area() const;

  constexpr gsl::span<byte_type> data() const;

  constexpr color_type operator()(unsigned int y, unsigned int x) const;

  constexpr ImageRowView<PixelFormat, Mutable> row(unsigned int y) const;

 private:
  byte_type* data_ = nullptr;
  unsigned int height_ = 0;
  unsigned int width_ = 0;
  unsigned int stride_ = 0;
};

template <class PixelFormat, bool Mutable>
constexpr ImageView<PixelFormat, Mutable>::ImageView(unsigned int height,
                                                     unsigned int width,
                                                     gsl::span<byte_type> data,
                                                     unsigned int stride)
    : data_(data.data()), height_(height), width_(width), stride_(stride) {
  Expects(width <= stride);
  const std::size_t expected_size =
      (height == 0)
          ? 0
          : ((height - 1) * stride + width) * PixelFormat::kBytesPerPixel;
  Expects(data.size() == expected_size);
}

template <class PixelFormat, bool Mutable>
template <class Enable>
constexpr ImageView<PixelFormat, Mutable>::ImageView(
    ImageView<PixelFormat, !Mutable> other)
    : ImageView(other.height(), other.width(), other.data(), other.stride()) {}

template <class PixelFormat, bool Mutable>
constexpr ImageView<PixelFormat, Mutable>::ImageView(
    ContinuousImageView<PixelFormat, Mutable> image)
    : ImageView(image.height(), image.width(), image.data(), image.width()) {}

template <class PixelFormat, bool Mutable>
template <class Enable>
constexpr ImageView<PixelFormat, Mutable>::ImageView(
    ContinuousImageView<PixelFormat, !Mutable> image)
    : ImageView(ContinuousImageView<PixelFormat, Mutable>(image)) {}

template <class PixelFormat, bool Mutable>
constexpr unsigned int ImageView<PixelFormat, Mutable>::height() const {
  return height_;
}

template <class PixelFormat, bool Mutable>
constexpr unsigned int ImageView<PixelFormat, Mutable>::width() const {
  return width_;
}

template <class PixelFormat, bool Mutable>
constexpr unsigned int ImageView<PixelFormat, Mutable>::stride() const {
  return stride_;
}

template <class PixelFormat, bool Mutable>
constexpr unsigned int ImageView<PixelFormat, Mutable>::area() const {
  return height_ * width_;
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageView<PixelFormat, Mutable>::data() const
    -> gsl::span<byte_type> {
  const std::size_t data_size =
      (height_ == 0)
          ? 0
          : ((height_ - 1) * stride_ + width_) * PixelFormat::kBytesPerPixel;
  return gsl::span<byte_type>(data_, data_size);
}

template <class PixelFormat, bool Mutable>
constexpr typename PixelFormat::color_type
ImageView<PixelFormat, Mutable>::operator()(unsigned int y,
                                            unsigned int x) const {
  Expects(y < height_);
  Expects(x < width_);
  const gsl::span<const std::byte, PixelFormat::kBytesPerPixel> pixel_data(
      data_ + (y * stride_ + x) * PixelFormat::kBytesPerPixel,
      PixelFormat::kBytesPerPixel);
  return PixelFormat::read(pixel_data);
}

template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable>
ImageView<PixelFormat, Mutable>::row(unsigned int y) const {
  Expects(y < height_);
  const gsl::span<byte_type> row_data(
      data_ + y * stride_ * PixelFormat::kBytesPerPixel,
      width_ * PixelFormat::kBytesPerPixel);
  return ImageRowView<PixelFormat>(row_data, width_);
}

}  // namespace imageview
