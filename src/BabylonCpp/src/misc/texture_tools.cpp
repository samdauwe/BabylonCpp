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

} // end of namespace BABYLON
