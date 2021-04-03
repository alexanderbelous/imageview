#pragma once

#include <imageview/ContinuousImageView.h>
#include <imageview/ImageView.h>
#include <imageview/pixel_formats/PixelFormatRGB24.h>

namespace imageview {
namespace examples {

// Computes the mean color of the given image.
// \param image - input image in RGB24 format.
// \return the mean color of pixels in @image,
//         or RGB24(0, 0, 0) if @image is empty.
RGB24 computeMeanColor(ImageView<PixelFormatRGB24> image);

// Assigns a new color to all pixels with the specified color.
// \param image - input image in RGB24 format.
// \param color_original - color that should be replaced.
// \param color_new - new color to replace with.
void replaceColor(ContinuousImageView<PixelFormatRGB24, true> image,
                  const RGB24& color_original, const RGB24& color_new);

}  // namespace examples
}  // namespace imageview
