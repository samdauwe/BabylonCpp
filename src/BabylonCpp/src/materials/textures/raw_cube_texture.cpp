#include <babylon/materials/textures/raw_cube_texture.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

RawCubeTexture::RawCubeTexture(Scene* scene,
                               const std::vector<ArrayBufferView>& data,
                               int size, unsigned int format, unsigned int type,
                               bool generateMipMaps, bool invertY,
                               unsigned int samplingMode,
                               const std::string& compression)
    : CubeTexture{"", scene}
{
  _texture = scene->getEngine()->createRawCubeTexture(
    data, size, format, type, generateMipMaps, invertY, samplingMode,
    compression);
}

RawCubeTexture::~RawCubeTexture() = default;

void RawCubeTexture::update(const std::vector<ArrayBufferView>& data,
                            unsigned int format, unsigned int type,
                            bool invertY, const std::string& compression,
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
