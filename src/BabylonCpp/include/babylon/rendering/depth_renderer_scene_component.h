#ifndef BABYLON_RENDERING_DEPTH_RENDERING_SCENE_COMPONENT_H
#define BABYLON_RENDERING_DEPTH_RENDERING_SCENE_COMPONENT_H

#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/scene_component_constants.h>

namespace BABYLON {

FWD_CLASS_SPTR(DepthRendererSceneComponent)
FWD_CLASS_SPTR(RenderTargetTexture)

/**
 * @brief Defines the Depth Renderer scene component responsible to manage a depth buffer useful in
 * several rendering techniques.
 */
class BABYLON_SHARED_EXPORT DepthRendererSceneComponent : public ISceneComponent {

public:
  /**
   * The component name helpful to identify the component in the list of scene components.
   */
  static constexpr const char* name = SceneComponentConstants::NAME_DEPTHRENDERER;

public:
  template <typename... Ts>
  static DepthRendererSceneComponentPtr New(Ts&&... args)
  {
    return std::shared_ptr<DepthRendererSceneComponent>(
      new DepthRendererSceneComponent(std::forward<Ts>(args)...));
  }
  ~DepthRendererSceneComponent() override; // = default

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
  DepthRendererSceneComponent(Scene* scene);

private:
  void _gatherRenderTargets(std::vector<RenderTargetTexturePtr>& renderTargets);
  void _gatherActiveCameraRenderTargets(std::vector<RenderTargetTexturePtr>& renderTargets);

}; // end of class DepthRendererSceneComponent

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_DEPTH_RENDERING_SCENE_COMPONENT_H
