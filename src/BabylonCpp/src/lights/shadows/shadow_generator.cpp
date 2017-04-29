#include <babylon/lights/shadows/shadow_generator.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/string.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/vector2.h>
#include <babylon/mesh/_instances_batch.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/post_process_manager.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

ShadowGenerator::ShadowGenerator(int mapSize, IShadowLight* light)
    : ShadowGenerator(ISize{mapSize, mapSize}, light)
{
}

ShadowGenerator::ShadowGenerator(const ISize& mapSize, IShadowLight* light)
    : blurScale{2.f}
    , forceBackFacesOnly{false}
    , _filter{ShadowGenerator::FILTER_NONE}
    , _blurBoxOffset{0}
    , _bias{0.00005f}
    , _lightDirection{Vector3::Zero()}
    , _light{light}
    , _scene{light->getScene()}
    , _shadowMap{nullptr}
    , _shadowMap2{nullptr}
    , _darkness{0}
    , _transparencyShadow{false}
    , _effect{nullptr}
    , _viewMatrix{Matrix::Zero()}
    , _projectionMatrix{Matrix::Zero()}
    , _transformMatrix{Matrix::Zero()}
    , _worldViewProjection{Matrix::Zero()}
    , _cacheInitialized{false}
    , _downSamplePostprocess{nullptr}
    , _boxBlurPostprocess{nullptr}
    , _mapSize{mapSize}
    , _currentFaceIndex{0}
    , _currentFaceIndexCache{0}
    , _useFullFloat{true}
    , _textureType{0}
{
  light->_shadowGenerator = this;
  light->_markMeshesAsLightDirty();

  // Texture type fallback from float to int if not supported.
  const auto& caps = _scene->getEngine()->getCaps();
  if (caps.textureFloat && caps.textureFloatLinearFiltering
      && caps.textureFloatRender) {
    _useFullFloat = true;
    _textureType  = Engine::TEXTURETYPE_FLOAT;
  }
  else {
    _useFullFloat = false;
    _textureType  = Engine::TEXTURETYPE_UNSIGNED_INT;
  }

  // Render target
  _shadowMap = std_util::make_unique<RenderTargetTexture>(
    light->name + "_shadowMap", mapSize, _scene, false, true, _textureType,
    light->needCube());
  _shadowMap->wrapU                     = Texture::CLAMP_ADDRESSMODE;
  _shadowMap->wrapV                     = Texture::CLAMP_ADDRESSMODE;
  _shadowMap->anisotropicFilteringLevel = 1;
  _shadowMap->updateSamplingMode(Texture::BILINEAR_SAMPLINGMODE);
  _shadowMap->renderParticles = false;

  _shadowMap->onBeforeRenderObservable.add(
    [this](unsigned int faceIndex) { _currentFaceIndex = faceIndex; });

  _shadowMap->onAfterUnbindObservable.add([this]() {
    if (!useBlurExponentialShadowMap()) {
      return;
    }

    if (!_shadowMap2) {
      _shadowMap2 = std_util::make_unique<RenderTargetTexture>(
        _light->name + "_shadowMap", _mapSize, _scene, false);
      _shadowMap2->wrapU = Texture::CLAMP_ADDRESSMODE;
      _shadowMap2->wrapV = Texture::CLAMP_ADDRESSMODE;
      _shadowMap2->updateSamplingMode(Texture::BILINEAR_SAMPLINGMODE);

      _downSamplePostprocess = std_util::make_unique<PassPostProcess>(
        "downScale", 1.f / blurScale, nullptr, Texture::BILINEAR_SAMPLINGMODE,
        _scene->getEngine(), false, _textureType);
      _downSamplePostprocess->onApplyObservable.add([this](Effect* effect) {
        effect->setTexture("textureSampler", _shadowMap.get());
      });

      setBlurBoxOffset(1.f);
    }

    _scene->postProcessManager->directRender(
      {_downSamplePostprocess.get(), _boxBlurPostprocess.get()},
      _shadowMap2->getInternalTexture());
  });

  _shadowMap->customRenderFunction
    = [this](const std::vector<SubMesh*>& opaqueSubMeshes,
             const std::vector<SubMesh*>& transparentSubMeshes,
             const std::vector<SubMesh*>& alphaTestSubMeshes) {

        for (const auto& opaqueSubMesh : opaqueSubMeshes) {
          renderSubMesh(opaqueSubMesh);
        }

        for (const auto& alphaTestSubMesh : alphaTestSubMeshes) {
          renderSubMesh(alphaTestSubMesh);
        }

        if (_transparencyShadow) {
          for (const auto& transparentSubMesh : transparentSubMeshes) {
            renderSubMesh(transparentSubMesh);
          }
        }
      };

  _shadowMap->onClearObservable.add([this](Engine* engine) {
    if (useExponentialShadowMap() || useBlurExponentialShadowMap()) {
      engine->clear(Color4(0.f, 0.f, 0.f, 0.f), true, true, true);
    }
    else {
      engine->clear(Color4(1.f, 1.f, 1.f, 1.f), true, true, true);
    }
  });
}

