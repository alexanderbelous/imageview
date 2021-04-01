#pragma once

#include <imageview/IsPixelFormat.h>

#include <gsl/span>
#include <gsl/gsl_assert>

#include <cstddef>
#include <stdexcept>

namespace imageview {

template <class PixelFormat, bool Mutable = false>
class ImageRowView {
 public:
  static_assert(IsPixelFormat<PixelFormat>::value, "Not a PixelFormat.");

  using byte_type =
      std::conditional_t<Mutable, std::byte, const std::byte>;
  using color_type = typename PixelFormat::color_type;

  constexpr ImageRowView() = default;

  constexpr ImageRowView(gsl::span<byte_type> data, unsigned int width);
  // Construct a read-only view from a mutable view.
  template <class Enable = std::enable_if_t<!Mutable>>
  constexpr ImageRowView(ImageRowView<PixelFormat, !Mutable> row_view);

  constexpr gsl::span<byte_type> data() const;

  constexpr int size() const;

  constexpr bool empty() const;

  constexpr std::size_t size_bytes() const;

  constexpr color_type operator[](std::size_t index) const;

  constexpr color_type at(std::size_t index) const;

  constexpr void setElement(std::size_t index, const color_type& color) const;

 private:
  constexpr gsl::span<byte_type, PixelFormat::kBytesPerPixel> getPixelData(
      std::size_t index) const;

  byte_type* data_ = nullptr;
  unsigned int width_ = 0;
};

template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable>::ImageRowView(
    gsl::span<byte_type> data, unsigned int width)
    : data_(data.data()), width_(width) {
  Expects(data.size() == width * PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
template <class Enable>
constexpr ImageRowView<PixelFormat, Mutable>::ImageRowView(
    ImageRowView<PixelFormat, !Mutable> row_view)
    : ImageRowView(row_view.data(), row_view.size()) {}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::data() const
    -> gsl::span<byte_type> {
  return gsl::span<byte_type>(data_, size_bytes());
}

template <class PixelFormat, bool Mutable>
constexpr int ImageRowView<PixelFormat, Mutable>::size() const {
  return width_;
}

template <class PixelFormat, bool Mutable>
constexpr bool ImageRowView<PixelFormat, Mutable>::empty() const {
  return width_ == 0;
}

template <class PixelFormat, bool Mutable>
constexpr std::size_t ImageRowView<PixelFormat, Mutable>::size_bytes() const {
  return static_cast<std::size_t>(width_) * PixelFormat::kBytesPerPixel;
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::getPixelData(
    std::size_t index) const
    -> gsl::span<byte_type, PixelFormat::kBytesPerPixel> {
  return gsl::span<byte_type, PixelFormat::kBytesPerPixel>(
      data_ + index * PixelFormat::kBytesPerPixel, PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::operator[](
    std::size_t index) const -> color_type {
  const gsl::span<const std::byte, PixelFormat::kBytesPerPixel> pixel_data =
      getPixelData(index);
  return PixelFormat::read(pixel_data);
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::at(std::size_t index) const
    -> color_type {
  if (index >= width_) {
    throw std::out_of_range(
        "ImageRowView::at(): attempting to access an element out of "
        "range.");
  }
  return (*this)[index];
}

template <class PixelFormat, bool Mutable>
constexpr void ImageRowView<PixelFormat, Mutable>::setElement(
    std::size_t index, const color_type& color) const {
  static_assert(Mutable, "SetElement() can only be called for mutable views.");
  if (index >= width_) {
    throw std::out_of_range(
        "ImageRowView::setElement(): attempting to access an element out of "
        "range.");
  }
  const gsl::span<std::byte, PixelFormat::kBytesPerPixel> pixel_data =
      getPixelData(index);
  return PixelFormat::write(color, pixel_data);
}

}  // namespace imageview
