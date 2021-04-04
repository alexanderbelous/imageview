#pragma once

#include <imageview/ImageRowView.h>
#include <imageview/IsPixelFormat.h>
#include <imageview/internal/ImageViewIterator.h>
#include <imageview/internal/ImageViewStorage.h>
#include <imageview/internal/PixelRef.h>

#include <gsl/assert>
#include <gsl/span>

#include <cstddef>
#include <type_traits>

namespace imageview {

// Non-owning view into a bitmap image with the specified pixel format.
//
// \param PixelFormat - specifies how colors are stored in the bitmap.
//        ContinuousImageView only supports images with fixed color depth (bits
//        per pixel). PixelFormat should satisfy IsPixelFormat trait, i.e. be
//        like
//          class MyPixelFormat {
//           public:
//            using color_type = MyColor;
//            static constexpr int kBytesPerPixel = N;
//            color_type read(gsl::span<const std::byte, kBytesPerPixel> pixel_data) const;
//            void write(const color_type& color, gsl::span<std::byte, kBytesPerPixel> pixel_data) const;
//          };
// \param Mutable - if true, ContinuousImageView provides write access to the
//        bitmap (naturally, this requires that ContinuousImageView is
//        constructed from a non-const pointer to the data). Otherwise, only
//        read-only access is provided.
template <class PixelFormat, bool Mutable = false>
class ContinuousImageView {
 public:
  static_assert(IsPixelFormat<PixelFormat>::value, "Not a PixelFormat.");

  using byte_type = std::conditional_t<Mutable, std::byte, const std::byte>;
  using value_type = typename PixelFormat::color_type;
  // If `Mutable == false`, then `reference` is an alias to `value_type`.
  // Otherwise, it is a proxy class, which mimics `value_type&`:
  // * is implicitly convertible to `value_type`.
  // * you can assign a value_type to it, changing the color of the referenced pixel.
  using reference = std::conditional_t<Mutable, detail::PixelRef<PixelFormat>, value_type>;
  // Constant LegacyInputIterator whose value_type is @value_type.
  // It is not, however, a LegacyForwardIterator, because
  //   std::iterator_traits<iterator>::reference
  // is not const value_type&.
  // Nevertheless, this iterator supports all arithmetic operations of LegacyRandomAccessIterator, e.g.,
  //   `it += n`, ` --it`, `it[n]`.
  using const_iterator = detail::ImageViewIterator<PixelFormat, false>;
  // LegacyInputIterator whose value_type is @value_type. If Mutable == true, then it also
  // models LegacyOutputIterator. It is not, however, a LegacyForwardIterator, because
  //   std::iterator_traits<iterator>::reference
  // is neither value_type& nor const value_type&.
  // Nevertheless, this iterator supports all arithmetic operations of LegacyRandomAccessIterator, e.g.,
  //   `it += n`, ` --it`, `it[n]`.
  using iterator = detail::ImageViewIterator<PixelFormat, Mutable>;

  // Construct an empty view.
  // This constructor is only available if PixelFormat is default-constructible.
  template <class Enable = std::enable_if_t<std::is_default_constructible_v<PixelFormat>>>
  constexpr ContinuousImageView() noexcept(noexcept(std::is_nothrow_default_constructible_v<PixelFormat>)) {}

  // Construct a view into an image.
  // This constructor is only available if PixelFormat is default-constructible
  // \param height - height of the image.
  // \param width - width of the image.
  // \param data - pointer to the uncompressed RGB24 bitmap data. The size of
  //        the array should be exactly
  //          height * width * PixelFormat::kBytesPerPixel.
  //        Pixels are assumed to be stored contiguously, with each pixel
  //        occupying exactly PixelFormat::kBytesPerPixel.
  template <class Enable = std::enable_if_t<std::is_default_constructible_v<PixelFormat>>>
  constexpr ContinuousImageView(unsigned int height, unsigned int width, gsl::span<byte_type> data) noexcept(
      std::is_nothrow_default_constructible_v<PixelFormat>);

