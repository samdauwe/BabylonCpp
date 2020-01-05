#include <babylon/particles/solid_particle.h>

#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/mesh.h>
#include <babylon/particles/solid_particle_system.h>

namespace BABYLON {

SolidParticle::SolidParticle(unsigned int particleIndex, int particleId, unsigned int positionIndex,
                             unsigned int indiceIndex, ModelShape* model, int iShapeId,
                             unsigned int iIdxInShape, SolidParticleSystem* sps,
                             const std::optional<BoundingInfo>& modelBoundingInfo,
                             const std::optional<size_t>& iMaterialIndex)
    : id{0}
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
    , _stillInvisible{false}
    , _rotationMatrix{{1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f}}
    , parentId{std::nullopt}
    , materialIndex{std::nullopt}
    , cullingStrategy{AbstractMesh::CULLINGSTRATEGY_BOUNDINGSPHERE_ONLY}
    , _globalPosition{Vector3::Zero()}
{
  idx        = particleIndex;
  id         = particleId;
  _pos       = positionIndex;
  _ind       = indiceIndex;
  _model     = model;
  shapeId    = iShapeId;
  idxInShape = iIdxInShape;
  _sps       = sps;
  if (modelBoundingInfo) {
    const auto& mdlInfo = *modelBoundingInfo;
    _modelBoundingInfo  = std::make_unique<BoundingInfo>(mdlInfo);
    _boundingInfo       = std::make_unique<BoundingInfo>(mdlInfo.minimum, mdlInfo.maximum);
  }
  if (materialIndex.has_value()) {
    materialIndex = *iMaterialIndex;
  }
}

SolidParticle::~SolidParticle() = default;

SolidParticle& SolidParticle::copyToRef(SolidParticle& target)
{
  target.position.copyFrom(position);
  target.rotation.copyFrom(rotation);
  if (rotationQuaternion) {
    if (target.rotationQuaternion) {
      target.rotationQuaternion->copyFrom(*rotationQuaternion);
    }
    else {
      target.rotationQuaternion = std::make_unique<Quaternion>(*rotationQuaternion);
    }
  }
  target.scaling.copyFrom(scaling);
  if (color) {
    if (target.color) {
      target.color->copyFrom(*color);
    }
    else {
      target.color = *color;
    }
  }
  target.uvs.copyFrom(uvs);
  target.velocity.copyFrom(velocity);
  target.pivot.copyFrom(pivot);
  target.translateFromPivot = translateFromPivot;
  target.alive              = alive;
  target.isVisible          = isVisible;
  target.parentId           = parentId;
  target.cullingStrategy    = cullingStrategy;
  if (materialIndex.has_value()) {
    target.materialIndex = *materialIndex;
  }
  return *this;
}

bool SolidParticle::intersectsMesh(const std::variant<Mesh*, SolidParticle*>& target) const
{
  std::shared_ptr<BoundingInfo> targetBoundingInfo
    = std::holds_alternative<Mesh*>(target) ? std::get<Mesh*>(target)->_boundingInfo :
                                              std::get<SolidParticle*>(target)->_boundingInfo;

  if (!_boundingInfo || !targetBoundingInfo) {
    return false;
  }
  if (_sps->_bSphereOnly) {
    return BoundingSphere::Intersects(_boundingInfo->boundingSphere,
                                      targetBoundingInfo->boundingSphere);
  }
  return _boundingInfo->intersects(*targetBoundingInfo, false);
}

bool SolidParticle::isInFrustum(const std::array<Plane, 6>& frustumPlanes)
{
  return _boundingInfo != nullptr && _boundingInfo->isInFrustum(frustumPlanes, cullingStrategy);
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
    Quaternion::RotationYawPitchRollToRef(_rotation.y, _rotation.x, _rotation.z, quaternion);
  }

  quaternion.toRotationMatrix(m);
}

} // end of namespace BABYLON
