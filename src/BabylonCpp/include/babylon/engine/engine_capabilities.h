#ifndef BABYLON_ENGINE_ENGINE_CAPABILITIES_H
#define BABYLON_ENGINE_ENGINE_CAPABILITIES_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT AnisotropicFilterExtension {
  static unsigned int TEXTURE_MAX_ANISOTROPY_EXT;
}; // end of struct TextureAnisotropicFilterExtension

struct BABYLON_SHARED_EXPORT InstancedArrays {
  ::std::function<void(unsigned int, int)> vertexAttribDivisorANGLE;
  ::std::function<void(unsigned int mode, int indexCount,
                       unsigned int indexFormat, unsigned int start,
                       int instancesCount)>
    drawElementsInstancedANGLE;
  ::std::function<void(unsigned int mode, int verticesStart, int verticesCount,
                       int instancesCount)>
    drawArraysInstancedANGLE;
}; // end of struct InstancedArrays

/**
 * @brief Class used to describe the capabilities of the engine relatively to
 * the current browser.
 */
struct BABYLON_SHARED_EXPORT EngineCapabilities {
  /** Maximum textures units per fragment shader */
  int maxTexturesImageUnits;
  /** Maximum texture units per vertex shader */
  int maxVertexTextureImageUnits;
  /** Maximum textures units in the entire pipeline */
  int maxCombinedTexturesImageUnits;
  /** Maximum texture size */
  int maxTextureSize;
  /** Maximum cube texture size */
  int maxCubemapTextureSize;
  /** Maximum render texture size */
  int maxRenderTextureSize;
  /** Maximum number of vertex attributes */
  int maxVertexAttribs;
  /** Maximum number of varyings */
  int maxVaryingVectors;
  /** Maximum number of uniforms per vertex shader */
  int maxVertexUniformVectors;
  /** Maximum number of uniforms per fragment shader */
  int maxFragmentUniformVectors;
  /** Defines if standard derivates (dx/dy) are supported */
  bool standardDerivatives;
  /** Defines if s3tc texture compression is supported */
  Nullable<GL::WEBGL_compressed_texture_s3tc> s3tc;
  /** Defines if pvrtc texture compression is supported */
  GL::any pvrtc; // WEBGL_compressed_texture_pvrtc;
  /** Defines if etc1 texture compression is supported */
  GL::any etc1; // WEBGL_compressed_texture_etc1;
  /** Defines if etc2 texture compression is supported */
  GL::any etc2; // WEBGL_compressed_texture_etc;
  /** Defines if astc texture compression is supported */
  GL::any astc; // WEBGL_compressed_texture_astc;
  /** Defines if float textures are supported */
  bool textureFloat;
  /** Defines if vertex array objects are supported */
  bool vertexArrayObject = false;
  /**
   * Gets the webgl extension for anisotropic filtering (null if not supported)
   */
  Nullable<GL::EXT_texture_filter_anisotropic>
    textureAnisotropicFilterExtension;
  /** Gets the maximum level of anisotropy supported */
  unsigned int maxAnisotropy;
  /** Defines if instancing is supported */
  bool instancedArrays;
  /** Defines if 32 bits indices are supported */
  bool uintIndices;
  /** Defines if high precision shaders are supported */
  bool highPrecisionShaderSupported;
  /** Defines if depth reading in the fragment shader is supported */
  bool fragmentDepthSupported;
  /** Defines if float texture linear filtering is supported*/
  bool textureFloatLinearFiltering;
  /** Defines if rendering to float textures is supported */
  bool textureFloatRender;
  /** Defines if half float textures are supported*/
  bool textureHalfFloat;
  /** Defines if half float texture linear filtering is supported*/
  bool textureHalfFloatLinearFiltering;
  /** Defines if rendering to half float textures is supported */
  bool textureHalfFloatRender;
  /** Defines if textureLOD shader command is supported */
  bool textureLOD;
  /** Defines if draw buffers extension is supported */
  bool drawBuffersExtension;
  /** Defines if depth textures are supported */
  bool depthTextureExtension;
  /** Defines if float color buffer are supported */
  bool colorBufferFloat;
  /** Gets disjoint timer query extension (null if not supported) */
  GL::EXT_disjoint_timer_query* timerQuery = nullptr;
  /** Defines if timestamp can be used with timer query */
  bool canUseTimestampForTimerQuery;
}; // end of struct EngineCapabilities

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_ENGINE_CAPABILITIES_H