  // Construct a view into an image.
  // \param height - height of the image.
  // \param width - width of the image.
  // \param data - pointer to the uncompressed RGB24 bitmap data. The size of
  //        the array should be exactly
  //          height * width * PixelFormat::kBytesPerPixel.
  //        Pixels are assumed to be stored contiguously, with each pixel
  //        occupying exactly PixelFormat::kBytesPerPixel.
  // \param pixel_format - PixelFormat instance to use.
  constexpr ContinuousImageView(unsigned int height, unsigned int width, gsl::span<byte_type> data,
                                const PixelFormat& pixel_format);

  // Construct a view into an image.
  // \param height - height of the image.
  // \param width - width of the image.
  // \param data - pointer to the uncompressed RGB24 bitmap data. The size of
  //        the array should be exactly
  //          height * width * PixelFormat::kBytesPerPixel.
  //        Pixels are assumed to be stored contiguously, with each pixel
  //        occupying exactly PixelFormat::kBytesPerPixel.
  // \param pixel_format - PixelFormat instance to use.
  constexpr ContinuousImageView(unsigned int height, unsigned int width, gsl::span<byte_type> data,
                                PixelFormat&& pixel_format);

  // Construct a read-only view from a mutable view.
  template <class Enable = std::enable_if_t<!Mutable>>
  constexpr ContinuousImageView(ContinuousImageView<PixelFormat, !Mutable> image);

  // Returns the height of the image.
  constexpr unsigned int height() const noexcept;

  // Returns the width of the image.
  constexpr unsigned int width() const noexcept;

  // Returns the total number of pixels.
  constexpr std::size_t area() const noexcept;

  // Returns true if the image has zero area, false otherwise.
  constexpr bool empty() const noexcept;

  // Returns the pixel format used by this image.
  constexpr const PixelFormat& pixelFormat() const noexcept;

  // Returns the pointer to the bitmap data.
  constexpr gsl::span<byte_type> data() const noexcept;

  // Returns the total size of the bitmap in bytes.
  constexpr std::size_t size_bytes() const noexcept;

  // Returns an iterator to the first pixel.
  constexpr iterator begin() const;

  // Returns a const iterator to the first pixel.
  constexpr const_iterator cbegin() const;

  // Returns an iterator past the last pixel.
  constexpr iterator end() const;

  // Returns a const iterator past the last pixel.
  constexpr const_iterator cend() const;

  // Access the specified pixel.
  // \param y - Y coordinate of the pixel. Should be within [0; height()).
  // \param x - X coordinate of the pixel. Should be within [0; width()).
  // \return the color of the specified pixel.
  constexpr reference operator()(unsigned int y, unsigned int x) const;

  // Returns a non-owning view into the specified row of the image.
  // \param y - 0-based index of the row. Should be within [0; height()).
  // \return a non-owning view into the row @y.
  constexpr ImageRowView<PixelFormat, Mutable> row(unsigned int y) const;

 private:
  constexpr gsl::span<byte_type, PixelFormat::kBytesPerPixel> getPixelData(unsigned int y, unsigned int x) const;

