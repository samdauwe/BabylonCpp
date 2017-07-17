#include <babylon/tools/texture_tools.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/post_process_manager.h>

namespace BABYLON {

std::unique_ptr<Texture> TextureTools::CreateResizedCopy(Texture* texture,
                                                         int width, int height,
                                                         bool useBilinearMode)
{
  auto scene  = texture->getScene();
  auto engine = scene->getEngine();

  auto rtt = std::make_unique<RenderTargetTexture>("resized" + texture->name, //
                                                   ISize{width, height},      //
                                                   scene,                     //
                                                   !texture->noMipmap(),      //
                                                   true,                      //
                                                   texture->_texture->type,   //
                                                   false,                     //
                                                   texture->_samplingMode,    //
                                                   false                      //
                                                   );

  rtt->wrapU                     = texture->wrapU;
  rtt->wrapV                     = texture->wrapV;
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

  auto passPostProcess = std::make_unique<PassPostProcess>(
    "pass", 1, nullptr,
    useBilinearMode ? TextureConstants::BILINEAR_SAMPLINGMODE :
                      TextureConstants::NEAREST_SAMPLINGMODE,
    engine, false, EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  passPostProcess->getEffect()->executeWhenCompiled([&](Effect* /*effect*/) {
    passPostProcess->setOnApply(
      [&](Effect* effect) { effect->setTexture("textureSampler", texture); });

    scene->postProcessManager->directRender({passPostProcess.get()},
                                            rtt->getInternalTexture());

    engine->unBindFramebuffer(rtt->getInternalTexture());
    rtt->disposeFramebufferObjects();
    passPostProcess->dispose();

    rtt->_texture->isReady = true;
  });

  return rtt;
}

} // end of namespace BABYLON
