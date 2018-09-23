#include <babylon/materials/textures/internal_texture.h>

#include <babylon/engine/depth_texture_creation_options.h>
#include <babylon/engine/engine.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/math/spherical_polynomial.h>

namespace BABYLON {

constexpr unsigned int InternalTexture::DATASOURCE_UNKNOWN;
constexpr unsigned int InternalTexture::DATASOURCE_URL;
constexpr unsigned int InternalTexture::DATASOURCE_TEMP;
constexpr unsigned int InternalTexture::DATASOURCE_RAW;
constexpr unsigned int InternalTexture::DATASOURCE_DYNAMIC;
constexpr unsigned int InternalTexture::DATASOURCE_RENDERTARGET;
constexpr unsigned int InternalTexture::DATASOURCE_MULTIRENDERTARGET;
constexpr unsigned int InternalTexture::DATASOURCE_CUBE;
constexpr unsigned int InternalTexture::DATASOURCE_CUBERAW;
constexpr unsigned int InternalTexture::DATASOURCE_CUBEPREFILTERED;
constexpr unsigned int InternalTexture::DATASOURCE_RAW3D;
constexpr unsigned int InternalTexture::DATASOURCE_DEPTHTEXTURE;
constexpr unsigned int InternalTexture::DATASOURCE_CUBERAW_RGBD;

InternalTexture::InternalTexture(Engine* engine, unsigned int dataSource)
    : isCube{false}
    , is3D{false}
    , _initialSlot{-1}
    , _designatedSlot{-1}
    , _dataSource{dataSource}
    , _cachedWrapU{0}
    , _cachedWrapV{0}
    , _cachedWrapR{0}
    , _comparisonFunction{0}
    , _sphericalPolynomial{nullptr}
    , _lodGenerationScale{0}
    , _lodGenerationOffset{0}
    , _lodTextureHigh{nullptr}
    , _lodTextureMid{nullptr}
    , _lodTextureLow{nullptr}
    , _isRGBD{false}
    , _references{1}
    , _engine{engine}
{
  previous = nullptr;
  next     = nullptr;

  _webGLTexture = engine->_createTexture();
}

InternalTexture::~InternalTexture()
{
}

Engine* InternalTexture::getEngine()
{
  return _engine;
}

unsigned int InternalTexture::dataSource() const
{
  return _dataSource;
}

void InternalTexture::incrementReferences()
{
  ++_references;
}

void InternalTexture::updateSize(int iWidth, int iHeight, int iDepth)
{
  width  = iWidth;
  height = iHeight;
  depth  = iDepth;

  baseWidth  = iWidth;
  baseHeight = iHeight;
  baseDepth  = iDepth;

  _size = width * height * depth;
}

void InternalTexture::_rebuild()
{
  InternalTexture* proxy;
  isReady                          = false;
  _cachedCoordinatesMode           = 0;
  _cachedWrapU                     = 0;
  _cachedWrapV                     = 0;
  _cachedWrapR                     = 0;
  _cachedAnisotropicFilteringLevel = 0;

  switch (_dataSource) {
    case InternalTexture::DATASOURCE_TEMP:
      return;
    case InternalTexture::DATASOURCE_URL: {
      proxy = _engine->createTexture(
        url, !generateMipMaps, invertY, nullptr, samplingMode,
        [this](InternalTexture*, EventState&) { isReady = true; }, nullptr,
        _buffer, nullptr, format);
      proxy->_swapAndDie(this);
    }
      return;
    case InternalTexture::DATASOURCE_RAW: {
      proxy = _engine->createRawTexture(_bufferView, baseWidth, baseHeight,
                                        format, generateMipMaps, invertY,
                                        samplingMode, _compression);
      proxy->_swapAndDie(this);
      isReady = true;
    }
      return;
    case InternalTexture::DATASOURCE_RAW3D: {
      proxy = _engine->createRawTexture3D(_bufferView, baseWidth, baseHeight,
                                          baseDepth, format, generateMipMaps,
                                          invertY, samplingMode, _compression);
      proxy->_swapAndDie(this);
      isReady = true;
    }
      return;
    case InternalTexture::DATASOURCE_DYNAMIC: {
      proxy = _engine->createDynamicTexture(baseWidth, baseHeight,
                                            generateMipMaps, samplingMode);
      proxy->_swapAndDie(this);
      // The engine will make sure to update content so no need to flag it as
      // isReady = true
    }
      return;
    case InternalTexture::DATASOURCE_RENDERTARGET: {
      RenderTargetCreationOptions options;
      options.generateDepthBuffer   = _generateDepthBuffer;
      options.generateMipMaps       = generateMipMaps;
      options.generateStencilBuffer = _generateStencilBuffer;
      options.samplingMode          = samplingMode;
      options.type                  = type;

      if (isCube) {
        auto size = ISize{width, width};
        proxy     = _engine->createRenderTargetCubeTexture(size, options);
      }
      else {
        auto size = ISize{width, height};
        proxy     = _engine->createRenderTargetTexture(size, options);
      }
      proxy->_swapAndDie(this);

      isReady = true;
    }
      return;
    case InternalTexture::DATASOURCE_DEPTHTEXTURE: {
      DepthTextureCreationOptions depthTextureOptions;
      depthTextureOptions.bilinearFiltering
        = samplingMode != TextureConstants::BILINEAR_SAMPLINGMODE;
      depthTextureOptions.comparisonFunction = _comparisonFunction;
      depthTextureOptions.generateStencil    = _generateStencilBuffer;
      depthTextureOptions.isCube             = isCube;

      auto size = ISize{width, height};
      // TODO FIXME
      proxy = _engine
                ->createDepthStencilTexture(ToVariant<int, ISize>(size),
                                            depthTextureOptions)
                .get();
      proxy->_swapAndDie(this);

      isReady = true;
    }
      return;
    case InternalTexture::DATASOURCE_CUBE: {
      proxy = _engine->createCubeTexture(
        url, nullptr, _files, !generateMipMaps,
        [this](InternalTexture*, EventState&) { isReady = true; }, nullptr,
        format, _extension);
      proxy->_swapAndDie(this);
    }
      return;
    case InternalTexture::DATASOURCE_CUBERAW: {
      proxy = _engine->createRawCubeTexture(_bufferViewArray, width, format,
                                            type, generateMipMaps, invertY,
                                            samplingMode, _compression);
      proxy->_swapAndDie(this);
      isReady = true;
    }
      return;
    case InternalTexture::DATASOURCE_CUBERAW_RGBD: {
      proxy = _engine->createRawCubeTexture({}, width, format, type,
                                            generateMipMaps, invertY,
                                            samplingMode, _compression);
#if 0
      RawCubeTexture::_UpdateRGBDAsync(
        proxy, _bufferViewArrayArray !, _sphericalPolynomial,
        _lodGenerationScale, _lodGenerationOffset)
        .then(() = > { isReady = true; });
#endif
      proxy->_swapAndDie(this);
    }
      return;
    case InternalTexture::DATASOURCE_CUBEPREFILTERED: {
      proxy = _engine->createPrefilteredCubeTexture(
        url, nullptr, _lodGenerationScale, _lodGenerationOffset,
        [this, &proxy](InternalTexture*, EventState&) {
          if (proxy) {
            proxy->_swapAndDie(this);
          }
          isReady = true;
        },
        nullptr, format, _extension);
      proxy->_sphericalPolynomial
        = ::std::unique_ptr<SphericalPolynomial>(&*_sphericalPolynomial);
    }
      return;
  }
}

void InternalTexture::_swapAndDie(InternalTexture* target)
{
  target->_webGLTexture = _webGLTexture;

  if (_framebuffer) {
    target->_framebuffer = _framebuffer;
  }

  if (_depthStencilBuffer) {
    target->_depthStencilBuffer = _depthStencilBuffer;
  }

  if (_lodTextureHigh) {
    if (target->_lodTextureHigh) {
      target->_lodTextureHigh->dispose();
    }
    target->_lodTextureHigh = _lodTextureHigh;
  }

  if (_lodTextureMid) {
    if (target->_lodTextureMid) {
      target->_lodTextureMid->dispose();
    }
    target->_lodTextureMid = _lodTextureMid;
  }

  if (_lodTextureLow) {
    if (target->_lodTextureLow) {
      target->_lodTextureLow->dispose();
    }
    target->_lodTextureLow = _lodTextureLow;
  }

  auto& cache = _engine->getLoadedTexturesCache();
  cache.erase(
    ::std::remove_if(
      cache.begin(), cache.end(),
      [this](const std::unique_ptr<InternalTexture>& internalTexture) {
        return internalTexture.get() == this;
      }),
    cache.end());
}

void InternalTexture::dispose()
{
  if (!_webGLTexture) {
    return;
  }

  --_references;
  if (_references == 0) {
    _engine->_releaseTexture(this);
    _webGLTexture = nullptr;
    previous      = nullptr;
    next          = nullptr;
  }
}

} // end of namespace BABYLON
