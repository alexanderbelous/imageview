#include <imageview/IsPixelFormat.h>

namespace imageview {
namespace {

class RGB565 {
 public:
  unsigned char red : 5;
  unsigned char green : 6;
  unsigned char blue : 5;
};

// TEST(IsPixelFormat, ValidClass) {
class PixelFormatRGB565 {
 public:
  using color_type = RGB565;
  static constexpr unsigned int kBytesPerPixel = 2;

  constexpr color_type read(std::span<const std::byte, 2> pixel_data) const;

  constexpr void write(const color_type& color, std::span<std::byte, 2> pixel_data) const;
};
static_assert(IsPixelFormat<PixelFormatRGB565>::value,
              "IsPixelFormat should be true for PixelFormatRGB565 - the class "
              "is a valid PixelFormat.");
// }
// TEST(IsPixelFormat, NoColorTypeTypedef) {
class NoColorType {
 public:
  static constexpr unsigned int kBytesPerPixel = 2;

  constexpr RGB565 read(std::span<const std::byte, 2> pixel_data) const;

  constexpr void write(const RGB565& color, std::span<std::byte, 2> pixel_data) const;
};
static_assert(!IsPixelFormat<NoColorType>::value,
              "IsPixelFormat should be false for NoColorType - "
              "the class doesn't have a member typedef 'color_type'.");
// }
// TEST(IsPixelFormat, NoKBytesPerPixel) {
class NoKBytesPerPixel {
 public:
  using color_type = RGB565;

  constexpr color_type read(std::span<const std::byte, 2> pixel_data) const;

  constexpr void write(const color_type& color, std::span<std::byte, 2> pixel_data) const;
};
static_assert(!IsPixelFormat<NoKBytesPerPixel>::value,
              "IsPixelFormat should be false for NoKBytesPerPixel - the class "
              "doesn't have a static member constant 'kBytesPerPixel'.");
// }
// TEST(IsPixelFormat, InvalidKBytesPerPixelType) {
class InvalidKBytesPerPixelType {
 public:
  using color_type = RGB565;
  static constexpr float kBytesPerPixel = 3.14f;

  constexpr color_type read(std::span<const std::byte> pixel_data) const;

  constexpr void write(const color_type& color, std::span<std::byte> pixel_data) const;
};
static_assert(!IsPixelFormat<InvalidKBytesPerPixelType>::value,
              "IsPixelFormat should be false for InvalidKBytesPerPixelType - "
              "its static member constant 'kBytesPerPixel' is not an integer.");
// }
// TEST(IsPixelFormat, NonConstKBytesPerPixel) {
class NonConstKBytesPerPixel {
 public:
  using color_type = RGB565;
  static int kBytesPerPixel;

  constexpr color_type read(std::span<const std::byte> pixel_data) const;

  constexpr void write(const color_type& color, std::span<std::byte> pixel_data) const;
};
int NonConstKBytesPerPixel::kBytesPerPixel = 2;
static_assert(!IsPixelFormat<NonConstKBytesPerPixel>::value,
              "IsPixelFormat should be false for NonConstKBytesPerPixel - "
              "its static member 'kBytesPerPixel' is not constant.");
// }
// TEST(IsPixelFormat, NonStaticKBytesPerPixel) {
class NonStaticKBytesPerPixel {
 public:
  using color_type = RGB565;
  const int kBytesPerPixel;

  constexpr color_type read(std::span<const std::byte> pixel_data) const;

  constexpr void write(const color_type& color, std::span<std::byte> pixel_data) const;
};
static_assert(!IsPixelFormat<NonStaticKBytesPerPixel>::value,
              "IsPixelFormat should be false for NonStaticKBytesPerPixel - "
              "its member 'kBytesPerPixel' is not static.");
// }

}  // namespace
}  // namespace imageview
