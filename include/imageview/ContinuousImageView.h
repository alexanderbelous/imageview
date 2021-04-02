#pragma once

#include <imageview/ImageRowView.h>
#include <imageview/IsPixelFormat.h>
#include <imageview/internal/ImageViewStorage.h>

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
//            static color_type read(
//                gsl::span<const std::byte, kBytesPerPixel> pixel_data);
//            static void write(const color_type& color,
//                gsl::span<std::byte, kBytesPerPixel> pixel_data);
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
  using color_type = typename PixelFormat::color_type;

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
  constexpr unsigned int area() const noexcept;
  // Returns the pixel format used by this image.
  constexpr const PixelFormat& pixelFormat() const noexcept;
  // Returns the pointer to the bitmap data.
  constexpr gsl::span<byte_type> data() const noexcept;
  // Returns the total size of the bitmap in bytes.
  constexpr std::size_t size_bytes() const noexcept;
  // Returns the color of the specified pixel.
  constexpr color_type operator()(unsigned int y, unsigned int x) const;
  // Assign the given color to the specified pixel.
  // This function fails at compile time if Mutable is false.
  // TODO: don't provide this function for immutable views at all.
  constexpr void setPixel(unsigned int y, unsigned int x, const color_type& color) const;
  // Returns a non-owning view into the specified row of the image.
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
    : storage_(data.data(), pixel_format), height_(height), width_(width) {}

template <class PixelFormat, bool Mutable>
constexpr ContinuousImageView<PixelFormat, Mutable>::ContinuousImageView(unsigned int height, unsigned int width,
                                                                         gsl::span<byte_type> data,
                                                                         PixelFormat&& pixel_format)
    : storage_(data.data(), std::move(pixel_format)), height_(height), width_(width) {}

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
constexpr unsigned int ContinuousImageView<PixelFormat, Mutable>::area() const noexcept {
  return height_ * width_;
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
  return static_cast<std::size_t>(area()) * PixelFormat::kBytesPerPixel;
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
constexpr typename PixelFormat::color_type ContinuousImageView<PixelFormat, Mutable>::operator()(unsigned int y,
                                                                                                 unsigned int x) const {
  const gsl::span<const std::byte, PixelFormat::kBytesPerPixel> pixel_data = getPixelData(y, x);
  return storage_.pixelFormat().read(pixel_data);
}

template <class PixelFormat, bool Mutable>
constexpr void ContinuousImageView<PixelFormat, Mutable>::setPixel(unsigned int y, unsigned int x,
                                                                   const color_type& color) const {
  static_assert(Mutable, "setPixel() can only be called for mutable views.");
  const gsl::span<std::byte, PixelFormat::kBytesPerPixel> pixel_data = getPixelData(y, x);
  storage_.pixelFormat().write(color, pixel_data);
}

template <class PixelFormat, bool Mutable>
constexpr ImageRowView<PixelFormat, Mutable> ContinuousImageView<PixelFormat, Mutable>::row(unsigned int y) const {
  Expects(y < height_);
  const std::size_t bytes_per_row = static_cast<std::size_t>(width_) * PixelFormat::kBytesPerPixel;
  const gsl::span<byte_type> row_data(storage_.data() + y * bytes_per_row, bytes_per_row);
  return ImageRowView<PixelFormat, Mutable>(row_data, width_, pixelFormat());
}

}  // namespace imageview
