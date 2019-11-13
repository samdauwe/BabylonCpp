#include <babylon/misc/mesh_exploder.h>

#include <algorithm>

#include <babylon/meshes/mesh.h>

namespace BABYLON {

MeshExploder::MeshExploder(const std::vector<MeshPtr>& meshes,
                           const MeshPtr& centerMesh)
    : _scaledDirection{Vector3::Zero()}
    , _newPosition{Vector3::Zero()}
    , _centerPosition{Vector3::Zero()}
{
  _meshes = meshes;

  if (centerMesh) {
    _centerMesh = centerMesh;
  }
  else {
    _setCenterMesh();
  }
  _meshes.erase(std::remove(_meshes.begin(), _meshes.end(), _centerMesh),
                _meshes.end());
  _centerPosition = _centerMesh->getAbsolutePosition();
  for (const auto& mesh : _meshes) {
    if (mesh) {
      _meshesOrigins.emplace_back(mesh->getAbsolutePosition());
      _toCenterVectors.emplace_back(Vector3::Zero());
      if (mesh->_boundingInfo && _centerMesh->_boundingInfo) {
        mesh->_boundingInfo->boundingBox.centerWorld.subtractToRef(
          _centerMesh->_boundingInfo->boundingBox.centerWorld,
          _toCenterVectors.back());
      }
    }
  }
}

MeshExploder::~MeshExploder() = default;

void MeshExploder::_setCenterMesh()
{
  auto averageCenter     = Vector3::Zero();
  auto totalCenters      = Vector3::Zero();
  float shortestToCenter = std::numeric_limits<float>::max();
  for (const auto& mesh : _meshes) {
    if (mesh) {
      const auto& boundingInfo = *mesh->getBoundingInfo();
      /*if (boundingInfo)*/ {
        totalCenters.addInPlace(boundingInfo.boundingBox.centerWorld);
      }
    }
  }
  averageCenter = totalCenters.scale(1.f / _meshes.size());
  for (const auto& mesh : _meshes) {
    if (mesh) {
      const auto& boundingInfo = *mesh->getBoundingInfo();
      /*if (boundingInfo)*/ {
        auto distanceToCenter
          = boundingInfo.boundingBox.centerWorld.subtract(averageCenter)
              .lengthSquared();
        if (distanceToCenter < shortestToCenter) {
          _centerMesh      = mesh;
          shortestToCenter = distanceToCenter;
        }
      }
    }
  }
}

std::string MeshExploder::getClassName() const
{
  return "MeshExploder";
}

std::vector<MeshPtr> MeshExploder::getMeshes() const
{
  auto meshArray = _meshes;
  meshArray.insert(meshArray.begin(), _centerMesh);
  return meshArray;
}

void MeshExploder::explode(float direction)
{
  for (size_t index = 0; index < _meshes.size(); ++index) {
    if ((index < _meshes.size() && _meshes[index])
        && (index < _meshesOrigins.size())
        && (index < _toCenterVectors.size())) {
      _toCenterVectors[index].scaleToRef(direction, _scaledDirection);
      _meshesOrigins[index].addToRef(_scaledDirection, _newPosition);
      _meshes[index]->setAbsolutePosition(_newPosition);
    }
  }
  _centerMesh->setAbsolutePosition(_centerPosition);
}

} // end of namespace BABYLON
