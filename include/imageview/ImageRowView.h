#pragma once

#include <imageview/IsPixelFormat.h>
#include <imageview/internal/ImageViewStorage.h>
#include <imageview/ImageViewIterator.h>

#include <gsl/assert>
#include <gsl/span>

#include <cstddef>
#include <stdexcept>

namespace imageview {

template <class PixelFormat, bool Mutable = false>
class ImageRowView {
 public:
  static_assert(IsPixelFormat<PixelFormat>::value, "Not a PixelFormat.");

  using byte_type = std::conditional_t<Mutable, std::byte, const std::byte>;
  using color_type = typename PixelFormat::color_type;
  using const_iterator = detail::ImageViewIterator<PixelFormat>;
  using iterator = const_iterator;

  constexpr ImageRowView() = default;

  constexpr ImageRowView(gsl::span<byte_type> data, unsigned int width);

  constexpr ImageRowView(gsl::span<byte_type> data, unsigned int width, const PixelFormat& pixel_format);

  constexpr ImageRowView(gsl::span<byte_type> data, unsigned int width, PixelFormat&& pixel_format);

  // Construct a read-only view from a mutable view.
  template <class Enable = std::enable_if_t<!Mutable>>
  constexpr ImageRowView(ImageRowView<PixelFormat, !Mutable> row_view);

  // Returns the pixel format used by this image.
  constexpr const PixelFormat& pixelFormat() const noexcept;

  constexpr gsl::span<byte_type> data() const noexcept;

  constexpr int size() const noexcept;

  constexpr bool empty() const noexcept;

  constexpr std::size_t size_bytes() const noexcept;

  constexpr iterator begin() const noexcept;

  constexpr iterator end() const noexcept;

  constexpr color_type operator[](std::size_t index) const;

  constexpr color_type at(std::size_t index) const;

  constexpr void setElement(std::size_t index, const color_type& color) const;

 private:
  constexpr gsl::span<byte_type, PixelFormat::kBytesPerPixel> getPixelData(std::size_t index) const;

  detail::ImageViewStorage<PixelFormat, Mutable> storage_;
  unsigned int width_ = 0;
};

template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable>::ImageRowView(gsl::span<byte_type> data, unsigned int width)
    : storage_(data.data()), width_(width) {
  Expects(data.size() == width * PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable>::ImageRowView(gsl::span<byte_type> data, unsigned int width,
                                                           const PixelFormat& pixel_format)
    : storage_(data.data(), pixel_format), width_(width) {
  Expects(data.size() == width * PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable>::ImageRowView(gsl::span<byte_type> data, unsigned int width,
                                                           PixelFormat&& pixel_format)
    : storage_(data.data(), std::move(pixel_format)), width_(width) {
  Expects(data.size() == width * PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
template <class Enable>
constexpr ImageRowView<PixelFormat, Mutable>::ImageRowView(ImageRowView<PixelFormat, !Mutable> row_view)
    : ImageRowView(row_view.data(), row_view.size(), row_view.pixelFormat()) {}

template <class PixelFormat, bool Mutable>
constexpr const PixelFormat& ImageRowView<PixelFormat, Mutable>::pixelFormat() const noexcept {
  return storage_.pixelFormat();
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::data() const noexcept -> gsl::span<byte_type> {
  return gsl::span<byte_type>(storage_.data(), size_bytes());
}

template <class PixelFormat, bool Mutable>
constexpr int ImageRowView<PixelFormat, Mutable>::size() const noexcept {
  return width_;
}

template <class PixelFormat, bool Mutable>
constexpr bool ImageRowView<PixelFormat, Mutable>::empty() const noexcept {
  return width_ == 0;
}

template <class PixelFormat, bool Mutable>
constexpr std::size_t ImageRowView<PixelFormat, Mutable>::size_bytes() const noexcept {
  return static_cast<std::size_t>(width_) * PixelFormat::kBytesPerPixel;
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::begin() const noexcept -> iterator {
  return iterator(storage_.data_, storage_.pixelFormat());
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::end() const noexcept -> iterator {
  return iterator(storage_.data_ + width_ * PixelFormat::kBytesPerPixel, storage_.pixelFormat());
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::getPixelData(std::size_t index) const
    -> gsl::span<byte_type, PixelFormat::kBytesPerPixel> {
  return gsl::span<byte_type, PixelFormat::kBytesPerPixel>(storage_.data() + index * PixelFormat::kBytesPerPixel,
                                                           PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::operator[](std::size_t index) const -> color_type {
  const gsl::span<const std::byte, PixelFormat::kBytesPerPixel> pixel_data = getPixelData(index);
  return storage_.pixelFormat().read(pixel_data);
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::at(std::size_t index) const -> color_type {
  if (index >= width_) {
    throw std::out_of_range(
        "ImageRowView::at(): attempting to access an element out of "
        "range.");
  }
  return (*this)[index];
}

template <class PixelFormat, bool Mutable>
constexpr void ImageRowView<PixelFormat, Mutable>::setElement(std::size_t index, const color_type& color) const {
  static_assert(Mutable, "SetElement() can only be called for mutable views.");
  if (index >= width_) {
    throw std::out_of_range(
        "ImageRowView::setElement(): attempting to access an element out of "
        "range.");
  }
  const gsl::span<std::byte, PixelFormat::kBytesPerPixel> pixel_data = getPixelData(index);
  return storage_.pixelFormat().write(color, pixel_data);
}

}  // namespace imageview