ShadowGenerator::~ShadowGenerator()
{
}

float ShadowGenerator::bias() const
{
  return _bias;
}

void ShadowGenerator::setBias(float iBias)
{
  _bias = iBias;
}

int ShadowGenerator::blurBoxOffset() const
{
  return _blurBoxOffset;
}

void ShadowGenerator::setBlurBoxOffset(int value)
{
  if (_blurBoxOffset == value) {
    return;
  }

  _blurBoxOffset = value;

  if (_boxBlurPostprocess) {
    _boxBlurPostprocess->dispose();
  }

  unsigned textureType = _useFullFloat ? Engine::TEXTURETYPE_FLOAT :
                                         Engine::TEXTURETYPE_UNSIGNED_INT;

  std::unique_ptr<PostProcess> _boxBlurPostprocessPtr(
    new PostProcess("DepthBoxBlur", "depthBoxBlur", {"screenSize", "boxOffset"},
                    {}, 1.f / blurScale, nullptr,
                    Texture::BILINEAR_SAMPLINGMODE, _scene->getEngine(), false,
                    "#define OFFSET " + std::to_string(value), textureType));
  _boxBlurPostprocess = std::move(_boxBlurPostprocessPtr);
  _boxBlurPostprocess->onApplyObservable.add([&](Effect* effect) {
    effect->setFloat2("screenSize",
                      static_cast<float>(_mapSize.width) / blurScale,
                      static_cast<float>(_mapSize.height) / blurScale);
  });
}

float ShadowGenerator::depthScale() const
{
  return _depthScale.hasValue() ? _depthScale.value : _light->getDepthScale();
}

void ShadowGenerator::setDepthScale(float value)
{
  _depthScale = value;
}

unsigned int ShadowGenerator::filter() const
{
  return _filter;
}

void ShadowGenerator::setFilter(unsigned int value)
{
  if (_filter == value) {
    return;
  }

  _filter = value;

  if (usePoissonSampling() || useExponentialShadowMap()
      || useBlurExponentialShadowMap()) {
    _shadowMap->anisotropicFilteringLevel = 16;
    _shadowMap->updateSamplingMode(Texture::BILINEAR_SAMPLINGMODE);
  }
  else {
    _shadowMap->anisotropicFilteringLevel = 1;
    _shadowMap->updateSamplingMode(Texture::NEAREST_SAMPLINGMODE);
  }

  _light->_markMeshesAsLightDirty();
}

bool ShadowGenerator::useExponentialShadowMap() const
{
  return filter() == ShadowGenerator::FILTER_EXPONENTIALSHADOWMAP;
}

void ShadowGenerator::setUseExponentialShadowMap(bool value)
{
  setFilter(value ? ShadowGenerator::FILTER_EXPONENTIALSHADOWMAP :
                    ShadowGenerator::FILTER_NONE);
}

bool ShadowGenerator::usePoissonSampling() const
{
  return filter() == ShadowGenerator::FILTER_POISSONSAMPLING;
}

