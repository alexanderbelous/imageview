#pragma once

#include <imageview/internal/ImageViewStorage.h>
#include <imageview/IsPixelFormat.h>

#include <cstddef>
#include <iterator>
#include <memory>

namespace imageview {
namespace detail {

template <class ColorType>
class ArrowProxy {
 public:
  constexpr explicit ArrowProxy(const ColorType& color) noexcept(
      noexcept(std::is_nothrow_copy_constructible_v<ColorType>))
      : color_(color) {}

  constexpr explicit ArrowProxy(ColorType&& color) noexcept(noexcept(std::is_nothrow_move_constructible_v<ColorType>))
      : color_(std::move(color)) {}

  constexpr const ColorType* operator->() const noexcept { return std::addressof(color_); }

 private:
  ColorType color_;
};

template <class PixelFormat>
class ImageViewIterator {
 public:
  static_assert(IsPixelFormat<PixelFormat>::value, "Not a PixelFormat.");

  using difference_type = std::ptrdiff_t;
  using value_type = typename PixelFormat::color_type;
  using pointer = ArrowProxy<value_type>;
  using reference = value_type;
  using iterator_category = std::input_iterator_tag;

  template <class Enable = std::enable_if_t<std::is_default_constructible_v<PixelFormat>>>
  constexpr ImageViewIterator() noexcept(noexcept(std::is_nothrow_default_constructible_v<PixelFormat>)) {}

  constexpr ImageViewIterator(const std::byte* data, const PixelFormat& pixel_format);

  constexpr const PixelFormat& pixelFormat() const noexcept;

  constexpr gsl::span<const std::byte, PixelFormat::kBytesPerPixel> getPixelData() const noexcept;

  constexpr reference operator*() const;

  constexpr pointer operator->() const;

  constexpr reference operator[](std::ptrdiff_t index) const;

  constexpr ImageViewIterator& operator++() noexcept;

  constexpr ImageViewIterator operator++(int);

  constexpr ImageViewIterator& operator+=(std::ptrdiff_t n) noexcept;

  constexpr ImageViewIterator operator+(std::ptrdiff_t n) const;

  constexpr ImageViewIterator& operator--() noexcept;

  constexpr ImageViewIterator operator--(int);

  constexpr ImageViewIterator& operator-=(std::ptrdiff_t n) noexcept;

  constexpr ImageViewIterator operator-(std::ptrdiff_t n) const;

  constexpr bool operator==(ImageViewIterator other) const noexcept;

  constexpr bool operator!=(ImageViewIterator other) const noexcept;

  constexpr bool operator<(ImageViewIterator other) const noexcept;

  constexpr bool operator<=(ImageViewIterator other) const noexcept;

  constexpr bool operator>(ImageViewIterator other) const noexcept;

  constexpr bool operator>=(ImageViewIterator other) const noexcept;

  constexpr std::ptrdiff_t operator-(ImageViewIterator other) const noexcept;

