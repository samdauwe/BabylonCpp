#ifndef BABYLON_DEBUG_RAY_HELPER_H
#define BABYLON_DEBUG_RAY_HELPER_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/culling/ray.h>
#include <babylon/maths/vector3.h>
#include <babylon/misc/event_state.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class AbstractMesh;
class Color3;
class LinesMesh;
class Scene;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using LinesMeshPtr    = std::shared_ptr<LinesMesh>;

/**
 * @brief As raycast might be hard to debug, the RayHelper can help rendering the different rays
 * in order to better appreciate the issue one might have.
 * @see https://doc.babylonjs.com/babylon101/raycasts#debugging
 */
class BABYLON_SHARED_EXPORT RayHelper {

public:
  /**
   * @brief Helper function to create a colored helper in a scene in one line.
   * @param ray Defines the ray we are currently tryin to visualize
   * @param scene Defines the scene the ray is used in
   * @param color Defines the color we want to see the ray in
   * @returns The newly created ray helper.
   */
  static std::unique_ptr<RayHelper> CreateAndShow(const Ray& ray, Scene* scene,
                                                  const Color3& color);

public:
  /**
   * @brief Instantiate a new ray helper.
   * As raycast might be hard to debug, the RayHelper can help rendering the different rays
   * in order to better appreciate the issue one might have.
   * @see https://doc.babylonjs.com/babylon101/raycasts#debugging
   * @param ray Defines the ray we are currently tryin to visualize
   */
  RayHelper(const Ray& ray);
  ~RayHelper(); // = default

  /**
   * @brief Shows the ray we are willing to debug.
   * @param scene Defines the scene the ray needs to be rendered in
   */
  void show(Scene* scene);

  /**
   * @brief Shows the ray we are willing to debug.
   * @param scene Defines the scene the ray needs to be rendered in
   * @param color Defines the color the ray needs to be rendered in
   */
  void show(Scene* scene, const Color3& color);

  /**
   * @brief Hides the ray we are debugging.
   */
  void hide();

  /**
   * @brief Attach a ray helper to a mesh so that we can easily see its orientation for instance or
   * information like its normals.
   * @param mesh Defines the mesh we want the helper attached to
   * @param meshSpaceDirection Defines the direction of the Ray in mesh space (local space of the
   * mesh node)
   * @param meshSpaceOrigin Defines the origin of the Ray in mesh space (local space of the mesh
   * node)
   * @param length Defines the length of the ray
   */
  void attachToMesh(const AbstractMeshPtr& mesh,
                    const Vector3& meshSpaceDirection = Vector3::Zero(),
                    const Vector3& meshSpaceOrigin = Vector3(0.f, 0.f, -1.f), float length = 0.f);

  /**
   * @brief Detach the ray helper from the mesh it has previously been attached
   * to.
   */
  void detachFromMesh();

  /**
   * @brief Dispose the helper and release its associated resources.
   */
  void dispose();

private:
  void _render();
  void _updateToMesh();

public:
  /**
   * Defines the ray we are currently tryin to visualize.
   */
  std::unique_ptr<Ray> ray;

private:
  std::vector<Vector3> _renderPoints;
  LinesMeshPtr _renderLine;
  std::function<void(Scene*, EventState&)> _renderFunction;
  Scene* _scene;

  Observer<Scene>::Ptr _onAfterRenderObserver;
  Observer<Scene>::Ptr _onAfterStepObserver;
  AbstractMeshPtr _attachedToMesh;
  Vector3 _meshSpaceDirection;
  Vector3 _meshSpaceOrigin;

}; // end of class RayHelper

} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_RAY_HELPER_H
