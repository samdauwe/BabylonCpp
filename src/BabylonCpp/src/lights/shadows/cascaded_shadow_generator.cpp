#include <babylon/lights/shadows/cascaded_shadow_generator.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/misc/depth_reducer.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

const std::vector<Vector3> CascadedShadowGenerator::frustumCornersNDCSpace{
  Vector3(-1.f, +1.f, -1.f), //
  Vector3(+1.f, +1.f, -1.f), //
  Vector3(+1.f, -1.f, -1.f), //
  Vector3(-1.f, -1.f, -1.f), //
  Vector3(-1.f, +1.f, +1.f), //
  Vector3(+1.f, +1.f, +1.f), //
  Vector3(+1.f, -1.f, +1.f), //
  Vector3(-1.f, -1.f, +1.f), //
};

CascadedShadowGenerator::CascadedShadowGenerator(int mapSize, const DirectionalLightPtr& light,
                                                 bool usefulFloatFirst)
    : ShadowGenerator{mapSize, light, usefulFloatFirst}
    , numCascades{this, &CascadedShadowGenerator::get_numCascades,
                  &CascadedShadowGenerator::set_numCascades}
    , freezeShadowCastersBoundingInfo{this,
                                      &CascadedShadowGenerator::get_freezeShadowCastersBoundingInfo,
                                      &CascadedShadowGenerator::set_freezeShadowCastersBoundingInfo}
    , shadowCastersBoundingInfo{this, &CascadedShadowGenerator::get_shadowCastersBoundingInfo,
                                &CascadedShadowGenerator::set_shadowCastersBoundingInfo}
    , minDistance{this, &CascadedShadowGenerator::get_minDistance}
    , maxDistance{this, &CascadedShadowGenerator::get_maxDistance}
    , shadowMaxZ{this, &CascadedShadowGenerator::get_shadowMaxZ,
                 &CascadedShadowGenerator::set_shadowMaxZ}
    , debug{this, &CascadedShadowGenerator::get_debug, &CascadedShadowGenerator::set_debug}
    , depthClamp{this, &CascadedShadowGenerator::get_depthClamp,
                 &CascadedShadowGenerator::set_depthClamp}
    , cascadeBlendPercentage{this, &CascadedShadowGenerator::get_cascadeBlendPercentage,
                             &CascadedShadowGenerator::set_cascadeBlendPercentage}
    , lambda{this, &CascadedShadowGenerator::get_lambda, &CascadedShadowGenerator::set_lambda}
    , autoCalcDepthBounds{this, &CascadedShadowGenerator::get_autoCalcDepthBounds,
                          &CascadedShadowGenerator::set_autoCalcDepthBounds}
    , autoCalcDepthBoundsRefreshRate{this,
                                     &CascadedShadowGenerator::get_autoCalcDepthBoundsRefreshRate,
                                     &CascadedShadowGenerator::set_autoCalcDepthBoundsRefreshRate}
    , _shadowCastersBoundingInfo{BoundingInfo{Vector3::Zero(), Vector3::Zero()}}
    , UpDir{Vector3::Up()}
    , ZeroVec{Vector3::Zero()}
    , _freezeShadowCastersBoundingInfoObservable{nullptr}
    , _depthRenderer{nullptr}
    , _depthReducer{nullptr}
{
  if (_scene->getEngine()->webGLVersion == 1.f) {
    throw std::runtime_error("CSM can only be used in WebGL2");
  }

  usePercentageCloserFiltering = true;
}

CascadedShadowGenerator::~CascadedShadowGenerator() = default;

unsigned int CascadedShadowGenerator::_validateFilter(unsigned int iFilter) const
{
  if (iFilter == ShadowGenerator::FILTER_NONE || iFilter == ShadowGenerator::FILTER_PCF
      || iFilter == ShadowGenerator::FILTER_PCSS) {
    return filter;
  }

  BABYLON_LOGF_ERROR("CascadedShadowGenerator", "Unsupported filter \"%u\"!", iFilter)

  return ShadowGenerator::FILTER_NONE;
}

unsigned int CascadedShadowGenerator::get_numCascades() const
{
  return _numCascades;
}

