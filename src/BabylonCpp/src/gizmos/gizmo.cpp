#include <babylon/gizmos/gizmo.h>

#include <babylon/bones/bone.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

Gizmo::Gizmo(const std::shared_ptr<UtilityLayerRenderer>& iGizmoLayer)
    : scaleRatio{this, &Gizmo::get_scaleRatio, &Gizmo::set_scaleRatio}
    , attachedMesh{this, &Gizmo::get_attachedMesh, &Gizmo::set_attachedMesh}
    , attachedNode{this, &Gizmo::get_attachedNode, &Gizmo::set_attachedNode}
    , gizmoLayer{iGizmoLayer}
    , updateGizmoRotationToMatchAttachedMesh{this,
                                             &Gizmo::get_updateGizmoRotationToMatchAttachedMesh,
                                             &Gizmo::set_updateGizmoRotationToMatchAttachedMesh}
    , updateGizmoPositionToMatchAttachedMesh{true}
    , updateScale{true}
    , _customMeshSet{false}
    , _interactionsEnabled{true}
    , _scaleRatio{1.f}
    , _updateGizmoRotationToMatchAttachedMesh{true}
    , _attachedMesh{nullptr}
    , _attachedNode{nullptr}
{
  _rootMesh                     = Mesh::New("gizmoRootNode", gizmoLayer->utilityLayerScene.get());
  _rootMesh->rotationQuaternion = Quaternion::Identity();
  _beforeRenderObserver         = gizmoLayer->utilityLayerScene->onBeforeRenderObservable.add(
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
  if (value) {
    _attachedNode = value;
  }
  _rootMesh->setEnabled(value ? true : false);
  _attachedNodeChanged(value);
}

NodePtr& Gizmo::get_attachedNode()
{
  return _attachedNode;
}

void Gizmo::set_attachedNode(const NodePtr& value)
{
  _attachedNode = value;
  _attachedMesh = nullptr;
  _rootMesh->setEnabled(value ? true : false);
  _attachedNodeChanged(value);
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
      "When setting a custom mesh on a gizmo, the custom meshes scene must be the same as the "
      "gizmos (eg. gizmo.gizmoLayer.utilityLayerScene)");
  }
  for (const auto& c : _rootMesh->getChildMeshes()) {
    c->dispose();
  }
  mesh->parent   = _rootMesh.get();
  _customMeshSet = true;
}

void Gizmo::_attachedNodeChanged(const NodePtr& /*value*/)
{
}

void Gizmo::_update()
{
  if (attachedNode()) {
    auto effectiveNode = attachedNode().get();
    if (attachedMesh()) {
      effectiveNode = attachedMesh()->_effectiveMesh() ?
                        attachedMesh()->_effectiveMesh() :
                        static_cast<AbstractMesh*>(attachedNode().get());
    }

    // Position
    if (updateGizmoPositionToMatchAttachedMesh) {
      const auto row      = effectiveNode->getWorldMatrix().getRow(3);
      const auto position = row ? row->toVector3() : Vector3(0.f, 0.f, 0.f);
      _rootMesh->position().copyFrom(position);
    }

    // Rotation
    if (updateGizmoRotationToMatchAttachedMesh) {
      std::optional<Vector3> scale       = std::nullopt;
      std::optional<Vector3> translation = std::nullopt;
      effectiveNode->getWorldMatrix().decompose(scale, _rootMesh->rotationQuaternion(),
                                                translation);
    }
    else {
      _rootMesh->rotationQuaternion()->set(0.f, 0.f, 0.f, 1.f);
    }

    // Scale
    if (updateScale) {
      const auto& activeCamera   = gizmoLayer->utilityLayerScene->activeCamera();
      const auto& cameraPosition = activeCamera->globalPosition();
      _rootMesh->position().subtractToRef(cameraPosition, _tempVector);
      const auto dist = _tempVector.length() * scaleRatio;
      _rootMesh->scaling().set(dist, dist, dist);

      // Account for handedness, similar to Matrix.decompose
      if (effectiveNode->_getWorldMatrixDeterminant() < 0.f) {
        _rootMesh->scaling().y *= -1.f;
      }
    }
    else {
      _rootMesh->scaling().setAll(scaleRatio());
    }
  }
}

void Gizmo::_matrixChanged()
{
  if (!_attachedNode) {
    return;
  }
  if (_attachedNode->getClassName() == "Mesh" || _attachedNode->getClassName() == "TransformNode") {
    auto transform = std::static_pointer_cast<TransformNode>(_attachedNode);
    Quaternion transformQuaternion(0.f, 0.f, 0.f, 1.f);
    if (transform->parent()) {
      Matrix parentInv;
      Matrix localMat;
      transform->parent()->getWorldMatrix().invertToRef(parentInv);
      _attachedNode->_worldMatrix.multiplyToRef(parentInv, localMat);
      std::optional<Vector3> iScaling                = transform->scaling();
      std::optional<Quaternion> iTransformQuaternion = transformQuaternion;
      std::optional<Vector3> iPosition               = transform->position();
      localMat.decompose(iScaling, iTransformQuaternion, iPosition);
      transform->scaling  = *iScaling;
      transformQuaternion = *iTransformQuaternion;
      transform->position = *iPosition;
    }
    else {
      std::optional<Vector3> iScaling                = transform->scaling();
      std::optional<Quaternion> iTransformQuaternion = transformQuaternion;
      std::optional<Vector3> iPosition               = transform->position();
      _attachedNode->_worldMatrix.decompose(iScaling, iTransformQuaternion, iPosition);
      transform->scaling  = *iScaling;
      transformQuaternion = *iTransformQuaternion;
      transform->position = *iPosition;
    }
    transform->rotation = transformQuaternion.toEulerAngles();
    if (transform->rotationQuaternion()) {
      transform->rotationQuaternion = transformQuaternion;
    }
  }
  else if (_attachedNode->getClassName() == "Bone") {
    const auto bone   = std::static_pointer_cast<Bone>(_attachedNode);
    const auto parent = bone->getParent();

    if (parent) {
      Matrix invParent;
      Matrix boneLocalMatrix;
      parent->getWorldMatrix().invertToRef(invParent);
      bone->getWorldMatrix().multiplyToRef(invParent, boneLocalMatrix);
      auto& lmat = bone->getLocalMatrix();
      lmat.copyFrom(boneLocalMatrix);
      bone->markAsDirty();
    }
  }
}

void Gizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  _rootMesh->dispose(doNotRecurse, disposeMaterialAndTextures);
  if (_beforeRenderObserver) {
    gizmoLayer->utilityLayerScene->onBeforeRenderObservable.remove(_beforeRenderObserver);
  }
}

} // end of namespace BABYLON
