#include <babylon/engines/extensions/cube_texture_extension.h>

#include <babylon/asio/asio.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/depth_texture_creation_options.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/materials/textures/iinternal_texture_loader.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/maths/isize.h>
#include <babylon/misc/file_tools.h>
#include <babylon/misc/ifile_request.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

CubeTextureExtension::CubeTextureExtension(ThinEngine* engine) : _this{engine}
{
}

CubeTextureExtension::~CubeTextureExtension() = default;

InternalTexturePtr
CubeTextureExtension::_createDepthStencilCubeTexture(int size,
                                                     const DepthTextureCreationOptions& options)
{
  auto internalTexture    = InternalTexture::New(_this, InternalTextureSource::Unknown);
  internalTexture->isCube = true;

  if (_this->webGLVersion() == 1.f) {
    BABYLON_LOG_ERROR("CubeTextureExtension", "Depth cube texture is not supported by WebGL 1.")
    return internalTexture;
  }

  auto internalOptions = options;
  internalOptions.bilinearFiltering
    = options.bilinearFiltering.has_value() ? *options.bilinearFiltering : false;
  internalOptions.comparisonFunction
    = options.comparisonFunction.has_value() ? *options.comparisonFunction : 0;
  internalOptions.generateStencil
    = options.generateStencil.has_value() ? *options.generateStencil : false;

  auto& gl = *_this->_gl;
  _this->_bindTextureDirectly(GL::TEXTURE_CUBE_MAP, internalTexture, true);

  _this->_setupDepthStencilTexture(internalTexture, size, *internalOptions.generateStencil,
                                   *internalOptions.bilinearFiltering,
                                   *internalOptions.comparisonFunction);

  // Create the depth/stencil buffer
  for (unsigned int face = 0; face < 6; ++face) {
    if (internalOptions.generateStencil.has_value() && *internalOptions.generateStencil) {
      gl.texImage2D(GL::TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL::DEPTH24_STENCIL8, size, size, 0,
                    GL::DEPTH_STENCIL, GL::UNSIGNED_INT_24_8, nullptr);
    }
    else {
      gl.texImage2D(GL::TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL::DEPTH_COMPONENT24, size, size, 0,
                    GL::DEPTH_COMPONENT, GL::UNSIGNED_INT, nullptr);
    }
  }

  _this->_bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);

  return internalTexture;
}