void CascadedShadowGenerator::set_numCascades(unsigned int value)
{
  value = std::min(std::max(value, CascadedShadowGenerator::MIN_CASCADES_COUNT),
                   CascadedShadowGenerator::MAX_CASCADES_COUNT);
  if (value == _numCascades) {
    return;
  }

  _numCascades = value;
  recreateShadowMap();
}

bool CascadedShadowGenerator::get_freezeShadowCastersBoundingInfo() const
{
  return _freezeShadowCastersBoundingInfo;
}

void CascadedShadowGenerator::set_freezeShadowCastersBoundingInfo(bool freeze)
{
  if (_freezeShadowCastersBoundingInfoObservable && freeze) {
    _scene->onBeforeRenderObservable.remove(_freezeShadowCastersBoundingInfoObservable);
    _freezeShadowCastersBoundingInfoObservable = nullptr;
  }

  if (!_freezeShadowCastersBoundingInfoObservable && !freeze) {
    _freezeShadowCastersBoundingInfoObservable = _scene->onBeforeRenderObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) { _computeShadowCastersBoundingInfo(); });
  }

  _freezeShadowCastersBoundingInfo = freeze;

  if (freeze) {
    _computeShadowCastersBoundingInfo();
  }
}

void CascadedShadowGenerator::_computeShadowCastersBoundingInfo()
{
  _scbiMin.copyFromFloats(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                          std::numeric_limits<float>::max());
  _scbiMax.copyFromFloats(std::numeric_limits<float>::lowest(),
                          std::numeric_limits<float>::lowest(),
                          std::numeric_limits<float>::lowest());

  if (_shadowMap && !_shadowMap->renderList().empty()) {
    const auto& renderList = _shadowMap->renderList();
    for (const auto& mesh : renderList) {
      if (!mesh) {
        continue;
      }

      const auto& boundingInfo = mesh->getBoundingInfo();
      const auto& boundingBox  = boundingInfo->boundingBox;

      _scbiMin.minimizeInPlace(boundingBox.minimumWorld);
      _scbiMax.maximizeInPlace(boundingBox.maximumWorld);
    }

    const auto& meshes = _scene->meshes;
    for (auto& mesh : meshes) {
      if (!mesh || !mesh->isVisible || !mesh->isEnabled() || !mesh->receiveShadows()) {
        continue;
      }

      const auto& boundingInfo = mesh->getBoundingInfo();
      const auto& boundingBox  = boundingInfo->boundingBox;

      _scbiMin.minimizeInPlace(boundingBox.minimumWorld);
      _scbiMax.maximizeInPlace(boundingBox.maximumWorld);
    }
  }

  _shadowCastersBoundingInfo.reConstruct(_scbiMin, _scbiMax);
}

BoundingInfo& CascadedShadowGenerator::get_shadowCastersBoundingInfo()
{
  return _shadowCastersBoundingInfo;
}

void CascadedShadowGenerator::set_shadowCastersBoundingInfo(const BoundingInfo& boundingInfo)
{
  _shadowCastersBoundingInfo = boundingInfo;
}

void CascadedShadowGenerator::setMinMaxDistance(float min, float max)
{
  if (stl_util::almost_equal(_minDistance, min) && stl_util::almost_equal(_maxDistance, max)) {
    return;
  }

  if (min > max) {
    min = 0;
    max = 1;
  }

  if (min < 0) {
    min = 0;
  }

  if (max > 1) {
    max = 1;
  }

  _minDistance    = min;
  _maxDistance    = max;
  _breaksAreDirty = true;
}

float CascadedShadowGenerator::get_minDistance() const
{
  return _minDistance;
}

float CascadedShadowGenerator::get_maxDistance() const
{
  return _maxDistance;
}

std::string CascadedShadowGenerator::getClassName() const
{
  return CascadedShadowGenerator::CLASSNAME;
}

std::optional<Vector3>
CascadedShadowGenerator::getCascadeMinExtents(unsigned int cascadeIndex) const
{
  if (cascadeIndex < _numCascades) {
    return _cascadeMinExtents[cascadeIndex];
  }
  else {
    return std::nullopt;
  }
}

std::optional<Vector3>
CascadedShadowGenerator::getCascadeMaxExtents(unsigned int cascadeIndex) const
{
  if (cascadeIndex < _numCascades) {
    return _cascadeMaxExtents[cascadeIndex];
  }
  else {
    return std::nullopt;
  }
}

