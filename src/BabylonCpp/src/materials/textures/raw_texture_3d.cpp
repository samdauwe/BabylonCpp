#include <babylon/materials/textures/raw_texture_3d.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

RawTexture3D::RawTexture3D(const ArrayBufferView& data, int width, int height,
                           int depth, unsigned int iFormat, Scene* scene,
                           bool generateMipMaps, bool invertY,
                           unsigned int samplingMode, unsigned int textureType)
    : Texture{nullptr, scene, !generateMipMaps, invertY}, format{iFormat}
{
  _engine = scene->getEngine();

  _texture = scene->getEngine()->createRawTexture3D(data,            //
                                                    width,           //
                                                    height,          //
                                                    depth,           //
                                                    iFormat,         //
                                                    generateMipMaps, //
                                                    invertY,         //
                                                    samplingMode,    //
                                                    "",              //
                                                    textureType      //
  );

  is3D = true;
}

RawTexture3D::~RawTexture3D()
{
}

void RawTexture3D::update(const ArrayBufferView& data)
{
  if (!_texture) {
    return;
  }
  _engine->updateRawTexture3D(_texture, data, _texture->format,
                              _texture->invertY, "", _texture->type);
}

} // end of namespace BABYLON
