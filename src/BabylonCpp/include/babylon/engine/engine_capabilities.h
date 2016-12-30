#ifndef BABYLON_ENGINE_ENGINE_CAPABILITIES_H
#define BABYLON_ENGINE_ENGINE_CAPABILITIES_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT AnisotropicFilterExtension {
  static unsigned int TEXTURE_MAX_ANISOTROPY_EXT;
}; // end of struct TextureAnisotropicFilterExtension

struct BABYLON_SHARED_EXPORT InstancedArrays {
  std::function<void(unsigned int, int)> vertexAttribDivisorANGLE;
  std::function<void(unsigned int mode, int indexCount,
                     unsigned int indexFormat, unsigned int start,
                     int instancesCount)>
    drawElementsInstancedANGLE;
  std::function<void(unsigned int mode, int verticesStart, int verticesCount,
                     int instancesCount)>
    drawArraysInstancedANGLE;
}; // end of struct InstancedArrays

struct BABYLON_SHARED_EXPORT EngineCapabilities {
  int maxTexturesImageUnits;
  int maxTextureSize;
  int maxCubemapTextureSize;
  int maxRenderTextureSize;
  int maxVertexAttribs;
  bool standardDerivatives;
  GL::any s3tc;
  bool textureFloat;
  bool textureAnisotropicFilterExtension;
  unsigned int maxAnisotropy;
  InstancedArrays* instancedArrays;
  bool uintIndices;
  bool highPrecisionShaderSupported;
  bool fragmentDepthSupported;
  bool textureFloatLinearFiltering;
  bool textureFloatRender;
  bool textureHalfFloat;
  bool textureHalfFloatLinearFiltering;
  bool textureHalfFloatRender;
  bool textureLOD;
  int drawBuffersExtension;
}; // end of struct EngineCapabilities

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_ENGINE_CAPABILITIES_H
