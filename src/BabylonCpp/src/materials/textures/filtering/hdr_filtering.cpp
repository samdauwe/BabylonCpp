#include <babylon/materials/textures/filtering/hdr_filtering.h>

#include <babylon/core/logging.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_renderer.h>
#include <babylon/materials/effect_wrapper.h>
#include <babylon/materials/effect_wrapper_creation_options.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/maths/isize.h>
#include <babylon/maths/scalar.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

HDRFiltering::HDRFiltering(ThinEngine* engine, const IHDRFilteringOptions& options)
    : _effectRenderer{nullptr}, _effectWrapper{nullptr}
{
  // pass
  _engine  = engine;
  hdrScale = options.hdrScale.value_or(Constants::TEXTURE_FILTERING_QUALITY_OFFLINE);
  quality  = options.quality.value_or(1u);
}

InternalTexturePtr HDRFiltering::_createRenderTarget(int size)
{
  auto textureType = Constants::TEXTURETYPE_UNSIGNED_BYTE;
  if (_engine->getCaps().textureHalfFloatRender) {
    textureType = Constants::TEXTURETYPE_HALF_FLOAT;
  }
  else if (_engine->getCaps().textureFloatRender) {
    textureType = Constants::TEXTURETYPE_FLOAT;
  }

  IRenderTargetOptions options;
  options.format                = Constants::TEXTUREFORMAT_RGBA;
  options.type                  = textureType;
  options.generateMipMaps       = false;
  options.generateDepthBuffer   = false;
  options.generateStencilBuffer = false;
  options.samplingMode          = Constants::TEXTURE_NEAREST_SAMPLINGMODE;

  auto texture = _engine->createRenderTargetCubeTexture(ISize{size, size}, options);
  _engine->updateTextureWrappingMode(texture,                              //
                                     Constants::TEXTURE_CLAMP_ADDRESSMODE, //
                                     Constants::TEXTURE_CLAMP_ADDRESSMODE, //
                                     Constants::TEXTURE_CLAMP_ADDRESSMODE);

  _engine->updateTextureSamplingMode(Constants::TEXTURE_TRILINEAR_SAMPLINGMODE, texture, true);

  return texture;
}

BaseTexturePtr HDRFiltering::_prefilterInternal(const BaseTexturePtr& texture)
{
  const auto width        = texture->getSize().width;
  const auto mipmapsCount = std::round(Scalar::Log2(width)) + 1;

  const auto effect        = _effectWrapper->effect;
  const auto outputTexture = _createRenderTarget(width);
  _effectRenderer->setViewport();

  const auto intTexture = texture->getInternalTexture();
  if (intTexture) {
    // Just in case generate fresh clean mips.
    _engine->updateTextureSamplingMode(Constants::TEXTURE_TRILINEAR_SAMPLINGMODE, intTexture, true);
  }

  _effectRenderer->applyEffectWrapper(_effectWrapper);

  const std::array<std::array<Vector3, 3>, 6> directions = {{
    {Vector3(0, 0, -1), Vector3(0, -1, 0), Vector3(1, 0, 0)},    // PositiveX
    {Vector3(0, 0, 1), Vector3(0, -1, 0), Vector3(-1, 0, 0)},    // NegativeX
    {{Vector3(1, 0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0)}},    // PositiveY
    {{Vector3(1, 0, 0), Vector3(0, 0, -1), Vector3(0, -1, 0)}},  // NegativeY
    {{Vector3(1, 0, 0), Vector3(0, -1, 0), Vector3(0, 0, 1)}},   // PositiveZ
    {{Vector3(-1, 0, 0), Vector3(0, -1, 0), Vector3(0, 0, -1)}}, // NegativeZ
  }};

  effect->setFloat("hdrScale", hdrScale);
  effect->setFloat2("vFilteringInfo", static_cast<float>(texture->getSize().width),
                    static_cast<float>(mipmapsCount));
  effect->setTexture("inputTexture", texture);

  for (unsigned int face = 0; face < 6; ++face) {
    effect->setVector3("up", directions[face][0]);
    effect->setVector3("right", directions[face][1]);
    effect->setVector3("front", directions[face][2]);

    for (unsigned int lod = 0; lod < mipmapsCount; ++lod) {

      _engine->bindFramebuffer(outputTexture, face, std::nullopt, std::nullopt, true, lod);
      _effectRenderer->applyEffectWrapper(_effectWrapper);

      auto alpha = static_cast<float>(
        std::pow(2, (lod - _lodGenerationOffset) / _lodGenerationScale) / width);
      if (lod == 0) {
        alpha = 0.f;
      }

      effect->setFloat("alphaG", alpha);

      _effectRenderer->draw();
    }
  }

  // Cleanup
  _effectRenderer->restoreStates();
  _engine->restoreDefaultFramebuffer();
  _engine->_releaseFramebufferObjects(outputTexture);
  _engine->_releaseTexture(texture->_texture);

  // Internal Swap
  outputTexture->_swapAndDie(texture->_texture);

  texture->_prefiltered = true;

  return texture;
}

EffectWrapperPtr HDRFiltering::_createEffect(const BaseTexturePtr& texture,
                                             const std::function<void(Effect* effect)>& onCompiled)
{
  std::vector<std::string> defines;
  if (texture->gammaSpace()) {
    defines.emplace_back("#define GAMMA_INPUT");
  }

  defines.emplace_back(
    StringTools::printf("#define NUM_SAMPLES %u%s", quality, "u")); // unsigned int

  EffectWrapperCreationOptions creationOptions;
  creationOptions.engine         = _engine;
  creationOptions.name           = "hdrFiltering";
  creationOptions.vertexShader   = "hdrFiltering";
  creationOptions.fragmentShader = "hdrFiltering";
  creationOptions.samplerNames   = {"inputTexture"};
  creationOptions.uniformNames   = {"vSampleDirections", "vWeights", "up",    "right", "front",
                                  "vFilteringInfo",    "hdrScale", "alphaG"};
  creationOptions.useShaderStore = true;
  creationOptions.defines        = defines;
  creationOptions.onCompiled     = onCompiled;

  const auto effectWrapper = std::make_shared<EffectWrapper>(creationOptions);

  return effectWrapper;
}

bool HDRFiltering::isReady(const BaseTexturePtr& texture) const
{
  return (texture->isReady() && _effectWrapper->effect->isReady());
}

void HDRFiltering::prefilter(const BaseTexturePtr& texture, const std::function<void()>& onFinished)
{
  if (_engine->webGLVersion() == 1.f) {
    BABYLON_LOG_WARN(
      "HDRFiltering",
      "HDR prefiltering is not available in WebGL 1., you can use real time filtering instead.");
    return;
  }

  {
    _effectRenderer = std::make_shared<EffectRenderer>(_engine);
    _effectWrapper  = _createEffect(texture);
    _effectWrapper->effect->executeWhenCompiled(
      [this, texture, onFinished](Effect * /*effect*/) -> void {
        _prefilterInternal(texture);
        _effectRenderer->dispose();
        _effectWrapper->dispose();
        if (onFinished) {
          onFinished();
        }
      });
  };
}

} // end of namespace BABYLON
