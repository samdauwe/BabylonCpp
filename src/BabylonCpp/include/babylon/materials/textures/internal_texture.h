#ifndef BABYLON_MATERIALS_TEXTURES_INTERNAL_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_INTERNAL_TEXTURE_H

#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/core/structs.h>
#include <babylon/materials/textures/hardware_texture_wrapper.h>
#include <babylon/materials/textures/iinternal_texture_tracker.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class ICanvasRenderingContext2D;
class ThinEngine;
FWD_CLASS_SPTR(BaseTexture)
FWD_CLASS_SPTR(InternalTexture)
FWD_CLASS_SPTR(SphericalPolynomial)

namespace GL {
class IGLFramebuffer;
class IGLRenderbuffer;
class IGLTexture;
} // namespace GL

using WebGLFramebufferPtr       = std::shared_ptr<GL::IGLFramebuffer>;
using WebGLRenderbufferPtr      = std::shared_ptr<GL::IGLRenderbuffer>;
using WebGLTexturePtr           = std::shared_ptr<GL::IGLTexture>;
using HardwareTextureWrapperPtr = std::shared_ptr<HardwareTextureWrapper<WebGLTexturePtr>>;

/**
 * @brief Class used to store data associated with WebGL texture data for the engine This class
 * should not be used directly.
 */
class BABYLON_SHARED_EXPORT InternalTexture : public std::enable_shared_from_this<InternalTexture>,
                                              public IInternalTextureTracker {

public:
  template <typename... Ts>
  static InternalTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<InternalTexture>(new InternalTexture(std::forward<Ts>(args)...));

    return texture;
  }
  ~InternalTexture(); // = default

  /**
   * @brief Gets the unique id of the internal texture.
   */
  size_t uniqueId() const;

  /**
   * @brief Gets the Engine the texture belongs to.
   * @returns The babylon engine
   */
  ThinEngine* getEngine();

  /**
   * @brief Gets the data source type of the texture.
   */
  InternalTextureSource dataSource() const;

  /**
   * @brief Increments the number of references (ie. the number of Texture that point to it).
   */
  void incrementReferences();

  /**
   * @brief Change the size of the texture (not the size of the content).
   * @param width defines the new width
   * @param height defines the new height
   * @param depth defines the new depth (1 by default)
   */
  void updateSize(int width, int height, int depth = 1);

  /**
   * @brief Rebuilds the internal texture.
   */
  void _rebuild();

  /**
   * @brief Hidden
   */
  void _swapAndDie(const InternalTexturePtr& target);

  /**
   * @brief Dispose the current allocated resources.
   */
  void dispose();

protected:
  /**
   * @brief Creates a new InternalTexture.
   * @param engine defines the engine to use
   * @param source defines the type of data that will be used
   * @param delayAllocation if the texture allocation should be delayed (default: false)
   */
  InternalTexture(ThinEngine* engine, InternalTextureSource source = InternalTextureSource::Unknown,
                  bool delayAllocation = false);

