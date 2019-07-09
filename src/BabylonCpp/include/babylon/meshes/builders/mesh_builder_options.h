#ifndef BABYLON_MESHES_BUILDERS_MESH_BUILDER_OPTIONS_H
#define BABYLON_MESHES_BUILDERS_MESH_BUILDER_OPTIONS_H

#include <array>

#include <babylon/babylon_api.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector4.h>

namespace BABYLON {

//------------------------------------------------------------------------------
// Box mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a box mesh.
 */
class BABYLON_SHARED_EXPORT BoxOptions {

public:
  BoxOptions();
  ~BoxOptions();

public:
  std::optional<float> size;
  std::optional<float> width;
  std::optional<float> height;
  std::optional<float> depth;
  std::array<Vector4, 6> faceUV;
  std::array<Color4, 6> faceColors;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;
  std::optional<bool> wrap;
  std::optional<unsigned int> topBaseAt;
  std::optional<unsigned int> bottomBaseAt;
  std::optional<bool> updatable;

}; // end of class BoxOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_BUILDERS_BOX_BUILDER_H
