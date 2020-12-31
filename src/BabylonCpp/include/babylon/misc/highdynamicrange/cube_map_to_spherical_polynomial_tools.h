#ifndef BABYLON_MISC_HIGH_DYNAMIC_RANGE_CUBE_MAP_TO_SPHERICAL_POLYNOMIAL_TOOLS_H
#define BABYLON_MISC_HIGH_DYNAMIC_RANGE_CUBE_MAP_TO_SPHERICAL_POLYNOMIAL_TOOLS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/misc/highdynamicrange/cube_map_info.h>
#include <babylon/misc/highdynamicrange/file_face_orientation.h>

#ifndef isNaN
#define isNaN(a) ((a) != (a))
#endif

namespace BABYLON {

class BaseTexture;
FWD_CLASS_SPTR(SphericalPolynomial)

/**
 * @brief Helper class dealing with the extraction of spherical polynomial dataArray from a cube
 * map.
 */
class BABYLON_SHARED_EXPORT CubeMapToSphericalPolynomialTools {

private:
  static std::array<FileFaceOrientation, 6> FileFaces;

public:
  /**
   * @brief Converts a texture to the according Spherical Polynomial data.
   * This extracts the first 3 orders only as they are the only one used in the lighting.
   *
   * @param texture The texture to extract the information from.
   * @return The Spherical Polynomial data.
   */
  static SphericalPolynomialPtr ConvertCubeMapTextureToSphericalPolynomial(BaseTexture& texture);

  /**
   * @brief Converts a cubemap to the according Spherical Polynomial data.
   * This extracts the first 3 orders only as they are the only one used in the lighting.
   *
   * @param cubeInfo The Cube map to extract the information from.
   * @return The Spherical Polynomial data.
   */
  static SphericalPolynomialPtr ConvertCubeMapToSphericalPolynomial(const CubeMapInfo& cubeInfo);

}; // end of class CubeMapToSphericalPolynomialTools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_HIGH_DYNAMIC_RANGE_CUBE_MAP_TO_SPHERICAL_POLYNOMIAL_TOOLS_H
