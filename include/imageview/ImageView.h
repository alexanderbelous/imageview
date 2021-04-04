#pragma once

#include <imageview/ContinuousImageView.h>
#include <imageview/internal/ImageViewStorage.h>
#include <imageview/internal/PixelRef.h>

#include <gsl/span>

#include <cstddef>

namespace imageview {

template <class PixelFormat, bool Mutable = false>
class ImageView {
 public:
  static_assert(IsPixelFormat<PixelFormat>::value, "Not a PixelFormat.");

  using byte_type = std::conditional_t<Mutable, std::byte, const std::byte>;
  using value_type = typename PixelFormat::color_type;
  // TODO: consider using 'const value_type' instead of 'value_type' for immutable views.
  using reference = std::conditional_t<Mutable, detail::PixelRef<PixelFormat>, value_type>;

  // Construct an empty view.
  template <class Enable = std::enable_if_t<std::is_default_constructible_v<PixelFormat>>>
  constexpr ImageView() noexcept(noexcept(std::is_nothrow_default_constructible_v<PixelFormat>)) {}

  // Construct a view into an image.
  // \param height - height of the image.
  // \param width - width of the image.
  // \param stride -
  // \param data - bitmap data.
  constexpr ImageView(unsigned int height, unsigned int width, unsigned int stride, gsl::span<byte_type> data);

  // Construct a view into an image.
  // \param height - height of the image.
  // \param width - width of the image.
  // \param stride -
  // \param data - bitmap data.
  // \param pixel_format - PixelFormat instance to use.
  constexpr ImageView(unsigned int height, unsigned int width, unsigned int stride, gsl::span<byte_type> data,
                      const PixelFormat& pixel_format);
  constexpr ImageView(unsigned int height, unsigned int width, unsigned int stride, gsl::span<byte_type> data,
                      PixelFormat&& pixel_format);

  // Construct a read-only view from a mutable view.
  template <class Enable = std::enable_if_t<!Mutable>>
  constexpr ImageView(ImageView<PixelFormat, !Mutable> other);

  constexpr ImageView(ContinuousImageView<PixelFormat, Mutable> image);
  // Construct a read-only view from a mutable contiguous view.
  template <class Enable = std::enable_if_t<!Mutable>>
  constexpr ImageView(ContinuousImageView<PixelFormat, !Mutable> image);

  // Return the height of the image
  constexpr unsigned int height() const noexcept;

  constexpr unsigned int width() const noexcept;

  constexpr unsigned int stride() const noexcept;

  constexpr unsigned int area() const noexcept;

  // Returns true if the image has zero area, false otherwise.
  constexpr bool empty() const noexcept;

  // Returns the pixel format used by this image.
  constexpr const PixelFormat& pixelFormat() const noexcept;

  constexpr gsl::span<byte_type> data() const noexcept;

  constexpr reference operator()(unsigned int y, unsigned int x) const;

  constexpr ImageRowView<PixelFormat, Mutable> row(unsigned int y) const;

