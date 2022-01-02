#include <babylon/rendering/pre_pass_renderer_scene_component.h>

#include <babylon/engines/scene.h>
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
                                             this, [this](Camera * /*camera*/) -> bool {
                                               _beforeCameraDraw();
                                               return true;
                                             });
  scene->_afterCameraDrawStage.registerStep(SceneComponentConstants::STEP_AFTERCAMERADRAW_PREPASS,
                                            this, [this](Camera * /*camera*/) -> bool {
                                              _afterCameraDraw();
                                              return true;
                                            });
  scene->_beforeClearStage.registerStep(SceneComponentConstants::STEP_BEFORECLEARSTAGE_PREPASS,
                                        this, [this]() -> void { _beforeClearStage(); });
  scene->_beforeRenderingMeshStage.registerStep(
    SceneComponentConstants::STEP_BEFORERENDERINGMESH_PREPASS, this,
    [this](Mesh* mesh, SubMesh* subMesh, const _InstancesBatchPtr& batch, Effect* effect) -> void {
      _beforeRenderingMeshStage(mesh, subMesh, batch, effect);
    });
  scene->_afterRenderingMeshStage.registerStep(
    SceneComponentConstants::STEP_AFTERRENDERINGMESH_PREPASS, this,
    [this](Mesh* mesh, SubMesh* /*subMesh*/, const _InstancesBatchPtr& /*batch*/,
           Effect * /*effect*/) -> void { _afterRenderingMeshStage(mesh); });
}

void PrePassRendererSceneComponent::_beforeCameraDraw()
{
  if (scene->prePassRenderer()) {
    scene->prePassRenderer()->_beforeCameraDraw();
  }
}

void PrePassRendererSceneComponent::_afterCameraDraw()
{
  if (scene->prePassRenderer()) {
    scene->prePassRenderer()->_afterCameraDraw();
  }
}

void PrePassRendererSceneComponent::_beforeClearStage()
{
  if (scene->prePassRenderer()) {
    scene->prePassRenderer()->clear();
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
  if (iScene->prePassRenderer() && effect) {
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
  // Nothing to do for this component
}

void PrePassRendererSceneComponent::dispose()
{
  // Nothing to do for this component
}

} // end of namespace BABYLON
