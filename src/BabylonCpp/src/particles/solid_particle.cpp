#include <babylon/particles/solid_particle.h>

#include <babylon/math/tmp_vectors.h>
#include <babylon/meshes/mesh.h>
#include <babylon/particles/solid_particle_system.h>

namespace BABYLON {

SolidParticle::SolidParticle(
  unsigned int particleIndex, unsigned int positionIndex,
  unsigned int iIndiceIndex, ModelShape* model, int iShapeId,
  unsigned int iIdxInShape, SolidParticleSystem* sps,
  const std::optional<BoundingInfo>& modelBoundingInfo)
    : idx{particleIndex}
    , color{Color4(1.f, 1.f, 1.f, 1.f)}
    , position{Vector3::Zero()}
    , rotation{Vector3::Zero()}
    , rotationQuaternion{nullptr}
    , scaling{Vector3::One()}
    , uvs{Vector4(0.f, 0.f, 1.f, 1.f)}
    , velocity{Vector3::Zero()}
    , pivot{Vector3::Zero()}
    , translateFromPivot{false}
    , alive{true}
    , isVisible{true}
    , _pos{positionIndex}
    , _ind{iIndiceIndex}
    , _model{model}
    , shapeId{iShapeId}
    , idxInShape{iIdxInShape}
    , _sps{sps}
    , _stillInvisible{false}
    , _rotationMatrix{{1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f}}
    , parentId{std::nullopt}
    , cullingStrategy{AbstractMesh::CULLINGSTRATEGY_BOUNDINGSPHERE_ONLY}
    , _globalPosition{Vector3::Zero()}
{
  if (modelBoundingInfo) {
    const auto& mdlInfo = *modelBoundingInfo;
    _modelBoundingInfo  = std::make_unique<BoundingInfo>(mdlInfo);
    _boundingInfo
      = std::make_unique<BoundingInfo>(mdlInfo.minimum, mdlInfo.maximum);
  }
}

SolidParticle::~SolidParticle() = default;

bool SolidParticle::intersectsMesh(Mesh* target) const
{
  if (!_boundingInfo || !target->_boundingInfo) {
    return false;
  }
  if (_sps->_bSphereOnly) {
    return BoundingSphere::Intersects(_boundingInfo->boundingSphere,
                                      target->_boundingInfo->boundingSphere);
  }
  return _boundingInfo->intersects(*target->_boundingInfo, false);
}

bool SolidParticle::intersectsMesh(SolidParticle* target) const
{
  if (!_boundingInfo || !target->_boundingInfo) {
    return false;
  }
  if (_sps->_bSphereOnly) {
    return BoundingSphere::Intersects(_boundingInfo->boundingSphere,
                                      target->_boundingInfo->boundingSphere);
  }
  return _boundingInfo->intersects(*target->_boundingInfo, false);
}

bool SolidParticle::isInFrustum(const std::array<Plane, 6>& frustumPlanes)
{
  return _boundingInfo != nullptr
         && _boundingInfo->isInFrustum(frustumPlanes, cullingStrategy);
}

void SolidParticle::getRotationMatrix(Matrix& m) const
{
  Quaternion quaternion;
  if (rotationQuaternion) {
    quaternion = *rotationQuaternion;
  }
  else {
    quaternion            = TmpVectors::QuaternionArray[0];
    const auto& _rotation = rotation;
    Quaternion::RotationYawPitchRollToRef(_rotation.y, _rotation.x, _rotation.z,
                                          quaternion);
  }

  quaternion.toRotationMatrix(m);
}

} // end of namespace BABYLON
