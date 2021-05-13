#include <babylon/postprocesses/motion_blur_post_process.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/materials/textures/pre_pass_render_target.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/misc/string_tools.h>
#include <babylon/rendering/geometry_buffer_renderer.h>
#include <babylon/rendering/motion_blur_configuration.h>
#include <babylon/rendering/pre_pass_renderer.h>

namespace BABYLON {

MotionBlurPostProcess::MotionBlurPostProcess(
  const std::string& iName, Scene* scene, const std::variant<float, PostProcessOptions>& options,
  const CameraPtr& camera, const std::optional<unsigned int>& samplingMode, Engine* engine,
  bool reusable, unsigned int textureType, bool blockCompilation, bool forceGeometryBuffer)
    : PostProcess{iName,
                  "motionBlur",
                  {"motionStrength", "motionScale", "screenSize", "inverseViewProjection",
                   "prevViewProjection"},
                  {"velocitySampler"},
                  options,
                  camera,
                  samplingMode,
                  engine,
                  reusable,
                  "#define GEOMETRY_SUPPORTED\n#define SAMPLES 64.0\n#define OBJECT_BASED",
                  textureType,
                  "",
                  {},
                  blockCompilation}
    , motionStrength{1.f}
    , motionBlurSamples{this, &MotionBlurPostProcess::get_motionBlurSamples,
                        &MotionBlurPostProcess::set_motionBlurSamples}
    , isObjectBased{this, &MotionBlurPostProcess::get_isObjectBased,
                    &MotionBlurPostProcess::set_isObjectBased}
    , _motionBlurSamples{32}
    , _isObjectBased{true}
    , _forceGeometryBuffer{false}
    , _geometryBufferRenderer{this, &MotionBlurPostProcess::get__geometryBufferRenderer}
    , _prePassRenderer{this, &MotionBlurPostProcess::get__prePassRenderer}
    , _invViewProjection{std::nullopt}
    , _previousViewProjection{std::nullopt}
    , _nullGeometryBufferRenderer{nullptr}
    , _nullPrePassRenderer{nullptr}
{
  _forceGeometryBuffer = forceGeometryBuffer;

  // Set up assets
  if (_forceGeometryBuffer) {
    scene->enableGeometryBufferRenderer();

    if (_geometryBufferRenderer()) {
      _geometryBufferRenderer()->enableVelocity = true;
    }
  }
  else {
    scene->enablePrePassRenderer();

    if (_prePassRenderer()) {
      _prePassRenderer()->markAsDirty();
      _prePassEffectConfiguration = std::make_shared<MotionBlurConfiguration>();
    }
  }

  _applyMode();
}

MotionBlurPostProcess::~MotionBlurPostProcess() = default;

std::string MotionBlurPostProcess::getClassName() const
{
  return "MotionBlurPostProcess";
}

GeometryBufferRendererPtr& MotionBlurPostProcess::get__geometryBufferRenderer()
{
  if (!_forceGeometryBuffer) {
    return _nullGeometryBufferRenderer;
  }

  return _scene->geometryBufferRenderer();
}

PrePassRendererPtr& MotionBlurPostProcess::get__prePassRenderer()
{
  if (_forceGeometryBuffer) {
    return _nullPrePassRenderer;
  }

  return _scene->prePassRenderer();
}

unsigned int MotionBlurPostProcess::get_motionBlurSamples() const
{
  return _motionBlurSamples;
}

void MotionBlurPostProcess::set_motionBlurSamples(unsigned int iSamples)
{
  _motionBlurSamples = iSamples;
  _updateEffect();
}

bool MotionBlurPostProcess::get_isObjectBased() const
{
  return _isObjectBased;
}

void MotionBlurPostProcess::set_isObjectBased(bool value)
{
  if (_isObjectBased == value) {
    return;
  }

  _isObjectBased = value;
  _applyMode();
}

void MotionBlurPostProcess::excludeSkinnedMesh(const AbstractMeshPtr& skinnedMesh)
{
  if (skinnedMesh->skeleton()) {
    std::vector<AbstractMeshPtr>* list = nullptr;
    const auto geometryBufferRenderer  = _geometryBufferRenderer();
    const auto prePassRenderer         = _prePassRenderer();
    if (geometryBufferRenderer) {
      list = &geometryBufferRenderer->excludedSkinnedMeshesFromVelocity;
    }
    else if (prePassRenderer) {
      list = &prePassRenderer->excludedSkinnedMesh;
    }
    else {
      return;
    }
    list->emplace_back(skinnedMesh);
  }
}

void MotionBlurPostProcess::removeExcludedSkinnedMesh(const AbstractMeshPtr& skinnedMesh)
{
  if (skinnedMesh->skeleton()) {
    std::vector<AbstractMeshPtr>* list = nullptr;
    const auto geometryBufferRenderer  = _geometryBufferRenderer();
    const auto prePassRenderer         = _prePassRenderer();
    if (geometryBufferRenderer) {
      list = &geometryBufferRenderer->excludedSkinnedMeshesFromVelocity;
    }
    else if (prePassRenderer) {
      list = &prePassRenderer->excludedSkinnedMesh;
    }
    else {
      return;
    }

    const auto index = stl_util::index_of(*list, skinnedMesh);
    if (index != -1) {
      stl_util::splice(*list, index, 1);
    }
  }
}

void MotionBlurPostProcess::dispose(Camera* camera)
{
  const auto geometryBufferRenderer = _geometryBufferRenderer();
  if (geometryBufferRenderer) {
    // Clear previous transformation matrices dictionary used to compute objects velocities
    geometryBufferRenderer->_previousTransformationMatrices.clear();
    geometryBufferRenderer->_previousBonesTransformationMatrices.clear();
    geometryBufferRenderer->excludedSkinnedMeshesFromVelocity.clear();
  }

  PostProcess::dispose(camera);
}

void MotionBlurPostProcess::_applyMode()
{
  const auto geometryBufferRenderer = _geometryBufferRenderer();
  const auto prePassRenderer        = _prePassRenderer();

  if (!geometryBufferRenderer && !prePassRenderer) {
    // We can't get a velocity or depth texture. So, work as a passthrough.
    BABYLON_LOG_WARN("MotionBlurPostProcess",
                     "Multiple Render Target support needed to compute object based motion blur");
    return updateEffect();
  }

  _updateEffect();

  _invViewProjection      = std::nullopt;
  _previousViewProjection = std::nullopt;

  if (isObjectBased) {
    if (prePassRenderer && _prePassEffectConfiguration) {
      if (_prePassEffectConfiguration->texturesRequired().empty()) {
        _prePassEffectConfiguration->texturesRequired().resize(1);
      }
      _prePassEffectConfiguration->texturesRequired()[0] = Constants::PREPASS_VELOCITY_TEXTURE_TYPE;
    }

    onApply = [this](Effect* effect, EventState& /*es*/) -> void { _onApplyObjectBased(effect); };
  }
  else {
    _invViewProjection      = Matrix::Identity();
    _previousViewProjection = Matrix::Identity();

    if (prePassRenderer && _prePassEffectConfiguration) {
      if (_prePassEffectConfiguration->texturesRequired().empty()) {
        _prePassEffectConfiguration->texturesRequired().resize(1);
      }
      _prePassEffectConfiguration->texturesRequired()[0] = Constants::PREPASS_DEPTH_TEXTURE_TYPE;
    }

    onApply = [this](Effect* effect, EventState& /*es*/) -> void { _onApplyScreenBased(effect); };
  }
}

void MotionBlurPostProcess::_onApplyObjectBased(Effect* effect)
{
  effect->setVector2("screenSize", Vector2(width, height));

  effect->setFloat("motionScale", _scene->getAnimationRatio());
  effect->setFloat("motionStrength", motionStrength);

  const auto geometryBufferRenderer = _geometryBufferRenderer();
  const auto prePassRenderer        = _prePassRenderer();

  if (geometryBufferRenderer) {
    const auto velocityIndex
      = geometryBufferRenderer->getTextureIndex(GeometryBufferRenderer::VELOCITY_TEXTURE_TYPE);
    effect->setTexture("velocitySampler",
                       velocityIndex >= 0
                           && velocityIndex < static_cast<int>(
                                geometryBufferRenderer->getGBuffer()->textures().size()) ?
                         geometryBufferRenderer->getGBuffer()->textures()[velocityIndex] :
                         nullptr);
  }
  else if (prePassRenderer) {
    const auto velocityIndex = prePassRenderer->getIndex(Constants::PREPASS_VELOCITY_TEXTURE_TYPE);
    effect->setTexture("velocitySampler",
                       velocityIndex >= 0
                           && velocityIndex < static_cast<int>(
                                prePassRenderer->getRenderTarget()->textures().size()) ?
                         prePassRenderer->getRenderTarget()->textures()[velocityIndex] :
                         nullptr);
  }
}

void MotionBlurPostProcess::_onApplyScreenBased(Effect* effect)
{
  const auto viewProjection = _scene->getProjectionMatrix().multiply(_scene->getViewMatrix());

  if (!_invViewProjection) {
    _invViewProjection = Matrix::Identity();
  }
  if (!_previousViewProjection) {
    _previousViewProjection = Matrix::Identity();
  }

  viewProjection.invertToRef(*_invViewProjection);
  effect->setMatrix("inverseViewProjection", *_invViewProjection);

  effect->setMatrix("prevViewProjection", *_previousViewProjection);
  _previousViewProjection = viewProjection;

  effect->setVector2("screenSize", Vector2(width, height));

  effect->setFloat("motionScale", _scene->getAnimationRatio());
  effect->setFloat("motionStrength", motionStrength);

  const auto geometryBufferRenderer = _geometryBufferRenderer();
  const auto prePassRenderer        = _prePassRenderer();

  if (geometryBufferRenderer) {
    const auto depthIndex
      = geometryBufferRenderer->getTextureIndex(GeometryBufferRenderer::DEPTH_TEXTURE_TYPE);
    effect->setTexture("depthSampler",
                       depthIndex >= 0
                           && depthIndex < static_cast<int>(
                                geometryBufferRenderer->getGBuffer()->textures().size()) ?
                         geometryBufferRenderer->getGBuffer()->textures()[depthIndex] :
                         nullptr);
  }
  else if (prePassRenderer) {
    const auto depthIndex = prePassRenderer->getIndex(Constants::PREPASS_DEPTH_TEXTURE_TYPE);
    effect->setTexture(
      "depthSampler",
      depthIndex >= 0
          && depthIndex < static_cast<int>(prePassRenderer->getRenderTarget()->textures().size()) ?
        prePassRenderer->getRenderTarget()->textures()[depthIndex] :
        nullptr);
  }
}

void MotionBlurPostProcess::_updateEffect()
{
  if (_geometryBufferRenderer() || _prePassRenderer()) {
    const std::vector<std::string> defines = {
      "#define GEOMETRY_SUPPORTED",                                                    //
      StringTools::printf("#define SAMPLES %d", static_cast<int>(_motionBlurSamples)), //
      _isObjectBased ? "#define OBJECT_BASED" : "#define SCREEN_BASED"                 //
    };

    updateEffect(StringTools::join(defines, "\n"));
  }
}

MotionBlurPostProcessPtr MotionBlurPostProcess::_Parse(const json& /*parsedPostProcess*/,
                                                       const CameraPtr& /*targetCamera*/,
                                                       Scene* /*scene*/,
                                                       const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