 private:
  detail::ImageViewStorage<PixelFormat, Mutable> storage_;
  unsigned int height_ = 0;
  unsigned int width_ = 0;
  unsigned int stride_ = 0;
};

template <class PixelFormat, bool Mutable>
constexpr ImageView<PixelFormat, Mutable>::ImageView(unsigned int height, unsigned int width, unsigned int stride,
                                                     gsl::span<byte_type> data)
    : storage_(data.data()), height_(height), width_(width), stride_(stride) {
  Expects(width <= stride);
  const std::size_t expected_size = (height == 0) ? 0 : ((height - 1) * stride + width) * PixelFormat::kBytesPerPixel;
  Expects(data.size() == expected_size);
}

template <class PixelFormat, bool Mutable>
constexpr ImageView<PixelFormat, Mutable>::ImageView(unsigned int height, unsigned int width, unsigned int stride,
                                                     gsl::span<byte_type> data, const PixelFormat& pixel_format)
    : storage_(data.data(), pixel_format), height_(height), width_(width), stride_(stride) {
  Expects(width <= stride);
  const std::size_t expected_size = (height == 0) ? 0 : ((height - 1) * stride + width) * PixelFormat::kBytesPerPixel;
  Expects(data.size() == expected_size);
}

template <class PixelFormat, bool Mutable>
constexpr ImageView<PixelFormat, Mutable>::ImageView(unsigned int height, unsigned int width, unsigned int stride,
                                                     gsl::span<byte_type> data, PixelFormat&& pixel_format)
    : storage_(data.data(), std::move(pixel_format)), height_(height), width_(width), stride_(stride) {
  Expects(width <= stride);
  const std::size_t expected_size = (height == 0) ? 0 : ((height - 1) * stride + width) * PixelFormat::kBytesPerPixel;
  Expects(data.size() == expected_size);
}

template <class PixelFormat, bool Mutable>
template <class Enable>
constexpr ImageView<PixelFormat, Mutable>::ImageView(ImageView<PixelFormat, !Mutable> other)
    : ImageView(other.height(), other.width(), other.stride(), other.data(), other.pixelFormat()) {}

template <class PixelFormat, bool Mutable>
constexpr ImageView<PixelFormat, Mutable>::ImageView(ContinuousImageView<PixelFormat, Mutable> image)
    : ImageView(image.height(), image.width(), image.width(), image.data()) {}

template <class PixelFormat, bool Mutable>
template <class Enable>
constexpr ImageView<PixelFormat, Mutable>::ImageView(ContinuousImageView<PixelFormat, !Mutable> image)
    : ImageView(ContinuousImageView<PixelFormat, Mutable>(image)) {}

template <class PixelFormat, bool Mutable>
constexpr unsigned int ImageView<PixelFormat, Mutable>::height() const noexcept {
  return height_;
}

template <class PixelFormat, bool Mutable>
constexpr unsigned int ImageView<PixelFormat, Mutable>::width() const noexcept {
  return width_;
}

template <class PixelFormat, bool Mutable>
constexpr unsigned int ImageView<PixelFormat, Mutable>::stride() const noexcept {
  return stride_;
}

template <class PixelFormat, bool Mutable>
constexpr unsigned int ImageView<PixelFormat, Mutable>::area() const noexcept {
  return height_ * width_;
}

template <class PixelFormat, bool Mutable>
constexpr bool ImageView<PixelFormat, Mutable>::empty() const noexcept {
  return height_ == 0 || width_ == 0;
}

template <class PixelFormat, bool Mutable>
constexpr const PixelFormat& ImageView<PixelFormat, Mutable>::pixelFormat() const noexcept {
  return storage_.pixelFormat();
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageView<PixelFormat, Mutable>::data() const noexcept -> gsl::span<byte_type> {
  const std::size_t data_size = (height_ == 0) ? 0 : ((height_ - 1) * stride_ + width_) * PixelFormat::kBytesPerPixel;
  return gsl::span<byte_type>(storage_.data(), data_size);
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageView<PixelFormat, Mutable>::operator()(unsigned int y, unsigned int x) const -> reference {
  Expects(y < height_);
  Expects(x < width_);
  const gsl::span<byte_type, PixelFormat::kBytesPerPixel> pixel_data(
      storage_.data() + (y * stride_ + x) * PixelFormat::kBytesPerPixel, PixelFormat::kBytesPerPixel);
  if constexpr (Mutable) {
    return detail::PixelRef<PixelFormat>(pixel_data, pixelFormat());
  } else {
    return pixelFormat().read(pixel_data);
  }
}

template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable> ImageView<PixelFormat, Mutable>::row(unsigned int y) const {
  Expects(y < height_);
  const gsl::span<byte_type> row_data(storage_.data() + y * stride_ * PixelFormat::kBytesPerPixel,
                                      width_ * PixelFormat::kBytesPerPixel);
  return ImageRowView<PixelFormat>(row_data, width_, pixelFormat());
}

}  // namespace imageview
