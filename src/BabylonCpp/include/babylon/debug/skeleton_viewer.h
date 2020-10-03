#ifndef BABYLON_DEBUG_SKELETON_VIEWER_H
#define BABYLON_DEBUG_SKELETON_VIEWER_H

#include <set>

#include <babylon/babylon_api.h>
#include <babylon/debug/iskeleton_viewer.h>
#include <babylon/maths/color3.h>
#include <babylon/misc/event_state.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class AbstractMesh;
class Bone;
class LinesMesh;
class Matrix;
class Scene;
class ShaderMaterial;
class Skeleton;
class StandardMaterial;
class Vector3;
class UtilityLayerRenderer;
using BonePtr                 = std::shared_ptr<Bone>;
using AbstractMeshPtr         = std::shared_ptr<AbstractMesh>;
using LinesMeshPtr            = std::shared_ptr<LinesMesh>;
using ShaderMaterialPtr       = std::shared_ptr<ShaderMaterial>;
using SkeletonPtr             = std::shared_ptr<Skeleton>;
using StandardMaterialPtr     = std::shared_ptr<StandardMaterial>;
using UtilityLayerRendererPtr = std::shared_ptr<UtilityLayerRenderer>;

namespace Debug {

/**
 * @brief Class used to render a debug view of a given skeleton.
 * @see http://www.babylonjs-playground.com/#1BZJVJ#8
 */
class BABYLON_SHARED_EXPORT SkeletonViewer {

public:
  /**
   * public Display constants BABYLON.SkeletonViewer.DISPLAY_LINES
   */
  static constexpr unsigned int DISPLAY_LINES = 0;

  /**
   * public Display constants BABYLON.SkeletonViewer.DISPLAY_SPHERES
   */
  static constexpr unsigned int DISPLAY_SPHERES = 1;

  /**
   * public Display constants BABYLON.SkeletonViewer.DISPLAY_SPHERE_AND_SPURS
   */
  static constexpr unsigned int DISPLAY_SPHERE_AND_SPURS = 2;

public:
  /**
   * @brief public static method to create a BoneWeight Shader
   * @param options The constructor options
   * @param scene The scene that the shader is scoped to
   * @returns The created ShaderMaterial
   * @see http://www.babylonjs-playground.com/#1BZJVJ#395
   */
  static ShaderMaterialPtr CreateBoneWeightShader(const IBoneWeightShaderOptions& options,
                                                  Scene* scene);

  /**
   * @brief public static method to create a BoneWeight Shader.
   * @param options The constructor options
   * @param scene The scene that the shader is scoped to
   * @returns The created ShaderMaterial
   */
  static ShaderMaterialPtr CreateSkeletonMapShader(const ISkeletonMapShaderOptions& options,
                                                   Scene* scene);

public:
  /**
   * @brief Creates a new SkeletonViewer
   * @param skeleton defines the skeleton to render
   * @param mesh defines the mesh attached to the skeleton
   * @param scene defines the hosting scene
   * @param autoUpdateBonesMatrices defines a boolean indicating if bones matrices must be forced to
   * update before rendering (true by default)
   * @param renderingGroupId defines the rendering group id to use with the viewer
   * @param options All of the extra constructor options for the SkeletonViewer
   */
  SkeletonViewer(const SkeletonPtr& skeleton, const AbstractMeshPtr& mesh, Scene* scene,
                 bool autoUpdateBonesMatrices = true, int renderingGroupId = 3,
                 const ISkeletonViewerOptions& options = {});
  ~SkeletonViewer(); // = default

  /**
   * @brief Update the viewer to sync with current skeleton state, only used to manually update.
   */
  void update();

  /**
   * @brief Changes the displayMode of the skeleton viewer.
   * @param mode The displayMode numerical value
   */
  void changeDisplayMode(unsigned int mode);

  /**
   * @brief Changes the displayMode of the skeleton viewer.
   * @param option String of the option name
   * @param value The numerical option value
   */
  void changeDisplayOptions(const std::string& option, float value);

  /**
   * @brief Release associated resources.
   */
  void dispose();

protected:
  /**
   * @brief Gets the Scene.
   */
  Scene*& get_scene();

  /**
   * @brief Gets the utilityLayer.
   */
  UtilityLayerRendererPtr& get_utilityLayer();

  /**
   * @brief Checks Ready Status.
   */
  bool get_isReady() const;

  /**
   * @brief Sets Ready Status.
   */
  void set_ready(bool value);

  /**
   * @brief Returns the mesh used to render the bones.
   */
  AbstractMeshPtr& get_debugMesh();