float CascadedShadowGenerator::get_shadowMaxZ() const
{
  if (!_scene || !_scene->activeCamera()) {
    return 0.f;
  }
  return _shadowMaxZ;
}

void CascadedShadowGenerator::set_shadowMaxZ(float value)
{
  if (!_scene || !_scene->activeCamera()) {
    _shadowMaxZ = value;
    return;
  }
  if (stl_util::almost_equal(_shadowMaxZ, value) || value < _scene->activeCamera()->minZ
      || value > _scene->activeCamera()->maxZ) {
    return;
  }
  _shadowMaxZ = value;
  _light->_markMeshesAsLightDirty();
  _breaksAreDirty = true;
}

bool CascadedShadowGenerator::get_debug() const
{
  return _debug;
}

void CascadedShadowGenerator::set_debug(bool dbg)
{
  _debug = dbg;
  _light->_markMeshesAsLightDirty();
}

bool CascadedShadowGenerator::get_depthClamp() const
{
  return _depthClamp;
}

void CascadedShadowGenerator::set_depthClamp(bool value)
{
  _depthClamp = value;
}

float CascadedShadowGenerator::get_cascadeBlendPercentage() const
{
  return _cascadeBlendPercentage;
}

void CascadedShadowGenerator::set_cascadeBlendPercentage(float value)
{
  _cascadeBlendPercentage = value;
  _light->_markMeshesAsLightDirty();
}

float CascadedShadowGenerator::get_lambda() const
{
  return _lambda;
}

void CascadedShadowGenerator::set_lambda(float value)
{
  const auto iLambda = std::min(std::max(value, 0.f), 1.f);
  if (stl_util::almost_equal(_lambda, iLambda)) {
    return;
  }
  _lambda         = lambda;
  _breaksAreDirty = true;
}

std::optional<Matrix> CascadedShadowGenerator::getCascadeViewMatrix(unsigned int cascadeNum) const
{
  if (cascadeNum < _numCascades) {
    return _viewMatrices[cascadeNum];
  }
  else {
    return std::nullopt;
  }
}

std::optional<Matrix>
CascadedShadowGenerator::getCascadeProjectionMatrix(unsigned int cascadeNum) const
{
  if (cascadeNum < _numCascades) {
    return _projectionMatrices[cascadeNum];
  }
  else {
    return std::nullopt;
  }
}

std::optional<Matrix>
CascadedShadowGenerator::getCascadeTransformMatrix(unsigned int cascadeNum) const
{
  if (cascadeNum < _numCascades) {
    return _transformMatrices[cascadeNum];
  }
  else {
    return std::nullopt;
  }
}

bool CascadedShadowGenerator::get_autoCalcDepthBounds() const
{
  return _autoCalcDepthBounds;
}

void CascadedShadowGenerator::set_autoCalcDepthBounds(bool value)
{
  auto camera = _scene->activeCamera();

  if (!camera) {
    return;
  }

  _autoCalcDepthBounds = value;

  if (!value) {
    if (_depthReducer) {
      _depthReducer->deactivate();
    }
    setMinMaxDistance(0, 1);
    return;
  }

  if (!_depthReducer) {
    _depthReducer = std::make_shared<DepthReducer>(camera);
    _depthReducer->onAfterReductionPerformed.add(
      [this](MinMaxFloats* minmax, EventState & /*es*/) -> void {
        auto min = minmax->min, max = minmax->max;
        if (min >= max) {
          min = 0.f;
          max = 1.f;
        }
        if (!stl_util::almost_equal(min, _minDistance)
            || !stl_util::almost_equal(max, _maxDistance)) {
          setMinMaxDistance(min, max);
        }
      });
    _depthReducer->setDepthRenderer(_depthRenderer);
  }

  _depthReducer->activate();
}

int CascadedShadowGenerator::get_autoCalcDepthBoundsRefreshRate() const
{
  if (_depthReducer && _depthReducer->depthRenderer()
      && _depthReducer->depthRenderer()->getDepthMap()) {
    return _depthReducer->depthRenderer()->getDepthMap()->refreshRate();
  }
  else {
    return -1;
  }
}

