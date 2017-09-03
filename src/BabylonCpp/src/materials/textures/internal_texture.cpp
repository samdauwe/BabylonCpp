#include <babylon/materials/textures/internal_texture.h>

#include <babylon/engine/engine.h>
#include <babylon/math/spherical_polynomial.h>

namespace BABYLON {

constexpr unsigned int InternalTexture::DATASOURCE_UNKNOWN;
constexpr unsigned int InternalTexture::DATASOURCE_URL;
constexpr unsigned int InternalTexture::DATASOURCE_TEMP;
constexpr unsigned int InternalTexture::DATASOURCE_RAW;
constexpr unsigned int InternalTexture::DATASOURCE_DYNAMIC;
constexpr unsigned int InternalTexture::DATASOURCE_RENDERTARGET;
constexpr unsigned int InternalTexture::DATASOURCE_MULTIRENDERTARGET;
constexpr unsigned int InternalTexture::DATASOURCE_CUBE;
constexpr unsigned int InternalTexture::DATASOURCE_CUBELOD;
constexpr unsigned int InternalTexture::DATASOURCE_CUBERAW;

InternalTexture::InternalTexture(Engine* engine, unsigned int dataSource)
    : _references{1}, _dataSource{dataSource}, _engine{engine}
{
  _webGLTexture = engine->_createTexture();
}

InternalTexture::~InternalTexture()
{
}

unsigned int InternalTexture::dataSource() const
{
  return _dataSource;
}

void InternalTexture::incrementReferences()
{
  ++_references;
}

void InternalTexture::updateSize(int iWidth, int iHeight)
{
  width      = iWidth;
  height     = iHeight;
  _size      = width * height;
  baseWidth  = width;
  baseHeight = height;
}

void InternalTexture::_rebuild()
{
}

void InternalTexture::dispose()
{
  if (!_webGLTexture) {
    return;
  }

  --_references;
  if (_references == 0) {
    _engine->_releaseTexture(this);
    _webGLTexture = nullptr;
  }
}

} // end of namespace BABYLON
