#include <babylon/rendering/geometry_buffer_renderer_scene_component.h>

#include <babylon/engine/scene.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/rendering/geometry_buffer_renderer.h>

namespace BABYLON {

GeometryBufferRendererSceneComponent::GeometryBufferRendererSceneComponent(
  Scene* iScene)
{
  scene = iScene;
}

GeometryBufferRendererSceneComponent::~GeometryBufferRendererSceneComponent()
{
}

void GeometryBufferRendererSceneComponent::_register()
{
  scene->_gatherRenderTargetsStage.registerStep(
    SceneComponentConstants::STEP_GATHERRENDERTARGETS_GEOMETRYBUFFERRENDERER,
    this, [this](vector_t<RenderTargetTexturePtr>& renderTargets) {
      _gatherRenderTargets(renderTargets);
    });
}

void GeometryBufferRendererSceneComponent::rebuild()
{
  // Nothing to do for this component
}

void GeometryBufferRendererSceneComponent::dispose()
{
  // Nothing to do for this component
}

void GeometryBufferRendererSceneComponent::_gatherRenderTargets(
  vector_t<RenderTargetTexturePtr>& renderTargets)
{
  if (scene->geometryBufferRenderer()) {
    renderTargets.emplace_back(scene->geometryBufferRenderer()->getGBuffer());
  }
}

} // end of namespace BABYLON