void CascadedShadowGenerator::set_autoCalcDepthBoundsRefreshRate(int value)
{
  if (_depthReducer && _depthReducer->depthRenderer()
      && _depthReducer->depthRenderer()->getDepthMap()) {
    _depthReducer->depthRenderer()->getDepthMap()->refreshRate = value;
  }
}

void CascadedShadowGenerator::splitFrustum()
{
  _breaksAreDirty = true;
}

void CascadedShadowGenerator::_splitFrustum()
{
  auto camera = _scene->activeCamera();
  if (!camera) {
    return;
  }

  const auto near = camera->minZ, //
    far           = camera->maxZ, //
    cameraRange   = far - near,   //
    iMinDistance  = _minDistance, //
    iMaxDistance  = _shadowMaxZ < far && _shadowMaxZ >= near ?
                     std::min((_shadowMaxZ - near) / (far - near), _maxDistance) :
                     _maxDistance;

  const auto minZ = near + iMinDistance * cameraRange, //
    maxZ          = near + iMaxDistance * cameraRange;

  const auto range = maxZ - minZ, //
    ratio          = maxZ / minZ;

  for (size_t cascadeIndex = 0; cascadeIndex < _cascades.size(); ++cascadeIndex) {
    const float p = static_cast<float>(cascadeIndex + 1) / _numCascades, //
      log         = minZ * static_cast<float>(std::pow(ratio, p)),       //
      uniform     = minZ + range * p;

    const auto d = _lambda * (log - uniform) + uniform;

    _cascades[cascadeIndex].prevBreakDistance
      = cascadeIndex == 0 ? iMinDistance : _cascades[cascadeIndex - 1].breakDistance;
    _cascades[cascadeIndex].breakDistance = (d - near) / cameraRange;

    _viewSpaceFrustumsZ[cascadeIndex] = near + _cascades[cascadeIndex].breakDistance * cameraRange;
    _frustumLengths[cascadeIndex]
      = (_cascades[cascadeIndex].breakDistance - _cascades[cascadeIndex].prevBreakDistance)
        * cameraRange;
  }

  _breaksAreDirty = false;
}

void CascadedShadowGenerator::_computeMatrices()
{
  auto scene = _scene;

  auto camera = scene->activeCamera();
  if (!camera) {
    return;
  }

  Vector3::NormalizeToRef(_light->getShadowDirection(0), _lightDirection);
  if (std::abs(Vector3::Dot(_lightDirection, Vector3::Up())) == 1.f) {
    _lightDirection.z = 0.0000000000001f; // Required to avoid perfectly perpendicular light
  }

  _cachedDirection.copyFrom(_lightDirection);

  for (unsigned int cascadeIndex = 0; cascadeIndex < _numCascades; ++cascadeIndex) {
    _computeFrustumInWorldSpace(cascadeIndex);
    _computeCascadeFrustum(cascadeIndex);

    _cascadeMaxExtents[cascadeIndex].subtractToRef(_cascadeMinExtents[cascadeIndex],
                                                   tmpv1); // tmpv1 = cascadeExtents

    // Get position of the shadow camera
    _frustumCenter[cascadeIndex].addToRef(_lightDirection.scale(_cascadeMinExtents[cascadeIndex].z),
                                          _shadowCameraPos[cascadeIndex]);

    // Come up with a new orthographic camera for the shadow caster
    Matrix::LookAtLHToRef(_shadowCameraPos[cascadeIndex], _frustumCenter[cascadeIndex], UpDir,
                          _viewMatrices[cascadeIndex]);

    auto minZ = 0.f, maxZ = tmpv1.z;

    // Try to tighten minZ and maxZ based on the bounding box of the shadow casters
    auto& boundingInfo = _shadowCastersBoundingInfo;

    boundingInfo.update(_viewMatrices[cascadeIndex]);

    maxZ = std::min(maxZ, boundingInfo.boundingBox.maximumWorld.z);

    if (!_depthClamp || filter == ShadowGenerator::FILTER_PCSS) {
      // If we don't use depth clamping, we must set minZ so that all shadow casters are in the
      // light frustum
      minZ = std::min(minZ, boundingInfo.boundingBox.minimumWorld.z);
    }
    else {
      // If using depth clamping, we can adjust minZ to reduce the [minZ, maxZ] range (and get some
      // additional precision in the shadow map)
      minZ = std::max(minZ, boundingInfo.boundingBox.minimumWorld.z);
    }

    Matrix::OrthoOffCenterLHToRef(
      _cascadeMinExtents[cascadeIndex].x, _cascadeMaxExtents[cascadeIndex].x,
      _cascadeMinExtents[cascadeIndex].y, _cascadeMaxExtents[cascadeIndex].y, minZ, maxZ,
      _projectionMatrices[cascadeIndex]);

    _cascadeMinExtents[cascadeIndex].z = minZ;
    _cascadeMaxExtents[cascadeIndex].z = maxZ;

    _viewMatrices[cascadeIndex].multiplyToRef(_projectionMatrices[cascadeIndex],
                                              _transformMatrices[cascadeIndex]);

    // Create the rounding matrix, by projecting the world-space origin and determining the
    // fractional offset in texel space
    Vector3::TransformCoordinatesToRef(ZeroVec, _transformMatrices[cascadeIndex],
                                       tmpv1); // tmpv1 = shadowOrigin
    tmpv1.scaleInPlace(_mapSize.width / 2.f);

    tmpv2.copyFromFloats(std::round(tmpv1.x), std::round(tmpv1.y),
                         std::round(tmpv1.z));                       // tmpv2 = roundedOrigin
    tmpv2.subtractInPlace(tmpv1).scaleInPlace(2.f / _mapSize.width); // tmpv2 = roundOffset

    Matrix::TranslationToRef(tmpv2.x, tmpv2.y, 0.f, matrix);

    _projectionMatrices[cascadeIndex].multiplyToRef(matrix, _projectionMatrices[cascadeIndex]);
    _viewMatrices[cascadeIndex].multiplyToRef(_projectionMatrices[cascadeIndex],
                                              _transformMatrices[cascadeIndex]);

    _transformMatrices[cascadeIndex].copyToArray(_transformMatricesAsArray, cascadeIndex * 16);
  }
}

