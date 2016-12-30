#include <babylon/materials/textures/hdr_cube_texture.h>

namespace BABYLON {

HDRCubeTexture::HDRCubeTexture(const std::string& /*url*/, Scene* scene,
                               size_t /*size*/, bool /*noMipmap*/,
                               bool /*generateHarmonics*/,
                               bool /*useInGammaSpace*/,
                               bool /*usePMREMGenerator*/)
    : BaseTexture(scene)
{
}

HDRCubeTexture::~HDRCubeTexture()
{
}

Float32Array HDRCubeTexture::loadBabylonTexture()
{
  return Float32Array();
}

Float32Array HDRCubeTexture::loadHDRTexture()
{
  return Float32Array();
}

void HDRCubeTexture::loadTexture()
{
}

HDRCubeTexture* HDRCubeTexture::clone()
{
  return nullptr;
}

void HDRCubeTexture::delayLoad()
{
}

Matrix* HDRCubeTexture::getReflectionTextureMatrix()
{
  return nullptr;
}

} // end of namespace BABYLON
