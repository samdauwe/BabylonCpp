#ifndef BABYLON_LAYERS_IHIGHLIGHT_LAYER_MESH_H
#define BABYLON_LAYERS_IHIGHLIGHT_LAYER_MESH_H

#include <babylon/babylon_api.h>
#include <babylon/math/color3.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

/**
 * @brief Storage interface grouping all the information required for glowing a
 * mesh.
 */
struct BABYLON_SHARED_EXPORT IHighlightLayerMesh {

  /**
   * The glowy mesh
   */
  MeshPtr mesh = nullptr;

  /**
   * The color of the glow
   */
  Color3 color;

  /**
   * The mesh render callback use to insert stencil information
   */
  Observer<Mesh>::Ptr observerHighlight = nullptr;

  /**
   * The mesh render callback use to come to the default behavior
   */
  Observer<Mesh>::Ptr observerDefault = nullptr;

  /**
   * If it exists, the emissive color of the material will be used to generate
   * the glow.
   * Else it falls back to the current color.
   */
  bool glowEmissiveOnly;

}; // end of struct IHighlightLayerMesh

} // end of namespace BABYLON

#endif // end of BABYLON_LAYERS_IHIGHLIGHT_LAYER_MESH_H
