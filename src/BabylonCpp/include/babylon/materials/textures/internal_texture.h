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
  static constexpr unsigned int DATASOURCE_CUBELOD           = 8;
  static constexpr unsigned int DATASOURCE_CUBERAW           = 9;

public:
  InternalTexture(Engine* engine,
                  unsigned int dataSource
                  = InternalTexture::DATASOURCE_UNKNOWN);
  ~InternalTexture();

  unsigned int dataSource() const;
  void incrementReferences();
  void updateSize(int width, int height);
  void _rebuild();
  void dispose();

public:
  bool isReady;
  bool isCube;
  string_t url;
  unsigned int samplingMode;
  bool generateMipMaps;
  unsigned int samples;
  unsigned int type;
  unsigned int format;
  Observable<InternalTexture> onLoadedObservable;
  int width;
  int height;
  int baseWidth;
  int baseHeight;
  bool invertY;

  int _size;
  ICanvas* _workingCanvas;
  ICanvasRenderingContext2D* _workingContext;
  unique_ptr_t<GL::IGLFramebuffer> _framebuffer;
  unique_ptr_t<GL::IGLRenderbuffer> _depthStencilBuffer;
  unique_ptr_t<GL::IGLFramebuffer> _MSAAFramebuffer;
  unique_ptr_t<GL::IGLRenderbuffer> _MSAARenderBuffer;
  unsigned int _cachedCoordinatesMode;
  unsigned int _cachedWrapU;
  unsigned int _cachedWrapV;
  bool _isDisabled;
  bool _generateStencilBuffer;
  bool _generateDepthBuffer;
  unique_ptr_t<SphericalPolynomial> _sphericalPolynomial;
  // The following three fields helps sharing generated fixed LODs for texture
  // filtering
  // In environment not supporting the textureLOD extension like EDGE. They are
  // for internal use only.
  // They are at the level of the gl texture to benefit from the cache.
  BaseTexture* _lodTextureHigh;
  BaseTexture* _lodTextureMid;
  BaseTexture* _lodTextureLow;

  unique_ptr_t<GL::IGLTexture> _webGLTexture;
  int _references;

  Variant<ArrayBuffer, Image> _buffer;

private:
  unsigned int _dataSource;
  Engine* _engine;

}; // end of struct InternalTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_INTERNAL_TEXTURE_H
