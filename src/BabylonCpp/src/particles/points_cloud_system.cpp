#include <babylon/particles/points_cloud_system.h>

#include <babylon/core/random.h>
#include <babylon/engines/engine_store.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/particles/cloud_point.h>
#include <babylon/particles/points_group.h>

namespace BABYLON {

PointsCloudSystem::PointsCloudSystem(const std::string& iName, size_t pointSize, Scene* scene,
                                     const std::optional<PointsCloudSystemOptions>& options)
    : nbParticles{0}
    , counter{0}
    , mesh{nullptr}
    , _updatable{true}
    , _isVisibilityBoxLocked{false}
    , _alwaysVisible{false}
    , _groupCounter{0}
    , _computeParticleColor{true}
    , _computeParticleTexture{true}
    , _computeParticleRotation{true}
    , _computeBoundingBox{false}
    , _isReady{false}
{
  name   = iName;
  _size  = pointSize;
  _scene = scene ? scene : EngineStore::LastCreatedScene();
  if (options && options->updatable.has_value()) {
    _updatable = options->updatable.value();
  }
  else {
    _updatable = true;
  }
}

PointsCloudSystem::~PointsCloudSystem() = default;

MeshPtr PointsCloudSystem::buildMeshSync()
{
  _isReady = true;
  return _buildMesh();
}

MeshPtr PointsCloudSystem::_buildMesh()
{
  if (nbParticles == 0) {
    // addPoints(1);
  }

  _positions32 = Float32Array(_positions);
  _uvs32       = Float32Array(_uvs);
  _colors32    = Float32Array(_colors);

  auto vertexData = std::unique_ptr<VertexData>();
  vertexData->set(_positions32, VertexBuffer::PositionKind);

  if (_uvs32.size() > 0) {
    vertexData->set(_uvs32, VertexBuffer::UVKind);
  }
  auto ec = 0; // emissive color value 0 for UVs, 1 for color
  if (_colors32.size() > 0) {
    ec = 1;
    vertexData->set(_colors32, VertexBuffer::ColorKind);
  }
  auto iMesh = Mesh::New(name, _scene);
  vertexData->applyToMesh(*iMesh, _updatable);
  mesh = iMesh;

  // free memory
  _positions.clear();
  _uvs.clear();
  _colors.clear();

  if (!_updatable) {
    particles.clear();
  }

  auto mat             = StandardMaterial::New("point cloud material", _scene);
  mat->emissiveColor   = Color3(ec, ec, ec);
  mat->disableLighting = true;
  mat->pointsCloud     = true;
  mat->pointSize       = _size;
  iMesh->material      = mat;

  return iMesh;
}

CloudPointPtr PointsCloudSystem::_addParticle(size_t idx, PointsGroup* group, size_t groupId,
                                              size_t idxInGroup)
{
  auto cp = std::make_shared<CloudPoint>(idx, group, groupId, idxInGroup, this);
  particles.emplace_back(cp);
  return cp;
}

void PointsCloudSystem::_randomUnitVector(CloudPoint& particle)
{
  particle.position = Vector3(Math::random(), Math::random(), Math::random());
  particle.color    = std::make_unique<Color4>(1.f, 1.f, 1.f, 1.f);
}

Color4 PointsCloudSystem::_getColorIndicesForCoord(const PointsGroup& pointsGroup, uint32_t x,
                                                   uint32_t y, uint32_t width) const
{
  const auto& imageData           = pointsGroup._groupImageData.uint8Array();
  const auto color                = y * (width * 4) + x * 4;
  const IndicesArray colorIndices = {color, color + 1, color + 2, color + 3};
  const auto redIndex             = colorIndices[0];
  const auto greenIndex           = colorIndices[1];
  const auto blueIndex            = colorIndices[2];
  const auto alphaIndex           = colorIndices[3];
  const auto redForCoord          = imageData[redIndex];
  const auto greenForCoord        = imageData[greenIndex];
  const auto blueForCoord         = imageData[blueIndex];
  const auto alphaForCoord        = imageData[alphaIndex];
  return Color4(redForCoord / 255.f, greenForCoord / 255.f, blueForCoord / 255.f, alphaForCoord);
}

} // end of namespace BABYLON
