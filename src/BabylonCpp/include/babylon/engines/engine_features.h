#ifndef BABYLON_ENGINES_ENGINE_FEATURES_H
#define BABYLON_ENGINES_ENGINE_FEATURES_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT EngineFeatures {
  /** Force using Bitmap when Bitmap or HTMLImageElement can be used */
  bool forceBitmapOverHTMLImageElement;

  /** Indicates that the engine support rendering to as well as copying to lod float textures */
  bool supportRenderAndCopyToLodForFloatTextures;

  /** Indicates that the engine support handling depth/stencil textures */
  bool supportDepthStencilTexture;

  /** Indicates that the engine support shadow samplers */
  bool supportShadowSamplers;

  /** Indicates to check the matrix bytes per bytes to know if it has changed or not. If false, only
   * the updateFlag of the matrix is checked */
  bool uniformBufferHardCheckMatrix;

  /** Indicates that prefiltered mipmaps can be generated in some processes (for eg when loading an
   * HDR cube texture) */
  bool allowTexturePrefiltering;

  /** Indicates to track the usage of ubos and to create new ones as necessary during a frame
   * duration */
  bool trackUbosInFrame;

  /** Indicates that the Cascaded Shadow Map technic is supported */
  bool supportCSM;

  /** Indicates that the textures transcoded by the basis transcoder must have power of 2 width and
   * height */
  bool basisNeedsPOT;

  /** Indicates that the engine supports 3D textures */
  bool support3DTextures;

  /** Indicates that constants need a type suffix in shaders (used by realtime filtering...) */
  bool needTypeSuffixInShaderConstants;

  /** Indicates that MSAA is supported */
  bool supportMSAA;

  /** Indicates that SSAO2 is supported */
  bool supportSSAO2;

  /** Indicates that some additional texture formats are supported (like TEXTUREFORMAT_R for eg) */
  bool supportExtendedTextureFormats;

  /** Indicates that the switch/case construct is supported in shaders */
  bool supportSwitchCaseInShader;

  /** Indicates that synchronous texture reading is supported */
  bool supportSyncTextureRead;

  /** Indicates that y should be inverted when dealing with bitmaps (notably in environment tools)
   */
  bool needsInvertingBitmap;

  /** @hidden */
  bool _collectUbosUpdatedInFrame;
}; // end of struct EngineFeatures

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_ENGINE_FEATURES_H
