#include <babylon/materials/textures/internal_texture.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/depth_texture_creation_options.h>
#include <babylon/engines/engine.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/iinternal_texture_loader.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/maths/spherical_polynomial.h>

namespace BABYLON {

InternalTexture::InternalTexture(Engine* engine, unsigned int dataSource,
                                 bool delayAllocation)
    : isReady{false}
    , isCube{false}
    , is3D{false}
    , isMultiview{false}
    , url{""}
    , generateMipMaps{false}
    , samples{0}
    , width{0}
    , height{0}
    , depth{0}
    , baseWidth{0}
    , baseHeight{0}
    , baseDepth{0}
    , invertY{false}
    , _invertVScale{false}
    , _associatedChannel{-1}
    , _dataSource{dataSource}
    , _size{0}
    , _extension{""}
    , _workingCanvas{nullptr}
    , _workingContext{nullptr}
    , _framebuffer{nullptr}
    , _depthStencilBuffer{nullptr}
    , _MSAAFramebuffer{nullptr}
    , _MSAARenderBuffer{nullptr}
    , _cachedCoordinatesMode{-1}
    , _cachedWrapU{-1}
    , _cachedWrapV{-1}
    , _cachedWrapR{-1}
    , _cachedAnisotropicFilteringLevel{-1}
    , _isDisabled{false}
    , _compression{""}
    , _generateStencilBuffer{false}
    , _generateDepthBuffer{false}
    , _comparisonFunction{0}
    , _sphericalPolynomial{nullptr}
    , _lodGenerationScale{0}
    , _lodGenerationOffset{0}
    , _colorTextureArray{nullptr}
    , _depthStencilTextureArray{nullptr}
    , _lodTextureHigh{nullptr}
    , _lodTextureMid{nullptr}
    , _lodTextureLow{nullptr}
    , _isRGBD{false}
    , _linearSpecularLOD{false}
    , _irradianceTexture{nullptr}
    , _webGLTexture{nullptr}
    , _references{1}
    , _engine{engine}
{
  previous = nullptr;
  next     = nullptr;

  if (!delayAllocation) {
    _webGLTexture = engine->_createTexture();
  }
}

InternalTexture::~InternalTexture() = default;

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
  InternalTexturePtr proxy;
  isReady                          = false;
  _cachedCoordinatesMode           = -1;
  _cachedWrapU                     = -1;
  _cachedWrapV                     = -1;
  _cachedWrapR                     = -1;
  _cachedAnisotropicFilteringLevel = -1;

  switch (_dataSource) {
    case InternalTexture::DATASOURCE_TEMP:
      return;
    case InternalTexture::DATASOURCE_URL: {
      proxy = _engine->createTexture(
        url, !generateMipMaps, invertY, nullptr, samplingMode,
        [this](InternalTexture*, EventState&) { isReady = true; }, nullptr,
        _buffer, nullptr, format);
      proxy->_swapAndDie(shared_from_this());
    }
      return;
    case InternalTexture::DATASOURCE_RAW: {
      proxy = _engine->createRawTexture(_bufferView, baseWidth, baseHeight,
                                        format, generateMipMaps, invertY,
                                        samplingMode, _compression);
      proxy->_swapAndDie(shared_from_this());
      isReady = true;
    }
      return;
    case InternalTexture::DATASOURCE_RAW3D: {
      proxy = _engine->createRawTexture3D(_bufferView, baseWidth, baseHeight,
                                          baseDepth, format, generateMipMaps,
                                          invertY, samplingMode, _compression);
      proxy->_swapAndDie(shared_from_this());
      isReady = true;
    }
      return;
    case InternalTexture::DATASOURCE_DYNAMIC: {
      proxy = _engine->createDynamicTexture(baseWidth, baseHeight,
                                            generateMipMaps, samplingMode);
      proxy->_swapAndDie(shared_from_this());
      // _engine->updateDynamicTexture(this, _engine->getRenderingCanvas(),
      // invertY, undefined, undefined, true);

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
      proxy->_swapAndDie(shared_from_this());

      isReady = true;
    }
      return;
    case InternalTexture::DATASOURCE_DEPTHTEXTURE: {
      DepthTextureCreationOptions depthTextureOptions;
      depthTextureOptions.bilinearFiltering
        = samplingMode != Constants::TEXTURE_BILINEAR_SAMPLINGMODE;
      depthTextureOptions.comparisonFunction = _comparisonFunction;
      depthTextureOptions.generateStencil    = _generateStencilBuffer;
      depthTextureOptions.isCube             = isCube;

      auto size = ISize{width, height};
      // TODO FIXME
      proxy = _engine->createDepthStencilTexture(size, depthTextureOptions);
      proxy->_swapAndDie(shared_from_this());

      isReady = true;
    }
      return;
    case InternalTexture::DATASOURCE_CUBE: {
      proxy = _engine->createCubeTexture(
        url, nullptr, _files, !generateMipMaps,
        [this](const std::optional<CubeTextureData>& /*data*/) {
          isReady = true;
        },
        nullptr, format, _extension);
      proxy->_swapAndDie(shared_from_this());
    }
      return;
    case InternalTexture::DATASOURCE_CUBERAW: {
      proxy = _engine->createRawCubeTexture(_bufferViewArray, width, format,
                                            type, generateMipMaps, invertY,
                                            samplingMode, _compression);
      proxy->_swapAndDie(shared_from_this());
      isReady = true;
    }
      return;
    case InternalTexture::DATASOURCE_CUBERAW_RGBD: {
      proxy = _engine->createRawCubeTexture({}, width, format, type,
                                            generateMipMaps, invertY,
                                            samplingMode, _compression);
#if 0
      InternalTexture::_UpdateRGBDAsync(
        proxy, _bufferViewArrayArray !, _sphericalPolynomial,
        _lodGenerationScale, _lodGenerationOffset)
        .then(() = > { isReady = true; });
#endif
      proxy->_swapAndDie(shared_from_this());
    }
      return;
    case InternalTexture::DATASOURCE_CUBEPREFILTERED: {
      proxy = _engine->createPrefilteredCubeTexture(
        url, nullptr, _lodGenerationScale, _lodGenerationOffset,
        [this, &proxy](const std::optional<CubeTextureData>& /*data*/) {
          if (proxy) {
            proxy->_swapAndDie(shared_from_this());
          }
          isReady = true;
        },
        nullptr, format, _extension);
      proxy->_sphericalPolynomial
        = std::unique_ptr<SphericalPolynomial>(&*_sphericalPolynomial);
    }
      return;
  }
}

void InternalTexture::_swapAndDie(const InternalTexturePtr& target)
{
  target->_webGLTexture = _webGLTexture;
  target->_isRGBD       = _isRGBD;

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

  if (_irradianceTexture) {
    if (target->_irradianceTexture) {
      target->_irradianceTexture->dispose();
    }
    target->_irradianceTexture = _irradianceTexture;
  }

  auto& cache = _engine->getLoadedTexturesCache();
  stl_util::remove_vector_elements_equal_sharedptr(cache, this);

  auto it = std::find(cache.begin(), cache.end(), target);
  if (it == cache.end()) {
    cache.emplace_back(target);
  }
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
  }
}

} // end of namespace BABYLON
