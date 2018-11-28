#ifndef BABYLON_TOOLS_HDR_PANORAMA_TO_CUBE_MAP_TOOLS_H
#define BABYLON_TOOLS_HDR_PANORAMA_TO_CUBE_MAP_TOOLS_H

#include <babylon/babylon_api.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>
#include <babylon/tools/hdr/cube_map_info.h>

namespace BABYLON {

/**
 * @brief Helper class usefull to convert panorama picture to their cubemap
 * representation in 6 faces.
 */
class BABYLON_SHARED_EXPORT PanoramaToCubeMapTools {

private:
  static std::array<Vector3, 4> FACE_FRONT;
  static std::array<Vector3, 4> FACE_BACK;
  static std::array<Vector3, 4> FACE_RIGHT;
  static std::array<Vector3, 4> FACE_LEFT;
  static std::array<Vector3, 4> FACE_DOWN;
  static std::array<Vector3, 4> FACE_UP;

public:
  /**
   * @brief Converts a panorma stored in RGB right to left up to down format
   * into a cubemap (6 faces).
   *
   * @param float32Array The source data.
   * @param inputWidth The width of the input panorama.
   * @param inputHeight The height of the input panorama.
   * @param size The willing size of the generated cubemap (each faces will be
   * size * size pixels)
   * @return The cubemap data
   */
  static CubeMapInfo ConvertPanoramaToCubemap(const Float32Array& float32Array,
                                              size_t inputWidth,
                                              size_t inputHeight, size_t size);

private:
  static Float32Array
  CreateCubemapTexture(size_t texSize, const std::array<Vector3, 4>& faceData,
                       const Float32Array& float32Array, size_t inputWidth,
                       size_t inputHeight);
  static Color3 CalcProjectionSpherical(const Vector3& vDir,
                                        const Float32Array& float32Array,
                                        size_t inputWidth, size_t inputHeight);

}; // end of struct PanoramaToCubeMapTools

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_HDR_PANORAMA_TO_CUBE_MAP_TOOLS_H