void CascadedShadowGenerator::_computeFrustumInWorldSpace(unsigned int cascadeIndex)
{
  if (!_scene->activeCamera()) {
    return;
  }

  const auto prevSplitDist = _cascades[cascadeIndex].prevBreakDistance, //
    splitDist              = _cascades[cascadeIndex].breakDistance;

  _scene->activeCamera()
    ->getViewMatrix(); // make sure the transformation matrix we get when calling
                       // 'getTransformationMatrix()' is calculated with an up to date view matrix

  const auto invViewProj = Matrix::Invert(_scene->activeCamera()->getTransformationMatrix());
  for (size_t cornerIndex = 0; cornerIndex < CascadedShadowGenerator::frustumCornersNDCSpace.size();
       ++cornerIndex) {
    Vector3::TransformCoordinatesToRef(CascadedShadowGenerator::frustumCornersNDCSpace[cornerIndex],
                                       invViewProj,
                                       _frustumCornersWorldSpace[cascadeIndex][cornerIndex]);
  }

  // Get the corners of the current cascade slice of the view frustum
  for (size_t cornerIndex = 0;
       cornerIndex < CascadedShadowGenerator::frustumCornersNDCSpace.size() / 2; ++cornerIndex) {
    tmpv1.copyFrom(_frustumCornersWorldSpace[cascadeIndex][cornerIndex + 4])
      .subtractInPlace(_frustumCornersWorldSpace[cascadeIndex][cornerIndex]);
    tmpv2.copyFrom(tmpv1).scaleInPlace(prevSplitDist); // near corner ray
    tmpv1.scaleInPlace(splitDist);                     // far corner ray

    tmpv1.addInPlace(_frustumCornersWorldSpace[cascadeIndex][cornerIndex]);

    _frustumCornersWorldSpace[cascadeIndex][cornerIndex + 4].copyFrom(tmpv1);
    _frustumCornersWorldSpace[cascadeIndex][cornerIndex].addInPlace(tmpv2);
  }
}

