#include <babylon/materials/textures/multi_render_target.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

MultiRenderTarget::MultiRenderTarget(const std::string& iName, Size size, std::size_t count,
                                     Scene* scene,
                                     const std::optional<IMultiRenderTargetOptions>& options)
    : RenderTargetTexture{iName, size, scene,
                          options && (*options).generateMipMaps ? *(*options).generateMipMaps :
                                                                  false,
                          options && (*options).doNotChangeAspectRatio ?
                            *(*options).doNotChangeAspectRatio :
                            true}
    , isSupported{this, &MultiRenderTarget::get_isSupported}
    , textures{this, &MultiRenderTarget::get_textures}
    , depthTexture{this, &MultiRenderTarget::get_depthTexture}
    , wrapU{this, &MultiRenderTarget::set_wrapU}
    , wrapV{this, &MultiRenderTarget::set_wrapV}
    , _nullTexture{nullptr}
{
  auto generateMipMaps
    = options && (*options).generateMipMaps ? *(*options).generateMipMaps : false;
  auto generateDepthTexture
    = options && (*options).generateDepthTexture ? *(*options).generateDepthTexture : false;
  auto doNotChangeAspectRatio
    = options && (*options).doNotChangeAspectRatio ? *(*options).doNotChangeAspectRatio : true;

  _engine = scene->getEngine();

  if (!isSupported()) {
    dispose();
    return;
  }

  Uint32Array types;
  Uint32Array samplingModes;

  for (std::size_t i = 0; i < count; ++i) {
    if (options && i < (*options).types.size()) {
      types.emplace_back((*options).types[i]);
    }
    else {
      types.emplace_back(options && (*options).defaultType ? *(*options).defaultType :
                                                             Constants::TEXTURETYPE_UNSIGNED_INT);
    }

    if (options && i < (*options).samplingModes.size()) {
      samplingModes.emplace_back((*options).samplingModes[i]);
    }
    else {
      samplingModes.emplace_back(TextureConstants::BILINEAR_SAMPLINGMODE);
    }
  }

  const auto generateDepthBuffer = !options || (*options).generateDepthBuffer == std::nullopt ?
                                     true :
                                     *(*options).generateDepthBuffer;
  const auto generateStencilBuffer = !options || (*options).generateStencilBuffer == std::nullopt ?
                                       false :
                                       *(*options).generateStencilBuffer;

  _size = size;

  _multiRenderTargetOptions = IMultiRenderTargetOptions{
    generateMipMaps,                    // generateMipMaps
    types,                              // types
    samplingModes,                      // samplingModes
    generateDepthBuffer,                // generateDepthBuffer
    generateStencilBuffer,              // generateStencilBuffer
    generateDepthTexture,               // generateDepthTexture
    doNotChangeAspectRatio,             // doNotChangeAspectRatio
    count,                              // textureCount
    Constants::TEXTURETYPE_UNSIGNED_INT // defaultType
  };

  _createInternalTextures();
  _createInternalTextures();
}

MultiRenderTarget::~MultiRenderTarget() = default;

bool MultiRenderTarget::get_isSupported() const
{
  return (_engine->webGLVersion() > 1.f) || (_engine->getCaps().drawBuffersExtension);
}

std::vector<TexturePtr>& MultiRenderTarget::get_textures()
{
  return _textures;
}

TexturePtr& MultiRenderTarget::get_depthTexture()
{
  return (!_textures.empty()) ? _textures.back() : _nullTexture;
}

void MultiRenderTarget::set_wrapU(unsigned int wrap)
{
  if (!_textures.empty()) {
    for (auto& texture : _textures) {
      texture->wrapU = wrap;
    }
  }
}

void MultiRenderTarget::set_wrapV(unsigned int wrap)
{
  if (!_textures.empty()) {
    for (auto& texture : _textures) {
      texture->wrapV = wrap;
    }
  }
}

void MultiRenderTarget::_rebuild()
{
  releaseInternalTextures();
  _createInternalTextures();

  for (std::size_t i = 0; i < _internalTextures.size(); ++i) {
    auto& texture     = _textures[i];
    texture->_texture = _internalTextures[i];
  }

  // Keeps references to frame buffer and stencil/depth buffer
  _texture = (!_internalTextures.empty()) ? _internalTextures[0] : nullptr;
}

void MultiRenderTarget::_createInternalTextures()
{
  _internalTextures = _engine->createMultipleRenderTarget(_size, _multiRenderTargetOptions);
}

void MultiRenderTarget::_createTextures()
{
  _textures.clear();
  for (const auto& internalTexture : _internalTextures) {
    auto texture      = Texture::New("", getScene());
    texture->_texture = internalTexture;
    _textures.emplace_back(texture);
  }

  // Keeps references to frame buffer and stencil/depth buffer
  _texture = (!_internalTextures.empty()) ? _internalTextures[0] : nullptr;
}

unsigned int MultiRenderTarget::get_samples() const
{
  return _samples;
}

void MultiRenderTarget::set_samples(unsigned int value)
{
  if (_samples == value) {
    return;
  }

  _samples = _engine->updateMultipleRenderTargetTextureSampleCount(_internalTextures, value);
}

void MultiRenderTarget::resize(Size size)
{
  releaseInternalTextures();
  _size = size;
  _createInternalTextures();
}

void MultiRenderTarget::unbindFrameBuffer(Engine* engine, unsigned int faceIndex)
{
  engine->unBindMultiColorAttachmentFramebuffer(_internalTextures, isCube, [&, faceIndex]() {
    auto _faceIndex = static_cast<int>(faceIndex);
    onAfterRenderObservable.notifyObservers(&_faceIndex);
  });
}

void MultiRenderTarget::dispose()
{
  releaseInternalTextures();

  RenderTargetTexture::dispose();
}

void MultiRenderTarget::releaseInternalTextures()
{
  if (_internalTextures.empty()) {
    return;
  }

  for (size_t i = _internalTextures.size(); i-- > 0;) {
    if (_internalTextures[i] != nullptr) {
      _internalTextures[i]->dispose();
    }
  }
  _internalTextures.clear();
}

} // end of namespace BABYLON
