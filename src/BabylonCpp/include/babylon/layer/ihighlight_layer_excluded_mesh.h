#ifndef BABYLON_LAYER_IHIGHLIGHT_LAYER_EXCLUDED_MESH_H
#define BABYLON_LAYER_IHIGHLIGHT_LAYER_EXCLUDED_MESH_H

#include <babylon/babylon_api.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class Mesh;

/**
 * @brief Storage interface grouping all the information required for an
 * excluded mesh.
 */
struct BABYLON_SHARED_EXPORT IHighlightLayerExcludedMesh {

  /**
   * The glowy mesh
   */
  Mesh* mesh;
  /**
   * The mesh render callback use to prevent stencil use
   */
  Observer<Mesh>::Ptr beforeRender;
  /**
   * The mesh render callback use to restore previous stencil use
   */
  Observer<Mesh>::Ptr afterRender;

}; // end of struct IHighlightLayerExcludedMesh

} // end of namespace BABYLON

#endif // end of BABYLON_LAYER_IHIGHLIGHT_LAYER_EXCLUDED_MESH_H
