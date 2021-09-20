#include <babylon/engines/render_target_wrapper.h>

#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

RenderTargetWrapper::RenderTargetWrapper(bool isMulti, bool isCube,
                                         const RenderTargetTextureSize& size, ThinEngine* engine)
    : _generateStencilBuffer{false}
    , _generateDepthBuffer{false}
    , _depthStencilTextureWithStencil{false}
    , texture{this, &RenderTargetWrapper::get_texture}
    , textures{this, &RenderTargetWrapper::get_textures}
    , samples{this, &RenderTargetWrapper::get_samples}
    , _nullTexture{nullptr}
{
  _isMulti             = isMulti;
  _isCube              = isCube;
  _size                = size;
  _engine              = engine;
  _depthStencilTexture = nullptr;
}

InternalTexturePtr& RenderTargetWrapper::get_texture()
{
  return _textures.empty() ? _nullTexture : _textures[0];
}

std::vector<InternalTexturePtr>& RenderTargetWrapper::get_textures()
{
  return _textures;
}

uint32_t RenderTargetWrapper::get_samples() const
{
  return texture() ? texture()->samples : 1u;
}

} // end of namespace BABYLON
