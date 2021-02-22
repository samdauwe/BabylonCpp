#include <babylon/materials/textures/internal_texture.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/core/logging.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/depth_texture_creation_options.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/iinternal_texture_loader.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/materials/textures/render_target_creation_options.h>
#include <babylon/maths/spherical_polynomial.h>

namespace BABYLON {

size_t InternalTexture::_Counter = 0;

InternalTexture::InternalTexture(ThinEngine* engine, InternalTextureSource source,
                                 bool delayAllocation)
    : isReady{false}
    , isCube{false}
    , is3D{false}
    , is2DArray{false}
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
    , _source{source}
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
    , _sphericalPolynomialPromise{nullptr}
    , _sphericalPolynomialComputed{false}
    , _lodGenerationScale{0}
    , _lodGenerationOffset{0}
    , _depthStencilTexture{nullptr}
    , _colorTextureArray{nullptr}
    , _depthStencilTextureArray{nullptr}
    , _lodTextureHigh{nullptr}
    , _lodTextureMid{nullptr}
    , _lodTextureLow{nullptr}
    , _isRGBD{false}
    , _linearSpecularLOD{false}
    , _irradianceTexture{nullptr}
    , _hardwareTexture{nullptr}
    , _references{1}
    , _gammaSpace{std::nullopt}
    , _engine{engine}
{
  previous  = nullptr;
  next      = nullptr;
  _uniqueId = InternalTexture::_Counter++;

  if (!delayAllocation) {
    _hardwareTexture = engine->_createHardwareTexture();
  }
}

InternalTexture::~InternalTexture() = default;

size_t InternalTexture::uniqueId() const
{
  return _uniqueId;
}

ThinEngine* InternalTexture::getEngine()
{
  return _engine;
}

InternalTextureSource InternalTexture::dataSource() const
{
  return _source;
}

void InternalTexture::incrementReferences()
{
  ++_references;
}

void InternalTexture::updateSize(int iWidth, int iHeight, int iDepth)
{
  _engine->updateTextureDimensions(this, iWidth, iHeight, iDepth);

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

  switch (_source) {
    case InternalTextureSource::Temp:
      return;
    case InternalTextureSource::Url: {
      proxy = _engine->createTexture(
        !_originalUrl.empty() ? _originalUrl : url, !generateMipMaps, invertY, nullptr,
        samplingMode, [this](InternalTexture*, EventState&) { isReady = true; }, nullptr, _buffer,
        nullptr, format);
      proxy->_swapAndDie(shared_from_this());
    }
      return;
    case InternalTextureSource::Raw: {
      proxy = _engine->createRawTexture(_bufferView, baseWidth, baseHeight, format, generateMipMaps,
                                        invertY, samplingMode, _compression);
      proxy->_swapAndDie(shared_from_this());
      isReady = true;
    }
      return;
    case InternalTextureSource::Raw3D: {
      proxy = _engine->createRawTexture3D(_bufferView, baseWidth, baseHeight, baseDepth, format,
                                          generateMipMaps, invertY, samplingMode, _compression);
      proxy->_swapAndDie(shared_from_this());
      isReady = true;
    }
      return;
    case InternalTextureSource::Raw2DArray: {
      proxy
        = _engine->createRawTexture2DArray(_bufferView, baseWidth, baseHeight, baseDepth, format,
                                           generateMipMaps, invertY, samplingMode, _compression);
      proxy->_swapAndDie(shared_from_this());

      isReady = true;
    }
      return;
    case InternalTextureSource::Dynamic: {
      proxy = _engine->createDynamicTexture(baseWidth, baseHeight, generateMipMaps, samplingMode);
      proxy->_swapAndDie(shared_from_this());
      _engine->updateDynamicTexture(shared_from_this(), _engine->getRenderingCanvas(), invertY,
                                    false, std::nullopt, true);

      // The engine will make sure to update content so no need to flag it as isReady = true
    }
      return;
    case InternalTextureSource::RenderTarget: {
      IRenderTargetOptions options;
      options.generateDepthBuffer   = _generateDepthBuffer;
      options.generateMipMaps       = generateMipMaps;
      options.generateStencilBuffer = _generateStencilBuffer;
      options.samplingMode          = samplingMode;
      options.type                  = type;

      if (isCube) {
        const auto size = ISize{width, width};
        proxy           = _engine->createRenderTargetCubeTexture(size, options);
      }
      else {
        std::optional<int> layers = std::nullopt;
        if (is2DArray) {
          layers = depth;
        }
        const auto size = RenderTargetSize{
          width,  // width
          height, //  height
          layers  // layers
        };
        proxy = _engine->createRenderTargetTexture(size, options);
      }
      proxy->_swapAndDie(shared_from_this());

      isReady = true;
    }
      return;
    case InternalTextureSource::Depth: {
      DepthTextureCreationOptions depthTextureOptions;
      depthTextureOptions.bilinearFiltering
        = samplingMode != Constants::TEXTURE_BILINEAR_SAMPLINGMODE;
      depthTextureOptions.comparisonFunction = _comparisonFunction;
      depthTextureOptions.generateStencil    = _generateStencilBuffer;
      depthTextureOptions.isCube             = isCube;
      depthTextureOptions.samples            = samples;

      std::optional<int> layers = std::nullopt;
      if (is2DArray) {
        layers = depth;
      }
      const auto size = RenderTargetSize{
        width,  // width
        height, //  height
        layers  // layers
      };
      proxy = _engine->createDepthStencilTexture(size, depthTextureOptions);
      proxy->_swapAndDie(shared_from_this());

      isReady = true;
    }
      return;
    case InternalTextureSource::Cube: {
      proxy = _engine->createCubeTexture(
        url, nullptr, _files, !generateMipMaps,
        [this](const std::optional<CubeTextureData>& /*data*/) { isReady = true; }, nullptr, format,
        _extension);
      proxy->_swapAndDie(shared_from_this());
    }
      return;
    case InternalTextureSource::CubeRaw: {
      proxy = _engine->createRawCubeTexture(_bufferViewArray, width, format, type, generateMipMaps,
                                            invertY, samplingMode, _compression);
      proxy->_swapAndDie(shared_from_this());
      isReady = true;
    }
      return;
    case InternalTextureSource::CubeRawRGBD: {
      proxy = _engine->createRawCubeTexture({}, width, format, type, generateMipMaps, invertY,
                                            samplingMode, _compression);
#if 0
      InternalTexture::_UpdateRGBDAsync(
        proxy, _bufferViewArrayArray !, _sphericalPolynomial,
        _lodGenerationScale, _lodGenerationOffset)
        .then(() = > { isReady = true; });
#else
      BABYLON_LOG_WARN("InternalTexture",
                       "Rebuilding internal textures of type 'CubeRawRGBD' not yet implemented");
#endif
      proxy->_swapAndDie(shared_from_this());
    }
      return;
    case InternalTextureSource::CubePrefiltered: {
      proxy = _engine->createPrefilteredCubeTexture(
        url, nullptr, _lodGenerationScale, _lodGenerationOffset,
        [this, &proxy](const std::optional<CubeTextureData>& /*data*/) -> void {
          if (proxy) {
            proxy->_swapAndDie(shared_from_this());
          }
          isReady = true;
        },
        nullptr, format, _extension);
      proxy->_sphericalPolynomial = std::unique_ptr<SphericalPolynomial>(&*_sphericalPolynomial);
    }
      return;
    default:
      break;
  }
}

void InternalTexture::_swapAndDie(const InternalTexturePtr& target)
{
  // TODO what about refcount on target?

  _hardwareTexture->setUsage(target->_source, generateMipMaps, isCube, width, height);

  target->_hardwareTexture = _hardwareTexture;
  target->_isRGBD          = _isRGBD;

  if (_framebuffer) {
    target->_framebuffer = _framebuffer;
  }

  if (_depthStencilBuffer) {
    target->_depthStencilBuffer = _depthStencilBuffer;
  }

  target->_depthStencilTexture = _depthStencilTexture;

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
  --_references;
  if (_references == 0) {
    _engine->_releaseTexture(shared_from_this());
    _hardwareTexture = nullptr;
  }
}

} // end of namespace BABYLON
