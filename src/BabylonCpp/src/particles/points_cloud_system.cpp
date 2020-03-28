#include <babylon/particles/points_cloud_system.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/collisions/picking_info.h>
#include <babylon/core/logging.h>
#include <babylon/core/random.h>
#include <babylon/culling/ray.h>
#include <babylon/engines/engine_store.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/tmp_vectors.h>
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
    , isAlwaysVisible{this, &PointsCloudSystem::get_isAlwaysVisible,
                      &PointsCloudSystem::set_isAlwaysVisible}
    , computeParticleRotation{this, &PointsCloudSystem::set_computeParticleRotation}
    , computeParticleColor{this, &PointsCloudSystem::get_computeParticleColor,
                           &PointsCloudSystem::set_computeParticleColor}
    , computeParticleTexture{this, &PointsCloudSystem::get_computeParticleTexture,
                             &PointsCloudSystem::set_computeParticleTexture}
    , computeBoundingBox{this, &PointsCloudSystem::get_computeBoundingBox,
                         &PointsCloudSystem::set_computeBoundingBox}
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
    addPoints(1);
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

CloudPointPtr PointsCloudSystem::_addParticle(size_t idx, const PointsGroupPtr& group,
                                              size_t groupId, size_t idxInGroup)
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

void PointsCloudSystem::_setPointsColorOrUV(const MeshPtr& mesh, const PointsGroupPtr& pointsGroup,
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

void PointsCloudSystem::_colorFromTexture(const MeshPtr& mesh, const PointsGroupPtr& pointsGroup,
                                          bool isVolume)
{
  if (mesh->material() == nullptr) {
    BABYLON_LOGF_WARN("PointsCloudSystem", "%s has no material.", mesh->name.c_str())
    pointsGroup->_groupImageData.clear();
    _setPointsColorOrUV(mesh, pointsGroup, isVolume, true, false);
    return;
  }

  auto mat         = mesh->material();
  auto textureList = mat->getActiveTextures();
  if (textureList.size() == 0) {
    BABYLON_LOGF_WARN("PointsCloudSystem", "%s has no useable texture.", mesh->name.c_str())
    pointsGroup->_groupImageData.clear();
    _setPointsColorOrUV(mesh, pointsGroup, isVolume, true, false);
    return;
  }

  auto clone = mesh->clone();
  clone->setEnabled(false);
  {
    auto n = pointsGroup->_textureNb;
    if (n > textureList.size() - 1) {
      n = textureList.size() - 1;
    }
    pointsGroup->_groupImageData = textureList[n]->readPixels();
    pointsGroup->_groupImgWidth  = static_cast<size_t>(textureList[n]->getSize().width);
    pointsGroup->_groupImgHeight = static_cast<size_t>(textureList[n]->getSize().height);
    _setPointsColorOrUV(clone, pointsGroup, isVolume, true, true);
    clone->dispose();
  }
  return;
}

Float32Array PointsCloudSystem::_calculateDensity(size_t nbPoints, const Float32Array& positions,
                                                  const IndicesArray& indices)
{
  Float32Array density;
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
  auto vec2    = Vector3::Zero();

  auto a    = 0.f; // length of side of triangle
  auto b    = 0.f; // length of side of triangle
  auto c    = 0.f; // length of side of triangle
  auto p    = 0.f; // perimeter of triangle
  auto area = 0.f;
  Float32Array areas;
  auto surfaceArea = 0.f;

  auto nbFacets = indices.size() / 3;

  // surface area
  for (size_t index = 0; index < nbFacets; index++) {
    id0 = indices[3 * index];
    id1 = indices[3 * index + 1];
    id2 = indices[3 * index + 2];
    v0X = positions[3 * id0];
    v0Y = positions[3 * id0 + 1];
    v0Z = positions[3 * id0 + 2];
    v1X = positions[3 * id1];
    v1Y = positions[3 * id1 + 1];
    v1Z = positions[3 * id1 + 2];
    v2X = positions[3 * id2];
    v2Y = positions[3 * id2 + 1];
    v2Z = positions[3 * id2 + 2];
    vertex0.set(v0X, v0Y, v0Z);
    vertex1.set(v1X, v1Y, v1Z);
    vertex2.set(v2X, v2Y, v2Z);
    vertex1.subtractToRef(vertex0, vec0);
    vertex2.subtractToRef(vertex1, vec1);
    vertex2.subtractToRef(vertex0, vec2);
    a    = vec0.length();
    b    = vec1.length();
    c    = vec2.length();
    p    = (a + b + c) / 2.f;
    area = std::sqrt(p * (p - a) * (p - b) * (p - c));
    surfaceArea += area;
    areas[index] = area;
  }
  auto pointCount = 0ull;
  for (size_t index = 0; index < nbFacets; index++) {
    density[index] = std::floor(nbPoints * areas[index] / surfaceArea);
    pointCount += static_cast<uint32_t>(density[index]);
  }

  auto diff           = nbPoints - pointCount;
  auto pointsPerFacet = std::floor(static_cast<float>(diff) / nbFacets);
  auto extraPoints    = diff % nbFacets;

  if (pointsPerFacet > 0) {
    for (size_t i; i < density.size(); ++i) {
      density[i] += pointsPerFacet;
    }
  }

  for (size_t index = 0; index < extraPoints; index++) {
    density[index] += 1;
  }

  return density;
}

size_t PointsCloudSystem::addPoints(
  size_t nb, const std::function<void(CloudPoint* particle, size_t i, size_t s)>& pointFunction)
{
  auto pointsGroup = std::make_shared<PointsGroup>(_groupCounter, pointFunction);
  CloudPointPtr cp = nullptr;

  // particles
  auto idx = nbParticles;
  for (size_t i = 0; i < nb; ++i) {
    cp = _addParticle(idx, pointsGroup, _groupCounter, i);
    if (pointsGroup && pointsGroup->_positionFunction) {
      pointsGroup->_positionFunction(cp.get(), idx, i);
    }
    stl_util::concat(_positions, {cp->position.x, cp->position.y, cp->position.z});
    if (cp->color) {
      stl_util::concat(_colors, {cp->color->r, cp->color->g, cp->color->b, cp->color->a});
    }
    if (cp->uv) {
      stl_util::concat(_uvs, {cp->uv->x, cp->uv->y});
    }
    idx++;
  }
  nbParticles += nb;
  ++_groupCounter;
  return this->_groupCounter;
}

size_t PointsCloudSystem::addSurfacePoints(
  const MeshPtr& mesh, size_t nb, const std::optional<PointColor> colorWith,
  const std::optional<std::variant<Color4, size_t>>& iColor, const std::optional<int> range)
{
  auto colored = colorWith.value_or(PointColor::Random);
  if (static_cast<int>(colored) < 0 || static_cast<int>(colored) > 3) {
    colored = PointColor::Random;
  }

  const auto meshPos = mesh->getVerticesData(VertexBuffer::PositionKind);
  const auto meshInd = mesh->getIndices();

  _groups.emplace_back(_groupCounter);
  auto pointsGroup = std::make_shared<PointsGroup>(_groupCounter, nullptr);

  pointsGroup->_groupDensity = _calculateDensity(nb, meshPos, meshInd);
  Color4 color;
  if (colored == PointColor::Color) {
    pointsGroup->_textureNb
      = iColor ? (std::holds_alternative<size_t>(*iColor) ? std::get<size_t>(*iColor) : 0) : 0;
  }
  else {
    color = iColor ? (std::holds_alternative<Color4>(*iColor) ? std::get<Color4>(*iColor) :
                                                                Color4(1.f, 1.f, 1.f, 1.f)) :
                     Color4(1.f, 1.f, 1.f, 1.f);
  }
  switch (colored) {
    case PointColor::Color:
      _colorFromTexture(mesh, pointsGroup, false);
      break;
    case PointColor::UV:
      _setPointsColorOrUV(mesh, pointsGroup, false, false, false);
      break;
    case PointColor::Random:
      _setPointsColorOrUV(mesh, pointsGroup, false);
      break;
    case PointColor::Stated:
      _setPointsColorOrUV(mesh, pointsGroup, false, std::nullopt, std::nullopt, color, range);
      break;
  }
  nbParticles += nb;
  ++_groupCounter;
  return _groupCounter - 1;
}

size_t PointsCloudSystem::addVolumePoints(const MeshPtr& mesh, size_t nb,
                                          const std::optional<PointColor> colorWith,
                                          const std::optional<std::variant<Color4, size_t>>& iColor,
                                          const std::optional<int> range)
{
  auto colored = colorWith.value_or(PointColor::Random);
  if (static_cast<int>(colored) < 0 || static_cast<int>(colored) > 3) {
    colored = PointColor::Random;
  }

  const auto meshPos = mesh->getVerticesData(VertexBuffer::PositionKind);
  const auto meshInd = mesh->getIndices();

  _groups.emplace_back(_groupCounter);
  auto pointsGroup = std::make_shared<PointsGroup>(_groupCounter, nullptr);

  pointsGroup->_groupDensity = _calculateDensity(nb, meshPos, meshInd);
  Color4 color;
  if (colored == PointColor::Color) {
    pointsGroup->_textureNb
      = iColor ? (std::holds_alternative<size_t>(*iColor) ? std::get<size_t>(*iColor) : 0) : 0;
  }
  else {
    color = iColor ? (std::holds_alternative<Color4>(*iColor) ? std::get<Color4>(*iColor) :
                                                                Color4(1.f, 1.f, 1.f, 1.f)) :
                     Color4(1.f, 1.f, 1.f, 1.f);
  }
  switch (colored) {
    case PointColor::Color:
      _colorFromTexture(mesh, pointsGroup, true);
      break;
    case PointColor::UV:
      _setPointsColorOrUV(mesh, pointsGroup, true, false, false);
      break;
    case PointColor::Random:
      _setPointsColorOrUV(mesh, pointsGroup, true);
      break;
    case PointColor::Stated:
      _setPointsColorOrUV(mesh, pointsGroup, true, std::nullopt, std::nullopt, color, range);
      break;
  }
  nbParticles += nb;
  ++_groupCounter;
  return _groupCounter - 1;
}

PointsCloudSystem& PointsCloudSystem::setParticles(size_t start, size_t end, bool update)
{
  if (!_updatable || !_isReady) {
    return *this;
  }

  // custom beforeUpdate
  beforeUpdateParticles(start, end, update);

  auto& rotMatrix  = TmpVectors::MatrixArray[0];
  MeshPtr mesh     = nullptr;
  auto colors32    = _colors32;
  auto positions32 = _positions32;
  auto uvs32       = _uvs32;

  auto& tempVectors = TmpVectors::Vector3Array;
  auto& camAxisX    = tempVectors[5].copyFromFloats(1.0, 0.0, 0.0);
  auto& camAxisY    = tempVectors[6].copyFromFloats(0.0, 1.0, 0.0);
  auto& camAxisZ    = tempVectors[7].copyFromFloats(0.0, 0.0, 1.0);
  auto& minimum     = tempVectors[8].setAll(std::numeric_limits<float>::max());
  auto& maximum     = tempVectors[9].setAll(std::numeric_limits<float>::lowest());

  Matrix::IdentityToRef(rotMatrix);
  size_t idx = 0; // current index of the particle

  if (mesh->isFacetDataEnabled()) {
    _computeBoundingBox = true;
  }

  end = (end == 0) ? nbParticles - 1 : end;
  end = (end >= nbParticles) ? nbParticles - 1 : end;
  if (_computeBoundingBox) {
    if (start != 0
        || end != nbParticles - 1) { // only some particles are updated, then use the current
                                     // existing BBox basis. Note : it can only increase.
      const auto& boundingInfo = mesh->_boundingInfo;
      if (boundingInfo) {
        minimum.copyFrom(boundingInfo->minimum);
        maximum.copyFrom(boundingInfo->maximum);
      }
    }
  }

  idx           = 0; // particle index
  size_t pindex = 0; // index in positions array
  size_t cindex = 0; // index in color array
  size_t uindex = 0; // index in uv array

  // particle loop
  for (size_t p = start; p <= end; ++p) {
    const auto& particle = particles[p];
    idx                  = particle->idx;
    pindex               = 3 * idx;
    cindex               = 4 * idx;
    uindex               = 2 * idx;

    // call to custom user function to update the particle properties
    updateParticle(particle);

    auto& particleRotationMatrix = particle->_rotationMatrix;
    const auto& particlePosition = particle->position;
    auto& particleGlobalPosition = particle->_globalPosition;

    if (_computeParticleRotation) {
      particle->getRotationMatrix(rotMatrix);
    }

    const auto particleHasParent = (particle->parentId.has_value());
    if (particleHasParent) {
      const auto& parent               = particles[*particle->parentId];
      const auto& parentRotationMatrix = parent->_rotationMatrix;
      const auto& parentGlobalPosition = parent->_globalPosition;

      const auto& rotatedY = particlePosition.x * parentRotationMatrix[1]
                             + particlePosition.y * parentRotationMatrix[4]
                             + particlePosition.z * parentRotationMatrix[7];
      const auto& rotatedX = particlePosition.x * parentRotationMatrix[0]
                             + particlePosition.y * parentRotationMatrix[3]
                             + particlePosition.z * parentRotationMatrix[6];
      const auto& rotatedZ = particlePosition.x * parentRotationMatrix[2]
                             + particlePosition.y * parentRotationMatrix[5]
                             + particlePosition.z * parentRotationMatrix[8];

      particleGlobalPosition.x = parentGlobalPosition.x + rotatedX;
      particleGlobalPosition.y = parentGlobalPosition.y + rotatedY;
      particleGlobalPosition.z = parentGlobalPosition.z + rotatedZ;

      if (_computeParticleRotation) {
        const auto& rotMatrixValues = rotMatrix.m();
        particleRotationMatrix[0]   = rotMatrixValues[0] * parentRotationMatrix[0]
                                    + rotMatrixValues[1] * parentRotationMatrix[3]
                                    + rotMatrixValues[2] * parentRotationMatrix[6];
        particleRotationMatrix[1] = rotMatrixValues[0] * parentRotationMatrix[1]
                                    + rotMatrixValues[1] * parentRotationMatrix[4]
                                    + rotMatrixValues[2] * parentRotationMatrix[7];
        particleRotationMatrix[2] = rotMatrixValues[0] * parentRotationMatrix[2]
                                    + rotMatrixValues[1] * parentRotationMatrix[5]
                                    + rotMatrixValues[2] * parentRotationMatrix[8];
        particleRotationMatrix[3] = rotMatrixValues[4] * parentRotationMatrix[0]
                                    + rotMatrixValues[5] * parentRotationMatrix[3]
                                    + rotMatrixValues[6] * parentRotationMatrix[6];
        particleRotationMatrix[4] = rotMatrixValues[4] * parentRotationMatrix[1]
                                    + rotMatrixValues[5] * parentRotationMatrix[4]
                                    + rotMatrixValues[6] * parentRotationMatrix[7];
        particleRotationMatrix[5] = rotMatrixValues[4] * parentRotationMatrix[2]
                                    + rotMatrixValues[5] * parentRotationMatrix[5]
                                    + rotMatrixValues[6] * parentRotationMatrix[8];
        particleRotationMatrix[6] = rotMatrixValues[8] * parentRotationMatrix[0]
                                    + rotMatrixValues[9] * parentRotationMatrix[3]
                                    + rotMatrixValues[10] * parentRotationMatrix[6];
        particleRotationMatrix[7] = rotMatrixValues[8] * parentRotationMatrix[1]
                                    + rotMatrixValues[9] * parentRotationMatrix[4]
                                    + rotMatrixValues[10] * parentRotationMatrix[7];
        particleRotationMatrix[8] = rotMatrixValues[8] * parentRotationMatrix[2]
                                    + rotMatrixValues[9] * parentRotationMatrix[5]
                                    + rotMatrixValues[10] * parentRotationMatrix[8];
      }
    }
    else {
      particleGlobalPosition.x = 0;
      particleGlobalPosition.y = 0;
      particleGlobalPosition.z = 0;

      if (_computeParticleRotation) {
        const auto& rotMatrixValues = rotMatrix.m();
        particleRotationMatrix[0]   = rotMatrixValues[0];
        particleRotationMatrix[1]   = rotMatrixValues[1];
        particleRotationMatrix[2]   = rotMatrixValues[2];
        particleRotationMatrix[3]   = rotMatrixValues[4];
        particleRotationMatrix[4]   = rotMatrixValues[5];
        particleRotationMatrix[5]   = rotMatrixValues[6];
        particleRotationMatrix[6]   = rotMatrixValues[8];
        particleRotationMatrix[7]   = rotMatrixValues[9];
        particleRotationMatrix[8]   = rotMatrixValues[10];
      }
    }

    auto& pivotBackTranslation = tempVectors[11];
    if (particle->translateFromPivot) {
      pivotBackTranslation.setAll(0.f);
    }
    else {
      pivotBackTranslation.copyFrom(particle->pivot);
    }

    // positions
    auto& tmpVertex = tempVectors[0];
    tmpVertex.copyFrom(particle->position);
    const auto vertexX = tmpVertex.x - particle->pivot.x;
    const auto vertexY = tmpVertex.y - particle->pivot.y;
    const auto vertexZ = tmpVertex.z - particle->pivot.z;

    auto rotatedX = vertexX * particleRotationMatrix[0] + vertexY * particleRotationMatrix[3]
                    + vertexZ * particleRotationMatrix[6];
    auto rotatedY = vertexX * particleRotationMatrix[1] + vertexY * particleRotationMatrix[4]
                    + vertexZ * particleRotationMatrix[7];
    auto rotatedZ = vertexX * particleRotationMatrix[2] + vertexY * particleRotationMatrix[5]
                    + vertexZ * particleRotationMatrix[8];

    rotatedX += pivotBackTranslation.x;
    rotatedY += pivotBackTranslation.y;
    rotatedZ += pivotBackTranslation.z;

    const auto px = positions32[pindex] = particleGlobalPosition.x + camAxisX.x * rotatedX
                                          + camAxisY.x * rotatedY + camAxisZ.x * rotatedZ;
    const auto py = positions32[pindex + 1] = particleGlobalPosition.y + camAxisX.y * rotatedX
                                              + camAxisY.y * rotatedY + camAxisZ.y * rotatedZ;
    const auto pz = positions32[pindex + 2] = particleGlobalPosition.z + camAxisX.z * rotatedX
                                              + camAxisY.z * rotatedY + camAxisZ.z * rotatedZ;

    if (_computeBoundingBox) {
      minimum.minimizeInPlaceFromFloats(px, py, pz);
      maximum.maximizeInPlaceFromFloats(px, py, pz);
    }

    if (_computeParticleColor && particle->color) {
      const auto& color    = *particle->color;
      auto& colors32       = _colors32;
      colors32[cindex]     = color.r;
      colors32[cindex + 1] = color.g;
      colors32[cindex + 2] = color.b;
      colors32[cindex + 3] = color.a;
    }
    if (_computeParticleTexture && particle->uv) {
      const auto& uv    = *particle->uv;
      auto& uvs32       = _uvs32;
      uvs32[uindex]     = uv.x;
      uvs32[uindex + 1] = uv.y;
    }
  }

  // if the VBO must be updated
  if (update) {
    if (_computeParticleColor) {
      mesh->updateVerticesData(VertexBuffer::ColorKind, colors32, false, false);
    }
    if (_computeParticleTexture) {
      mesh->updateVerticesData(VertexBuffer::UVKind, uvs32, false, false);
    }
    mesh->updateVerticesData(VertexBuffer::PositionKind, positions32, false, false);
  }

  if (_computeBoundingBox) {
    if (mesh->_boundingInfo) {
      mesh->_boundingInfo->reConstruct(minimum, maximum, mesh->_worldMatrix);
    }
    else {
      mesh->_boundingInfo = std::make_shared<BoundingInfo>(minimum, maximum, mesh->_worldMatrix);
    }
  }
  afterUpdateParticles(start, end, update);
  return *this;
}

void PointsCloudSystem::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  mesh->dispose();
  // drop references to internal big arrays for the GC
  _positions.clear();
  _indices.clear();
  _normals.clear();
  _uvs.clear();
  _colors.clear();
  _indices32.clear();
  _positions32.clear();
  _uvs32.clear();
  _colors32.clear();
}

