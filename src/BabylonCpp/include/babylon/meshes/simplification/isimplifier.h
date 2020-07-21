#ifndef BABYLON_MESHES_SIMPLIFICATION_ISIMPLIFIER_H
#define BABYLON_MESHES_SIMPLIFICATION_ISIMPLIFIER_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief A simplifier interface for future simplification implementations
 * @see https://doc.babylonjs.com/how_to/in-browser_mesh_simplification
 */
class BABYLON_SHARED_EXPORT ISimplifier {

public:
  /**
   * @brief Simplification of a given mesh according to the given settings.
   * Since this requires computation, it is assumed that the function runs async.
   * @param settings The settings of the simplification, including quality and distance
   * @param successCallback A callback that will be called after the mesh was simplified.
   * @param errorCallback in case of an error, this callback will be called. optional.
   */
  virtual void simplify() = 0;

}; // end of class ISimplifier

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_SIMPLIFICATION_ISIMPLIFIER_H