 private:
  detail::ImageViewStorage<PixelFormat, false> storage_;
};

template <class PixelFormat>
constexpr ImageViewIterator<PixelFormat>::ImageViewIterator(const std::byte* data, const PixelFormat& pixel_format)
    : storage_(data, pixel_format) {}

template <class PixelFormat>
constexpr const PixelFormat& ImageViewIterator<PixelFormat>::pixelFormat() const noexcept {
  return storage_.pixelFormat();
}

template <class PixelFormat>
constexpr auto ImageViewIterator<PixelFormat>::getPixelData() const noexcept
    -> gsl::span<const std::byte, PixelFormat::kBytesPerPixel> {
  return gsl::span<const std::byte, PixelFormat::kBytesPerPixel>(storage_.data_, PixelFormat::kBytesPerPixel);
}

template <class PixelFormat>
constexpr auto ImageViewIterator<PixelFormat>::operator*() const -> reference {
  constexpr unsigned int kBytesPerPixel = PixelFormat::kBytesPerPixel;
  return storage_.pixelFormat().read(gsl::span<const std::byte, kBytesPerPixel>(storage_.data_, kBytesPerPixel));
}

template <class PixelFormat>
constexpr auto ImageViewIterator<PixelFormat>::operator->() const -> pointer {
  return pointer(*this);
}

template <class PixelFormat>
constexpr auto ImageViewIterator<PixelFormat>::operator[](std::ptrdiff_t index) const -> reference {
  constexpr unsigned int kBytesPerPixel = PixelFormat::kBytesPerPixel;
  const gsl::span<const std::byte, kBytesPerPixel> pixel_data(storage_.data_ + index * kBytesPerPixel, kBytesPerPixel);
  return storage_.pixelFormat().read(pixel_data);
}

template <class PixelFormat>
constexpr ImageViewIterator<PixelFormat>& ImageViewIterator<PixelFormat>::operator++() noexcept {
  storage_.data_ += PixelFormat::kBytesPerPixel;
  return *this;
}

template <class PixelFormat>
constexpr ImageViewIterator<PixelFormat> ImageViewIterator<PixelFormat>::operator++(int) {
  const std::byte* current_data = storage_.data_;
  storage_.data_ += PixelFormat::kBytesPerPixel;
  return ImageViewIterator(current_data, storage_.pixelFormat());
}

template <class PixelFormat>
constexpr ImageViewIterator<PixelFormat>& ImageViewIterator<PixelFormat>::operator+=(std::ptrdiff_t n) noexcept {
  storage_.data_ += (n * PixelFormat::kBytesPerPixel);
  return *this;
}

template <class PixelFormat>
constexpr ImageViewIterator<PixelFormat> ImageViewIterator<PixelFormat>::operator+(std::ptrdiff_t n) const {
  return ImageViewIterator(storage_.data_ + n * PixelFormat::kBytesPerPixel, storage_.pixelFormat());
}

template <class PixelFormat>
constexpr ImageViewIterator<PixelFormat>& ImageViewIterator<PixelFormat>::operator--() noexcept {
  storage_.data_ -= PixelFormat::kBytesPerPixel;
  return *this;
}

template <class PixelFormat>
constexpr ImageViewIterator<PixelFormat> ImageViewIterator<PixelFormat>::operator--(int) {
  const std::byte* current_data = storage_.data_;
  storage_.data_ -= PixelFormat::kBytesPerPixel;
  return ImageViewIterator(current_data, storage_.pixelFormat());
}

template <class PixelFormat>
constexpr ImageViewIterator<PixelFormat>& ImageViewIterator<PixelFormat>::operator-=(std::ptrdiff_t n) noexcept {
  storage_.data_ -= (n * PixelFormat::kBytesPerPixel);
  return *this;
}

template <class PixelFormat>
constexpr ImageViewIterator<PixelFormat> ImageViewIterator<PixelFormat>::operator-(std::ptrdiff_t n) const {
  return ImageViewIterator(storage_.data_ - n * PixelFormat::kBytesPerPixel, storage_.pixelFormat());
}

template <class PixelFormat>
constexpr bool ImageViewIterator<PixelFormat>::operator==(ImageViewIterator other) const noexcept {
  // TODO: Shouldn't we also compare storage_.pixelFormat()?
  // Problem is, I don't want to require that PixelFormat is EqualityComparable.
  // A possible solution is to store PixelFormat by reference and compare the addresses.
  // However, this is kinda stupid for empty PixelFormat classes. I could introduce something like
  //   template<class PixelFormat, class Enable = void>
  //   class IteratorStorage {
  //     const std::byte* data_;
  //     const PixelFormat* pixel_format_;
  //   };
  //   template<class PixelFormat>
  //   class IteratorStorage<PixelFormat, std::enable_if_t</* Is PixelFormat eligible for empty base optimization? */>>:
  //     private PixelFormat {
  //     const std::byte* data;
  //   };
  // UPD: actually, we don't have to compare pixel formats: comparison operators only need to work "in the domain of
  // ==". Comparing 2 iterators from different image views is already an error.
  return storage_.data_ == other.storage_.data_;
}

template <class PixelFormat>
constexpr bool ImageViewIterator<PixelFormat>::operator!=(ImageViewIterator other) const noexcept {
  return storage_.data_ != other.storage_.data_;
}

template <class PixelFormat>
constexpr bool ImageViewIterator<PixelFormat>::operator<(ImageViewIterator other) const noexcept {
  return storage_.data_ < other.storage_.data_;
}

template <class PixelFormat>
constexpr bool ImageViewIterator<PixelFormat>::operator<=(ImageViewIterator other) const noexcept {
  return storage_.data_ <= other.storage_.data_;
}

template <class PixelFormat>
constexpr bool ImageViewIterator<PixelFormat>::operator>(ImageViewIterator other) const noexcept {
  return storage_.data_ > other.storage_.data_;
}

template <class PixelFormat>
constexpr bool ImageViewIterator<PixelFormat>::operator>=(ImageViewIterator other) const noexcept {
  return storage_.data_ >= other.storage_.data_;
}

template <class PixelFormat>
constexpr std::ptrdiff_t ImageViewIterator<PixelFormat>::operator-(ImageViewIterator other) const noexcept {
  static_assert(PixelFormat::kBytesPerPixel != 0, "Division by zero.");
  return (storage_.data_ - other.storage_.data_) / PixelFormat::kBytesPerPixel;
}

}  // namespace detail
}  // namespace imageview