InternalTexturePtr CubeTextureExtension::createCubeTexture(
  std::string rootUrl, Scene* scene, const std::vector<std::string>& files, bool noMipmap,
  const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& onError,
  unsigned int format, const std::string& forcedExtension, bool createPolynomials, float lodScale,
  float lodOffset, const InternalTexturePtr& fallback)
{
  auto texture    = fallback ? fallback : InternalTexture::New(_this, InternalTextureSource::Cube);
  texture->isCube = true;
  texture->url    = rootUrl;
  texture->generateMipMaps      = !noMipmap;
  texture->_lodGenerationScale  = lodScale;
  texture->_lodGenerationOffset = lodOffset;

  if (!_this->_doNotHandleContextLost) {
    texture->_extension = forcedExtension;
    texture->_files     = files;
  }

  const auto originalRootUrl = rootUrl;
  if (_this->_transformTextureUrl && !fallback) {
    rootUrl = _this->_transformTextureUrl(rootUrl);
  }

  const auto lastDot = StringTools::lastIndexOf(rootUrl, ".");
  const auto extension
    = !forcedExtension.empty() ?
        forcedExtension :
        (lastDot > -1 ?
           StringTools::toLowerCase(rootUrl.substr(static_cast<unsigned long>(lastDot))) :
           "");

  IInternalTextureLoaderPtr loader = nullptr;
  for (const auto& availableLoader : ThinEngine::_TextureLoaders) {
    if (availableLoader->canLoad(extension)) {
      loader = availableLoader;
      break;
    }
  }

  const auto onInternalError
    = [=](const std::string& message, const std::string& exception) -> void {
    if (rootUrl == originalRootUrl) {
      if (onError) {
        onError(message, exception);
      }
    }
    else {
      // fall back to the original url if the transformed url fails to load
      BABYLON_LOGF_WARN("CubeTextureExtension", "Failed to load %s, falling back to the %s",
                        rootUrl.c_str(), originalRootUrl.c_str())
      createCubeTexture(originalRootUrl, scene, files, noMipmap, onLoad, onError, format,
                        forcedExtension, createPolynomials, lodScale, lodOffset, texture);
    }
  };

  if (loader) {
    const auto onloaddata = [=](const std::variant<std::string, ArrayBufferView>& data,
                                const std::string & /*responseURL*/) -> void {
      _this->_bindTextureDirectly(GL::TEXTURE_CUBE_MAP, texture, true);
      loader->loadCubeData(data, texture, createPolynomials, onLoad, onError);
    };
    if (!files.empty() && files.size() == 6) {
      if (loader->supportCascades()) {
        BABYLON_LOG_WARN("Engine", "Cascade loading of files not yet supported!")
        // _cascadeLoadFiles(scene, onloaddata, files, onError);
      }
      else if (onError) {
        onError("Textures type does not support cascades.", "");
      }
    }
    else {
      _this->_loadFile(rootUrl, onloaddata, nullptr, true, onInternalError);
    }
  }
  else {
    if (files.empty()) {
      BABYLON_LOG_ERROR("Engine", "Cannot load cubemap because files were not defined")
    }

    _cascadeLoadImgs(
      rootUrl, scene,
      [=](const std::vector<Image>& imgs) {
        const auto width
          = _this->needPOTTextures() ?
              ThinEngine::GetExponentOfTwo(imgs[0].width, _this->_caps.maxCubemapTextureSize) :
              imgs[0].width;
        const auto height = width;

        const std::vector<GL::GLenum> faces{
          GL::TEXTURE_CUBE_MAP_POSITIVE_X, GL::TEXTURE_CUBE_MAP_POSITIVE_Y,
          GL::TEXTURE_CUBE_MAP_POSITIVE_Z, GL::TEXTURE_CUBE_MAP_NEGATIVE_X,
          GL::TEXTURE_CUBE_MAP_NEGATIVE_Y, GL::TEXTURE_CUBE_MAP_NEGATIVE_Z};

        _this->_bindTextureDirectly(GL::TEXTURE_CUBE_MAP, texture, true);
        _this->_unpackFlipY(false);

        const auto internalFormat = format ? _this->_getInternalFormat(format) : GL::RGBA;
        for (size_t index = 0; index < faces.size(); index++) {
#if 0
          _workingContext->drawImage(imgs[index], 0, 0, imgs[index].width,
                                     imgs[index].height, 0, 0, width, height);
          _gl->texImage2D(faces[index], 0, internalFormat, internalFormat,
                        GL::UNSIGNED_BYTE, _workingCanvas);
#else
          _this->_gl->texImage2D(faces[index], 0, static_cast<int>(internalFormat),
                                 imgs[index].width, imgs[index].height, 0, GL::RGBA,
                                 GL::UNSIGNED_BYTE, &imgs[index].data);
#endif
        }

        if (!noMipmap) {
          _this->_gl->generateMipmap(GL::TEXTURE_CUBE_MAP);
        }

        _setCubeMapTextureParams(!noMipmap);

        texture->width   = width;
        texture->height  = height;
        texture->isReady = true;
        if (format) {
          texture->format = format;
        }

        texture->onLoadedObservable.notifyObservers(texture.get());
        texture->onLoadedObservable.clear();

        if (onLoad) {
          onLoad(std::nullopt);
        }
      },
      files, onError);
  }

  _this->_internalTexturesCache.emplace_back(texture);

  return texture;
}

void CubeTextureExtension::_cascadeLoadImgs(
  const std::string& /*rootUrl*/, Scene* /*scene*/,
  const std::function<void(const std::vector<Image>& images)>& onfinish,
  const std::vector<std::string>& files,
  const std::function<void(const std::string& message, const std::string& exception)>& onError)
{
  std::vector<Image> images;
  for (size_t index = 0; index < 6; ++index) {
    FileTools::LoadImageFromUrl(
      files[index], [&images](const Image& img) { images.emplace_back(img); }, onError, false);
  }

  if (images.size() == 6) {
    onfinish(images);
  }
  else {
    if (onError) {
      onError("Unable to cascade load images!", "");
    }
  }
}

void CubeTextureExtension::_setCubeMapTextureParams(bool loadMipmap)
{
  auto& gl = *_this->_gl;
  gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MAG_FILTER, GL::LINEAR);
  gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MIN_FILTER,
                   loadMipmap ? GL::LINEAR_MIPMAP_LINEAR : GL::LINEAR);
  gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_S, GL::CLAMP_TO_EDGE);
  gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_T, GL::CLAMP_TO_EDGE);

  _this->_bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);
}

} // end of namespace BABYLON
