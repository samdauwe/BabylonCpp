#include <babylon/materials/textures/raw_texture_2d_array.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

RawTexture2DArray::RawTexture2DArray(const ArrayBufferView& data, int width, int height, int depth,
                                     unsigned int iFormat, Scene* scene, bool generateMipMaps,
                                     bool iInvertY, unsigned int iSamplingMode,
                                     unsigned int iTextureType)
    : Texture{nullptr, scene, !generateMipMaps, iInvertY}
    , format{iFormat}
    , depth{this, &RawTexture2DArray::get_depth}
{
  _texture = scene->getEngine()->createRawTexture2DArray(data,            //
                                                         width,           //
                                                         height,          //
                                                         depth,           //
                                                         iFormat,         //
                                                         generateMipMaps, //
                                                         invertY,         //
                                                         iSamplingMode,   //
                                                         "",              //
                                                         iTextureType     //
  );

  _depth    = depth;
  is2DArray = true;
}

RawTexture2DArray::~RawTexture2DArray() = default;

int RawTexture2DArray::get_depth() const
{
  return _depth;
}

void RawTexture2DArray::update(const ArrayBufferView& data)
{
  if (!_texture || !_getEngine()) {
    return;
  }
  _getEngine()->updateRawTexture2DArray(_texture, data, _texture->format, _texture->invertY, "",
                                        _texture->type);
}

std::unique_ptr<RawTexture2DArray>
RawTexture2DArray::CreateRGBATexture(const ArrayBufferView& data, int width, int height, int depth,
                                     Scene* scene, bool generateMipMaps, bool invertY,
                                     unsigned int samplingMode, unsigned int type)
{
  return std::make_unique<RawTexture2DArray>(data, width, height, depth,
                                             Constants::TEXTUREFORMAT_RGBA, scene, generateMipMaps,
                                             invertY, samplingMode, type);
}

} // end of namespace BABYLON