void ShadowGenerator::setUsePoissonSampling(bool value)
{
  setFilter(value ? ShadowGenerator::FILTER_POISSONSAMPLING :
                    ShadowGenerator::FILTER_NONE);
}

bool ShadowGenerator::useBlurExponentialShadowMap() const
{
  return filter() == ShadowGenerator::FILTER_BLUREXPONENTIALSHADOWMAP;
}

void ShadowGenerator::setUseBlurExponentialShadowMap(bool value)
{
  if (_light->needCube() && value) {
    // Blurring the cubemap is going to be too expensive. Reverting to unblurred
    // version
    setUseExponentialShadowMap(true);
  }
  else {
    setFilter(value ? ShadowGenerator::FILTER_BLUREXPONENTIALSHADOWMAP :
                      ShadowGenerator::FILTER_NONE);
  }
}

void ShadowGenerator::renderSubMesh(SubMesh* subMesh)
{
  auto mesh   = subMesh->getRenderingMesh();
  auto scene  = _scene;
  auto engine = scene->getEngine();

  // Culling
  engine->setState(subMesh->getMaterial()->backFaceCulling);

  // Managing instances
  auto batch = mesh->_getInstancesRenderList(subMesh->_id);

  if (batch->mustReturn) {
    return;
  }

  bool hardwareInstancedRendering
    = (engine->getCaps().instancedArrays != false)
      && (batch->visibleInstances.find(subMesh->_id)
          != batch->visibleInstances.end())
      && (!batch->visibleInstances[subMesh->_id].empty());

  if (isReady(subMesh, hardwareInstancedRendering)) {
    engine->enableEffect(_effect);
    mesh->_bind(subMesh, _effect, Material::TriangleFillMode);
    auto material = subMesh->getMaterial();

    _effect->setFloat2("biasAndScale", bias(), depthScale());

    _effect->setMatrix("viewProjection", getTransformMatrix());
    _effect->setVector3("lightPosition", getLight()->position);

    if (getLight()->needCube()) {
      _effect->setFloat2("depthValues", scene->activeCamera->minZ,
                         scene->activeCamera->maxZ);
    }

    // Alpha test
    if (material && material->needAlphaTesting()) {
      auto alphaTexture = material->getAlphaTestTexture();
      _effect->setTexture("diffuseSampler", alphaTexture);
      _effect->setMatrix("diffuseMatrix", *alphaTexture->getTextureMatrix());
    }

    // Bones
    if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
      _effect->setMatrices("mBones",
                           mesh->skeleton()->getTransformMatrices(mesh));
    }

    if (forceBackFacesOnly) {
      engine->setState(true, 0, false, true);
    }

    // Draw
    mesh->_processRendering(subMesh, _effect, Material::TriangleFillMode, batch,
                            hardwareInstancedRendering,
                            [this](bool /*isInstance*/, Matrix world,
                                   Material* /*effectiveMaterial*/) {
                              _effect->setMatrix("world", world);
                            });

    if (forceBackFacesOnly) {
      engine->setState(true, 0, false, false);
    }
  }
  else {
    // Need to reset refresh rate of the shadowMap
    _shadowMap->resetRefreshCounter();
  }
}

