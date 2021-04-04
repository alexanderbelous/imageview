imageview
---------

imageview is a header-only C++ library that provides a set of lightweight
adapters for type-safe image processing.

### Problem statement

Let's assume that you want to write a function that checks if the given image
contains a red square of the size NxN (located anywhere within the image). What
should be the signature of this function?
If you are using a specific image library, e.g., OpenCV, you can declare it as
  `bool hasReadSquare(const cv::Mat3b& image, unsigned int n);`
This has several drawbacks though:
* The function only accepts images stored as `cv::Mat3b`. If you want to invoke it
  with an image stored otherwise, you'll have to create a temporary copy [^1].
* `cv::Mat3b` defines a matrix whose elements have three 1-byte channels. It
  doesn't, however, specify, what these channels represent. For example, they
  could be RGB channels, or BGR channels, or even a different color space, such
  as YUV.

In order to support other image representations, you can declare the function
as
```
bool hasReadSquare(std::span<const MyColor> data, unsigned int height,
                   unsigned int width, unsigned int n);
```
but that would render it useless for bitmaps that were not allocated as a
`MyColor` array (due to the strict aliasing rule).

Fortunately, there's an exception for the strict aliasing rule: any object can
be examined as an array of bytes. Using an additional parameter for the pixel
format, you can declare the function as
```
bool hasReadSquare(std::span<const std::byte> data,
                   PixelFormatEnum pixel_format, unsigned int height,
                   unsigned int width, unsigned int n);
```
However, this is not very flexible, since enums in C++ cannot be extended after
they have been defined.

imageview goes one step further - instead of an enumeration type, it uses a
trait class, so the signature becomes
```
bool hasReadSquare(ImageView<PixelFormatRGB24> image, unsigned int n);
```
where `ImageView` aggregates all image-related parameters (data, height, width).

### Usage

imageview is a header-only library, so you don't need to compile it.

The library provides 3 main classes:
* `ContinuousImageView` provides a non-owning view into an image whose pixels are
  stored continuously in memory:
    `{a11, a12, a13, ..., a1N, a21, a22, a23, ... a2N, ... aMN}`
* `ImageView` is a more relaxed version: while it still requires that pixels
  within the same row are stored continuously, it allows "gaps" between rows.
  This can be useful when you want to take a rectangular sub-image.
* `ImageRowView` is virtually the same as `ContinuousImageView` into an image with
  height equal to 1.

Example:
```
  // Load the image via thirdparty API.
  unsigned int width = 0;
  unsigned int height = 0;
  std::vector<unsigned char> data;
  loadImageRGB24(&data, &width, &height);

  // Construct a mutable view into the image.
  const ContinuousImageView<PixelFormatRGB24, true> image(height, width, gsl::as_writable_bytes(gsl::span{data}));

  // Get the color of the pixel {y = 5, x = 6}.
  const RGB24 color_56 = image(5, 6);

  / Set the color of the pixel {y = 3, x = 4}.
  image(3, 4) = RGB24(0, 0, 255);

  // Construct an immutable view into a subimage by removing 10 rows/columns from each side.
  const ImageView<PixelFormatRGB24> sub_image = imageview::crop(
    image, 10, 10, height - 20, width - 20);

  // Compute the number of green pixels in the cropped image.
  static constexpr RGB24 kGreen(0, 255, 0);
  std::size_t num_green_pixels = 0;
  for (int y = 0; y < image.height(); ++y) {
    const ImageRowView<PixelFormatRGB24> image_row = image.row(y);
    num_green_pixels += std::count(image_row.begin(), image_row.end(), kGreen);
  }
```

[^1]: Technically, cv::Mat can be constructed from the existing data without
      copying. However, accessing pixel data of such images as cv::Vec3b would
      almost certainly violate the strict aliasing rule (unless your data is
      actually an array of cv::Vec3b objects), which is undefined behavior.