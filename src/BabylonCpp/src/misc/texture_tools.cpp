#include <babylon/misc/texture_tools.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/postprocesses/pass_post_process.h>
#include <babylon/postprocesses/post_process_manager.h>

namespace BABYLON {

Float32Array TextureTools::_FloatView;
Int32Array TextureTools::_Int32View;

TexturePtr TextureTools::CreateResizedCopy(const TexturePtr& texture, int width, int height,
                                           bool useBilinearMode)
{
  auto scene  = texture->getScene();
  auto engine = scene->getEngine();

  auto rtt = RenderTargetTexture::New("resized" + texture->name,       //
                                      RenderTargetSize{width, height}, //
                                      scene,                           //
                                      !texture->noMipmap(),            //
                                      true,                            //
                                      texture->_texture->type,         //
                                      false,                           //
                                      texture->samplingMode(),         //
                                      false                            //
  );

  rtt->wrapU                     = texture->wrapU();
  rtt->wrapV                     = texture->wrapV();
  rtt->uOffset                   = texture->uOffset;
  rtt->vOffset                   = texture->vOffset;
  rtt->uScale                    = texture->uScale;
  rtt->vScale                    = texture->vScale;
  rtt->uAng                      = texture->uAng;
  rtt->vAng                      = texture->vAng;
  rtt->wAng                      = texture->wAng;
  rtt->coordinatesIndex          = texture->coordinatesIndex;
  rtt->level                     = texture->level;
  rtt->anisotropicFilteringLevel = texture->anisotropicFilteringLevel;
  rtt->_texture->isReady         = false;

  texture->wrapU = TextureConstants::CLAMP_ADDRESSMODE;
  texture->wrapV = TextureConstants::CLAMP_ADDRESSMODE;

  auto passPostProcess
    = PassPostProcess::New("pass", 1.f, nullptr,
                           useBilinearMode ? TextureConstants::BILINEAR_SAMPLINGMODE :
                                             TextureConstants::NEAREST_SAMPLINGMODE,
                           engine, false, Constants::TEXTURETYPE_UNSIGNED_INT);
  passPostProcess->getEffect()->executeWhenCompiled([&](Effect* /*effect*/) {
    passPostProcess->onApply
      = [&](Effect* effect, EventState&) { effect->setTexture("textureSampler", texture); };

    auto internalTexture = rtt->getInternalTexture();

    if (internalTexture) {
      scene->postProcessManager->directRender({passPostProcess}, internalTexture);

      engine->unBindFramebuffer(internalTexture);
      rtt->disposeFramebufferObjects();
      passPostProcess->dispose();

      internalTexture->isReady = true;
    }
  });

  return rtt;
}

InternalTexturePtr TextureTools::ApplyPostProcess(const std::string& postProcessName,
                                                  const InternalTexturePtr& internalTexture,
                                                  Scene* scene, const std::optional<int>& iType,
                                                  const std::optional<unsigned int>& iSamplingMode,
                                                  const std::optional<unsigned int>& iFormat)
{
  // Gets everything ready.
  const auto engine = static_cast<Engine*>(internalTexture->getEngine());

  internalTexture->isReady = false;

  const auto samplingMode = iSamplingMode.value_or(internalTexture->samplingMode);
  const auto type         = iType.has_value() ?
                      (iType.value() == -1 ? Constants::TEXTURETYPE_UNSIGNED_BYTE :
                                             static_cast<unsigned int>(iType.value())) :
                      internalTexture->type;
  const auto format = iFormat.value_or(internalTexture->format);

  {
    // Create the post process
    const auto postProcess
      = PostProcess::New("postprocess", postProcessName, {}, {}, 1.f, nullptr, samplingMode, engine,
                         false, "", type, "", {}, false, format);

    // Hold the output of the decoding.
    RenderTargetSize size{
      internalTexture->width, // width
      internalTexture->height // height
    };
    IRenderTargetOptions options;
    {
      options.generateDepthBuffer   = false;
      options.generateMipMaps       = false;
      options.generateStencilBuffer = false;
      options.samplingMode          = samplingMode;
      options.type                  = type;
      options.format                = format;
    }
    const auto encodedTexture = engine->createRenderTargetTexture(size, options);

    postProcess->getEffect()->executeWhenCompiled([=](Effect * /*effect*/) -> void {
      // PP Render Pass
      postProcess->onApply = [=](Effect* effect, EventState & /*es*/) -> void {
        effect->_bindTexture("textureSampler", internalTexture);
        effect->setFloat2("scale", 1.f, 1.f);
      };
      scene->postProcessManager->directRender({postProcess}, encodedTexture, true);

      // Cleanup
      engine->restoreDefaultFramebuffer();
      engine->_releaseTexture(internalTexture);
      engine->_releaseFramebufferObjects(encodedTexture);
      if (postProcess) {
        postProcess->dispose();
      }

      // Internal Swap
      encodedTexture->_swapAndDie(internalTexture);

      // Ready to get rolling again.
      internalTexture->type    = type;
      internalTexture->format  = Constants::TEXTUREFORMAT_RGBA;
      internalTexture->isReady = true;
    });
  }

  return internalTexture;
}

int TextureTools::ToHalfFloat(float value)
{
  if (!TextureTools::_FloatView.empty()) {
    TextureTools::_FloatView = Float32Array(1);
    TextureTools::_Int32View = Int32Array(1);
  }

  TextureTools::_FloatView[0] = value;
  TextureTools::_Int32View[0] = static_cast<int32_t>(value);
  const auto x                = TextureTools::_Int32View[0];

  auto bits    = (x >> 16) & 0x8000; /* Get the sign */
  auto m       = (x >> 12) & 0x07ff; /* Keep one extra bit for rounding */
  const auto e = (x >> 23) & 0xff;   /* Using int is faster here */

  /* If zero, or denormal, or exponent underflows too much for a denormal
   * half, return signed zero. */
  if (e < 103) {
    return bits;
  }

  /* If NaN, return NaN. If Inf or exponent overflow, return Inf. */
  if (e > 142) {
    bits |= 0x7c00;
    /* If exponent was 0xff and one mantissa bit was set, it means NaN,
     * not Inf, so make sure we set one mantissa bit too. */
    bits |= ((e == 255) ? 0 : 1) && (x & 0x007fffff);
    return bits;
  }

  /* If exponent underflows but not too much, return a denormal */
  if (e < 113) {
    m |= 0x0800;
    /* Extra rounding may overflow and set mantissa to 0 and exponent
     * to 1, which is OK. */
    bits |= (m >> (114 - e)) + ((m >> (113 - e)) & 1);
    return bits;
  }

  bits |= ((e - 112) << 10) | (m >> 1);
  bits += m & 1;
  return bits;
}

int TextureTools::FromHalfFloat(int value)
{
  const auto s = (value & 0x8000) >> 15;
  const auto e = (value & 0x7C00) >> 10;
  const auto f = value & 0x03FF;

  if (e == 0) {
    return static_cast<int>((s ? -1 : 1) * std::pow(2, -14) * (f / std::pow(2, 10)));
  }
  else if (e == 0x1F) {
    return f ? std::numeric_limits<int>::quiet_NaN() :
               ((s ? -1 : 1) * std::numeric_limits<int>::infinity());
  }

  return static_cast<int>((s ? -1 : 1) * std::pow(2, e - 15) * (1 + (f / std::pow(2, 10))));
}

} // end of namespace BABYLON