  detail::ImageViewStorage<PixelFormat, Mutable> storage_;
  unsigned int height_ = 0;
  unsigned int width_ = 0;
};

// Convert a ContinuousImageView into an ImageRowView.
// \param image - input image.
// \return an ImageRowView referring to the same data as image; the number of
//         elements in the returned view equals image.area().
template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable> flatten(ContinuousImageView<PixelFormat, Mutable> image) {
  return ImageRowView<PixelFormat, Mutable>(image.data(), image.area(), image.pixelFormat());
}

template <class PixelFormat, bool Mutable>
template <class Enable>
constexpr ContinuousImageView<PixelFormat, Mutable>::ContinuousImageView(
    unsigned int height, unsigned int width,
    gsl::span<byte_type> data) noexcept(std::is_nothrow_default_constructible_v<PixelFormat>)
    : storage_(data.data()), height_(height), width_(width) {
  Expects(data.size() == height * width * PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
constexpr ContinuousImageView<PixelFormat, Mutable>::ContinuousImageView(unsigned int height, unsigned int width,
                                                                         gsl::span<byte_type> data,
                                                                         const PixelFormat& pixel_format)
    : storage_(data.data(), pixel_format), height_(height), width_(width) {
  Expects(data.size() == height * width * PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
constexpr ContinuousImageView<PixelFormat, Mutable>::ContinuousImageView(unsigned int height, unsigned int width,
                                                                         gsl::span<byte_type> data,
                                                                         PixelFormat&& pixel_format)
    : storage_(data.data(), std::move(pixel_format)), height_(height), width_(width) {
  Expects(data.size() == height * width * PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
template <class Enable>
constexpr ContinuousImageView<PixelFormat, Mutable>::ContinuousImageView(
    ContinuousImageView<PixelFormat, !Mutable> image)
    : ContinuousImageView(image.height(), image.width(), image.data(), image.pixelFormat()) {}

template <class PixelFormat, bool Mutable>
constexpr unsigned int ContinuousImageView<PixelFormat, Mutable>::height() const noexcept {
  return height_;
}

template <class PixelFormat, bool Mutable>
constexpr unsigned int ContinuousImageView<PixelFormat, Mutable>::width() const noexcept {
  return width_;
}

template <class PixelFormat, bool Mutable>
constexpr std::size_t ContinuousImageView<PixelFormat, Mutable>::area() const noexcept {
  return static_cast<std::size_t>(height_) * width_;
}

template <class PixelFormat, bool Mutable>
constexpr bool ContinuousImageView<PixelFormat, Mutable>::empty() const noexcept {
  return height_ == 0 || width_ == 0;
}

template <class PixelFormat, bool Mutable>
constexpr const PixelFormat& ContinuousImageView<PixelFormat, Mutable>::pixelFormat() const noexcept {
  return storage_.pixelFormat();
}

template <class PixelFormat, bool Mutable>
constexpr auto ContinuousImageView<PixelFormat, Mutable>::data() const noexcept -> gsl::span<byte_type> {
  return gsl::span<byte_type>(storage_.data(), size_bytes());
}

template <class PixelFormat, bool Mutable>
constexpr std::size_t ContinuousImageView<PixelFormat, Mutable>::size_bytes() const noexcept {
  return area() * PixelFormat::kBytesPerPixel;
}

template <class PixelFormat, bool Mutable>
constexpr auto ContinuousImageView<PixelFormat, Mutable>::begin() const -> iterator {
  return iterator(storage_.data(), pixelFormat());
}

template <class PixelFormat, bool Mutable>
constexpr auto ContinuousImageView<PixelFormat, Mutable>::cbegin() const -> const_iterator {
  return const_iterator(storage_.data(), pixelFormat());
}

template <class PixelFormat, bool Mutable>
constexpr auto ContinuousImageView<PixelFormat, Mutable>::end() const -> iterator {
  return iterator(storage_.data() + size_bytes(), pixelFormat());
}

template <class PixelFormat, bool Mutable>
constexpr auto ContinuousImageView<PixelFormat, Mutable>::cend() const -> const_iterator {
  return const_iterator(storage_.data() + size_bytes(), pixelFormat());
}

template <class PixelFormat, bool Mutable>
constexpr auto ContinuousImageView<PixelFormat, Mutable>::getPixelData(unsigned int y, unsigned int x) const
    -> gsl::span<byte_type, PixelFormat::kBytesPerPixel> {
  Expects(y < height_);
  Expects(x < width_);
  const std::size_t offset = (y * width_ + x) * PixelFormat::kBytesPerPixel;
  return gsl::span<byte_type, PixelFormat::kBytesPerPixel>(storage_.data() + offset, PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
constexpr auto ContinuousImageView<PixelFormat, Mutable>::operator()(unsigned int y, unsigned int x) const
    -> reference {
  const gsl::span<byte_type, PixelFormat::kBytesPerPixel> pixel_data = getPixelData(y, x);
  if constexpr (Mutable) {
    return detail::PixelRef<PixelFormat>(pixel_data, storage_.pixelFormat());
  } else {
    return storage_.pixelFormat().read(pixel_data);
  }
}

template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable> ContinuousImageView<PixelFormat, Mutable>::row(unsigned int y) const {
  Expects(y < height_);
  const std::size_t bytes_per_row = static_cast<std::size_t>(width_) * PixelFormat::kBytesPerPixel;
  const gsl::span<byte_type> row_data(storage_.data() + y * bytes_per_row, bytes_per_row);
  return ImageRowView<PixelFormat, Mutable>(row_data, width_, pixelFormat());
}

}  // namespace imageview
