#include <babylon/materials/textures/raw_texture_2d_array.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

RawTexture2DArray::RawTexture2DArray(const ArrayBufferView& data, int width, int height, int depth,
                                     unsigned int iFormat, Scene* scene, bool generateMipMaps,
                                     bool iInvertY, unsigned int iSamplingMode,
                                     unsigned int iTextureType)
    : Texture{nullptr, scene, !generateMipMaps, iInvertY}, format{iFormat}
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

  is2DArray = true;
}

RawTexture2DArray::~RawTexture2DArray() = default;

void RawTexture2DArray::update(const ArrayBufferView& data)
{
  if (!_texture) {
    return;
  }
  _getEngine()->updateRawTexture2DArray(_texture, data, _texture->format, _texture->invertY, "",
                                        _texture->type);
}

} // end of namespace BABYLON
