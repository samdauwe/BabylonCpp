#ifndef BABYLON_RENDERING_PRE_PASS_RENDERER_SCENE_COMPONENT_H
#define BABYLON_RENDERING_PRE_PASS_RENDERER_SCENE_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/scene_component_constants.h>

namespace BABYLON {

struct _InstancesBatch;
class Effect;
class AbstractMesh;
class AbstractScene;
class SubMesh;
FWD_STRUCT_SPTR(_InstancesBatch)
FWD_CLASS_SPTR(PrePassRendererSceneComponent)

/**
 * @brief Defines the Geometry Buffer scene component responsible to manage a G-Buffer useful
 * in several rendering techniques.
 */
class BABYLON_SHARED_EXPORT PrePassRendererSceneComponent : public ISceneComponent {

public:
  /**
   * The component name helpfull to identify the component in the list of scene components.
   */
  static constexpr const char* name = SceneComponentConstants::NAME_PREPASSRENDERER;

public:
  template <typename... Ts>
  static PrePassRendererSceneComponentPtr New(Ts&&... args)
  {
    return std::shared_ptr<PrePassRendererSceneComponent>(
      new PrePassRendererSceneComponent(std::forward<Ts>(args)...));
  }
  ~PrePassRendererSceneComponent() override; // = default

  /**
   * @brief Registers the component in a given scene.
   */
  void _register() override;

  /**
   * @brief Rebuilds the elements related to this component in case of context lost for instance.
   */
  void rebuild() override;

  /**
   * @brief Disposes the component and the associated resources.
   */
  void dispose() override;

protected:
  /**
   * @brief Creates a new instance of the component for the given scene.
   * @param scene Defines the scene to register the component in
   */
  PrePassRendererSceneComponent(Scene* scene);

private:
  void _beforeCameraDraw();
  void _afterCameraDraw();
  void _beforeClearStage();
  void _beforeRenderingMeshStage(AbstractMesh* mesh, SubMesh* subMesh,
                                 const _InstancesBatchPtr& batch, Effect* effect);
  void _afterRenderingMeshStage(AbstractMesh* mesh);

public:
  /**
   * The scene the component belongs to.
   */
  Scene* scene;

}; // end of class PrePassRendererSceneComponent

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_PRE_PASS_RENDERER_SCENE_COMPONENT_H