void CascadedShadowGenerator::_computeCascadeFrustum(unsigned int cascadeIndex)
{
  _cascadeMinExtents[cascadeIndex].copyFromFloats(std::numeric_limits<float>::max(),
                                                  std::numeric_limits<float>::max(),
                                                  std::numeric_limits<float>::max());
  _cascadeMaxExtents[cascadeIndex].copyFromFloats(std::numeric_limits<float>::lowest(),
                                                  std::numeric_limits<float>::lowest(),
                                                  std::numeric_limits<float>::lowest());
  _frustumCenter[cascadeIndex].copyFromFloats(0.f, 0.f, 0.f);

  const auto camera = _scene->activeCamera();

  if (!camera) {
    return;
  }

  // Calculate the centroid of the view frustum slice
  for (size_t cornerIndex = 0; cornerIndex < _frustumCornersWorldSpace[cascadeIndex].size();
       ++cornerIndex) {
    _frustumCenter[cascadeIndex].addInPlace(_frustumCornersWorldSpace[cascadeIndex][cornerIndex]);
  }

  _frustumCenter[cascadeIndex].scaleInPlace(1.f / _frustumCornersWorldSpace[cascadeIndex].size());

  if (stabilizeCascades) {
    // Calculate the radius of a bounding sphere surrounding the frustum corners
    auto sphereRadius = 0.f;
    for (size_t cornerIndex = 0; cornerIndex < _frustumCornersWorldSpace[cascadeIndex].size();
         ++cornerIndex) {
      const auto dist = _frustumCornersWorldSpace[cascadeIndex][cornerIndex]
                          .subtractToRef(_frustumCenter[cascadeIndex], tmpv1)
                          .length();
      sphereRadius = std::max(sphereRadius, dist);
    }

    sphereRadius = std::ceil(sphereRadius * 16) / 16.f;

    _cascadeMaxExtents[cascadeIndex].copyFromFloats(sphereRadius, sphereRadius, sphereRadius);
    _cascadeMinExtents[cascadeIndex].copyFromFloats(-sphereRadius, -sphereRadius, -sphereRadius);
  }
  else {
    // Create a temporary view matrix for the light
    const auto& lightCameraPos = _frustumCenter[cascadeIndex];

    _frustumCenter[cascadeIndex].addToRef(_lightDirection, tmpv1); // tmpv1 = look at

    Matrix::LookAtLHToRef(lightCameraPos, tmpv1, UpDir, matrix); // matrix = lightView

    // Calculate an AABB around the frustum corners
    for (size_t cornerIndex = 0; cornerIndex < _frustumCornersWorldSpace[cascadeIndex].size();
         ++cornerIndex) {
      Vector3::TransformCoordinatesToRef(_frustumCornersWorldSpace[cascadeIndex][cornerIndex],
                                         matrix, tmpv1);

      _cascadeMinExtents[cascadeIndex].minimizeInPlace(tmpv1);
      _cascadeMaxExtents[cascadeIndex].maximizeInPlace(tmpv1);
    }
  }
}

void CascadedShadowGenerator::_initializeGenerator()
{
  penumbraDarkness                           = 1.f;
  _numCascades                               = CascadedShadowGenerator::DEFAULT_CASCADES_COUNT;
  stabilizeCascades                          = false;
  _freezeShadowCastersBoundingInfoObservable = nullptr;
  freezeShadowCastersBoundingInfo            = false;
  _scbiMin                                   = Vector3(0.f, 0.f, 0.f);
  _scbiMax                                   = Vector3(0.f, 0.f, 0.f);
  _shadowCastersBoundingInfo = BoundingInfo(Vector3(0.f, 0.f, 0.f), Vector3(0.f, 0.f, 0.f));
  _breaksAreDirty            = true;
  _minDistance               = 0.f;
  _maxDistance               = 1.f;
  _currentLayer              = 0;
  _shadowMaxZ                = _scene->activeCamera()->maxZ;
  _debug                     = false;
  _depthClamp                = true;
  _cascadeBlendPercentage    = 0.1f;
  _lambda                    = 0.5f;
  _autoCalcDepthBounds       = false;

  ShadowGenerator::_initializeGenerator();
}

