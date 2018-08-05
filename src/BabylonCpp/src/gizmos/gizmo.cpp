#include <babylon/gizmos/gizmo.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

Gizmo::Gizmo(const shared_ptr_t<UtilityLayerRenderer>& iGizmoLayer)
    : attachedMesh{this, &Gizmo::get_attachedMesh, &Gizmo::set_attachedMesh}
    , gizmoLayer{iGizmoLayer}
    , updateGizmoRotationToMatchAttachedMesh{true}
    , updateGizmoPositionToMatchAttachedMesh{true}
    , _customMeshSet{false}
    , _updateScale{true}
    , _interactionsEnabled{true}
    , _scaleFactor{3.f}
{
  _rootMesh = Mesh::New("gizmoRootNode", gizmoLayer->utilityLayerScene.get());
  _beforeRenderObserver
    = gizmoLayer->utilityLayerScene->onBeforeRenderObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) { _update(); });
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

void Gizmo::setCustomMesh(Mesh* mesh)
{
  if (mesh->getScene() != gizmoLayer->utilityLayerScene.get()) {
    throw ::std::runtime_error(
      "When setting a custom mesh on a gizmo, the custom meshes scene must be "
      "the same as the gizmos (eg. gizmo.gizmoLayer.utilityLayerScene)");
  }
  for (auto& c : _rootMesh->getChildMeshes()) {
    c->dispose();
  }
  mesh->parent   = _rootMesh;
  _customMeshSet = true;
}

void Gizmo::_attachedMeshChanged(AbstractMesh* /*value*/)
{
}

void Gizmo::_update()
{
  if (attachedMesh) {
    if (updateGizmoRotationToMatchAttachedMesh) {
      if (!_rootMesh->rotationQuaternion()) {
        _rootMesh->setRotationQuaternion(Quaternion::RotationYawPitchRoll(
          _rootMesh->rotation().y, _rootMesh->rotation().x,
          _rootMesh->rotation().z));
      }

      // Remove scaling before getting rotation matrix to get rotation matrix
      // unmodified by scale
      _tempVector.copyFrom(attachedMesh()->scaling());
      if (attachedMesh()->scaling().x < 0.f) {
        attachedMesh()->scaling().x *= -1.f;
      }
      if (attachedMesh()->scaling().y < 0.f) {
        attachedMesh()->scaling().y *= -1.f;
      }
      if (attachedMesh()->scaling().z < 0.f) {
        attachedMesh()->scaling().z *= -1.f;
      }
      attachedMesh()->computeWorldMatrix().getRotationMatrixToRef(_tmpMatrix);
      attachedMesh()->scaling().copyFrom(_tempVector);
      attachedMesh()->computeWorldMatrix();
      Quaternion::FromRotationMatrixToRef(_tmpMatrix,
                                          *_rootMesh->rotationQuaternion());
    }
    if (updateGizmoPositionToMatchAttachedMesh) {
      _rootMesh->position().copyFrom(attachedMesh()->absolutePosition());
    }
    if (_updateScale && gizmoLayer->utilityLayerScene->activeCamera
        && attachedMesh) {
      auto cameraPosition
        = gizmoLayer->utilityLayerScene->activeCamera->position;
      /*
      if((<WebVRFreeCamera>gizmoLayer.utilityLayerScene.activeCamera).devicePosition){
          cameraPosition =
      (<WebVRFreeCamera>gizmoLayer.utilityLayerScene.activeCamera).devicePosition;
      }*/
      _rootMesh->position().subtractToRef(cameraPosition, _tempVector);
      auto dist = _tempVector.length() / _scaleFactor;
      _rootMesh->scaling().set(dist, dist, dist);
    }
  }
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
