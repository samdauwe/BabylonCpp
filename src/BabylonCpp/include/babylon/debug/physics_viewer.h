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
  PhysicsViewer(Scene* scene);
  ~PhysicsViewer();

  void showImpostor(PhysicsImpostor* impostor);
  void hideImpostor(PhysicsImpostor* impostor);
  void dispose();

protected:
  void _updateDebugMeshes();

private:
  Material* _getDebugMaterial(Scene* scene);
  AbstractMesh* _getDebugBoxMesh(Scene* scene);
  AbstractMesh* _getDebugSphereMesh(Scene* scene);
  AbstractMesh* _getDebugMesh(PhysicsImpostor* impostor, Scene* scene);

protected:
  vector_t<PhysicsImpostor*> _impostors;
  vector_t<AbstractMesh*> _meshes;
  Scene* _scene;
  size_t _numMeshes;
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
