#ifndef BABYLON_DEBUG_PHYSICS_VIEWER_H
#define BABYLON_DEBUG_PHYSICS_VIEWER_H

#include <functional>
#include <memory>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class AbstractMesh;
class EventState;
struct IPhysicsEnginePlugin;
class Material;
class Mesh;
class PhysicsImpostor;
class Scene;
class StandardMaterial;
class UtilityLayerRenderer;
using AbstractMeshPtr         = std::shared_ptr<AbstractMesh>;
using MaterialPtr             = std::shared_ptr<Material>;
using MeshPtr                 = std::shared_ptr<Mesh>;
using StandardMaterialPtr     = std::shared_ptr<StandardMaterial>;
using UtilityLayerRendererPtr = std::shared_ptr<UtilityLayerRenderer>;

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
  ~PhysicsViewer(); // = default

  /**
   * @brief Renders a specified physic impostor.
   * @param impostor defines the impostor to render
   * @param targetMesh defines the mesh represented by the impostor
   * @returns the new debug mesh used to render the impostor
   */
  AbstractMeshPtr showImpostor(PhysicsImpostor* impostor,
                               const MeshPtr& targetMesh = nullptr);

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
  MaterialPtr _getDebugMaterial(Scene* scene);
  AbstractMeshPtr _getDebugBoxMesh(Scene* scene);
  AbstractMeshPtr _getDebugSphereMesh(Scene* scene);
  AbstractMeshPtr _getDebugCylinderMesh(Scene* scene);
  AbstractMeshPtr _getDebugMeshMesh(const MeshPtr& mesh, Scene* scene);
  AbstractMeshPtr _getDebugMesh(PhysicsImpostor* impostor,
                                const MeshPtr& targetMesh);

protected:
  /**
   * @brief Hidden
   */
  std::vector<PhysicsImpostor*> _impostors;

  /**
   * @brief Hidden
   */
  std::vector<AbstractMeshPtr> _meshes;

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
  std::function<void(Scene* scene, EventState& es)> _renderFunction;
  UtilityLayerRendererPtr _utilityLayer;
  MeshPtr _debugBoxMesh;
  MeshPtr _debugSphereMesh;
  MeshPtr _debugCylinderMesh;
  StandardMaterialPtr _debugMaterial;
  std::vector<MeshPtr> _debugMeshMeshes;

}; // end of class PhysicsViewer

} // end of namespace Debug
} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_PHYSICS_VIEWER_H
