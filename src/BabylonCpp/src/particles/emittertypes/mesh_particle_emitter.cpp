#include <babylon/particles/emittertypes/mesh_particle_emitter.h>

#include <babylon/core/json_util.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {

MeshParticleEmitter::MeshParticleEmitter(const AbstractMeshPtr& iMesh)
    : direction1{Vector3(0.f, 1.f, 0.f)}
    , direction2{Vector3(0.f, 1.f, 0.f)}
    , useMeshNormalsForDirection{true}
    , mesh{this, &MeshParticleEmitter::get_mesh, &MeshParticleEmitter::set_mesh}
    , _storedNormal{Vector3::Zero()}
    , _mesh{nullptr}
{
  mesh = iMesh;
}

MeshParticleEmitter::~MeshParticleEmitter() = default;

AbstractMeshPtr& MeshParticleEmitter::get_mesh()
{
  return _mesh;
}

void MeshParticleEmitter::set_mesh(const AbstractMeshPtr& value)
{
  if (_mesh == value) {
    return;
  }

  _mesh = value;

  if (value) {
    _indices   = value->getIndices();
    _positions = value->getVerticesData(VertexBuffer::PositionKind);
    _normals   = value->getVerticesData(VertexBuffer::NormalKind);
  }
  else {
    _indices.clear();
    _positions.clear();
    _normals.clear();
  }
}

void MeshParticleEmitter::startDirectionFunction(const Matrix& worldMatrix,
                                                 Vector3& directionToUpdate, Particle* /*particle*/,
                                                 bool isLocal)
{
  if (useMeshNormalsForDirection && !_normals.empty()) {
    Vector3::TransformNormalToRef(_storedNormal, worldMatrix, directionToUpdate);
    return;
  }

  const auto randX = Scalar::RandomRange(direction1.x, direction2.x);
  const auto randY = Scalar::RandomRange(direction1.y, direction2.y);
  const auto randZ = Scalar::RandomRange(direction1.z, direction2.z);

  if (isLocal) {
    directionToUpdate.copyFromFloats(randX, randY, randZ);
    return;
  }

  Vector3::TransformNormalFromFloatsToRef(randX, randY, randZ, worldMatrix, directionToUpdate);
}

void MeshParticleEmitter::startPositionFunction(const Matrix& worldMatrix,
                                                Vector3& positionToUpdate, Particle* /*particle*/,
                                                bool isLocal)
{
  if (_indices.empty() || _positions.empty()) {
    return;
  }

  const auto randomFaceIndex = static_cast<size_t>(3 * Math::random() * (_indices.size() / 3));
  const auto bu              = Math::random();
  const auto bv              = Math::random() * (1.f - bu);
  const auto bw              = 1.f - bu - bv;

  const auto faceIndexA = _indices[randomFaceIndex];
  const auto faceIndexB = _indices[randomFaceIndex + 1];
  const auto faceIndexC = _indices[randomFaceIndex + 2];
  auto& vertexA         = TmpVectors::Vector3Array[0];
  auto& vertexB         = TmpVectors::Vector3Array[1];
  auto& vertexC         = TmpVectors::Vector3Array[2];
  auto& randomVertex    = TmpVectors::Vector3Array[3];

  Vector3::FromArrayToRef(_positions, faceIndexA * 3, vertexA);
  Vector3::FromArrayToRef(_positions, faceIndexB * 3, vertexB);
  Vector3::FromArrayToRef(_positions, faceIndexC * 3, vertexC);

  randomVertex.x = bu * vertexA.x + bv * vertexB.x + bw * vertexC.x;
  randomVertex.y = bu * vertexA.y + bv * vertexB.y + bw * vertexC.y;
  randomVertex.z = bu * vertexA.z + bv * vertexB.z + bw * vertexC.z;

  if (isLocal) {
    positionToUpdate.copyFromFloats(randomVertex.x, randomVertex.y, randomVertex.z);
  }
  else {
    Vector3::TransformCoordinatesFromFloatsToRef(randomVertex.x, randomVertex.y, randomVertex.z,
                                                 worldMatrix, positionToUpdate);
  }

  if (useMeshNormalsForDirection && !_normals.empty()) {
    Vector3::FromArrayToRef(_normals, faceIndexA * 3, vertexA);
    Vector3::FromArrayToRef(_normals, faceIndexB * 3, vertexB);
    Vector3::FromArrayToRef(_normals, faceIndexC * 3, vertexC);

    _storedNormal.x = bu * vertexA.x + bv * vertexB.x + bw * vertexC.x;
    _storedNormal.y = bu * vertexA.y + bv * vertexB.y + bw * vertexC.y;
    _storedNormal.z = bu * vertexA.z + bv * vertexB.z + bw * vertexC.z;
  }
}

std::unique_ptr<IParticleEmitterType> MeshParticleEmitter::clone() const
{
  auto newOne = std::make_unique<MeshParticleEmitter>();

  return newOne;
}

void MeshParticleEmitter::applyToShader(Effect* effect)
{
  effect->setVector3("direction1", direction1);
  effect->setVector3("direction2", direction2);
}

std::string MeshParticleEmitter::getEffectDefines() const
{
  return "";
}

std::string MeshParticleEmitter::getClassName() const
{
  return "MeshParticleEmitter";
}

json MeshParticleEmitter::serialize() const
{
  return nullptr;
}

void MeshParticleEmitter::parse(const json& serializationObject, Scene* scene)
{
  Vector3::FromArrayToRef(json_util::get_array<float>(serializationObject, "direction1"), 0,
                          direction1);
  Vector3::FromArrayToRef(json_util::get_array<float>(serializationObject, "direction2"), 0,
                          direction2);

  if (json_util::has_valid_key_value(serializationObject, "meshId")) {
    mesh = scene->getLastMeshByID(json_util::get_string(serializationObject, "meshId"));
  }

  useMeshNormalsForDirection
    = json_util::get_bool(serializationObject, "useMeshNormalsForDirection");
}

} // end of namespace BABYLON
