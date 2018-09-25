#ifndef BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_SCENE_COMPONENT_H
#define BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_SCENE_COMPONENT_H

#include <babylon/babylon_global.h>
#include <babylon/engine/iscene_component.h>
#include <babylon/engine/scene_component_constants.h>

namespace BABYLON {

/**
 * @brief Defines the Geometry Buffer scene component responsible to manage a
 * G-Buffer usefull in several rendering techniques.
 */
class BABYLON_SHARED_EXPORT GeometryBufferRendererSceneComponent
    : public ISceneComponent {

public:
  /**
   * The component name helpfull to identify the component in the list of scene
   * components.
   */
  static constexpr const char* name
    = SceneComponentConstants::NAME_GEOMETRYBUFFERRENDERER;

public:
  template <typename... Ts>
  static GeometryBufferRendererSceneComponentPtr New(Ts&&... args)
  {
    return shared_ptr_t<GeometryBufferRendererSceneComponent>(
      new GeometryBufferRendererSceneComponent(std::forward<Ts>(args)...));
  }
  virtual ~GeometryBufferRendererSceneComponent();

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

protected:
  /**
   * @brief Creates a new instance of the component for the given scene.
   * @param scene Defines the scene to register the component in
   */
  GeometryBufferRendererSceneComponent(Scene* scene);

private:
  void _gatherRenderTargets(vector_t<RenderTargetTexturePtr>& renderTargets);

}; // end of class GeometryBufferRendererSceneComponent

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_SCENE_COMPONENT_H