bool ShadowGenerator::isReady(SubMesh* subMesh, bool useInstances)
{
  std::vector<std::string> defines;

  if (_useFullFloat) {
    defines.emplace_back("#define FULLFLOAT");
  }

  if (useExponentialShadowMap() || useBlurExponentialShadowMap()) {
    defines.emplace_back("#define ESM");
  }

  if (getLight()->needCube()) {
    defines.emplace_back("#define CUBEMAP");
  }

  std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

  auto mesh     = subMesh->getMesh();
  auto material = subMesh->getMaterial();

  // Alpha test
  if (material && material->needAlphaTesting()) {
    defines.emplace_back("#define ALPHATEST");
    if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
      defines.emplace_back("#define UV1");
    }
    if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
      auto alphaTexture = material->getAlphaTestTexture();
      if (alphaTexture->coordinatesIndex == 1) {
        attribs.emplace_back(VertexBuffer::UV2KindChars);
        defines.emplace_back("#define UV2");
      }
    }
  }

  // Bones
  if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
    attribs.emplace_back(VertexBuffer::MatricesIndicesKindChars);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKindChars);
    if (mesh->numBoneInfluencers() > 4) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKindChars);
      attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKindChars);
    }
    defines.emplace_back("#define NUM_BONE_INFLUENCERS "
                         + std::to_string(mesh->numBoneInfluencers()));
    defines.emplace_back(
      String::concat("#define BonesPerMesh "
                     + std::to_string(mesh->skeleton()->bones.size() + 1)));
  }
  else {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
  }

  // Instances
  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
    attribs.emplace_back(VertexBuffer::World0KindChars);
    attribs.emplace_back(VertexBuffer::World1KindChars);
    attribs.emplace_back(VertexBuffer::World2KindChars);
    attribs.emplace_back(VertexBuffer::World3KindChars);
  }

  // Get correct effect
  std::string joined = String::join(defines, '\n');
  if (_cachedDefines != joined) {
    _cachedDefines = joined;
    _effect        = _scene->getEngine()->createEffect(
      "shadowMap", attribs,
      {"world", "mBones", "viewProjection", "diffuseMatrix", "lightPosition",
       "depthValues", "biasAndScale"},
      {"diffuseSampler"}, joined);
  }

  return _effect->isReady();
}

RenderTargetTexture* ShadowGenerator::getShadowMap()
{
  return _shadowMap.get();
}

RenderTargetTexture* ShadowGenerator::getShadowMapForRendering()
{
  if (_shadowMap2) {
    return _shadowMap2.get();
  }

  return _shadowMap.get();
}

IShadowLight* ShadowGenerator::getLight()
{
  return _light;
}

Matrix ShadowGenerator::getTransformMatrix()
{
  auto scene = _scene;
  if (_currentRenderID == scene->getRenderId()
      && _currentFaceIndexCache == _currentFaceIndex) {
    return _transformMatrix;
  }

  _currentRenderID       = scene->getRenderId();
  _currentFaceIndexCache = _currentFaceIndex;

  auto& lightPosition = _light->position;
  Vector3::NormalizeToRef(_light->getShadowDirection(_currentFaceIndex),
                          _lightDirection);

  if (std_util::almost_equal(
        std::abs(Vector3::Dot(_lightDirection, Vector3::Up())), 1.f)) {
    // Required to avoid perfectly perpendicular light
    _lightDirection.z = 0.0000000000001f;
  }

  if (_light->computeTransformedPosition()) {
    lightPosition = _light->transformedPosition;
  }

  if (_light->needRefreshPerFrame() || !_cacheInitialized
      || !lightPosition.equals(_cachedPosition)
      || !_lightDirection.equals(_cachedDirection)) {

    _cachedPosition   = lightPosition;
    _cachedDirection  = _lightDirection;
    _cacheInitialized = true;

    Matrix::LookAtLHToRef(lightPosition, lightPosition.add(_lightDirection),
                          Vector3::Up(), _viewMatrix);

    _light->setShadowProjectionMatrix(_projectionMatrix, _viewMatrix,
                                      getShadowMap()->renderList);

    _viewMatrix.multiplyToRef(_projectionMatrix, _transformMatrix);
  }

  return _transformMatrix;
}

float ShadowGenerator::getDarkness() const
{
  return _darkness;
}

ShadowGenerator& ShadowGenerator::setDarkness(float darkness)
{
  if (darkness >= 1.f) {
    _darkness = 1.f;
  }
  else if (darkness <= 0.f) {
    _darkness = 0.f;
  }
  else {
    _darkness = darkness;
  }
  return *this;
}

ShadowGenerator& ShadowGenerator::setTransparencyShadow(bool hasShadow)
{
  _transparencyShadow = hasShadow;
  return *this;
}

