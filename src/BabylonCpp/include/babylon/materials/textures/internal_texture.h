#ifndef BABYLON_MATERIALS_TEXTURES_INTERNAL_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_INTERNAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/core/variant.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

namespace GL {
class IGLFramebuffer;
class IGLRenderbuffer;
class IGLTexture;
} // namespace GL

class BABYLON_SHARED_EXPORT InternalTexture {

public:
  static constexpr unsigned int DATASOURCE_UNKNOWN           = 0;
  static constexpr unsigned int DATASOURCE_URL               = 1;
  static constexpr unsigned int DATASOURCE_TEMP              = 2;
  static constexpr unsigned int DATASOURCE_RAW               = 3;
  static constexpr unsigned int DATASOURCE_DYNAMIC           = 4;
  static constexpr unsigned int DATASOURCE_RENDERTARGET      = 5;
  static constexpr unsigned int DATASOURCE_MULTIRENDERTARGET = 6;
  static constexpr unsigned int DATASOURCE_CUBE              = 7;
  static constexpr unsigned int DATASOURCE_CUBERAW           = 8;
  static constexpr unsigned int DATASOURCE_CUBEPREFILTERED   = 9;
  static constexpr unsigned int DATASOURCE_RAW3D             = 10;

public:
  InternalTexture(Engine* engine, unsigned int dataSource
                                  = InternalTexture::DATASOURCE_UNKNOWN);
  ~InternalTexture();

  unsigned int dataSource() const;
  void incrementReferences();
  void updateSize(int width, int height, int depth = 1);
  void _rebuild();
  void dispose();

private:
  void _swapAndDie(InternalTexture* target);

public:
  bool isReady;
  bool isCube;
  bool is3D;
  string_t url;
  unsigned int samplingMode;
  bool generateMipMaps;
  unsigned int samples;
  unsigned int type;
  unsigned int format;
  Observable<InternalTexture> onLoadedObservable;
  int width;
  int height;
  int depth;
  int baseWidth;
  int baseHeight;
  int baseDepth;
  bool invertY;

  // Private
  unsigned int _dataSource;
  Variant<ArrayBuffer, Image> _buffer;
  ArrayBuffer _bufferView;
  vector_t<ArrayBuffer> _bufferViewArray;
  int _size;
  string_t _extension;
  vector_t<string_t> _files;
  ICanvas* _workingCanvas;
  ICanvasRenderingContext2D* _workingContext;
  shared_ptr_t<GL::IGLFramebuffer> _framebuffer;
  shared_ptr_t<GL::IGLRenderbuffer> _depthStencilBuffer;
  unique_ptr_t<GL::IGLFramebuffer> _MSAAFramebuffer;
  unique_ptr_t<GL::IGLRenderbuffer> _MSAARenderBuffer;
  unsigned int _cachedCoordinatesMode;
  unsigned int _cachedWrapU;
  unsigned int _cachedWrapV;
  unsigned int _cachedWrapR;
  unsigned int _cachedAnisotropicFilteringLevel;
  bool _isDisabled;
  string_t _compression;
  bool _generateStencilBuffer;
  bool _generateDepthBuffer;
  unique_ptr_t<SphericalPolynomial> _sphericalPolynomial;
  float _lodGenerationScale;
  float _lodGenerationOffset;
  // The following three fields helps sharing generated fixed LODs for texture
  // filtering
  // In environment not supporting the textureLOD extension like EDGE. They are
  // for internal use only.
  // They are at the level of the gl texture to benefit from the cache.
  BaseTexture* _lodTextureHigh;
  BaseTexture* _lodTextureMid;
  BaseTexture* _lodTextureLow;

  shared_ptr_t<GL::IGLTexture> _webGLTexture;
  int _references;

private:
  Engine* _engine;

}; // end of struct InternalTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_INTERNAL_TEXTURE_H
