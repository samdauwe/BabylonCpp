#ifndef BABYLON_CULLING_OCTREES_OCTREE_SCENE_COMPONENT_H
#define BABYLON_CULLING_OCTREES_OCTREE_SCENE_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/culling/ray.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/scene_component_constants.h>

namespace BABYLON {

class AbstractMesh;
class Collider;
class OctreeSceneComponent;
class SubMesh;
using OctreeSceneComponentPtr = std::shared_ptr<OctreeSceneComponent>;

/**
 * @brief Defines the octree scene component responsible to manage any octrees
 * in a given scene.
 */
class BABYLON_SHARED_EXPORT OctreeSceneComponent : public ISceneComponent {

public:
  /**
   * The component name help to identify the component in the list of scene
   * components.
   */
  static constexpr const char* name = SceneComponentConstants::NAME_OCTREE;

public:
  template <typename... Ts>
  static OctreeSceneComponentPtr New(Ts&&... args)
  {
    return std::shared_ptr<OctreeSceneComponent>(
      new OctreeSceneComponent(std::forward<Ts>(args)...));
  }
  virtual ~OctreeSceneComponent(); // = default

  /**
   * @brief Registers the component in a given scene.
   */
  void _register() override;

  /**
   * @brief Return the list of active meshes.
   * @returns the list of active meshes
   */
  std::vector<AbstractMesh*> getActiveMeshCandidates();

  /**
   * @brief Return the list of active sub meshes.
   * @param mesh The mesh to get the candidates sub meshes from
   * @returns the list of active sub meshes
   */
  std::vector<SubMesh*> getActiveSubMeshCandidates(AbstractMesh* mesh);

  /**
   * @brief Return the list of sub meshes intersecting with a given local ray.
   * @param mesh defines the mesh to find the submesh for
   * @param localRay defines the ray in local space
   * @returns the list of intersecting sub meshes
   */
  std::vector<SubMesh*> getIntersectingSubMeshCandidates(AbstractMesh* mesh,
                                                         const Ray& localRay);

  /**
   * @brief Return the list of sub meshes colliding with a collider.
   * @param mesh defines the mesh to find the submesh for
   * @param collider defines the collider to evaluate the collision against
   * @returns the list of colliding sub meshes
   */
  std::vector<SubMesh*> getCollidingSubMeshCandidates(AbstractMesh* mesh,
                                                      const Collider& collider);

  /**
   * @brief Rebuilds the elements related to this component in case of
   * context lost for instance.
   */
  void rebuild() override;

  /**
   * @brief Disposes the component and the associated resources.
   */
  void dispose() override;

protected:
  /**
   * @brief Creates a new instance of the component for the given scene
   * @param scene Defines the scene to register the component in
   */
  OctreeSceneComponent(Scene* scene);

  bool get_checksIsEnabled() const;

public:
  /**
   * Indicates if the meshes have been checked to make sure they are isEnabled()
   */
  ReadOnlyProperty<OctreeSceneComponent, bool> checksIsEnabled;

private:
  bool _checksIsEnabled;
  Ray _tempRay;

}; // end of class OctreeSceneComponent

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_OCTREES_OCTREE_SCENE_COMPONENT_H