Vector2 ShadowGenerator::_packHalf(float depth)
{
  const float scale = depth * 255.f;
  const float fract = scale - std::floor(scale);

  return Vector2(depth - fract / 255.f, fract);
}

void ShadowGenerator::dispose()
{
  _shadowMap->dispose();
  _shadowMap.reset(nullptr);

  if (_shadowMap2) {
    _shadowMap2->dispose();
    _shadowMap2.reset(nullptr);
  }

  if (_downSamplePostprocess) {
    _downSamplePostprocess->dispose();
    _downSamplePostprocess.reset(nullptr);
  }

  if (_boxBlurPostprocess) {
    _boxBlurPostprocess->dispose();
    _boxBlurPostprocess.reset(nullptr);
  }

  _light->_shadowGenerator = nullptr;
  _light->_markMeshesAsLightDirty();
}

Json::object ShadowGenerator::serialize() const
{
  return Json::object();
}

ShadowGenerator*
ShadowGenerator::Parse(const Json::value& parsedShadowGenerator, Scene* scene)
{
  // Casting to point light, as light is missing the position attr and
  // typescript complains.
  const std::string parsedShadowGeneratorLightId
    = Json::GetString(parsedShadowGenerator, "lightId");

  if (parsedShadowGeneratorLightId.empty()) {
    return nullptr;
  }

  auto light = dynamic_cast<PointLight*>(
    scene->getLightByID(parsedShadowGeneratorLightId));

  if (!light) {
    return nullptr;
  }

  auto size            = Json::GetNumber(parsedShadowGenerator, "mapSize", 0);
  auto shadowGenerator = new ShadowGenerator(ISize(size, size), light);

  for (const auto& renderItem :
       Json::GetArray(parsedShadowGenerator, "renderList")) {
    auto meshes = scene->getMeshesByID(renderItem.get<std::string>());
    for (auto& mesh : meshes) {
      shadowGenerator->getShadowMap()->renderList.emplace_back(mesh);
    }
  }

  if (Json::GetBool(parsedShadowGenerator, "usePoissonSampling")) {
    shadowGenerator->setUsePoissonSampling(true);
  }
  else if (Json::GetBool(parsedShadowGenerator, "useExponentialShadowMap")) {
    shadowGenerator->setUseExponentialShadowMap(true);
  }
  else if (Json::GetBool(parsedShadowGenerator,
                         "useBlurExponentialShadowMap")) {
    shadowGenerator->setUseBlurExponentialShadowMap(true);
  }
  // Backward compat
  else if (Json::GetBool(parsedShadowGenerator, "useVarianceShadowMap")) {
    shadowGenerator->setUseExponentialShadowMap(true);
  }
  else if (Json::GetBool(parsedShadowGenerator, "useBlurVarianceShadowMap")) {
    shadowGenerator->setUseExponentialShadowMap(true);
  }

  if (parsedShadowGenerator.contains("depthScale")) {
    shadowGenerator->setDepthScale(
      Json::GetNumber(parsedShadowGenerator, "depthScale", 30.f));
  }

  if (parsedShadowGenerator.contains("blurScale")) {
    shadowGenerator->blurScale
      = Json::GetNumber(parsedShadowGenerator, "blurScale", 2.f);
  }

  if (parsedShadowGenerator.contains("blurBoxOffset")) {
    shadowGenerator->setBlurBoxOffset(
      Json::GetNumber(parsedShadowGenerator, "blurBoxOffset", 0));
  }

  if (parsedShadowGenerator.contains("bias")) {
    shadowGenerator->setBias(
      Json::GetNumber(parsedShadowGenerator, "bias", 0.00005f));
  }

  if (parsedShadowGenerator.contains("darkness")) {
    shadowGenerator->setDarkness(
      Json::GetNumber(parsedShadowGenerator, "darkness", 0.f));
  }

  shadowGenerator->forceBackFacesOnly
    = Json::GetBool(parsedShadowGenerator, "forceBackFacesOnly");

  return shadowGenerator;
}

} // end of namespace BABYLON
