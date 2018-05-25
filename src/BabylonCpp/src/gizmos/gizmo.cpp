#include <babylon/gizmos/gizmo.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

Gizmo::Gizmo(UtilityLayerRenderer* iGizmoLayer) : gizmoLayer{iGizmoLayer}
{
  _rootMesh = Mesh::New("gizmoRootNode", gizmoLayer->utilityLayerScene.get());
  _beforeRenderObserver
    = gizmoLayer->utilityLayerScene->onBeforeRenderObservable.add(
      [&](Scene* /*scene*/, EventState& /*es*/) {
        if (gizmoLayer->utilityLayerScene->activeCamera && attachedMesh) {
          auto dist
            = attachedMesh->position()
                .subtract(gizmoLayer->utilityLayerScene->activeCamera->position)
                .length()
              / 5.f;
          _rootMesh->scaling().set(dist, dist, dist);
        }
        if (attachedMesh) {
          _rootMesh->position().copyFrom(attachedMesh->position());
        }
      });
}

Gizmo::~Gizmo()
{
}

void Gizmo::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  _rootMesh->dispose();
  if (_beforeRenderObserver) {
    gizmoLayer->utilityLayerScene->onBeforeRenderObservable.remove(
      _beforeRenderObserver);
  }
}

} // end of namespace BABYLON
