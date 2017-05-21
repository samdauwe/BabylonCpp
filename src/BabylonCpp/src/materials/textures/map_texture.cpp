#include <babylon/materials/textures/map_texture.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/math/size.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

MapTexture::MapTexture(const std::string& iName, Scene* scene,
                       const ISize& size, unsigned int samplingMode,
                       bool useMipMap, float margin)
    : Texture{"", scene, !useMipMap, false, samplingMode}, _size{size}
{
  name  = iName;
  wrapU = Texture::CLAMP_ADDRESSMODE;
  wrapV = Texture::CLAMP_ADDRESSMODE;

  // Create the rectPackMap that will allocate portion of the texture
  _rectPackingMap
    = std::make_unique<RectPackingMap>(Size(size.width, size.height), margin);

  // Create the texture that will store the content
  RenderTargetOptions options;
  options.generateMipMaps = !noMipmap();
  options.type            = Engine::TEXTURETYPE_UNSIGNED_INT;

  _texture = scene->getEngine()->createRenderTargetTexture(size, options);
}

MapTexture::~MapTexture()
{
}

PackedRect* MapTexture::allocateRect(const Size& size)
{
  return _rectPackingMap->addRect(size);
}

void MapTexture::freeRect(PackedRect* rectInfo)
{
  rectInfo->freeContent();
}

float MapTexture::freeSpace() const
{
  return _rectPackingMap->freeSpace();
}

void MapTexture::bindTextureForRect(const PackedRect& rect, bool clear)
{
  bindTextureForPosSize(rect.pos(), *rect.contentSize(), clear);
}

void MapTexture::bindTextureForPosSize(const Vector2& pos, const Size& size,
                                       bool clear)
{
  const int posX = static_cast<int>(pos.x);
  const int posY = static_cast<int>(pos.y);

  auto engine = getScene()->getEngine();
  engine->bindFramebuffer(_texture);
  _replacedViewport = std::make_unique<Viewport>(
    engine->setDirectViewport(posX, posY, size.width, size.height));
  if (clear) {
    // We only want to clear the part of the texture we're binding to, only the
    // scissor can help us to achieve that
    engine->scissorClear(posX, posY, size.width, size.height,
                         Color4(0.f, 0.f, 0.f, 0.f));
  }
}

void MapTexture::unbindTexture(bool dumpForDebug)
{
  auto engine = getScene()->getEngine();

  // Dump ?
  if (dumpForDebug) {
    Tools::DumpFramebuffer(_size.width, _size.height, engine);
  }

  if (_replacedViewport) {
    engine->setViewport(*_replacedViewport);
    _replacedViewport = nullptr;
  }

  engine->unBindFramebuffer(_texture);
}

bool MapTexture::canRescale() const
{
  return false;
}

std::unique_ptr<MapTexture> MapTexture::clone() const
{
  return nullptr;
}

} // end of namespace BABYLON
