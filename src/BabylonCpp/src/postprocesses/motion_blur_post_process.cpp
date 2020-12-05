#include <babylon/postprocesses/motion_blur_post_process.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/multi_render_target.h>
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
    , _geometryBufferRenderer{nullptr}
    , _prePassRenderer{nullptr}
    , _invViewProjection{std::nullopt}
    , _previousViewProjection{std::nullopt}
{
  _forceGeometryBuffer = forceGeometryBuffer;

  // Set up assets
  if (_forceGeometryBuffer) {
    _geometryBufferRenderer = scene->enableGeometryBufferRenderer();

    if (_geometryBufferRenderer) {
      _geometryBufferRenderer->enableVelocity = true;
    }
  }
  else {
    _prePassRenderer = scene->enablePrePassRenderer();

    if (_prePassRenderer) {
      _prePassRenderer->markAsDirty();
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
    if (_geometryBufferRenderer) {
      list = &_geometryBufferRenderer->excludedSkinnedMeshesFromVelocity;
    }
    else if (_prePassRenderer) {
      list = &_prePassRenderer->excludedSkinnedMesh;
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
    if (_geometryBufferRenderer) {
      list = &_geometryBufferRenderer->excludedSkinnedMeshesFromVelocity;
    }
    else if (_prePassRenderer) {
      list = &_prePassRenderer->excludedSkinnedMesh;
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
  if (_geometryBufferRenderer) {
    // Clear previous transformation matrices dictionary used to compute objects velocities
    _geometryBufferRenderer->_previousTransformationMatrices.clear();
    _geometryBufferRenderer->_previousBonesTransformationMatrices.clear();
    _geometryBufferRenderer->excludedSkinnedMeshesFromVelocity.clear();
  }

  PostProcess::dispose(camera);
}

void MotionBlurPostProcess::_applyMode()
{
  if (!_geometryBufferRenderer && !_prePassRenderer) {
    // We can't get a velocity or depth texture. So, work as a passthrough.
    BABYLON_LOG_WARN("MotionBlurPostProcess",
                     "Multiple Render Target support needed to compute object based motion blur");
    return updateEffect();
  }

  _updateEffect();

  _invViewProjection      = std::nullopt;
  _previousViewProjection = std::nullopt;

  if (isObjectBased) {
    if (_prePassRenderer && _prePassEffectConfiguration) {
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

    if (_prePassRenderer && _prePassEffectConfiguration) {
      if (_prePassEffectConfiguration->texturesRequired().empty()) {
        _prePassEffectConfiguration->texturesRequired().resize(1);
      }
      _prePassEffectConfiguration->texturesRequired()[0]
        = Constants::PREPASS_DEPTHNORMAL_TEXTURE_TYPE;
    }

    onApply = [this](Effect* effect, EventState& /*es*/) -> void { _onApplyScreenBased(effect); };
  }
}

void MotionBlurPostProcess::_onApplyObjectBased(Effect* effect)
{
  effect->setVector2("screenSize", Vector2(width, height));

  effect->setFloat("motionScale", _scene->getAnimationRatio());
  effect->setFloat("motionStrength", motionStrength);

  if (_geometryBufferRenderer) {
    const auto velocityIndex
      = _geometryBufferRenderer->getTextureIndex(GeometryBufferRenderer::VELOCITY_TEXTURE_TYPE);
    effect->setTexture("velocitySampler",
                       velocityIndex >= 0
                           && velocityIndex < static_cast<int>(
                                _geometryBufferRenderer->getGBuffer()->textures().size()) ?
                         _geometryBufferRenderer->getGBuffer()->textures()[velocityIndex] :
                         nullptr);
  }
  else if (_prePassRenderer) {
    const auto velocityIndex = _prePassRenderer->getIndex(Constants::PREPASS_VELOCITY_TEXTURE_TYPE);
    effect->setTexture("velocitySampler", velocityIndex >= 0
                                              && velocityIndex < static_cast<int>(
                                                   _prePassRenderer->prePassRT->textures().size()) ?
                                            _prePassRenderer->prePassRT->textures()[velocityIndex] :
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

  if (_geometryBufferRenderer) {
    const auto depthIndex
      = _geometryBufferRenderer->getTextureIndex(GeometryBufferRenderer::DEPTHNORMAL_TEXTURE_TYPE);
    effect->setTexture("depthSampler",
                       depthIndex >= 0
                           && depthIndex < static_cast<int>(
                                _geometryBufferRenderer->getGBuffer()->textures().size()) ?
                         _geometryBufferRenderer->getGBuffer()->textures()[depthIndex] :
                         nullptr);
  }
  else if (_prePassRenderer) {
    const auto depthIndex = _prePassRenderer->getIndex(Constants::PREPASS_DEPTHNORMAL_TEXTURE_TYPE);
    effect->setTexture("depthSampler", depthIndex >= 0
                                           && depthIndex < static_cast<int>(
                                                _prePassRenderer->prePassRT->textures().size()) ?
                                         _prePassRenderer->prePassRT->textures()[depthIndex] :
                                         nullptr);
  }
}

void MotionBlurPostProcess::_updateEffect()
{
  if (_geometryBufferRenderer || _prePassRenderer) {
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
