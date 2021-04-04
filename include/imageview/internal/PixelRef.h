#pragma once

#include <imageview/IsPixelFormat.h>
#include <imageview/internal/ImageViewStorage.h>

#include <gsl/span>

namespace imageview {
namespace detail {

template <class PixelFormat>
class PixelRef {
 public:
  static_assert(IsPixelFormat<PixelFormat>::value, "Not a PixelFormat.");

  using color_type = typename PixelFormat::color_type;

  constexpr PixelRef(gsl::span<std::byte, PixelFormat::kBytesPerPixel> pixel_data,
                     std::reference_wrapper<const PixelFormat> pixel_format);

  constexpr PixelRef(const PixelRef& other) = default;
  constexpr PixelRef(PixelRef&&) = default;
  ~PixelRef() = default;

  // Implicit conversion to color_type.
  // \return the color of the referenced pixel.
  constexpr operator color_type() const;

  // Assigns the specified color to the referenced pixel.
  // \param color - color to assign.
  // \return *this.
  constexpr PixelRef& operator=(const color_type& color);

  // Assigns the specified color to the referenced pixel.
  // Note: PixelRef has reference semantics: copy and move assignment operators change the value of the
  // referenced pixel rather than PixelRef object itself.
  constexpr PixelRef& operator=(const PixelRef& other);
  constexpr PixelRef& operator=(PixelRef&& other);

 private:
  // TODO: store PixelFormat by reference if it's not eligible for empty base optimization,
  // otherwise by value (as an empty base).
  detail::ImageViewStorage<PixelFormat, true> storage_;
};

template <class PixelFormat>
constexpr PixelRef<PixelFormat>::PixelRef(gsl::span<std::byte, PixelFormat::kBytesPerPixel> pixel_data,
                                          std::reference_wrapper<const PixelFormat> pixel_format)
    : storage_(pixel_data.data(), pixel_format.get()) {}

template <class PixelFormat>
constexpr PixelRef<PixelFormat>::operator color_type() const {
  constexpr std::size_t kBytesPerPixel = PixelFormat::kBytesPerPixel;
  const gsl::span<const std::byte, kBytesPerPixel> pixel_data(storage_.data_, kBytesPerPixel);
  return storage_.pixelFormat().read(pixel_data);
}

template <class PixelFormat>
constexpr PixelRef<PixelFormat>& PixelRef<PixelFormat>::operator=(const color_type& color) {
  constexpr std::size_t kBytesPerPixel = PixelFormat::kBytesPerPixel;
  const gsl::span<std::byte, kBytesPerPixel> pixel_data(storage_.data_, kBytesPerPixel);
  storage_.pixelFormat().write(color, pixel_data);
  return *this;
}

template <class PixelFormat>
constexpr PixelRef<PixelFormat>& PixelRef<PixelFormat>::operator=(const PixelRef& other) {
  // TODO: In theory, we could just memcpy() the binary data. However, if PixelFormat is stateful,
  // and the state of our PixelFormat differs from the state of @other, then simply copying the binary
  // data might lead to the wrong result.
  // Possible workaround: add a constexpr if for the case when PixelFormat is an empty class.
  return *this = static_cast<color_type>(other);
}

template <class PixelFormat>
constexpr PixelRef<PixelFormat>& PixelRef<PixelFormat>::operator=(PixelRef&& other) {
  return *this = static_cast<color_type>(other);
}

}  // namespace detail
}  // namespace imageview
