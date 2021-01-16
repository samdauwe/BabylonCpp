#include <babylon/materials/textures/dynamic_texture.h>

#include <nlohmann/json.hpp>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/icanvas_rendering_context2D.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

DynamicTexture::DynamicTexture(const std::string& iName, const DynamicTextureOptions& options,
                               Scene* scene, bool generateMipMaps, unsigned int iSamplingMode,
                               unsigned int format, const std::optional<bool>& invertY)
    : Texture{nullptr, scene,   !generateMipMaps, invertY, iSamplingMode,
              nullptr, nullptr, std::nullopt,     false,   format}
    , canRescale{this, &DynamicTexture::get_canRescale}
    , _generateMipMaps{generateMipMaps}
{
  name  = iName;
  wrapU = TextureConstants::CLAMP_ADDRESSMODE;
  wrapV = TextureConstants::CLAMP_ADDRESSMODE;

  _generateMipMaps = generateMipMaps;

  const auto engine = _getEngine();
  if (!engine) {
    return;
  }

  if (options.canvas) {
    _canvas = options.canvas;
    _texture
      = engine->createDynamicTexture(options.width, options.height, generateMipMaps, samplingMode);
  }
  else {
    _texture
      = engine->createDynamicTexture(options.width, options.height, generateMipMaps, samplingMode);
  }

  if (_canvas) {
    const auto textureSize = getSize();
    _canvas->width   = textureSize.width;
    _canvas->height  = textureSize.height;
    _context         = _canvas->getContext2d();
  }
}

DynamicTexture::~DynamicTexture() = default;

std::string DynamicTexture::getClassName() const
{
  return "DynamicTexture";
}

bool DynamicTexture::get_canRescale() const
{
  return true;
}

void DynamicTexture::_recreate(const ISize& textureSize)
{
  _canvas->width  = textureSize.width;
  _canvas->height = textureSize.height;

  releaseInternalTexture();

  _texture = _getEngine()->createDynamicTexture(textureSize.width, textureSize.height,
                                                _generateMipMaps, samplingMode());
}

void DynamicTexture::scale(float ratio)
{
  auto textureSize = getSize();

  textureSize *= ratio;

  _recreate(textureSize);
}

void DynamicTexture::scaleTo(int width, int height)
{
  auto textureSize = getSize();

  textureSize.width  = width;
  textureSize.height = height;

  _recreate(textureSize);
}

ICanvasRenderingContext2D* DynamicTexture::getContext()
{
  return _context;
}

void DynamicTexture::clear()
{
  auto size = getSize();
  _context->fillRect(0, 0, size.width, size.height);
}

void DynamicTexture::update(bool iInvertY, bool premulAlpha)
{
  _getEngine()->updateDynamicTexture(_texture, _canvas, iInvertY, premulAlpha, *_format);
}

void DynamicTexture::drawText(const std::string& text, int x, int y, const std::string& font,
                              const std::string& color, const std::string& clearColor,
                              bool iInvertY, bool _update)
{
  auto size = getSize();
  if (!clearColor.empty()) {
    _context->fillStyle = clearColor;
    _context->fillRect(0, 0, size.width, size.height);
  }

  _context->font = font;

  if (x == 0) {
    auto textSize = _context->measureText(text);
    x             = (size.width - textSize.width) / 2;
  }

  if (y == 0) {
    auto _font = font;
    StringTools::replaceInPlace(_font, "/\\D/g", "");
    auto fontSize = static_cast<float>(std::stoi(_font));
    y             = static_cast<int>((static_cast<float>(size.height) / 2.f) + (fontSize / 3.65f));
  }

  _context->fillStyle = color;
  _context->fillText(text, x, y);

  if (_update) {
    update(iInvertY);
  }
}

DynamicTexturePtr DynamicTexture::clone()
{
  auto scene = getScene();

  if (!scene) {
    return nullptr;
  }

  auto textureSize = getSize();
  DynamicTextureOptions options;
  options.canvas  = _canvas;
  options.width   = textureSize.width;
  options.height  = textureSize.height;
  auto newTexture = DynamicTexture::New(name, options, scene, _generateMipMaps);

  // Base texture
  newTexture->hasAlpha = hasAlpha();
  newTexture->level    = level;

  // Dynamic Texture
  newTexture->wrapU = wrapU();
  newTexture->wrapV = wrapV();

  return newTexture;
}

json DynamicTexture::serialize() const
{
  return nullptr;
}

void DynamicTexture::_rebuild(bool /*forceFullRebuild*/)
{
  update();
}

} // end of namespace BABYLON