  /**
   * @brief Sets the debugMesh
   */
  void set_debugMesh(const AbstractMeshPtr& value);

  /**
   * @brief Gets the material
   */
  StandardMaterialPtr& get_material();

  /**
   * @brief Sets the material
   */
  void set_material(const StandardMaterialPtr& value);

  /**
   * @brief Gets the material
   */
  unsigned int get_displayMode() const;

  /**
   * @brief Sets the material
   */
  void set_displayMode(unsigned int value);

  /**
   * @brief Sets a boolean indicating if the viewer is enabled.
   */
  void set_isEnabled(bool value);

  /**
   * @brief Gets a boolean indicating if the viewer is enabled.
   */
  bool get_isEnabled() const;

private:
  /**
   * @brief The Dynamic bindings for the update functions.
   */
  void _bindObs();

  void _getBonePosition(Vector3& position, const Bone& bone, const Matrix& meshMat, float x = 0.f,
                        float y = 0.f, float z = 0.f) const;
  void _resizeDebugLines(size_t bonesSize);
  void _getLinesForBonesWithLength(const std::vector<BonePtr>& bones, const Matrix& meshMat);
  void _getLinesForBonesNoLength(const std::vector<BonePtr>& bones);

  /**
   * @brief function to revert the mesh and scene back to the initial state.
   */
  void _revert(bool animationState);

  /**
   * @brief function to build and bind sphere joint points and spur bone representations.
   */
  void _buildSpheresAndSpurs(bool spheresOnly = true);

  /**
   * @brief Update the viewer to sync with current skeleton state, only used for the line display.
   */
  void _displayLinesUpdate();

public:
  /**
   * Gets or sets the color used to render the skeleton
   */
  Color3 color;

  /**
   * Gets the Scene.
   */
  ReadOnlyProperty<SkeletonViewer, Scene*> scene;

  /**
   * Gets the utilityLayer.
   */
  ReadOnlyProperty<SkeletonViewer, UtilityLayerRendererPtr> utilityLayer;

  /**
   * Checks Ready Status.
   */
  ReadOnlyProperty<SkeletonViewer, bool> isReady;

  /**
   * Sets Ready Status.
   */
  WriteOnlyProperty<SkeletonViewer, bool> ready;

  /**
   *
   * Returns the mesh used to render the bones
   */
  Property<SkeletonViewer, AbstractMeshPtr> debugMesh;

  /**
   * Gets the material */
  Property<SkeletonViewer, StandardMaterialPtr> material;

  /**
   * Gets the material
   */
  Property<SkeletonViewer, unsigned int> displayMode;

  /**
   * Defines the skeleton to render
   */
  SkeletonPtr skeleton;

  /**
   * Defines the mesh attached to the skeleton
   */
  AbstractMeshPtr mesh;

  /**
   * Defines a boolean indicating if bones matrices must be forced to update before rendering
   * (true by default)
   */
  bool autoUpdateBonesMatrices;

  /**
   * Defines the rendering group id to use with the viewer
   */
  int renderingGroupId;

  /**
   * is the options for the viewer
   */
  ISkeletonViewerOptions options;

  /**
   * Gets or sets a boolean indicating if the viewer is enabled
   */
  Property<SkeletonViewer, bool> isEnabled;

private:
  /**
   * @brief private static method to create a BoneWeight Shader.
   * @param size The size of the buffer to create (usually the bone count)
   * @param colorMap The gradient data to generate
   * @param scene The scene that the shader is scoped to
   * @returns an Array of floats from the color gradient values
   */
  static Float32Array _CreateBoneMapColorBuffer(size_t size,
                                   const std::vector<ISkeletonMapShaderColorMapKnot>& colorMap,
                                   Scene* scene);

private:
  /**
   * The Scene scope.
   */
  Scene* _scene;

  /**
   * Array of the points of the skeleton fo the line view.
   */
  std::vector<std::vector<Vector3>> _debugLines;

  /**
   * The SkeletonViewers Mesh.
   */
  AbstractMeshPtr _debugMesh;

  /**
   * If SkeletonViewer is enabled.
   */
  bool _isEnabled;

  /**
   * If SkeletonViewer is ready.
   */
  bool _ready;

  /**
   * SkeletonViewer render observable.
   */
  Observer<Scene>::Ptr _obs;

  /**
   * The Utility Layer to render the gizmos in.
   */
  UtilityLayerRendererPtr _utilityLayer;

  std::set<int> _boneIndices;

}; // end of class SkeletonViewer

} // end of namespace Debug
} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_SKELETON_VIEWER_H
