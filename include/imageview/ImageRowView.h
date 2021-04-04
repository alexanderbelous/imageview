#pragma once

#include <imageview/IsPixelFormat.h>
#include <imageview/internal/ImageViewIterator.h>
#include <imageview/internal/ImageViewStorage.h>
#include <imageview/internal/PixelRef.h>

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
  using value_type = typename PixelFormat::color_type;
  // TODO: consider using 'const value_type' instead of 'value_type' for immutable views.
  using reference = std::conditional_t<Mutable, detail::PixelRef<PixelFormat>, value_type>;

  // Constant LegacyInputIterator whose value_type is @value_type. The type satisfies all
  // requirements of LegacyRandomAccessIterator except the multipass guarantee: for dereferenceable iterators a and b
  // with a == b, there is no requirement that *a and *b are bound to the same object.
  using const_iterator = detail::ImageViewIterator<PixelFormat, false>;
  using iterator = detail::ImageViewIterator<PixelFormat, Mutable>;

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
  constexpr iterator begin() const;

  // Returns a const iterator to the first pixel.
  constexpr const_iterator cbegin() const;

  // Returns an iterator past the last pixel.
  constexpr iterator end() const;

  // Returns a const iterator past the last pixel.
  constexpr const_iterator cend() const;

  // Returns the color of the specified pixel.
  // \param index - 0-based index of the pixel. No bounds checking is performed - the behavior is undefined if @index is
  // outside [0; size()).
  constexpr reference operator[](std::size_t index) const;

  // Returns the color of the specified pixel.
  // \param index - 0-based index of the pixel.
  // \throw std::out_of_range if @index is outside [0; size()).
  constexpr reference at(std::size_t index) const;

 private:
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
constexpr auto ImageRowView<PixelFormat, Mutable>::begin() const -> iterator {
  return iterator(storage_.data_, storage_.pixelFormat());
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::cbegin() const -> const_iterator {
  return const_iterator(storage_.data_, storage_.pixelFormat());
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::end() const -> iterator {
  return iterator(storage_.data_ + width_ * PixelFormat::kBytesPerPixel, storage_.pixelFormat());
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::cend() const -> const_iterator {
  return const_iterator(storage_.data_ + width_ * PixelFormat::kBytesPerPixel, storage_.pixelFormat());
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::operator[](std::size_t index) const -> reference {
  const gsl::span<byte_type, PixelFormat::kBytesPerPixel> pixel_data(
      storage_.data() + index * PixelFormat::kBytesPerPixel, PixelFormat::kBytesPerPixel);
  if constexpr (Mutable) {
    return detail::PixelRef<PixelFormat>(pixel_data, pixelFormat());
  } else {
    return pixelFormat().read(pixel_data);
  }
}

template <class PixelFormat, bool Mutable>
constexpr auto ImageRowView<PixelFormat, Mutable>::at(std::size_t index) const -> reference {
  if (index >= width_) {
    throw std::out_of_range("ImageRowView::at(): attempting to access an element out of range.");
  }
  return (*this)[index];
}

}  // namespace imageview
