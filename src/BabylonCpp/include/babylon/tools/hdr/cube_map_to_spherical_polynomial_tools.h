#ifndef BABYLON_TOOLS_HDR_CUBE_MAP_TO_SPHERICAL_POLYNOMIAL_TOOLS_H
#define BABYLON_TOOLS_HDR_CUBE_MAP_TO_SPHERICAL_POLYNOMIAL_TOOLS_H

#include <babylon/babylon_global.h>
#include <babylon/tools/hdr/cube_map_info.h>
#include <babylon/tools/hdr/file_face_orientation.h>

namespace BABYLON {

/**
 * @brief Helper class dealing with the extraction of spherical polynomial
 * dataArray from a cube map.
 */
class BABYLON_SHARED_EXPORT CubeMapToSphericalPolynomialTools {

private:
  static array_t<FileFaceOrientation, 6> FileFaces;

public:
  /**
   * @brief Converts a texture to the according Spherical Polynomial data.
   * This extracts the first 3 orders only as they are the only one used in the
   * lighting.
   *
   * @param texture The texture to extract the information from.
   * @return The Spherical Polynomial data.
   */
  static unique_ptr_t<SphericalPolynomial>
  ConvertCubeMapTextureToSphericalPolynomial(BaseTexture* texture);

  /**
   * @brief Converts a cubemap to the according Spherical Polynomial data.
   * This extracts the first 3 orders only as they are the only one used in
   * the lighting.
   *
   * @param cubeInfo The Cube map to extract the information from.
   * @return The Spherical Polynomial data.
   */
  static unique_ptr_t<SphericalPolynomial>
  ConvertCubeMapToSphericalPolynomial(const CubeMapInfo& cubeInfo);

}; // end of class CubeMapToSphericalPolynomialTools

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_HDR_CUBE_MAP_TO_SPHERICAL_POLYNOMIAL_TOOLS_H
