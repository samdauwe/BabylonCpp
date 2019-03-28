#include <babylon/materials/textures/procedurals/procedural_texture_scene_component.h>

#include <babylon/engine/scene.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

ProceduralTextureSceneComponent::ProceduralTextureSceneComponent(Scene* iScene)
{
  ISceneComponent::name = ProceduralTextureSceneComponent::name;
  scene                 = iScene;
}

ProceduralTextureSceneComponent::~ProceduralTextureSceneComponent()
{
}

void ProceduralTextureSceneComponent::_register()
{
  scene->_beforeClearStage.registerStep(
    SceneComponentConstants::STEP_BEFORECLEAR_PROCEDURALTEXTURE, this,
    [this]() { _beforeClear(); });
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
    Tools::StartPerformanceCounter("Procedural textures",
                                   scene->proceduralTextures.size() > 0);
    for (const auto& proceduralTexture : scene->proceduralTextures) {
      if (proceduralTexture->_shouldRender()) {
        proceduralTexture->render();
      }
    }
    Tools::EndPerformanceCounter("Procedural textures",
                                 scene->proceduralTextures.size() > 0);
  }
}

} // namespace BABYLON
