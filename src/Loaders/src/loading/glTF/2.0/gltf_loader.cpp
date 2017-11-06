#include <babylon/loading/glTF/2.0/gltf_loader.h>

#include <babylon/core/logging.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {
namespace GLTF2 {

GLTFLoader::GLTFLoader()
{
}

GLTFLoader::~GLTFLoader()
{
}

unsigned int
GLTFLoader::_GetTextureWrapMode(const Nullable<ETextureWrapMode>& iMode)
{
  const auto wrapModeToUint
    = [](const ETextureWrapMode val) { return static_cast<unsigned int>(val); };

  // Set defaults if undefined
  const unsigned int mode = iMode.isNull() ?
                              wrapModeToUint(ETextureWrapMode::REPEAT) :
                              wrapModeToUint(*iMode);

  if (mode == wrapModeToUint(ETextureWrapMode::CLAMP_TO_EDGE)) {
    return TextureConstants::CLAMP_ADDRESSMODE;
  }
  else if (mode == wrapModeToUint(ETextureWrapMode::MIRRORED_REPEAT)) {
    return TextureConstants::MIRROR_ADDRESSMODE;
  }
  else if (mode == wrapModeToUint(ETextureWrapMode::REPEAT)) {
    return TextureConstants::WRAP_ADDRESSMODE;
  }
  else {
    BABYLON_LOGF_WARN("_GetTextureWrapMode", "Invalid texture wrap mode (%ul)",
                      mode);
    return TextureConstants::WRAP_ADDRESSMODE;
  }
}

unsigned int GLTFLoader::_GetTextureSamplingMode(
  const Nullable<ETextureMagFilter>& iMagFilter,
  const Nullable<ETextureMinFilter>& iMinFilter)
{
  const auto magFilterToUint = [](const ETextureMagFilter val) {
    return static_cast<unsigned int>(val);
  };

  const auto minFilterToUint = [](const ETextureMinFilter val) {
    return static_cast<unsigned int>(val);
  };

  // Set defaults if undefined
  const unsigned int magFilter = iMagFilter.isNull() ?
                                   magFilterToUint(ETextureMagFilter::LINEAR) :
                                   magFilterToUint(*iMagFilter);
  const unsigned int minFilter
    = iMinFilter.isNull() ?
        minFilterToUint(ETextureMinFilter::LINEAR_MIPMAP_LINEAR) :
        minFilterToUint(*iMinFilter);

  if (magFilter == magFilterToUint(ETextureMagFilter::LINEAR)) {
    if (minFilter == minFilterToUint(ETextureMinFilter::NEAREST)) {
      return TextureConstants::LINEAR_NEAREST;
    }
    else if (minFilter == minFilterToUint(ETextureMinFilter::LINEAR)) {
      return TextureConstants::LINEAR_LINEAR;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::NEAREST_MIPMAP_NEAREST)) {
      return TextureConstants::LINEAR_NEAREST_MIPNEAREST;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::LINEAR_MIPMAP_NEAREST)) {
      return TextureConstants::LINEAR_LINEAR_MIPNEAREST;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::NEAREST_MIPMAP_LINEAR)) {
      return TextureConstants::LINEAR_NEAREST_MIPLINEAR;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::LINEAR_MIPMAP_LINEAR)) {
      return TextureConstants::LINEAR_LINEAR_MIPLINEAR;
    }
    else {
      BABYLON_LOGF_WARN("_GetTextureSamplingMode",
                        "Invalid texture minification filter (%ul)", minFilter);
      return TextureConstants::LINEAR_LINEAR_MIPLINEAR;
    }
  }
  else {
    if (magFilter != magFilterToUint(ETextureMagFilter::NEAREST)) {
      BABYLON_LOGF_WARN("_GetTextureSamplingMode",
                        "Invalid texture magnification filter (%ul)",
                        magFilter);
    }

    if (minFilter == minFilterToUint(ETextureMinFilter::NEAREST)) {
      return TextureConstants::LINEAR_NEAREST;
    }
    else if (minFilter == minFilterToUint(ETextureMinFilter::LINEAR)) {
      return TextureConstants::LINEAR_LINEAR;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::NEAREST_MIPMAP_NEAREST)) {
      return TextureConstants::LINEAR_NEAREST_MIPNEAREST;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::LINEAR_MIPMAP_NEAREST)) {
      return TextureConstants::LINEAR_LINEAR_MIPNEAREST;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::NEAREST_MIPMAP_LINEAR)) {
      return TextureConstants::LINEAR_NEAREST_MIPLINEAR;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::LINEAR_MIPMAP_LINEAR)) {
      return TextureConstants::LINEAR_LINEAR_MIPLINEAR;
    }
    else {
      BABYLON_LOGF_WARN("_GetTextureSamplingMode",
                        "Invalid texture minification filter (%ul)", minFilter);
      return TextureConstants::LINEAR_LINEAR_MIPLINEAR;
    }
  }
}

unsigned int GLTFLoader::_GetNumComponents(const string_t& type)
{
  if (type == "SCALAR") {
    return 1;
  }
  else if (type == "VEC2") {
    return 2;
  }
  else if (type == "VEC3") {
    return 3;
  }
  else if (type == "VEC4") {
    return 4;
  }
  else if (type == "MAT2") {
    return 4;
  }
  else if (type == "MAT3") {
    return 9;
  }
  else if (type == "MAT4") {
    return 16;
  }

  return 0;
}

} // end of namespace GLTF2
} // end of namespace BABYLON
