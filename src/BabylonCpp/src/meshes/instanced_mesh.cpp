#include <babylon/meshes/instanced_mesh.h>

#include <babylon/core/logging.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/engines/scene.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/rendering/edges_renderer.h>
#include <babylon/rendering/rendering_group.h>

namespace BABYLON {

InstancedMesh::InstancedMesh(const std::string& iName, const MeshPtr& source)
    : AbstractMesh(iName, source->getScene())
    , _indexInSourceMeshInstanceArray{-1}
    , sourceMesh{this, &InstancedMesh::get_sourceMesh}
    , _currentLOD{nullptr}
{
  source->addInstance(this);

  _sourceMesh = source;

  _unIndexed = source->_unIndexed;

  position().copyFrom(source->position());
  rotation().copyFrom(source->rotation());
  scaling().copyFrom(source->scaling());

  if (source->rotationQuaternion()) {
    rotationQuaternion = source->rotationQuaternion();
  }

  animations = source->animations;
  for (const auto& range : source->getAnimationRanges()) {
    if (range != nullptr) {
      createAnimationRange(range->name, range->from, range->to);
    }
  }

  infiniteDistance = source->infiniteDistance();

  setPivotMatrix(source->getPivotMatrix());

  refreshBoundingInfo();
}

InstancedMesh::~InstancedMesh() = default;

void InstancedMesh::syncSubMeshes(const InstancedMeshPtr& instancedMesh)
{
  instancedMesh->_syncSubMeshes();
}

std::string InstancedMesh::getClassName() const
{
  return "InstancedMesh";
}

Type InstancedMesh::type() const
{
  return Type::INSTANCEDMESH;
}

void InstancedMesh::_resyncLightSources()
{
  // Do nothing as all the work will be done by source mesh
}

void InstancedMesh::_resyncLighSource(const LightPtr& /*light*/)
{
  // Do nothing as all the work will be done by source mesh
}

void InstancedMesh::_removeLightSource(const LightPtr& /*light*/, bool /*dispose*/)
{
  // Do nothing as all the work will be done by source mesh
}

bool InstancedMesh::get_receiveShadows() const
{
  return _sourceMesh->receiveShadows();
}

MaterialPtr& InstancedMesh::get_material()
{
  return _sourceMesh->material();
}

float InstancedMesh::get_visibility() const
{
  return _sourceMesh->visibility();
}

SkeletonPtr& InstancedMesh::get_skeleton()
{
  return _sourceMesh->skeleton();
}

int InstancedMesh::get_renderingGroupId() const
{
  return _sourceMesh->renderingGroupId;
}

void InstancedMesh::set_renderingGroupId(int value)
{
  if (!_sourceMesh || value == _sourceMesh->renderingGroupId) {
    return;
  }

  // no-op with warning
  BABYLON_LOG_WARN("InstancedMesh",
                   "Note - setting renderingGroupId of an instanced mesh has "
                   "no effect on the scene")
}

size_t InstancedMesh::getTotalVertices() const
{
  return _sourceMesh ? _sourceMesh->getTotalVertices() : 0;
}

size_t InstancedMesh::getTotalIndices() const
{
  return _sourceMesh->getTotalIndices();
}

MeshPtr& InstancedMesh::get_sourceMesh()
{
  return _sourceMesh;
}

InstancedMeshPtr InstancedMesh::createInstance(const std::string& name)
{
  return _sourceMesh->createInstance(name);
}

bool InstancedMesh::isReady(bool completeCheck, bool /*forceInstanceSupport*/)
{
  return _sourceMesh->isReady(completeCheck, true);
}

Float32Array InstancedMesh::getVerticesData(const std::string& kind, bool copyWhenShared,
                                            bool forceCopy)
{
  return _sourceMesh->getVerticesData(kind, copyWhenShared, forceCopy);
}

AbstractMesh* InstancedMesh::setVerticesData(const std::string& kind, const Float32Array& data,
                                             bool updatable, const std::optional<size_t>& stride)
{
  if (sourceMesh()) {
    sourceMesh()->setVerticesData(kind, data, updatable, stride);
  }
  return sourceMesh().get();
}

AbstractMesh* InstancedMesh::updateVerticesData(const std::string& kind, const Float32Array& data,
                                                bool updateExtends, bool makeItUnique)
{
  if (sourceMesh()) {
    sourceMesh()->updateVerticesData(kind, data, updateExtends, makeItUnique);
  }
  return sourceMesh().get();
}

AbstractMesh* InstancedMesh::setIndices(const IndicesArray& indices, size_t totalVertices,
                                        bool /*updatable*/)
{
  if (sourceMesh()) {
    sourceMesh()->setIndices(indices, totalVertices);
  }
  return sourceMesh().get();
}

bool InstancedMesh::isVerticesDataPresent(const std::string& kind) const
{
  return _sourceMesh->isVerticesDataPresent(kind);
}

IndicesArray InstancedMesh::getIndices(bool /*copyWhenShared*/, bool /*forceCopy*/)
{
  return _sourceMesh->getIndices();
}

std::vector<Vector3>& InstancedMesh::_positions()
{
  return _sourceMesh->_positions();
}

InstancedMesh& InstancedMesh::refreshBoundingInfo(bool applySkeleton)
{
  if (_boundingInfo && _boundingInfo->isLocked()) {
    return *this;
  }

  const auto bias
    = _sourceMesh->geometry() ? _sourceMesh->geometry()->boundingBias() : std::nullopt;
  _refreshBoundingInfo(_sourceMesh->_getPositionData(applySkeleton), bias);
  return *this;
}

void InstancedMesh::_preActivate()
{
  if (_currentLOD) {
    _currentLOD->_preActivate();
  }
}

bool InstancedMesh::_activate(int renderId, bool intermediateRendering)
{
  if (_sourceMesh->subMeshes.empty()) {
    BABYLON_LOG_WARN("InstancedMesh", "Instances should only be created for meshes with geometry.")
  }

  if (_currentLOD) {
    auto differentSign
      = (_currentLOD->_getWorldMatrixDeterminant() > 0.f) != (_getWorldMatrixDeterminant() > 0.f);
    if (differentSign) {
      _internalAbstractMeshDataInfo._actAsRegularMesh = true;
      return true;
    }
    _internalAbstractMeshDataInfo._actAsRegularMesh = false;

    _currentLOD->_registerInstanceForRenderId(this, renderId);

    if (intermediateRendering) {
      if (!_currentLOD->_internalAbstractMeshDataInfo._isActiveIntermediate) {
        _currentLOD->_internalAbstractMeshDataInfo._onlyForInstancesIntermediate = true;
        return true;
      }
    }
    else {
      if (!_currentLOD->_internalAbstractMeshDataInfo._isActive) {
        _currentLOD->_internalAbstractMeshDataInfo._onlyForInstances = true;
        return true;
      }
    }
  }
  return false;
}

void InstancedMesh::_postActivate()
{
  if (_edgesRenderer && _edgesRenderer->isEnabled && _sourceMesh->_renderingGroup) {
    _sourceMesh->_renderingGroup->_edgesRenderers.emplace_back(_edgesRenderer);
  }
}

Matrix& InstancedMesh::getWorldMatrix()
{
  if (_currentLOD && _currentLOD->billboardMode() != TransformNode::BILLBOARDMODE_NONE
      && _currentLOD->_masterMesh != this) {
    const auto& tempMaster   = _currentLOD->_masterMesh;
    _currentLOD->_masterMesh = this;
    TmpVectors::Vector3Array[7].copyFrom(_currentLOD->position());
    _currentLOD->position().set(0.f, 0.f, 0.f);
    TmpVectors::MatrixArray[0].copyFrom(_currentLOD->computeWorldMatrix(true));
    _currentLOD->position().copyFrom(TmpVectors::Vector3Array[7]);
    _currentLOD->_masterMesh = tempMaster;
    return TmpVectors::MatrixArray[0];
  }

  return AbstractMesh::getWorldMatrix();
}

bool InstancedMesh::isAnInstance() const
{
  return true;
}

AbstractMesh* InstancedMesh::getLOD(const CameraPtr& camera, BoundingSphere* /*boundingSphere*/)
{
  if (!camera) {
    return this;
  }

  const auto& boundingInfo = getBoundingInfo();

  auto currentLOD = sourceMesh()->getLOD(getScene()->activeCamera, &boundingInfo->boundingSphere);
  _currentLOD     = dynamic_cast<Mesh*>(currentLOD);

  if (_currentLOD == sourceMesh().get()) {
    return sourceMesh().get();
  }

  return _currentLOD;
}

Mesh* InstancedMesh::_preActivateForIntermediateRendering(int renderId)
{
  auto iSourceMesh = std::static_pointer_cast<Mesh>(sourceMesh());
  return iSourceMesh ? iSourceMesh->_preActivateForIntermediateRendering(renderId) : nullptr;
}

InstancedMesh& InstancedMesh::_syncSubMeshes()
{
  releaseSubMeshes();
  if (!_sourceMesh->subMeshes.empty()) {
    for (const auto& subMesh : _sourceMesh->subMeshes) {
      subMesh->clone(shared_from_base<InstancedMesh>(), _sourceMesh);
    }
  }

  return *this;
}

bool InstancedMesh::_generatePointsArray()
{
  return _sourceMesh->_generatePointsArray();
}

InstancedMeshPtr InstancedMesh::clone(const std::string& /*iNname*/, Node* newParent,
                                      bool doNotCloneChildren)
{
  auto result = _sourceMesh->createInstance(name);

  // Deep copy
  // Tools.DeepCopy(this, result, ["name"], []);

  // Bounding info
  refreshBoundingInfo();

  // Parent
  if (newParent) {
    std::static_pointer_cast<Node>(result)->parent = newParent;
  }

  if (!doNotCloneChildren) {
    // Children
    for (auto& mesh : getScene()->meshes) {
      if (mesh->parent() == this) {
        // mesh->clone(mesh->name, std::static_pointer_cast<Node>(result));
      }
    }
  }

  result->computeWorldMatrix(true);

  return result;
}

void InstancedMesh::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  // Remove from mesh
  _sourceMesh->removeInstance(this);
  AbstractMesh::dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
