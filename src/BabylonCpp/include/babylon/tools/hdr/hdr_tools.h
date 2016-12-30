#ifndef BABYLON_TOOLS_HDR_HDR_TOOLS_H
#define BABYLON_TOOLS_HDR_HDR_TOOLS_H

#include <babylon/babylon_global.h>
#include <babylon/tools/hdr/cube_map_info.h>
#include <babylon/tools/hdr/hdr_info.h>

namespace BABYLON {
namespace Internals {

/**
 * This groups tools to convert HDR texture to native colors array.
 */
class BABYLON_SHARED_EXPORT HDRTools {

public:
  /**
   * Reads header information from an RGBE texture stored in a native array.
   * More information on this format are available here:
   * https://en.wikipedia.org/wiki/RGBE_image_format
   *
   * @param uint8array The binary file stored in  native array.
   * @return The header information.
   */
  static HDRInfo RGBE_ReadHeader(const Uint8Array& uint8array);

  /**
   * Returns the cubemap information (each faces texture data) extracted from an
   * RGBE texture.
   * This RGBE texture needs to store the information as a panorama.
   *
   * More information on this format are available here:
   * https://en.wikipedia.org/wiki/RGBE_image_format
   *
   * @param buffer The binary file stored in an array buffer.
   * @param size The expected size of the extracted cubemap.
   * @return The Cube Map information.
   */
  static CubeMapInfo GetCubeMapTextureData(const Uint8Array& buffer,
                                           size_t size);

  /**
   * Returns the pixels data extracted from an RGBE texture.
   * This pixels will be stored left to right up to down in the R G B order in
   * one array.
   *
   * More information on this format are available here:
   * https://en.wikipedia.org/wiki/RGBE_image_format
   *
   * @param uint8array The binary file stored in an array buffer.
   * @param hdrInfo The header information of the file.
   * @return The pixels data in RGB right to left up to down order.
   */
  static Float32Array RGBE_ReadPixels(const Uint8Array& uint8array,
                                      const HDRInfo& hdrInfo);

private:
  static float Ldexp(float mantissa, float exponent);
  static void Rgbe2float(Float32Array& float32array, float red, float green,
                         float blue, float exponent, size_t index);
  static std::string readStringLine(const Uint8Array& uint8array,
                                    size_t startIndex);
  static Float32Array RGBE_ReadPixels_RLE(const Uint8Array& uint8array,
                                          const HDRInfo& hdrInfo);

}; // end of struct HDRTools

} // end of namespace Internals
} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_HDR_HDR_TOOLS_H
