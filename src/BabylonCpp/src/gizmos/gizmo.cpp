#include <babylon/gizmos/gizmo.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

Gizmo::Gizmo(UtilityLayerRenderer* iGizmoLayer)
    : attachedMesh{this, &Gizmo::get_attachedMesh, &Gizmo::set_attachedMesh}
    , gizmoLayer{iGizmoLayer}
    , updateGizmoRotationToMatchAttachedMesh{true}
    , updateGizmoPositionToMatchAttachedMesh{true}
    , _updateScale{true}
    , _interactionsEnabled{true}
{
  _rootMesh = Mesh::New("gizmoRootNode", gizmoLayer->utilityLayerScene.get());
  _beforeRenderObserver
    = gizmoLayer->utilityLayerScene->onBeforeRenderObservable.add(
      [&](Scene* /*scene*/, EventState& /*es*/) {
        if (_updateScale && gizmoLayer->utilityLayerScene->activeCamera
            && attachedMesh) {
          auto dist
            = attachedMesh()
                ->position()
                .subtract(gizmoLayer->utilityLayerScene->activeCamera->position)
                .length()
              / 3.f;
          _rootMesh->scaling().set(dist, dist, dist);
        }
        if (attachedMesh) {
          if (updateGizmoRotationToMatchAttachedMesh) {
            if (!_rootMesh->rotationQuaternion()) {
              _rootMesh->setRotationQuaternion(Quaternion());
            }
            Quaternion::FromRotationMatrixToRef(
              attachedMesh()->getWorldMatrix()->getRotationMatrix(),
              *_rootMesh->rotationQuaternion());
          }
          if (updateGizmoPositionToMatchAttachedMesh) {
            _rootMesh->position().copyFrom(attachedMesh()->absolutePosition());
          }
        }
      });
  attachedMesh = nullptr;
}

Gizmo::~Gizmo()
{
}

AbstractMesh*& Gizmo::get_attachedMesh()
{
  return _attachedMesh;
}

void Gizmo::set_attachedMesh(AbstractMesh* const& value)
{
  _attachedMesh = value;
  _rootMesh->setEnabled(value ? true : false);
  _attachedMeshChanged(value);
}

void Gizmo::_attachedMeshChanged(AbstractMesh* /*value*/)
{
}

void Gizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  _rootMesh->dispose(doNotRecurse, disposeMaterialAndTextures);
  if (_beforeRenderObserver) {
    gizmoLayer->utilityLayerScene->onBeforeRenderObservable.remove(
      _beforeRenderObserver);
  }
}

} // end of namespace BABYLON
