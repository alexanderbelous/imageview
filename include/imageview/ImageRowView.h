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

  // Constant LegacyInputIterator whose value_type is color_type. The type satisfies all
  // requirements of LegacyRandomAccessIterator except the multipass guarantee: for dereferenceable iterators a and b
  // with a == b, there is no requirement that *a and *b are bound to the same object.
  using const_iterator = detail::ImageViewIterator<PixelFormat>;
  using iterator = const_iterator;

  // Constructs an empty view.
  // This constructor is only available if PixelFormat is default-constructible.
  template <class Enable = std::enable_if_t<std::is_default_constructible_v<PixelFormat>>>
  constexpr ImageRowView() noexcept(noexcept(std::is_nothrow_default_constructible_v<PixelFormat>)){};

  // Constructs an empty view.
  // \param pixel_format - instance of PixelFormat to use.
  constexpr explicit ImageRowView(const PixelFormat& pixel_format) noexcept(
      noexcept(std::is_nothrow_copy_constructible_v<PixelFormat>));

  // Constructs an empty view.
  // \param pixel_format - instance of PixelFormat to use.
  constexpr explicit ImageRowView(PixelFormat&& pixel_format) noexcept(
      noexcept(std::is_nothrow_move_constructible_v<PixelFormat>));

  // Constructs a flat view into the given bitmap.
  // This constructor is only available if PixelFormat is default-constructible.
  // \param data - bitmap data.
  // \param width - the number of pixels in the bitmap.
  //
  // Expects(data.size() == width * PixelFormat::kBytesPerPixel)
  template <class Enable = std::enable_if_t<std::is_default_constructible_v<PixelFormat>>>
  constexpr ImageRowView(gsl::span<byte_type> data,
                         std::size_t width) noexcept(noexcept(std::is_nothrow_default_constructible_v<PixelFormat>));

  // Constructs a flat view into the given bitmap.
  // \param data - bitmap data.
  // \param width - the number of pixels in the bitmap.
  // \param pixel_format - instance of PixelFormat to use.
  //
  // Expects(data.size() == width * PixelFormat::kBytesPerPixel)
  constexpr ImageRowView(gsl::span<byte_type> data, std::size_t width, const PixelFormat& pixel_format) noexcept(
      noexcept(std::is_nothrow_copy_constructible_v<PixelFormat>));

  // Constructs a flat view into the given bitmap.
  // \param data - bitmap data.
  // \param width - the number of pixels in the bitmap.
  // \param pixel_format - instance of PixelFormat to use.
  //
  // Expects(data.size() == width * PixelFormat::kBytesPerPixel)
  constexpr ImageRowView(gsl::span<byte_type> data, std::size_t width, PixelFormat&& pixel_format) noexcept(
      noexcept(std::is_nothrow_move_constructible_v<PixelFormat>));

  // Construct a read-only view from a mutable view.
  template <class Enable = std::enable_if_t<!Mutable>>
  constexpr ImageRowView(ImageRowView<PixelFormat, !Mutable> row_view);

  // Returns the pixel format used by this image.
  constexpr const PixelFormat& pixelFormat() const noexcept;

  // Returns the bitmap data.
  constexpr gsl::span<byte_type> data() const noexcept;

  // Returns the total number of pixels in this view.
  constexpr std::size_t size() const noexcept;

  // Returns true if the view is empty (i.e. has 0 pixels), false otherwise.
  constexpr bool empty() const noexcept;

  // Returns the size of the referenced bitmap in bytes, i.e.
  //   size() * PixelFormat::kBytesPerPixel
  constexpr std::size_t size_bytes() const noexcept;

  // Returns an iterator to the first pixel.
  constexpr const_iterator begin() const;

  // Returns an iterator past the last pixel.
  constexpr const_iterator end() const;

  // Returns the color of the specified pixel.
  // \param index - 0-based index of the pixel. No bounds checking is performed - the behavior is undefined if @index is
  // outside [0; size()).
  constexpr color_type operator[](std::size_t index) const;

  // Returns the color of the specified pixel.
  // \param index - 0-based index of the pixel.
  // \throw std::out_of_range if @index is outside [0; size()).
  constexpr color_type at(std::size_t index) const;

  // Sets the color of the specified pixel to the given value.
  // \param index - 0-based index of the pixel.
  // \param color - color to assign.
  // \throw std::out_of_range if @index is outside [0; size()).
  constexpr void setElement(std::size_t index, const color_type& color) const;

 private:
  constexpr gsl::span<byte_type, PixelFormat::kBytesPerPixel> getPixelData(std::size_t index) const;

  detail::ImageViewStorage<PixelFormat, Mutable> storage_;
  std::size_t width_ = 0;
};

template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable>::ImageRowView(const PixelFormat& pixel_format) noexcept(
    noexcept(std::is_nothrow_copy_constructible_v<PixelFormat>))
    : storage_(nullptr, pixel_format) {}

template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable>::ImageRowView(PixelFormat&& pixel_format) noexcept(
    noexcept(std::is_nothrow_move_constructible_v<PixelFormat>))
    : storage_(nullptr, std::move(pixel_format)) {}

template <class PixelFormat, bool Mutable>
template <class Enable>
constexpr ImageRowView<PixelFormat, Mutable>::ImageRowView(gsl::span<byte_type> data, std::size_t width) noexcept(
    noexcept(std::is_nothrow_default_constructible_v<PixelFormat>))
    : storage_(data.data()), width_(width) {
  Expects(data.size() == width * PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable>::ImageRowView(
    gsl::span<byte_type> data, std::size_t width,
    const PixelFormat& pixel_format) noexcept(noexcept(std::is_nothrow_copy_constructible_v<PixelFormat>))
    : storage_(data.data(), pixel_format), width_(width) {
  Expects(data.size() == width * PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable>::ImageRowView(
    gsl::span<byte_type> data, std::size_t width,
    PixelFormat&& pixel_format) noexcept(noexcept(std::is_nothrow_move_constructible_v<PixelFormat>))
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
constexpr std::size_t ImageRowView<PixelFormat, Mutable>::size() const noexcept {
  return width_;
}

template <class PixelFormat, bool Mutable>
constexpr bool ImageRowView<PixelFormat, Mutable>::empty() const noexcept {
  return width_ == 0;
}

template <class PixelFormat, bool Mutable>
constexpr std::size_t ImageRowView<PixelFormat, Mutable>::size_bytes() const noexcept {
  return width_ * PixelFormat::kBytesPerPixel;
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::begin() const -> const_iterator {
  return const_iterator(storage_.data_, storage_.pixelFormat());
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::end() const -> const_iterator {
  return const_iterator(storage_.data_ + width_ * PixelFormat::kBytesPerPixel, storage_.pixelFormat());
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
    throw std::out_of_range("ImageRowView::at(): attempting to access an element out of range.");
  }
  return (*this)[index];
}

template <class PixelFormat, bool Mutable>
constexpr void ImageRowView<PixelFormat, Mutable>::setElement(std::size_t index, const color_type& color) const {
  static_assert(Mutable, "SetElement() can only be called for mutable views.");
  if (index >= width_) {
    throw std::out_of_range("ImageRowView::setElement(): attempting to access an element out of range.");
  }
  const gsl::span<std::byte, PixelFormat::kBytesPerPixel> pixel_data = getPixelData(index);
  return storage_.pixelFormat().write(color, pixel_data);
}

}  // namespace imageview
