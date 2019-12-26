#include <babylon/materials/textures/procedurals/procedural_texture_scene_component.h>

#include <babylon/engines/scene.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

ProceduralTextureSceneComponent::ProceduralTextureSceneComponent(Scene* iScene)
{
  ISceneComponent::name = ProceduralTextureSceneComponent::name;
  scene                 = iScene;
}

ProceduralTextureSceneComponent::~ProceduralTextureSceneComponent() = default;

void ProceduralTextureSceneComponent::_register()
{
  scene->_beforeClearStage.registerStep(SceneComponentConstants::STEP_BEFORECLEAR_PROCEDURALTEXTURE,
                                        this, [this]() { _beforeClear(); });
}

void ProceduralTextureSceneComponent::rebuild()
{
  // Nothing to do here.
}

void ProceduralTextureSceneComponent::dispose()
{
  // Nothing to do here.
}

void ProceduralTextureSceneComponent::_beforeClear()
{
  if (scene->proceduralTexturesEnabled) {
    Tools::StartPerformanceCounter("Procedural textures", !scene->proceduralTextures.empty());
    for (const auto& proceduralTexture : scene->proceduralTextures) {
      if (proceduralTexture->_shouldRender()) {
        proceduralTexture->render();
      }
    }
    Tools::EndPerformanceCounter("Procedural textures", !scene->proceduralTextures.empty());
  }
}

} // namespace BABYLON