void CascadedShadowGenerator::_createTargetRenderTexture()
{
  ShadowGenerator::_initializeShadowMap();

  if (_shadowMap == nullptr) {
    return;
  }

  _transformMatricesAsArray = Float32Array(_numCascades * 16);
  _viewSpaceFrustumsZ       = Float32Array(_numCascades);
  _frustumLengths           = Float32Array(_numCascades);
  _lightSizeUVCorrection    = Float32Array(_numCascades * 2);
  _depthCorrection          = Float32Array(_numCascades);

  _cascades.resize(_numCascades);
  _viewMatrices.resize(_numCascades);
  _projectionMatrices.resize(_numCascades);
  _transformMatrices.resize(_numCascades);
  _cascadeMinExtents.resize(_numCascades);
  _cascadeMaxExtents.resize(_numCascades);
  _frustumCenter.resize(_numCascades);
  _shadowCameraPos.resize(_numCascades);
  _frustumCornersWorldSpace.resize(_numCascades);

  for (size_t cascadeIndex = 0; cascadeIndex < _numCascades; ++cascadeIndex) {
    _cascades[cascadeIndex] = ICascade{
      0.f, // prevBreakDistance
      0.f, // breakDistance
    };

    _viewMatrices[cascadeIndex]       = Matrix::Zero();
    _projectionMatrices[cascadeIndex] = Matrix::Zero();
    _transformMatrices[cascadeIndex]  = Matrix::Zero();
    _cascadeMinExtents[cascadeIndex]  = Vector3();
    _cascadeMaxExtents[cascadeIndex]  = Vector3();
    _frustumCenter[cascadeIndex]      = Vector3();
    _shadowCameraPos[cascadeIndex]    = Vector3();
    _frustumCornersWorldSpace[cascadeIndex]
      = std::vector<Vector3>(CascadedShadowGenerator::frustumCornersNDCSpace.size());

    for (size_t i = 0; i < CascadedShadowGenerator::frustumCornersNDCSpace.size(); ++i) {
      _frustumCornersWorldSpace[cascadeIndex][i] = Vector3();
    }
  }

  _shadowMap->onBeforeRenderObservable.add(
    [this](int* layer, EventState & /*es*/) -> void { _currentLayer = *layer; });

  _shadowMap->onBeforeBindObservable.add(
    [this](RenderTargetTexture* /*texture*/, EventState & /*es*/) -> void {
      if (_breaksAreDirty) {
        _splitFrustum();
      }
      _computeMatrices();
    });

  _splitFrustum();
}

void CascadedShadowGenerator::_bindCustomEffectForRenderSubMeshForShadowMap(SubMesh* /*subMesh*/,
                                                                            Effect* effect)
{
  effect->setMatrix("viewProjection",
                    *getCascadeTransformMatrix(static_cast<unsigned>(_currentLayer)));
}

void CascadedShadowGenerator::_isReadyCustomDefines(std::vector<std::string>& defines,
                                                    SubMesh* /*subMesh*/, bool /*useInstances*/)
{
  if (_depthClamp && _filter != ShadowGenerator::FILTER_PCSS) {
    defines.emplace_back("#define DEPTHCLAMP");
  }
}

void CascadedShadowGenerator::prepareDefines(MaterialDefines& defines, unsigned int lightIndex)
{
  ShadowGenerator::prepareDefines(defines, lightIndex);

  const auto& scene        = _scene;
  const auto& light        = _light;
  const auto lightIndexStr = std::to_string(lightIndex);

  if (!scene->shadowsEnabled() || !light->shadowEnabled()) {
    return;
  }

  defines.boolDef["SHADOWCSM" + lightIndexStr]             = true;
  defines.boolDef["SHADOWCSMDEBUG" + lightIndexStr]        = debug;
  defines.boolDef["SHADOWCSMNUM_CASCADES" + lightIndexStr] = numCascades;
  defines.boolDef["SHADOWCSM_RIGHTHANDED" + lightIndexStr] = scene->useRightHandedSystem();

  const auto camera = scene->activeCamera();

  if (camera && _shadowMaxZ < camera->maxZ) {
    defines.boolDef["SHADOWCSMUSESHADOWMAXZ" + lightIndexStr] = true;
  }

  if (cascadeBlendPercentage == 0.f) {
    defines.boolDef["SHADOWCSMNOBLEND" + lightIndexStr] = true;
  }
}

