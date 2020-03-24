#include <babylon/particles/points_cloud_system.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/collisions/picking_info.h>
#include <babylon/core/random.h>
#include <babylon/culling/ray.h>
#include <babylon/engines/engine_store.h>
#include <babylon/materials/standard_material.h>
#include <babylon/maths/scalar.h>
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

void PointsCloudSystem::_setPointsColorOrUV(const MeshPtr& mesh, PointsGroup* pointsGroup,
                                            bool isVolume,
                                            const std::optional<bool>& colorFromTexture,
                                            const std::optional<bool>& hasTexture,
                                            const std::optional<Color4>& color,
                                            std::optional<int> iRange)
{
  if (isVolume) {
    mesh->updateFacetData();
  }

  const auto& boundInfo = mesh->getBoundingInfo();
  const auto diameter   = 2 * boundInfo->boundingSphere.radius;

  auto meshPos       = mesh->getVerticesData(VertexBuffer::PositionKind);
  const auto meshInd = mesh->getIndices();
  const auto meshUV  = mesh->getVerticesData(VertexBuffer::UVKind);
  const auto meshCol = mesh->getVerticesData(VertexBuffer::ColorKind);

  auto place = Vector3::Zero();
  mesh->computeWorldMatrix();
  auto meshMatrix = mesh->getWorldMatrix();
  if (!meshMatrix.isIdentity()) {
    for (size_t p = 0; p < meshPos.size() / 3; p++) {
      Vector3::TransformCoordinatesFromFloatsToRef(meshPos[3 * p], meshPos[3 * p + 1],
                                                   meshPos[3 * p + 2], meshMatrix, place);
      meshPos[3 * p]     = place.x;
      meshPos[3 * p + 1] = place.y;
      meshPos[3 * p + 2] = place.z;
    }
  }

  size_t idxPoints = 0;

  auto index   = 0;
  auto id0     = 0u;
  auto id1     = 0u;
  auto id2     = 0u;
  auto v0X     = 0.f;
  auto v0Y     = 0.f;
  auto v0Z     = 0.f;
  auto v1X     = 0.f;
  auto v1Y     = 0.f;
  auto v1Z     = 0.f;
  auto v2X     = 0.f;
  auto v2Y     = 0.f;
  auto v2Z     = 0.f;
  auto vertex0 = Vector3::Zero();
  auto vertex1 = Vector3::Zero();
  auto vertex2 = Vector3::Zero();
  auto vec0    = Vector3::Zero();
  auto vec1    = Vector3::Zero();

  auto uv0X  = 0.f;
  auto uv0Y  = 0.f;
  auto uv1X  = 0.f;
  auto uv1Y  = 0.f;
  auto uv2X  = 0.f;
  auto uv2Y  = 0.f;
  auto uv0   = Vector2::Zero();
  auto uv1   = Vector2::Zero();
  auto uv2   = Vector2::Zero();
  auto uvec0 = Vector2::Zero();
  auto uvec1 = Vector2::Zero();

  auto col0X   = 0.f;
  auto col0Y   = 0.f;
  auto col0Z   = 0.f;
  auto col0A   = 0.f;
  auto col1X   = 0.f;
  auto col1Y   = 0.f;
  auto col1Z   = 0.f;
  auto col1A   = 0.f;
  auto col2X   = 0.f;
  auto col2Y   = 0.f;
  auto col2Z   = 0.f;
  auto col2A   = 0.f;
  auto col0    = Vector4::Zero();
  auto col1    = Vector4::Zero();
  auto col2    = Vector4::Zero();
  auto colvec0 = Vector4::Zero();
  auto colvec1 = Vector4::Zero();

  auto lamda = 0.f;
  auto mu    = 0.f;
  auto range = iRange.value_or(0);

  Vector3 facetPoint;
  Vector2 uvPoint;
  auto colPoint = Vector4(0.f, 0.f, 0.f, 0.f);

  auto norm          = Vector3::Zero();
  auto tang          = Vector3::Zero();
  auto biNorm        = Vector3::Zero();
  auto angle         = 0.f;
  auto facetPlaneVec = Vector3::Zero();

  auto gap      = 0.f;
  auto distance = 0.f;
  auto ray      = Ray(Vector3::Zero(), Vector3(1.f, 0.f, 0.f));
  PickingInfo pickInfo;
  auto direction = Vector3::Zero();

  for (auto index = 0u; index < meshInd.size() / 3; index++) {
    id0 = meshInd[3 * index];
    id1 = meshInd[3 * index + 1];
    id2 = meshInd[3 * index + 2];
    v0X = meshPos[3 * id0];
    v0Y = meshPos[3 * id0 + 1];
    v0Z = meshPos[3 * id0 + 2];
    v1X = meshPos[3 * id1];
    v1Y = meshPos[3 * id1 + 1];
    v1Z = meshPos[3 * id1 + 2];
    v2X = meshPos[3 * id2];
    v2Y = meshPos[3 * id2 + 1];
    v2Z = meshPos[3 * id2 + 2];
    vertex0.set(v0X, v0Y, v0Z);
    vertex1.set(v1X, v1Y, v1Z);
    vertex2.set(v2X, v2Y, v2Z);
    vertex1.subtractToRef(vertex0, vec0);
    vertex2.subtractToRef(vertex1, vec1);

    if (!meshUV.empty()) {
      uv0X = meshUV[2 * id0];
      uv0Y = meshUV[2 * id0 + 1];
      uv1X = meshUV[2 * id1];
      uv1Y = meshUV[2 * id1 + 1];
      uv2X = meshUV[2 * id2];
      uv2Y = meshUV[2 * id2 + 1];
      uv0.set(uv0X, uv0Y);
      uv1.set(uv1X, uv1Y);
      uv2.set(uv2X, uv2Y);
      uv1.subtractToRef(uv0, uvec0);
      uv2.subtractToRef(uv1, uvec1);
    }

    if (!meshCol.empty() && colorFromTexture) {
      col0X = meshCol[4 * id0];
      col0Y = meshCol[4 * id0 + 1];
      col0Z = meshCol[4 * id0 + 2];
      col0A = meshCol[4 * id0 + 3];
      col1X = meshCol[4 * id1];
      col1Y = meshCol[4 * id1 + 1];
      col1Z = meshCol[4 * id1 + 2];
      col1A = meshCol[4 * id1 + 3];
      col2X = meshCol[4 * id2];
      col2Y = meshCol[4 * id2 + 1];
      col2Z = meshCol[4 * id2 + 2];
      col2A = meshCol[4 * id2 + 3];
      col0.set(col0X, col0Y, col0Z, col0A);
      col1.set(col1X, col1Y, col1Z, col1A);
      col2.set(col2X, col2Y, col2Z, col2A);
      col1.subtractToRef(col0, colvec0);
      col2.subtractToRef(col1, colvec1);
    }

    auto width  = 0.f;
    auto height = 0.f;
    auto deltaS = 0.f;
    auto deltaV = 0.f;
    auto h      = 0.f;
    auto s      = 0.f;
    auto v      = 0.f;
    Color3 hsvCol;
    auto statedColor = Color3(0.f, 0.f, 0.f);
    auto colPoint3   = Color3(0.f, 0.f, 0.f);
    Color4 pointColors;
    CloudPointPtr particle = nullptr;

    for (auto i = 0u; i < pointsGroup->_groupDensity[index]; ++i) {
      idxPoints = particles.size();
      _addParticle(idxPoints, pointsGroup, _groupCounter, index + i);
      particle = particles[idxPoints];
      // form a point inside the facet v0, v1, v2;
      lamda      = Scalar::RandomRange(0.f, 1.f);
      mu         = Scalar::RandomRange(0.f, 1.f);
      facetPoint = vertex0.add(vec0.scale(lamda)).add(vec1.scale(lamda * mu));
      if (isVolume) {
        norm          = mesh->getFacetNormal(index).normalize().scale(-1);
        tang          = vec0.clone()->normalize();
        biNorm        = Vector3::Cross(norm, tang);
        angle         = Scalar::RandomRange(0.f, 2.f * Math::PI);
        facetPlaneVec = tang.scale(std::cos(angle)).add(biNorm.scale(std::sin(angle)));
        angle         = Scalar::RandomRange(0.1f, Math::PI_2);
        direction     = facetPlaneVec.scale(std::cos(angle)).add(norm.scale(std::sin(angle)));

        ray.origin    = facetPoint.add(direction.scale(0.00001f));
        ray.direction = direction;
        ray.length    = diameter;
        pickInfo      = ray.intersectsMesh(mesh.get());
        if (pickInfo.hit) {
          distance = pickInfo.pickedPoint->subtract(facetPoint).length();
          gap      = Scalar::RandomRange(0.f, 1.f) * distance;
          facetPoint.addInPlace(direction.scale(gap));
        }
      }
      particle->position = facetPoint.copy();
      stl_util::concat(_positions,
                       {particle->position.x, particle->position.y, particle->position.z});
      if (colorFromTexture.has_value()) {
        if (!meshUV.empty()) {
          uvPoint = uv0.add(uvec0.scale(lamda)).add(uvec1.scale(lamda * mu));
          if (colorFromTexture) { // Set particle color to texture color
            if (hasTexture && pointsGroup->_groupImageData) {
              width       = pointsGroup->_groupImgWidth;
              height      = pointsGroup->_groupImgHeight;
              pointColors = _getColorIndicesForCoord(
                *pointsGroup, static_cast<uint32_t>(std::round(uvPoint.x * width)),
                static_cast<uint32_t>(std::round(uvPoint.y * height)),
                static_cast<uint32_t>(width));
              particle->color = std::make_unique<Color4>(pointColors);
              stl_util::concat(_colors,
                               {pointColors.r, pointColors.g, pointColors.b, pointColors.a});
            }
            else {
              if (!meshCol.empty()) { // failure in texture and colors available
                colPoint = col0.add(colvec0.scale(lamda)).add(colvec1.scale(lamda * mu));
                particle->color
                  = std::make_unique<Color4>(colPoint.x, colPoint.y, colPoint.z, colPoint.w);
                stl_util::concat(_colors, {colPoint.x, colPoint.y, colPoint.z, colPoint.w});
              }
              else {
                colPoint = col0.set(Math::random(), Math::random(), Math::random(), 1.f);
                particle->color
                  = std::make_unique<Color4>(colPoint.x, colPoint.y, colPoint.z, colPoint.w);
                stl_util::concat(_colors, {colPoint.x, colPoint.y, colPoint.z, colPoint.w});
              }
            }
          }
          else { // Set particle uv based on a mesh uv
            particle->uv = std::make_unique<Vector2>(uvPoint);
            stl_util::concat(_uvs, {particle->uv->x, particle->uv->y});
          }
        }
      }
      else {
        if (color) {
          statedColor.set(color->r, color->g, color->b);
          deltaS = Scalar::RandomRange(-range, range);
          deltaV = Scalar::RandomRange(-range, range);
          hsvCol = statedColor.toHSV();
          h      = hsvCol.r;
          s      = hsvCol.g + deltaS;
          v      = hsvCol.b + deltaV;
          if (s < 0.f) {
            s = 0.f;
          }
          if (s > 1.f) {
            s = 1.f;
          }
          if (v < 0.f) {
            v = 0.f;
          }
          if (v > 1.f) {
            v = 1.f;
          }
          Color3::HSVtoRGBToRef(h, s, v, colPoint3);
          colPoint.set(colPoint3.r, colPoint3.g, colPoint3.b, 1);
        }
        else {
          colPoint = col0.set(Math::random(), Math::random(), Math::random(), 1.f);
        }
        particle->color = std::make_unique<Color4>(colPoint.x, colPoint.y, colPoint.z, colPoint.w);
        stl_util::concat(_colors, {colPoint.x, colPoint.y, colPoint.z, colPoint.w});
      }
    }
  }
}

} // end of namespace BABYLON