PointsCloudSystem& PointsCloudSystem::refreshVisibleSize()
{
  if (!_isVisibilityBoxLocked) {
    mesh->refreshBoundingInfo();
  }
  return *this;
}

void PointsCloudSystem::setVisibilityBox(float size)
{
  auto vis = size / 2.f;
  mesh->_boundingInfo
    = std::make_shared<BoundingInfo>(Vector3(-vis, -vis, -vis), Vector3(vis, vis, vis));
}

bool PointsCloudSystem::get_isAlwaysVisible() const
{
  return _alwaysVisible;
}

void PointsCloudSystem::set_isAlwaysVisible(bool val)
{
  _alwaysVisible                 = val;
  mesh->alwaysSelectAsActiveMesh = val;
}

void PointsCloudSystem::set_computeParticleRotation(bool val)
{
  _computeParticleRotation = val;
}

void PointsCloudSystem::set_computeParticleColor(bool val)
{
  _computeParticleColor = val;
}

void PointsCloudSystem::set_computeParticleTexture(bool val)
{
  _computeParticleTexture = val;
}

bool PointsCloudSystem::get_computeParticleColor() const
{
  return _computeParticleColor;
}

bool PointsCloudSystem::get_computeParticleTexture() const
{
  return _computeParticleTexture;
}

void PointsCloudSystem::set_computeBoundingBox(bool val)
{
  _computeBoundingBox = val;
}

bool PointsCloudSystem::get_computeBoundingBox() const
{
  return _computeBoundingBox;
}

void PointsCloudSystem::initParticles()
{
}

CloudPointPtr PointsCloudSystem::recycleParticle(const CloudPointPtr& particle)
{
  return particle;
}

CloudPointPtr PointsCloudSystem::updateParticle(const CloudPointPtr& particle)
{
  return particle;
}

void PointsCloudSystem::beforeUpdateParticles(std::optional<size_t> /*start*/,
                                              std::optional<size_t> /*stop*/,
                                              std::optional<bool> /*update*/)
{
}

void PointsCloudSystem::afterUpdateParticles(std::optional<size_t> /*start*/,
                                             std::optional<size_t> /*stop*/,
                                             std::optional<bool> /*update*/)
{
}

} // end of namespace BABYLON