public:
  /**
   * Defines if the texture is ready
   */
  bool isReady;
  /**
   * Defines if the texture is a cube texture
   */
  bool isCube;
  /**
   * Defines if the texture contains 3D data
   */
  bool is3D;
  /**
   * Defines if the texture contains 2D array data
   */
  bool is2DArray;
  /**
   * Defines if the texture contains multiview data
   */
  bool isMultiview;
  /**
   * Gets the URL used to load this texture
   */
  std::string url;
  /**
   * @hidden
   */
  std::string _originalUrl; // not empty only if different from url
  /**
   * Gets the sampling mode of the texture
   */
  unsigned int samplingMode;
  /**
   * Gets a boolean indicating if the texture needs mipmaps generation
   */
  bool generateMipMaps;
  /**
   * Gets the number of samples used by the texture (WebGL2+ only)
   */
  unsigned int samples;
  /**
   * Gets the type of the texture (int, float...)
   */
  unsigned int type;
  /**
   * Gets the format of the texture (RGB, RGBA...)
   */
  unsigned int format;
  /**
   * Observable called when the texture is loaded
   */
  Observable<InternalTexture> onLoadedObservable;
  /**
   * Gets the width of the texture
   */
  int width;
  /**
   * Gets the height of the texture
   */
  int height;
  /**
   * Gets the depth of the texture
   */
  int depth;
  /**
   * Gets the initial width of the texture (It could be rescaled if the current system does not
   * support non power of two textures)
   */
  int baseWidth;
  /**
   * Gets the initial height of the texture (It could be rescaled if the current system does not
   * support non power of two textures)
   */
  int baseHeight;
  /**
   * Gets the initial depth of the texture (It could be rescaled if the current system does not
   * support non power of two textures)
   */
  int baseDepth;
  /**
   * Gets a boolean indicating if the texture is inverted on Y axis
   */
  bool invertY;

  // Private
  /** @hidden */
  bool _invertVScale;
  /** @hidden */
  int _associatedChannel;
  /** @hidden */
  InternalTextureSource _source;
  /** @hidden */
  std::variant<std::string, ArrayBuffer, ArrayBufferView, Image> _buffer;
  /** @hidden */
  ArrayBuffer _bufferView;
  /** @hidden */
  std::vector<ArrayBufferView> _bufferViewArray;
  /** @hidden */
  std::vector<std::vector<ArrayBuffer>> _bufferViewArrayArray;
  /** @hidden */
  int _size;
  /** @hidden */
  std::string _extension;
  /** @hidden */
  std::vector<std::string> _files;
  /** @hidden */
  ICanvas* _workingCanvas;
  /** @hidden */
  ICanvasRenderingContext2D* _workingContext;
  /** @hidden */
  WebGLFramebufferPtr _framebuffer;
  /** @hidden */
  WebGLRenderbufferPtr _depthStencilBuffer;
  /** @hidden */
  WebGLFramebufferPtr _MSAAFramebuffer;
  /** @hidden */
  WebGLRenderbufferPtr _MSAARenderBuffer;
  /** @hidden */
  Uint32Array _attachments;
  /** @hidden */
  std::vector<InternalTexturePtr> _textureArray;
  /** @hidden */
  int _cachedCoordinatesMode;
  /** @hidden */
  int _cachedWrapU;
  /** @hidden */
  int _cachedWrapV;
  /** @hidden */
  int _cachedWrapR;
  /** @hidden */
  int _cachedAnisotropicFilteringLevel;
  /** @hidden */
  bool _isDisabled;
  /** @hidden */
  std::string _compression;
  /** @hidden */
  bool _generateStencilBuffer;
  /** @hidden */
  bool _generateDepthBuffer;
  /** @hidden */
  int _comparisonFunction;
  /** @hidden */
  SphericalPolynomialPtr _sphericalPolynomial;
  /** @hidden */
  bool _sphericalPolynomialComputed;
  /** @hidden */
  float _lodGenerationScale;
  /** @hidden */
  float _lodGenerationOffset;
  /** @hidden */
  InternalTexturePtr _depthStencilTexture;

  // Multiview
  /** Hidden */
  WebGLTexturePtr _colorTextureArray;
  /** Hidden */
  WebGLTexturePtr _depthStencilTextureArray;

  // The following three fields helps sharing generated fixed LODs for texture filtering
  // In environment not supporting the textureLOD extension like EDGE. They are for internal use
  // only. They are at the level of the gl texture to benefit from the cache.
  BaseTexturePtr _lodTextureHigh;
  BaseTexturePtr _lodTextureMid;
  BaseTexturePtr _lodTextureLow;
  bool _isRGBD;

  /** Hidden */
  bool _linearSpecularLOD;
  /** Hidden */
  BaseTexturePtr _irradianceTexture;

  /** @hidden */
  HardwareTextureWrapperPtr _hardwareTexture;

  int _references;

  /** @hidden */
  std::optional<bool> _gammaSpace;

private:
  ThinEngine* _engine;
  size_t _uniqueId;

  static size_t _Counter;

}; // end of struct InternalTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_INTERNAL_TEXTURE_H
