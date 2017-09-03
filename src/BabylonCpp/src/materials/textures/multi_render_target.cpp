#include <babylon/materials/textures/multi_render_target.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

MultiRenderTarget::MultiRenderTarget(const std::string& name, Size size,
                                     std::size_t count, Scene* scene)
    : MultiRenderTarget{
        name, size, count, scene,
        IMultiRenderTargetOptions{false, {}, {}, true, false, false, true, 0}}
{
}

MultiRenderTarget::MultiRenderTarget(const std::string& name, Size size,
                                     std::size_t count, Scene* scene,
                                     const IMultiRenderTargetOptions& options)
    : RenderTargetTexture{name, size, scene, options.generateMipMaps,
                          options.doNotChangeAspectRatio}
{
  const auto generateMipMaps        = options.generateMipMaps;
  const auto generateDepthTexture   = options.generateDepthTexture;
  const auto doNotChangeAspectRatio = options.doNotChangeAspectRatio;

  if (!isSupported()) {
    dispose();
    return;
  }

  std::vector<unsigned int> types;
  std::vector<unsigned int> samplingModes;

  for (std::size_t i = 0; i < count; ++i) {
    if (i < options.types.size()) {
      types.emplace_back(options.types[i]);
    }
    else {
      types.emplace_back(EngineConstants::TEXTURETYPE_FLOAT);
    }

    if (i < options.samplingModes.size()) {
      samplingModes.emplace_back(options.samplingModes[i]);
    }
    else {
      samplingModes.emplace_back(TextureConstants::BILINEAR_SAMPLINGMODE);
    }
  }

  const auto generateDepthBuffer   = options.generateDepthBuffer;
  const auto generateStencilBuffer = options.generateStencilBuffer;

  _count = count;
  _size  = size;

  _multiRenderTargetOptions = IMultiRenderTargetOptions{
    generateMipMaps,        // generateMipMaps
    types,                  // types
    samplingModes,          // samplingModes
    generateDepthBuffer,    // generateDepthBuffer
    generateStencilBuffer,  // generateStencilBuffer
    generateDepthTexture,   // generateDepthTexture
    doNotChangeAspectRatio, // doNotChangeAspectRatio
    count                   // textureCount
  };

  _internalTextures = scene->getEngine()->createMultipleRenderTarget(
    size, _multiRenderTargetOptions);

  _createInternalTextures();
}

MultiRenderTarget::~MultiRenderTarget()
{
}

bool MultiRenderTarget::isSupported() const
{
  auto engine = getScene()->getEngine();
  return (engine->webGLVersion() > 1.f)
         || (engine->getCaps().drawBuffersExtension);
}

std::vector<Texture*>& MultiRenderTarget::textures()
{
  return _textures;
}

Texture* MultiRenderTarget::depthTexture()
{
  return (!_textures.empty()) ? _textures.back() : nullptr;
}

void MultiRenderTarget::_createInternalTextures()
{
  _textures.clear();
  for (std::size_t i = 0; i < _internalTextures.size(); ++i) {
    auto texture      = Texture::New("", getScene());
    texture->_texture = _internalTextures[i];
    _textures.emplace_back(texture);
  }

  // Keeps references to frame buffer and stencil/depth buffer
  _texture = (!_internalTextures.empty()) ? _internalTextures[0] : nullptr;
}

unsigned int MultiRenderTarget::samples() const
{
  return _samples;
}

void MultiRenderTarget::setSamples(unsigned int value)
{
  if (_samples == value) {
    return;
  }

  for (auto& _webGLTexture : _internalTextures) {
    _samples = getScene()->getEngine()->updateRenderTargetTextureSampleCount(
      _webGLTexture, value);
  }
}

void MultiRenderTarget::resize(Size size)
{
  releaseInternalTextures();
  _internalTextures = getScene()->getEngine()->createMultipleRenderTarget(
    size, _multiRenderTargetOptions);
  _createInternalTextures();
}

void MultiRenderTarget::dispose(bool doNotRecurse)
{
  releaseInternalTextures();

  RenderTargetTexture::dispose(doNotRecurse);
}

void MultiRenderTarget::releaseInternalTextures()
{
  if (_internalTextures.empty()) {
    return;
  }

  for (std::size_t i = _internalTextures.size(); i-- > 0;) {
    if (_internalTextures[i] != nullptr) {
      _internalTextures[i]->dispose();
    }
  }
  _internalTextures.clear();
}

} // end of namespace BABYLON
