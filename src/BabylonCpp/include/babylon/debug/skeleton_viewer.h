#ifndef BABYLON_DEBUG_SKELETON_VIEWER_H
#define BABYLON_DEBUG_SKELETON_VIEWER_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/math/color3.h>
#include <babylon/misc/event_state.h>

namespace BABYLON {

class AbstractMesh;
class Bone;
class LinesMesh;
class Matrix;
class Scene;
class Skeleton;
class Vector3;
class UtilityLayerRenderer;
using BonePtr                 = std::shared_ptr<Bone>;
using AbstractMeshPtr         = std::shared_ptr<AbstractMesh>;
using LinesMeshPtr            = std::shared_ptr<LinesMesh>;
using SkeletonPtr             = std::shared_ptr<Skeleton>;
using UtilityLayerRendererPtr = std::shared_ptr<UtilityLayerRenderer>;

namespace Debug {

/**
 * @brief Class used to render a debug view of a given skeleton.
 * @see http://www.babylonjs-playground.com/#1BZJVJ#8
 */
class BABYLON_SHARED_EXPORT SkeletonViewer {

public:
  /**
   * @brief Creates a new SkeletonViewer.
   * @param skeleton defines the skeleton to render
   * @param mesh defines the mesh attached to the skeleton
   * @param scene defines the hosting scene
   * @param autoUpdateBonesMatrices defines a boolean indicating if bones
   * matrices must be forced to update before rendering (true by default)
   * @param renderingGroupId defines the rendering group id to use with the
   * viewer
   */
  SkeletonViewer(const SkeletonPtr& skeleton, const AbstractMeshPtr& mesh, Scene* scene,
                 bool autoUpdateBonesMatrices = true, int renderingGroupId = 1);
  ~SkeletonViewer(); // = default

  /**
   * @brief Update the viewer to sync with current skeleton state.
   */
  void update();

  /**
   * @brief Release associated resources.
   */
  void dispose();

protected:
  /**
   * @brief Returns the mesh used to render the bones.
   */
  LinesMeshPtr& get_debugMesh();

  /**
   * @brief Sets a boolean indicating if the viewer is enabled.
   */
  void set_isEnabled(bool value);

  /**
   * @brief Gets a boolean indicating if the viewer is enabled.
   */
  [[nodiscard]] bool get_isEnabled() const;

private:
  void _getBonePosition(Vector3& position, const Bone& bone, const Matrix& meshMat, float x = 0.f,
                        float y = 0.f, float z = 0.f) const;
  void _getLinesForBonesWithLength(const std::vector<BonePtr>& bones, const Matrix& meshMat);
  void _resizeDebugLines(size_t bonesSize);
  void _getLinesForBonesNoLength(const std::vector<BonePtr>& bones, const Matrix& meshMat);

public:
  /**
   * Gets or sets the color used to render the skeleton
   */
  Color3 color;

  /**
   * Returns the mesh used to render the bones
   */
  ReadOnlyProperty<SkeletonViewer, LinesMeshPtr> debugMesh;

  /**
   * Defines the skeleton to render
   */
  SkeletonPtr skeleton;

  /**
   * Defines the mesh attached to the skeleton
   */
  AbstractMeshPtr mesh;

  /**
   * Defines a boolean indicating if bones matrices must be forced to update
   * before rendering (true by default)
   */
  bool autoUpdateBonesMatrices;

  /**
   * Defines the rendering group id to use with the viewer
   */
  int renderingGroupId;

  Property<SkeletonViewer, bool> isEnabled;

private:
  Scene* _scene;
  std::vector<std::vector<Vector3>> _debugLines;
  LinesMeshPtr _debugMesh;
  bool _isEnabled;
  std::function<void(Scene*, EventState&)> _renderFunction;
  UtilityLayerRendererPtr _utilityLayer;

}; // end of class SkeletonViewer

} // end of namespace Debug
} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_SKELETON_VIEWER_H
