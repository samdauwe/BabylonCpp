#include <babylon/gizmos/gizmo.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

Gizmo::Gizmo(const std::shared_ptr<UtilityLayerRenderer>& iGizmoLayer)
    : scaleRatio{this, &Gizmo::get_scaleRatio, &Gizmo::set_scaleRatio}
    , attachedMesh{this, &Gizmo::get_attachedMesh, &Gizmo::set_attachedMesh}
    , gizmoLayer{iGizmoLayer}
    , updateGizmoRotationToMatchAttachedMesh{this,
                                             &Gizmo::
                                               get_updateGizmoRotationToMatchAttachedMesh,
                                             &Gizmo::
                                               set_updateGizmoRotationToMatchAttachedMesh}
    , updateGizmoPositionToMatchAttachedMesh{true}
    , updateScale{true}
    , _customMeshSet{false}
    , _interactionsEnabled{true}
    , _scaleRatio{1.f}
    , _updateGizmoRotationToMatchAttachedMesh{true}
    , _attachedMesh{nullptr}
{
  _rootMesh = Mesh::New("gizmoRootNode", gizmoLayer->utilityLayerScene.get());
  _rootMesh->rotationQuaternion = Quaternion::Identity();
  _beforeRenderObserver
    = gizmoLayer->utilityLayerScene->onBeforeRenderObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) { _update(); });
  attachedMesh = nullptr;
}

Gizmo::~Gizmo() = default;

void Gizmo::set_scaleRatio(float value)
{
  _scaleRatio = value;
}

float Gizmo::get_scaleRatio() const
{
  return _scaleRatio;
}

AbstractMeshPtr& Gizmo::get_attachedMesh()
{
  return _attachedMesh;
}

void Gizmo::set_attachedMesh(const AbstractMeshPtr& value)
{
  _attachedMesh = value;
  _rootMesh->setEnabled(value ? true : false);
  _attachedMeshChanged(value);
}

void Gizmo::set_updateGizmoRotationToMatchAttachedMesh(bool value)
{
  _updateGizmoRotationToMatchAttachedMesh = value;
}

bool Gizmo::get_updateGizmoRotationToMatchAttachedMesh() const
{
  return _updateGizmoRotationToMatchAttachedMesh;
}

void Gizmo::setCustomMesh(const MeshPtr& mesh, bool /*useGizmoMaterial*/)
{
  if (mesh->getScene() != gizmoLayer->utilityLayerScene.get()) {
    throw std::runtime_error(
      "When setting a custom mesh on a gizmo, the custom meshes scene must be "
      "the same as the gizmos (eg. gizmo.gizmoLayer.utilityLayerScene)");
  }
  for (const auto& c : _rootMesh->getChildMeshes()) {
    c->dispose();
  }
  mesh->parent   = _rootMesh.get();
  _customMeshSet = true;
}

void Gizmo::_attachedMeshChanged(const AbstractMeshPtr& /*value*/)
{
}

void Gizmo::_update()
{
  if (attachedMesh()) {
    auto effectiveMesh = attachedMesh()->_effectiveMesh() ?
                           attachedMesh()->_effectiveMesh() :
                           attachedMesh().get();

    // Position
    if (updateGizmoPositionToMatchAttachedMesh) {
      _rootMesh->position().copyFrom(effectiveMesh->absolutePosition);
    }

    // Rotation
    if (updateGizmoRotationToMatchAttachedMesh) {
      std::optional<Vector3> scale       = std::nullopt;
      std::optional<Vector3> translation = std::nullopt;
      effectiveMesh->getWorldMatrix().decompose(
        scale, _rootMesh->rotationQuaternion(), translation);
    }
    else {
      _rootMesh->rotationQuaternion()->set(0.f, 0.f, 0.f, 1.f);
    }

    // Scale
    if (updateScale) {
      const auto& activeCamera = gizmoLayer->utilityLayerScene->activeCamera();
      const auto& cameraPosition = activeCamera->globalPosition();
      _rootMesh->position().subtractToRef(cameraPosition, _tempVector);
      const auto dist = _tempVector.length() * scaleRatio;
      _rootMesh->scaling().set(dist, dist, dist);

      // Account for handedness, similar to Matrix.decompose
      if (effectiveMesh->_getWorldMatrixDeterminant() < 0.f) {
        _rootMesh->scaling().y *= -1.f;
      }
    }
    else {
      _rootMesh->scaling().setAll(scaleRatio());
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
