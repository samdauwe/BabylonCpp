#ifndef BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_SCENE_COMPONENT_H
#define BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_SCENE_COMPONENT_H

#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/scene_component_constants.h>

namespace BABYLON {

class ProceduralTextureSceneComponent;
using ProceduralTextureSceneComponentPtr
  = std::shared_ptr<ProceduralTextureSceneComponent>;

/**
 * @brief Defines the Procedural Texture scene component responsible to manage
 * any Procedural Texture in a given scene.
 */
class BABYLON_SHARED_EXPORT ProceduralTextureSceneComponent
    : public ISceneComponent {

public:
  /**
   * The component name helpfull to identify the component in the list of scene
   * components.
   */
  static constexpr const char* name
    = SceneComponentConstants::NAME_PROCEDURALTEXTURE;

public:
  template <typename... Ts>
  static ProceduralTextureSceneComponentPtr New(Ts&&... args)
  {
    return std::shared_ptr<ProceduralTextureSceneComponent>(
      new ProceduralTextureSceneComponent(std::forward<Ts>(args)...));
  }
  virtual ~ProceduralTextureSceneComponent();

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
  ProceduralTextureSceneComponent(Scene* scene);

private:
  void _beforeClear();

}; // end of class DepthRendererSceneComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_SCENE_COMPONENT_H
