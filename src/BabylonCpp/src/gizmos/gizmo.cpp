#include <babylon/gizmos/gizmo.h>

#include <babylon/bones/bone.h>
#include <babylon/cameras/camera.h>
#include <babylon/cameras/target_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

Gizmo::Gizmo(const std::shared_ptr<UtilityLayerRenderer>& iGizmoLayer)
    : scaleRatio{this, &Gizmo::get_scaleRatio, &Gizmo::set_scaleRatio}
    , isHovered{this, &Gizmo::get_isHovered}
    , attachedMesh{this, &Gizmo::get_attachedMesh, &Gizmo::set_attachedMesh}
    , attachedNode{this, &Gizmo::get_attachedNode, &Gizmo::set_attachedNode}
    , gizmoLayer{iGizmoLayer}
    , updateGizmoRotationToMatchAttachedMesh{this,
                                             &Gizmo::get_updateGizmoRotationToMatchAttachedMesh,
                                             &Gizmo::set_updateGizmoRotationToMatchAttachedMesh}
    , updateGizmoPositionToMatchAttachedMesh{true}
    , updateScale{true}
    , _scaleRatio{1.f}
    , _isHovered{false}
    , _customMeshSet{false}
    , _updateGizmoRotationToMatchAttachedMesh{true}
    , _interactionsEnabled{true}
    , _attachedMesh{nullptr}
    , _attachedNode{nullptr}
    , _beforeRenderObserver{nullptr}
    , _tempQuaternion{Quaternion(0.f, 0.f, 0.f, 1.f)}
    , _tempVector{Vector3()}
    , _tempVector2{Vector3()}
    , _rightHandtoLeftHandMatrix{Matrix::RotationY(Math::PI)}
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

bool Gizmo::get_isHovered() const
{
  return _isHovered;
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
      _rootMesh->position().subtractToRef(cameraPosition, *_tempVector);
      const auto dist = _tempVector->length() * scaleRatio;
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

  auto camera = std::static_pointer_cast<Camera>(_attachedNode);

  if (camera && camera->_isCamera) {
    Matrix worldMatrix;
    Matrix worldMatrixUC;
    if (camera->parent()) {
      auto& parentInv = _tempMatrix2;
      camera->parent()->_worldMatrix.invertToRef(parentInv);
      _attachedNode->_worldMatrix.multiplyToRef(parentInv, _tempMatrix1);
      worldMatrix = _tempMatrix1;
    }
    else {
      worldMatrix = _attachedNode->_worldMatrix;
    }

    if (camera->getScene()->useRightHandedSystem()) {
      // avoid desync with RH matrix computation. Otherwise, rotation of PI around Y axis happens
      // each frame resulting in axis flipped because worldMatrix is computed as inverse of
      // viewMatrix.
      _rightHandtoLeftHandMatrix.multiplyToRef(worldMatrix, _tempMatrix2);
      worldMatrixUC = _tempMatrix2;
    }
    else {
      worldMatrixUC = worldMatrix;
    }

    worldMatrixUC.decompose(_tempVector2, _tempQuaternion, _tempVector);

    const auto inheritsTargetCamera = _attachedNode->getClassName() == "FreeCamera"
                                      || _attachedNode->getClassName() == "FlyCamera"
                                      || _attachedNode->getClassName() == "ArcFollowCamera"
                                      || _attachedNode->getClassName() == "TargetCamera"
                                      || _attachedNode->getClassName() == "TouchCamera"
                                      || _attachedNode->getClassName() == "UniversalCamera";

    if (inheritsTargetCamera) {
      auto targetCamera      = std::static_pointer_cast<TargetCamera>(_attachedNode);
      targetCamera->rotation = _tempQuaternion->toEulerAngles();
      if (targetCamera->rotationQuaternion) {
        targetCamera->rotationQuaternion->copyFrom(*_tempQuaternion);
      }
    }

    camera->position().copyFrom(*_tempVector);
  }
  else if ((std::static_pointer_cast<Mesh>(_attachedNode)
            && std::static_pointer_cast<Mesh>(_attachedNode)->_isMesh())
           || _attachedNode->getClassName() == "AbstractMesh"
           || _attachedNode->getClassName() == "TransformNode"
           || _attachedNode->getClassName() == "InstancedMesh") {
    auto transform = std::static_pointer_cast<TransformNode>(_attachedNode);
    Quaternion transformQuaternion(0.f, 0.f, 0.f, 1.f);
    if (transform->parent()) {
      auto& parentInv = _tempMatrix1;
      auto& localMat  = _tempMatrix2;
      transform->parent()->getWorldMatrix().invertToRef(parentInv);
      _attachedNode->getWorldMatrix().multiplyToRef(parentInv, localMat);
      std::optional<Vector3> iScaling  = transform->scaling();
      std::optional<Vector3> iPosition = transform->position();
      localMat.decompose(iScaling, _tempQuaternion, iPosition);
      transform->scaling  = *iScaling;
      transform->position = *iPosition;
    }
    else {
      std::optional<Vector3> iScaling  = transform->scaling();
      std::optional<Vector3> iPosition = transform->position();
      _attachedNode->_worldMatrix.decompose(iScaling, _tempQuaternion, iPosition);
      transform->scaling  = *iScaling;
      transform->position = *iPosition;
    }

    if (!transform->billboardMode()) {
      if (transform->rotationQuaternion()) {
        transform->rotationQuaternion()->copyFrom(*_tempQuaternion);
      }
      else {
        transform->rotation = _tempQuaternion->toEulerAngles();
      }
    }
  }
  else if (_attachedNode->getClassName() == "Bone") {
    const auto bone   = std::static_pointer_cast<Bone>(_attachedNode);
    const auto parent = bone->getParent();

    if (parent) {
      auto& invParent       = _tempMatrix1;
      auto& boneLocalMatrix = _tempMatrix2;
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