void CascadedShadowGenerator::bindShadowLight(const std::string& lightIndex,
                                              const EffectPtr& effect)
{
  const auto& light = _light;
  const auto& scene = _scene;

  if (!scene->shadowsEnabled() || !light->shadowEnabled()) {
    return;
  }

  const auto camera = scene->activeCamera();
  if (!camera) {
    return;
  }

  const auto shadowMap = getShadowMap();
  if (!shadowMap) {
    return;
  }

  const auto width = shadowMap->getSize().width;

  effect->setMatrices("lightMatrix" + lightIndex, _transformMatricesAsArray);
  effect->setArray("viewFrustumZ" + lightIndex, _viewSpaceFrustumsZ);
  effect->setFloat("cascadeBlendFactor" + lightIndex,
                   cascadeBlendPercentage == 0.f ? 10000.f : 1.f / cascadeBlendPercentage);
  effect->setArray("frustumLengths" + lightIndex, _frustumLengths);

  // Only PCF uses depth stencil texture.
  if (_filter == ShadowGenerator::FILTER_PCF) {
    effect->setDepthStencilTexture("shadowSampler" + lightIndex, shadowMap);
    light->_uniformBuffer->updateFloat4("shadowsInfo", getDarkness(), static_cast<float>(width),
                                        1.f / width, frustumEdgeFalloff, lightIndex);
  }
  else if (_filter == ShadowGenerator::FILTER_PCSS) {
    for (size_t cascadeIndex = 0; cascadeIndex < _numCascades; ++cascadeIndex) {
      _lightSizeUVCorrection[cascadeIndex * 2 + 0]
        = cascadeIndex == 0 ? 1.f :
                              (_cascadeMaxExtents[0].x - _cascadeMinExtents[0].x)
                                / (_cascadeMaxExtents[cascadeIndex].x
                                   - _cascadeMinExtents[cascadeIndex].x); // x correction
      _lightSizeUVCorrection[cascadeIndex * 2 + 1]
        = cascadeIndex == 0 ? 1.f :
                              (_cascadeMaxExtents[0].y - _cascadeMinExtents[0].y)
                                / (_cascadeMaxExtents[cascadeIndex].y
                                   - _cascadeMinExtents[cascadeIndex].y); // y correction
      _depthCorrection[cascadeIndex]
        = cascadeIndex == 0 ?
            1.f :
            (_cascadeMaxExtents[cascadeIndex].z - _cascadeMinExtents[cascadeIndex].z)
              / (_cascadeMaxExtents[0].z - _cascadeMinExtents[0].z);
    }
    effect->setDepthStencilTexture("shadowSampler" + lightIndex, shadowMap);
    effect->setTexture("depthSampler" + lightIndex, shadowMap);
    effect->setArray2("lightSizeUVCorrection" + lightIndex, _lightSizeUVCorrection);
    effect->setArray("depthCorrection" + lightIndex, _depthCorrection);
    effect->setFloat("penumbraDarkness" + lightIndex, penumbraDarkness);
    light->_uniformBuffer->updateFloat4("shadowsInfo", getDarkness(), 1.f / width,
                                        _contactHardeningLightSizeUVRatio * width,
                                        frustumEdgeFalloff, lightIndex);
  }
  else {
    effect->setTexture("shadowSampler" + lightIndex, shadowMap);
    light->_uniformBuffer->updateFloat4("shadowsInfo", getDarkness(), static_cast<float>(width),
                                        1.f / width, frustumEdgeFalloff, lightIndex);
  }

  light->_uniformBuffer->updateFloat2(
    "depthValues", getLight()->getDepthMinZ(*camera),
    getLight()->getDepthMinZ(*camera) + getLight()->getDepthMaxZ(*camera), lightIndex);
}

Matrix CascadedShadowGenerator::getTransformMatrix()
{
  return getCascadeTransformMatrix(0).value_or(Matrix{});
}

void CascadedShadowGenerator::dispose()
{
  ShadowGenerator::dispose();

  if (_freezeShadowCastersBoundingInfoObservable) {
    _scene->onBeforeRenderObservable.remove(_freezeShadowCastersBoundingInfoObservable);
    _freezeShadowCastersBoundingInfoObservable = nullptr;
  }

  if (_depthReducer) {
    _depthReducer->dispose();
    _depthReducer = nullptr;
  }
}

json CascadedShadowGenerator::serialize() const
{
  return nullptr;
}

ShadowGenerator* CascadedShadowGenerator::Parse(const json& /*parsedShadowGenerator*/,
                                                Scene* /*scene*/)
{
  return nullptr;
}

} // end of namespace BABYLON
