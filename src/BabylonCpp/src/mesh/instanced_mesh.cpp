#include <babylon/mesh/instanced_mesh.h>

#include <babylon/culling/bounding_info.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>

namespace BABYLON {

InstancedMesh::InstancedMesh(const std::string& _name, Mesh* source)
    : AbstractMesh(_name, source->getScene())
{
  source->instances.emplace_back(this);

  _sourceMesh = source;

  position().copyFrom(source->position());
  rotation().copyFrom(source->rotation());
  scaling().copyFrom(source->scaling());

  if (source->rotationQuaternionSet()) {
    setRotationQuaternion(source->rotationQuaternion());
  }

  infiniteDistance = source->infiniteDistance;

  setPivotMatrix(source->getPivotMatrix());

  refreshBoundingInfo();
  _syncSubMeshes();
}

InstancedMesh::~InstancedMesh()
{
}

const char* InstancedMesh::getClassName() const
{
  return "InstancedMesh";
}

IReflect::Type InstancedMesh::type() const
{
  return IReflect::Type::INSTANCEDMESH;
}

bool InstancedMesh::receiveShadows() const
{
  return _sourceMesh->receiveShadows();
}

Material* InstancedMesh::material() const
{
  return _sourceMesh->material();
}

float InstancedMesh::visibility() const
{
  return _sourceMesh->visibility;
}

Skeleton* InstancedMesh::skeleton()
{
  return _sourceMesh->skeleton();
}

unsigned int InstancedMesh::renderingGroupId() const
{
  return _sourceMesh->renderingGroupId;
}

size_t InstancedMesh::getTotalVertices() const
{
  return _sourceMesh->getTotalVertices();
}

Mesh* InstancedMesh::sourceMesh() const
{
  return _sourceMesh;
}

Float32Array InstancedMesh::getVerticesData(unsigned int kind,
                                            bool copyWhenShared, bool forceCopy)
{
  return _sourceMesh->getVerticesData(kind, copyWhenShared, forceCopy);
}

Mesh* InstancedMesh::setVerticesData(unsigned int kind,
                                     const Float32Array& data, bool updatable,
                                     int stride)
{
  if (sourceMesh()) {
    sourceMesh()->setVerticesData(kind, data, updatable, stride);
  }
  return sourceMesh();
}

Mesh* InstancedMesh::updateVerticesData(unsigned int kind,
                                        const Float32Array& data,
                                        bool updateExtends, bool makeItUnique)
{
  if (sourceMesh()) {
    sourceMesh()->updateVerticesData(kind, data, updateExtends, makeItUnique);
  }
  return sourceMesh();
}

Mesh* InstancedMesh::setIndices(const IndicesArray& indices,
                                size_t totalVertices)
{
  if (sourceMesh()) {
    sourceMesh()->setIndices(indices, totalVertices);
  }
  return sourceMesh();
}

bool InstancedMesh::isVerticesDataPresent(unsigned int kind)
{
  return _sourceMesh->isVerticesDataPresent(kind);
}

IndicesArray InstancedMesh::getIndices(bool /*copyWhenShared*/)
{
  return _sourceMesh->getIndices();
}

std::vector<Vector3> InstancedMesh::_positions()
{
  return _sourceMesh->_positions;
}

InstancedMesh& InstancedMesh::refreshBoundingInfo()
{
  auto meshBB = _sourceMesh->getBoundingInfo();

  _boundingInfo = std::unique_ptr<BoundingInfo>(
    new BoundingInfo(meshBB->minimum, meshBB->maximum));

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

AbstractMesh* InstancedMesh::getLOD(Camera* /*camera*/,
                                    BoundingSphere* /*boundingSphere*/)
{
  AbstractMesh* currentLOD = sourceMesh()->getLOD(
    getScene()->activeCamera, &getBoundingInfo()->boundingSphere);
  _currentLOD = dynamic_cast<Mesh*>(currentLOD);

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
      subMesh->clone(this, _sourceMesh);
    }
  }

  return *this;
}

bool InstancedMesh::_generatePointsArray()
{
  return _sourceMesh->_generatePointsArray();
}

InstancedMesh* InstancedMesh::clone(const std::string& /*iNname*/,
                                    Node* newParent, bool doNotCloneChildren)
{
  auto result = _sourceMesh->createInstance(name);

  // Deep copy
  // Tools.DeepCopy(this, result, ["name"], []);

  // Bounding info
  refreshBoundingInfo();

  // Parent
  if (newParent) {
    static_cast<Node*>(result)->setParent(newParent);
  }

  if (!doNotCloneChildren) {
    // Children
    for (auto& mesh : getScene()->meshes) {
      if (mesh->parent() == this) {
        mesh->clone(mesh->name, dynamic_cast<Node*>(result));
      }
    }
  }

  result->computeWorldMatrix(true);

  return result;
}

void InstancedMesh::dispose(bool doNotRecurse)
{
  // Remove from mesh
  _sourceMesh->instances.erase(std::remove(_sourceMesh->instances.begin(),
                                           _sourceMesh->instances.end(), this),
                               _sourceMesh->instances.end());

  AbstractMesh::dispose(doNotRecurse);
}

} // end of namespace BABYLON
