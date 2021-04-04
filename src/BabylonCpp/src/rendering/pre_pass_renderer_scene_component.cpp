#include <babylon/rendering/pre_pass_renderer_scene_component.h>

#include <babylon/engines/scene.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/rendering/pre_pass_renderer.h>
#include <babylon/rendering/sub_surface_configuration.h>

namespace BABYLON {

PrePassRendererSceneComponent::PrePassRendererSceneComponent(Scene* iScene) : scene{iScene}
{
}

PrePassRendererSceneComponent::~PrePassRendererSceneComponent() = default;

void PrePassRendererSceneComponent::_register()
{
  scene->_beforeCameraDrawStage.registerStep(SceneComponentConstants::STEP_BEFORECAMERADRAW_PREPASS,
                                             this, [this](Camera* camera) -> bool {
                                               _beforeCameraDraw(camera);
                                               return true;
                                             });
  scene->_afterCameraDrawStage.registerStep(SceneComponentConstants::STEP_AFTERCAMERADRAW_PREPASS,
                                            this, [this](Camera* camera) -> bool {
                                              _afterCameraDraw(camera);
                                              return true;
                                            });
  scene->_beforeRenderTargetDrawStage.registerStep(
    SceneComponentConstants::STEP_BEFORERENDERTARGETDRAW_PREPASS, this,
    [this](const RenderTargetTexturePtr& renderTarget, int faceIndex, int layer) -> void {
      _beforeRenderTargetDraw(renderTarget, faceIndex, layer);
    });
  scene->_afterRenderTargetDrawStage.registerStep(
    SceneComponentConstants::STEP_AFTERCAMERADRAW_PREPASS, this,
    [this](const RenderTargetTexturePtr& renderTarget, int faceIndex, int layer) -> void {
      _afterRenderTargetDraw(renderTarget, faceIndex, layer);
    });

  scene->_beforeClearStage.registerStep(SceneComponentConstants::STEP_BEFORECLEARSTAGE_PREPASS,
                                        this, [this]() -> void { _beforeClearStage(); });
  scene->_beforeRenderTargetClearStage.registerStep(
    SceneComponentConstants::STEP_BEFORERENDERTARGETCLEARSTAGE_PREPASS, this,
    [this](const RenderTargetTexturePtr& renderTarget, int faceIndex, int layer) -> void {
      _beforeRenderTargetClearStage(renderTarget, faceIndex, layer);
    });

  scene->_beforeRenderingMeshStage.registerStep(
    SceneComponentConstants::STEP_BEFORERENDERINGMESH_PREPASS, this,
    [this](Mesh* mesh, SubMesh* subMesh, const _InstancesBatchPtr& batch, Effect* effect) -> void {
      _beforeRenderingMeshStage(mesh, subMesh, batch, effect);
    });
  scene->_afterRenderingMeshStage.registerStep(
    SceneComponentConstants::STEP_AFTERRENDERINGMESH_PREPASS, this,
    [this](Mesh* mesh, SubMesh* /*subMesh*/, const _InstancesBatchPtr& /*batch*/,
           Effect* /*effect*/) -> void { _afterRenderingMeshStage(mesh); });
}

void PrePassRendererSceneComponent::_beforeRenderTargetDraw(
  const RenderTargetTexturePtr& renderTarget, int faceIndex, int layer)
{
  if (scene->prePassRenderer()) {
    scene->prePassRenderer()->_setRenderTarget(renderTarget->_prePassRenderTarget);
    scene->prePassRenderer()->_beforeDraw(nullptr, faceIndex, layer);
  }
}

void PrePassRendererSceneComponent::_afterRenderTargetDraw(
  const RenderTargetTexturePtr& /*renderTarget*/, int faceIndex, int layer)
{
  if (scene->prePassRenderer()) {
    scene->prePassRenderer()->_afterDraw(faceIndex, layer);
  }
}

void PrePassRendererSceneComponent::_beforeRenderTargetClearStage(
  const RenderTargetTexturePtr& renderTarget, int /*faceIndex*/, int /*layer*/)
{
  if (scene->prePassRenderer()) {
    if (!renderTarget->_prePassRenderTarget) {
      renderTarget->_prePassRenderTarget = scene->prePassRenderer()->_createRenderTarget(
        renderTarget->name + "_prePassRTT", renderTarget);
    }
    scene->prePassRenderer()->_setRenderTarget(renderTarget->_prePassRenderTarget);
    scene->prePassRenderer()->_clear();
  }
}

void PrePassRendererSceneComponent::_beforeCameraDraw(Camera* camera)
{
  if (scene->prePassRenderer()) {
    scene->prePassRenderer()->_setRenderTarget(nullptr);
    scene->prePassRenderer()->_beforeDraw(camera);
  }
}

void PrePassRendererSceneComponent::_afterCameraDraw(Camera* /*camera*/)
{
  if (scene->prePassRenderer()) {
    scene->prePassRenderer()->_afterDraw();
  }
}

void PrePassRendererSceneComponent::_beforeClearStage()
{
  if (scene->prePassRenderer()) {
    scene->prePassRenderer()->_setRenderTarget(nullptr);
    scene->prePassRenderer()->_clear();
  }
}

void PrePassRendererSceneComponent::_beforeRenderingMeshStage(AbstractMesh* mesh, SubMesh* subMesh,
                                                              const _InstancesBatchPtr& /*batch*/,
                                                              Effect* effect)
{
  if (!effect) {
    return;
  }

  // Render to MRT
  const auto iScene = mesh->getScene();
  if (iScene->prePassRenderer()) {
    iScene->prePassRenderer()->bindAttachmentsForEffect(*effect, subMesh);
  }
}

void PrePassRendererSceneComponent::_afterRenderingMeshStage(AbstractMesh* mesh)
{
  const auto iScene = mesh->getScene();

  if (iScene->prePassRenderer()) {
    iScene->prePassRenderer()->restoreAttachments();
  }
}

void PrePassRendererSceneComponent::rebuild()
{
  // Release textures first
  scene->disablePrePassRenderer();

  // Re-enable
  scene->enablePrePassRenderer();
}

void PrePassRendererSceneComponent::dispose()
{
  scene->disablePrePassRenderer();
}

} // end of namespace BABYLON
