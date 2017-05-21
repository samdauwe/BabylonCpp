#include <babylon/particles/solid_particle.h>

#include <babylon/culling/bounding_info.h>
#include <babylon/mesh/mesh.h>
#include <babylon/particles/solid_particle_system.h>

namespace BABYLON {

SolidParticle::SolidParticle(unsigned int particleIndex,
                             unsigned int positionIndex, ModelShape* model,
                             int iShapeId, unsigned int iIdxInShape,
                             SolidParticleSystem* sps)
    : idx{particleIndex}
    , color{new Color4(1.f, 1.f, 1.f, 1.f)}
    , position{Vector3::Zero()}
    , rotation{Vector3::Zero()}
    , scaling{Vector3(1.f, 1.f, 1.f)}
    , uvs{Vector4(0.f, 0.f, 1.f, 1.f)}
    , velocity{Vector3::Zero()}
    , alive{true}
    , isVisible{true}
    , _pos{positionIndex}
    , _model{model}
    , shapeId{iShapeId}
    , idxInShape{iIdxInShape}
    , _sps{sps}
{
}

SolidParticle::SolidParticle(unsigned int particleIndex,
                             unsigned int positionIndex, ModelShape* model,
                             int iShapeId, unsigned int iIdxInShape,
                             SolidParticleSystem* sps,
                             const BoundingInfo& modelBoundingInfo)
    : SolidParticle(particleIndex, positionIndex, model, iShapeId, iIdxInShape,
                    sps)
{
  _modelBoundingInfo = std::make_unique<BoundingInfo>(modelBoundingInfo);
  _boundingInfo      = std::make_unique<BoundingInfo>(modelBoundingInfo.minimum,
                                                 modelBoundingInfo.maximum);
}

SolidParticle::~SolidParticle()
{
}

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

} // end of namespace BABYLON
