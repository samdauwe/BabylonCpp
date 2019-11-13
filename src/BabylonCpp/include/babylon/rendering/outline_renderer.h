#ifndef BABYLON_RENDERING_OUTLINE_RENDERER_H
#define BABYLON_RENDERING_OUTLINE_RENDERER_H

#include <string>

#include <babylon/babylon_api.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/scene_component_constants.h>

namespace BABYLON {

struct _InstancesBatch;
class AbstractMesh;
class OutlineRenderer;
class Effect;
class Engine;
class Scene;
class SubMesh;
using _InstancesBatchPtr = std::shared_ptr<_InstancesBatch>;
using EffectPtr          = std::shared_ptr<Effect>;
using OutlineRendererPtr = std::shared_ptr<OutlineRenderer>;

/**
 * @brief This class is responsible to draw bothe outline/overlay of meshes.
 * It should not be used directly but through the available method on mesh.
 */
class BABYLON_SHARED_EXPORT OutlineRenderer : public ISceneComponent {

private:
  /**
   * Stencil value used to avoid outline being seen within the mesh when the
   * mesh is transparent
   */
  static constexpr int _StencilReference = 0x04;

public:
  /**
   *The name of the component. Each component must have a unique name.
   */
  static constexpr const char* name
    = SceneComponentConstants::NAME_OUTLINERENDERER;

public:
  template <typename... Ts>
  static OutlineRendererPtr New(Ts&&... args)
  {
    auto outlineRenderer = std::shared_ptr<OutlineRenderer>(
      new OutlineRenderer(std::forward<Ts>(args)...));
    outlineRenderer->addToScene(outlineRenderer);

    return outlineRenderer;
  }
  virtual ~OutlineRenderer(); // = default

  void addToScene(const OutlineRendererPtr& outlineRenderer);

  /**
   * @brief Registers the component in a given scene.
   */
  void _register() override;

  /**
   * @brief Rebuilds the elements related to this component in case of
   * context lost for instance.
   */
  void rebuild() override;

  /**
   * @brief Disposes the component and the associated resources.
   */
  void dispose() override;

  /**
   * @brief Renders the outline in the canvas.
   * @param subMesh Defines the sumesh to render
   * @param batch Defines the batch of meshes in case of instances
   * @param useOverlay Defines if the rendering is for the overlay or the
   * outline
   */
  void render(SubMesh* subMesh, const _InstancesBatchPtr& batch,
              bool useOverlay = false);

  /**
   * @brief Returns whether or not the outline renderer is ready for a given
   * submesh. All the dependencies e.g. submeshes, texture, effect... mus be
   * ready
   * @param subMesh Defines the submesh to check readyness for
   * @param useInstances Defines wheter wee are trying to render instances or
   * not
   * @returns true if ready otherwise false
   */
  bool isReady(SubMesh* subMesh, bool useInstances);

protected:
  /**
   * @brief Instantiates a new outline renderer. (There could be only one per
   * scene).
   * @param scene Defines the scene it belongs to
   */
  OutlineRenderer(Scene* scene);

private:
  void _beforeRenderingMesh(AbstractMesh* mesh, SubMesh* subMesh,
                            const _InstancesBatchPtr& batch);
  void _afterRenderingMesh(AbstractMesh* mesh, SubMesh* subMesh,
                           const _InstancesBatchPtr& batch);

public:
  /**
   * Defines a zOffset to prevent zFighting between the overlay and the mesh.
   */
  float zOffset;

private:
  Engine* _engine;
  EffectPtr _effect;
  std::string _cachedDefines;
  bool _savedDepthWrite;

}; // end of class OutlineRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_OUTLINE_RENDERER_H
