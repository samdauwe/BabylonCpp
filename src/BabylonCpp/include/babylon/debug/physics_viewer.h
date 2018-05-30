#ifndef BABYLON_DEBUG_PHYSICS_VIEWER_H
#define BABYLON_DEBUG_PHYSICS_VIEWER_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Debug {

/**
 * @brief Used to show the physics impostor around the specific mesh.
 */
class BABYLON_SHARED_EXPORT PhysicsViewer {

public:
  /**
   * @brief Creates a new PhysicsViewer.
   * @param scene defines the hosting scene
   */
  PhysicsViewer(Scene* scene);
  ~PhysicsViewer();

  /**
   * @brief Renders a specified physic impostor.
   * @param impostor defines the impostor to render
   */
  void showImpostor(PhysicsImpostor* impostor);

  /**
   * @brief Hides a specified physic impostor.
   * @param impostor defines the impostor to hide
   */
  void hideImpostor(PhysicsImpostor* impostor);

  /**
   * @brief Releases all resources.
   */
  void dispose();

protected:
  /**
   * @brief Hidden
   */
  void _updateDebugMeshes();

private:
  Material* _getDebugMaterial(Scene* scene);
  AbstractMesh* _getDebugBoxMesh(Scene* scene);
  AbstractMesh* _getDebugSphereMesh(Scene* scene);
  AbstractMesh* _getDebugMesh(PhysicsImpostor* impostor, Scene* scene);

protected:
  /**
   * @brief Hidden
   */
  vector_t<PhysicsImpostor*> _impostors;

  /**
   * @brief Hidden
   */
  vector_t<AbstractMesh*> _meshes;

  /**
   * @brief Hidden
   */
  Scene* _scene;

  /**
   * @brief Hidden
   */
  size_t _numMeshes;

  /**
   * @brief Hidden
   */
  IPhysicsEnginePlugin* _physicsEnginePlugin;

private:
  ::std::function<void(Scene* scene, EventState& es)> _renderFunction;
  Mesh* _debugBoxMesh;
  Mesh* _debugSphereMesh;
  StandardMaterial* _debugMaterial;

}; // end of class PhysicsViewer

} // end of namespace Debug
} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_PHYSICS_VIEWER_H
