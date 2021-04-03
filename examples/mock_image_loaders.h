#pragma once

namespace imageview {
namespace examples {

// Realistic C-style API for loading an image.
// \param path - path to the file.
// \param data - output parameter; the function sets *data to point to the loaded bitmap data.
//        The caller is responsible for freeing the allocated memory via std::free().
// \param width - output parameter; the function sets *width eqaul to the width of the loaded image.
// \param height - output parameter; the function sets *width eqaul to the height of the loaded image.
// \return 0 on success, nonzero otherwise.
int loadImageRGB24(const char* path, unsigned char** data, unsigned int* width, unsigned int* height);

}  // namespace examples
}  // namespace imageview
