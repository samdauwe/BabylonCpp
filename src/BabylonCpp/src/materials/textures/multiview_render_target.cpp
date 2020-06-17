#include <babylon/materials/textures/multiview_render_target.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

MultiviewRenderTarget::MultiviewRenderTarget(Scene* scene, const ISize& size)
    : RenderTargetTexture{
      "multiview rtt",                                           // name
      RenderTargetSize{size.width, size.height},                 // size
      scene,                                                     // scene
      false,                                                     // generateMipMaps
      true,                                                      // doNotChangeAspectRatio
      static_cast<unsigned int>(InternalTextureSource::Unknown), // type
      false,                                                     // isCube
      TextureConstants::TRILINEAR_SAMPLINGMODE,                  // samplingMode
      false,                                                     // generateDepthBuffer
      false,                                                     // generateStencilBuffer
      true,                                                      // isMulti
      Constants::TEXTUREFORMAT_RGBA,                             // format
      true                                                       // delayAllocation
    }
{
  auto internalTexture
    = scene->getEngine()->createMultiviewRenderTargetTexture(getRenderWidth(), getRenderHeight());
  internalTexture->isMultiview = true;
  internalTexture->format      = Constants::TEXTUREFORMAT_RGBA;
  _texture                     = internalTexture;
  samples = std::min(static_cast<unsigned int>(_getEngine()->getCaps().maxSamples), samples());
}

MultiviewRenderTarget::~MultiviewRenderTarget() = default;

void MultiviewRenderTarget::_bindFrameBuffer(unsigned int /*faceIndex*/)
{
  if (!_texture) {
    return;
  }
  getScene()->getEngine()->bindMultiviewFramebuffer(_texture);
}

unsigned int MultiviewRenderTarget::getViewCount() const
{
  return 2;
}

} // end of namespace BABYLON
