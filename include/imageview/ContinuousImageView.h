#pragma once

#include <imageview/ImageRowView.h>
#include <imageview/IsPixelFormat.h>

#include <gsl/assert>
#include <gsl/span>

#include <cstddef>
#include <type_traits>

namespace imageview {

// Non-owning view into a bitmap image with the specified pixel format.
template <class PixelFormat, bool Mutable = false>
class ContinuousImageView {
 public:
  static_assert(IsPixelFormat<PixelFormat>::value, "Not a PixelFormat.");

  using byte_type =
      std::conditional_t<Mutable, std::byte, const std::byte>;
  using color_type = typename PixelFormat::color_type;

  // Construct an empty view.
  constexpr ContinuousImageView() = default;
  // Construct a view into an image.
  // \param height - height of the image.
  // \param width - width of the image.
  // \param data - pointer to the uncompressed RGB24 bitmap data. The size of
  //        the array should be exactly
  //          height * width * PixelFormat::kBytesPerPixel.
  //        Pixels are assumed to be stored contiguously, with each pixel
  //        occupying exactly PixelFormat::kBytesPerPixel.
  constexpr ContinuousImageView(unsigned int height, unsigned int width,
                                gsl::span<byte_type> data);
  // Construct a read-only view from a mutable view.
  template <class Enable = std::enable_if_t<!Mutable>>
  constexpr ContinuousImageView(
      ContinuousImageView<PixelFormat, !Mutable> image);
  // Returns the height of the image.
  constexpr unsigned int height() const;
  // Returns the width of the image.
  constexpr unsigned int width() const;
  // Returns the total number of pixels.
  constexpr unsigned int area() const;
  // Returns the pointer to the bitmap data.
  constexpr gsl::span<byte_type> data() const;
  // Returns the total size of the bitmap in bytes.
  constexpr std::size_t size_bytes() const;
  // Returns the color of the specified pixel.
  constexpr color_type operator()(unsigned int y, unsigned int x) const;
  // Assign the given color to the specified pixel.
  // This function fails at compile time if Mutable is false.
  constexpr void setPixel(unsigned int y, unsigned int x,
                          const color_type& color) const;
  // Returns a non-owning view into the specified row of the image.
  constexpr ImageRowView<PixelFormat, Mutable> row(unsigned int y) const;

 private:
  constexpr gsl::span<byte_type, PixelFormat::kBytesPerPixel> getPixelData(
      unsigned int y, unsigned int x) const;

  byte_type* data_ = nullptr;
  unsigned int height_ = 0;
  unsigned int width_ = 0;
  // TODO: use empty base optimization.
  // PixelFormat pixel_format_;
};

// Convert a ContinuousImageView into an ImageRowView.
// \param image - input image.
// \return an ImageRowView referring to the same data as image; the number of
//         elements in the returned view equals image.area().
template<class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable> flatten(
    ContinuousImageView<PixelFormat, Mutable> image) {
  return ImageRowView<PixelFormat, Mutable>(image.data(), image.area());
}

template <class PixelFormat, bool Mutable>
constexpr ContinuousImageView<PixelFormat, Mutable>::ContinuousImageView(
    unsigned int height, unsigned int width, gsl::span<byte_type> data)
    : data_(data.data()), height_(height), width_(width) {
  Expects(data.size() == height * width * PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
template <class Enable>
constexpr ContinuousImageView<PixelFormat, Mutable>::ContinuousImageView(
    ContinuousImageView<PixelFormat, !Mutable> image)
    : ContinuousImageView(image.height(), image.width(), image.data()) {}

template <class PixelFormat, bool Mutable>
constexpr unsigned int ContinuousImageView<PixelFormat, Mutable>::height()
    const {
  return height_;
}

template <class PixelFormat, bool Mutable>
constexpr unsigned int ContinuousImageView<PixelFormat, Mutable>::width()
    const {
  return width_;
}

template <class PixelFormat, bool Mutable>
constexpr unsigned int ContinuousImageView<PixelFormat, Mutable>::area() const {
  return height_ * width_;
}

template <class PixelFormat, bool Mutable>
constexpr auto ContinuousImageView<PixelFormat, Mutable>::data() const
    -> gsl::span<byte_type> {
  return gsl::span<byte_type>(data_, size_bytes());
}

template <class PixelFormat, bool Mutable>
constexpr std::size_t ContinuousImageView<PixelFormat, Mutable>::size_bytes()
    const {
  return static_cast<std::size_t>(area()) * PixelFormat::kBytesPerPixel;
}

template <class PixelFormat, bool Mutable>
constexpr auto ContinuousImageView<PixelFormat, Mutable>::getPixelData(
    unsigned int y, unsigned int x) const
    -> gsl::span<byte_type, PixelFormat::kBytesPerPixel> {
  Expects(y < height_);
  Expects(x < width_);
  const std::size_t offset = (y * width_ + x) * PixelFormat::kBytesPerPixel;
  return gsl::span<byte_type, PixelFormat::kBytesPerPixel>(
      data_ + offset, PixelFormat::kBytesPerPixel);
}

template <class PixelFormat, bool Mutable>
constexpr typename PixelFormat::color_type
ContinuousImageView<PixelFormat, Mutable>::operator()(unsigned int y,
                                                      unsigned int x) const {
  const gsl::span<const std::byte, PixelFormat::kBytesPerPixel> pixel_data =
      getPixelData(y, x);
  return PixelFormat::read(pixel_data);
}

template <class PixelFormat, bool Mutable>
constexpr void ContinuousImageView<PixelFormat, Mutable>::setPixel(
    unsigned int y, unsigned int x, const color_type& color) const {
  static_assert(Mutable, "setPixel() can only be called for mutable views.");
  const gsl::span<std::byte, PixelFormat::kBytesPerPixel> pixel_data =
      getPixelData(y, x);
  PixelFormat::write(color, pixel_data);
}

template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable>
ContinuousImageView<PixelFormat, Mutable>::row(unsigned int y) const {
  Expects(y < height_);
  const std::size_t bytes_per_row =
      static_cast<std::size_t>(width_) * PixelFormat::kBytesPerPixel;
  const gsl::span<byte_type> row_data(data_ + y * bytes_per_row, bytes_per_row);
  return ImageRowView<PixelFormat, Mutable>(row_data, width_);
}

}  // namespace imageview
