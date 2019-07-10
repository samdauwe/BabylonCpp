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

//------------------------------------------------------------------------------
// Cylinder or cone mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a cylinder or cone mesh.
 */
class BABYLON_SHARED_EXPORT CylinderOptions {

public:
  CylinderOptions();
  ~CylinderOptions();

public:
  std::optional<float> height;
  std::optional<float> diameterTop;
  std::optional<float> diameterBottom;
  std::optional<float> diameter;
  std::optional<unsigned int> tessellation;
  std::optional<unsigned int> subdivisions;
  std::optional<float> arc;
  std::vector<Color4> faceColors;
  std::vector<Vector4> faceUV;
  std::optional<bool> updatable;
  std::optional<bool> hasRings;
  std::optional<bool> enclose;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;

}; // end of class CylinderOptions

//------------------------------------------------------------------------------
// Decal mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a decal mesh.
 */
class BABYLON_SHARED_EXPORT DecalOptions {

public:
  DecalOptions();
  ~DecalOptions();

public:
  std::optional<Vector3> position;
  std::optional<Vector3> normal;
  std::optional<Vector3> size;
  std::optional<float> angle;

}; // end of class DecalOptions

//------------------------------------------------------------------------------
// Disc mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a disc mesh.
 */
class BABYLON_SHARED_EXPORT DiscOptions {

public:
  DiscOptions();
  ~DiscOptions();

public:
  std::optional<float> radius;
  std::optional<unsigned int> tessellation;
  std::optional<float> arc;
  std::optional<bool> updatable;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;

}; // end of class DiscOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_BUILDERS_BOX_BUILDER_H
