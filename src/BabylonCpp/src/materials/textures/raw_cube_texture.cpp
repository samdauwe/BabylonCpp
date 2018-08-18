#include <babylon/materials/textures/raw_cube_texture.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

RawCubeTexture::RawCubeTexture(Scene* scene, const vector_t<Uint8Array>& data,
                               int size, unsigned int format, unsigned int type,
                               bool generateMipMaps, bool invertY,
                               unsigned int samplingMode,
                               const string_t& compression)
    : CubeTexture{"", scene}
{
  _texture = scene->getEngine()->createRawCubeTexture(
    data, size, format, type, generateMipMaps, invertY, samplingMode,
    compression);
}

RawCubeTexture::~RawCubeTexture()
{
}

void RawCubeTexture::update(const vector_t<Uint8Array>& data,
                            unsigned int format, unsigned int type,
                            bool invertY, const string_t& compression,
                            unsigned int /*level*/)
{
  _texture->getEngine()->updateRawCubeTexture(_texture, data, format, type,
                                              invertY, compression);
}

CubeTexture* RawCubeTexture::clone() const
{
  return nullptr;
}

} // end of namespace BABYLON
