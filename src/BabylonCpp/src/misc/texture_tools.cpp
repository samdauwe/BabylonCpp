#include <babylon/misc/texture_tools.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/postprocesses/pass_post_process.h>
#include <babylon/postprocesses/post_process_manager.h>

namespace BABYLON {

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
  const auto format       = iFormat.value_or(internalTexture->format);

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

    postProcess->getEffect()->executeWhenCompiled([=](Effect* /*effect*/) -> void {
      // PP Render Pass
      postProcess->onApply = [=](Effect* effect, EventState& /*es*/) -> void {
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

} // end of namespace BABYLON
