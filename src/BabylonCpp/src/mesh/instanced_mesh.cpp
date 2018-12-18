#include <babylon/mesh/instanced_mesh.h>

#include <babylon/core/logging.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>

namespace BABYLON {

InstancedMesh::InstancedMesh(const std::string& _name, Mesh* source)
    : AbstractMesh(_name, source->getScene())
    , sourceMesh{this, &InstancedMesh::get_sourceMesh}
    , _sourceMesh{source}
    , _currentLOD{nullptr}
{
  source->instances.emplace_back(this);

  position().copyFrom(source->position());
  rotation().copyFrom(source->rotation());
  scaling().copyFrom(source->scaling());

  if (source->rotationQuaternion()) {
    rotationQuaternion = source->rotationQuaternion();
  }

  infiniteDistance = source->infiniteDistance;

  setPivotMatrix(source->getPivotMatrix());

  refreshBoundingInfo();
}

InstancedMesh::~InstancedMesh()
{
}

void InstancedMesh::syncSubMeshes(const InstancedMeshPtr& instancedMesh)
{
  instancedMesh->_syncSubMeshes();
}

const std::string InstancedMesh::getClassName() const
{
  return "InstancedMesh";
}

IReflect::Type InstancedMesh::type() const
{
  return IReflect::Type::INSTANCEDMESH;
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
                   "no effect on the scene");
}

size_t InstancedMesh::getTotalVertices() const
{
  return _sourceMesh->getTotalVertices();
}

Mesh*& InstancedMesh::get_sourceMesh()
{
  return _sourceMesh;
}

bool InstancedMesh::isReady(bool completeCheck, bool /*forceInstanceSupport*/)
{
  return _sourceMesh->isReady(completeCheck, true);
}

Float32Array InstancedMesh::getVerticesData(unsigned int kind,
                                            bool copyWhenShared, bool forceCopy)
{
  return _sourceMesh->getVerticesData(kind, copyWhenShared, forceCopy);
}

AbstractMesh*
InstancedMesh::setVerticesData(unsigned int kind, const Float32Array& data,
                               bool updatable,
                               const std::optional<size_t>& stride)
{
  if (sourceMesh()) {
    sourceMesh()->setVerticesData(kind, data, updatable, stride);
  }
  return sourceMesh();
}

AbstractMesh* InstancedMesh::updateVerticesData(unsigned int kind,
                                                const Float32Array& data,
                                                bool updateExtends,
                                                bool makeItUnique)
{
  if (sourceMesh()) {
    sourceMesh()->updateVerticesData(kind, data, updateExtends, makeItUnique);
  }
  return sourceMesh();
}

AbstractMesh* InstancedMesh::setIndices(const IndicesArray& indices,
                                        size_t totalVertices,
                                        bool /*updatable*/)
{
  if (sourceMesh()) {
    sourceMesh()->setIndices(indices, totalVertices);
  }
  return sourceMesh();
}

bool InstancedMesh::isVerticesDataPresent(unsigned int kind) const
{
  return _sourceMesh->isVerticesDataPresent(kind);
}

IndicesArray InstancedMesh::getIndices(bool /*copyWhenShared*/,
                                       bool /*forceCopy*/)
{
  return _sourceMesh->getIndices();
}

std::vector<Vector3>& InstancedMesh::_positions()
{
  return _sourceMesh->_positions();
}

InstancedMesh& InstancedMesh::refreshBoundingInfo()
{
  auto meshBB = _sourceMesh->getBoundingInfo();

  _boundingInfo
    = std::make_unique<BoundingInfo>(meshBB.minimum, meshBB.maximum);

  _updateBoundingInfo();

  return *this;
}

void InstancedMesh::_preActivate()
{
  if (_currentLOD) {
    _currentLOD->_preActivate();
  }
}

void InstancedMesh::_activate(int renderId)
{
  if (_currentLOD) {
    _currentLOD->_registerInstanceForRenderId(this, renderId);
  }
}

AbstractMesh* InstancedMesh::getLOD(const CameraPtr& camera,
                                    BoundingSphere* /*boundingSphere*/)
{
  if (!camera) {
    return this;
  }

  auto boundingInfo = getBoundingInfo();

  auto currentLOD = sourceMesh()->getLOD(getScene()->activeCamera,
                                         &boundingInfo.boundingSphere);
  _currentLOD     = dynamic_cast<Mesh*>(currentLOD);

  if (_currentLOD == sourceMesh()) {
    return this;
  }

  return _currentLOD;
}

InstancedMesh& InstancedMesh::_syncSubMeshes()
{
  releaseSubMeshes();
  if (!_sourceMesh->subMeshes.empty()) {
    for (auto& subMesh : _sourceMesh->subMeshes) {
      subMesh->clone(shared_from_base<InstancedMesh>(), _sourceMesh);
    }
  }

  return *this;
}

bool InstancedMesh::_generatePointsArray()
{
  return _sourceMesh->_generatePointsArray();
}

InstancedMeshPtr InstancedMesh::clone(const std::string& /*iNname*/,
                                      Node* newParent, bool doNotCloneChildren)
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
  _sourceMesh->instances.erase(std::remove(_sourceMesh->instances.begin(),
                                           _sourceMesh->instances.end(), this),
                               _sourceMesh->instances.end());

  AbstractMesh::dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
