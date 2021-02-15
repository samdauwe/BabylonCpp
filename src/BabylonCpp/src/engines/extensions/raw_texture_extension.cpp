#include <babylon/engines/extensions/raw_texture_extension.h>

#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

RawTextureExtension::RawTextureExtension(ThinEngine* engine) : _this{engine}
{
}

RawTextureExtension::~RawTextureExtension()
{
}

InternalTexturePtr RawTextureExtension::createRawTexture(
  const Uint8Array& data, int width, int height, unsigned int format, bool generateMipMaps,
  bool invertY, unsigned int samplingMode, const std::string& compression, unsigned int type)
{
  auto texture             = InternalTexture::New(_this, InternalTextureSource::Raw);
  texture->baseWidth       = width;
  texture->baseHeight      = height;
  texture->width           = width;
  texture->height          = height;
  texture->format          = format;
  texture->generateMipMaps = generateMipMaps;
  texture->samplingMode    = samplingMode;
  texture->invertY         = invertY;
  texture->_compression    = compression;
  texture->type            = type;

  if (!_this->_doNotHandleContextLost) {
    texture->_bufferView = data;
  }

  _this->updateRawTexture(texture, data, format, invertY, compression, type);
  _this->_bindTextureDirectly(GL::TEXTURE_2D, texture, true);

  // Filters
  auto filters = _this->_getSamplingParameters(samplingMode, generateMipMaps);

  _this->_gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, filters.mag);
  _this->_gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, filters.min);

  if (generateMipMaps) {
    _this->_gl->generateMipmap(GL::TEXTURE_2D);
  }

  _this->_bindTextureDirectly(GL::TEXTURE_2D, nullptr);

  _this->_internalTexturesCache.emplace_back(texture);

  return texture;
}

void RawTextureExtension::updateRawTexture(const InternalTexturePtr& texture,
                                           const Uint8Array& data, unsigned int format,
                                           bool invertY, const std::string& compression,
                                           unsigned int type)
{
  if (!texture) {
    return;
  }

  // babylon's internalSizedFomat but gl's texImage2D internalFormat
  auto internalSizedFomat = _this->_getRGBABufferInternalSizedFormat(type, format);

  // babylon's internalFormat but gl's texImage2D format
  auto internalFormat = _this->_getInternalFormat(format);
  auto textureType    = _this->_getWebGLTextureType(type);
  _this->_bindTextureDirectly(GL::TEXTURE_2D, texture, true);
  _this->_unpackFlipY(invertY);

  if (!_this->_doNotHandleContextLost) {
    texture->_bufferView  = data;
    texture->format       = format;
    texture->type         = type;
    texture->invertY      = invertY;
    texture->_compression = compression;
  }

  if (texture->width % 4 != 0) {
    _this->_gl->pixelStorei(GL::UNPACK_ALIGNMENT, 1);
  }

  if (!compression.empty() && !data.empty()) {
    // Not implemented yet
  }
  else {
    auto _internalSizedFomat = static_cast<GL::GLint>(internalSizedFomat);
    _this->_gl->texImage2D(GL::TEXTURE_2D, 0, _internalSizedFomat, texture->width, texture->height,
                           0, internalFormat, textureType, &data);
  }

  if (texture->generateMipMaps) {
    _this->_gl->generateMipmap(GL::TEXTURE_2D);
  }
  _this->_bindTextureDirectly(GL::TEXTURE_2D, nullptr);
  // resetTextureCache();
  texture->isReady = true;
}

InternalTexturePtr RawTextureExtension::createRawCubeTexture(
  const std::vector<ArrayBufferView>& data, int size, unsigned int format, unsigned int type,
  bool generateMipMaps, bool invertY, unsigned int samplingMode, const std::string& compression)
{
  auto& gl        = *_this->_gl;
  auto texture    = InternalTexture::New(_this, InternalTextureSource::CubeRaw);
  texture->isCube = true;
  texture->format = format;
  texture->type   = type;
  if (!_this->_doNotHandleContextLost) {
    texture->_bufferViewArray = data;
  }

  auto textureType = _this->_getWebGLTextureType(type);

  // Mipmap generation needs a sized internal format that is both color-renderable and
  // texture-filterable
  if (textureType == GL::FLOAT && !_this->_caps.textureFloatLinearFiltering) {
    generateMipMaps = false;
    samplingMode    = Constants::TEXTURE_NEAREST_SAMPLINGMODE;
    BABYLON_LOG_WARN("RawTextureExtension",
                     "Float texture filtering is not supported. Mipmap generation and sampling "
                     "mode are forced to false and TEXTURE_NEAREST_SAMPLINGMODE, respectively.")
  }
  else if (textureType == GL::HALF_FLOAT_OES && !_this->_caps.textureHalfFloatLinearFiltering) {
    generateMipMaps = false;
    samplingMode    = Constants::TEXTURE_NEAREST_SAMPLINGMODE;
    BABYLON_LOG_WARN(
      "RawTextureExtension",
      "Half float texture filtering is not supported. Mipmap generation and sampling mode are "
      "forced to false and TEXTURE_NEAREST_SAMPLINGMODE, respectively.")
  }
  else if (textureType == GL::FLOAT && !_this->_caps.textureFloatRender) {
    generateMipMaps = false;
    BABYLON_LOG_WARN(
      "RawTextureExtension",
      "Render to float textures is not supported. Mipmap generation forced to false.")
  }
  else if (textureType == GL::HALF_FLOAT && !_this->_caps.colorBufferFloat) {
    generateMipMaps = false;
    BABYLON_LOG_WARN(
      "RawTextureExtension",
      "Render to half float textures is not supported. Mipmap generation forced to false.")
  }

  auto width  = size;
  auto height = width;

  texture->width  = width;
  texture->height = height;

  // Double check on POT to generate Mips.
  auto isPot = !_this->needPOTTextures()
               || (Tools::IsExponentOfTwo(static_cast<size_t>(texture->width))
                   && Tools::IsExponentOfTwo(static_cast<size_t>(texture->height)));
  if (!isPot) {
    generateMipMaps = false;
  }

  // Upload data if needed. The texture won't be ready until then.
  if (!data.empty()) {
    _this->updateRawCubeTexture(texture, data, format, type, invertY, compression);
  }

  _this->_bindTextureDirectly(GL::TEXTURE_CUBE_MAP, texture, true);

  // Filters
  if (!data.empty() && generateMipMaps) {
    gl.generateMipmap(GL::TEXTURE_CUBE_MAP);
  }

  const auto filters = _this->_getSamplingParameters(samplingMode, generateMipMaps);
  gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MAG_FILTER, filters.mag);
  gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MIN_FILTER, filters.min);

  gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_S, GL::CLAMP_TO_EDGE);
  gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_T, GL::CLAMP_TO_EDGE);
  _this->_bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);

  texture->generateMipMaps = generateMipMaps;
  texture->samplingMode    = samplingMode;

  return texture;
}

void RawTextureExtension::updateRawCubeTexture(const InternalTexturePtr& texture,
                                               const std::vector<ArrayBufferView>& data,
                                               unsigned int format, unsigned int type, bool invertY,
                                               const std::string& compression, unsigned int level)
{
  texture->_bufferViewArray = data;
  texture->format           = format;
  texture->type             = type;
  texture->invertY          = invertY;
  texture->_compression     = compression;

  auto& gl                = *_this->_gl;
  auto textureType        = _this->_getWebGLTextureType(type);
  auto internalFormat     = _this->_getInternalFormat(format);
  auto internalSizedFomat = _this->_getRGBABufferInternalSizedFormat(type);

  auto needConversion = false;
  if (internalFormat == GL::RGB) {
    internalFormat = GL::RGBA;
    needConversion = true;
  }

  _this->_bindTextureDirectly(GL::TEXTURE_CUBE_MAP, texture, true);
  _this->_unpackFlipY(invertY);

  if (texture->width % 4 != 0) {
    gl.pixelStorei(GL::UNPACK_ALIGNMENT, 1);
  }

  const std::vector<unsigned int> facesIndex
    = {GL::TEXTURE_CUBE_MAP_POSITIVE_X, GL::TEXTURE_CUBE_MAP_NEGATIVE_X,
       GL::TEXTURE_CUBE_MAP_POSITIVE_Y, GL::TEXTURE_CUBE_MAP_NEGATIVE_Y,
       GL::TEXTURE_CUBE_MAP_POSITIVE_Z, GL::TEXTURE_CUBE_MAP_NEGATIVE_Z};

  // Data are known to be in +X +Y +Z -X -Y -Z
  for (std::size_t index = 0; index < facesIndex.size(); ++index) {
    const auto& faceData = data[index];

    if (!compression.empty()) {
#if 0
      _gl->compressedTexImage2D(facesIndex[index], level,
                                getCaps().s3tc[compression], texture->width,
                                texture->height, 0, faceData.uint8Array);
#endif
    }
    else {
      if (needConversion) {
        auto convertedFaceData
          = _convertRGBtoRGBATextureData(faceData, texture->width, texture->height, type)
              .uint8Array();
        gl.texImage2D(facesIndex[index], static_cast<int>(level),
                      static_cast<int>(internalSizedFomat), texture->width, texture->height, 0,
                      internalFormat, textureType, &convertedFaceData);
      }
      else {
        gl.texImage2D(facesIndex[index], static_cast<int>(level),
                      static_cast<int>(internalSizedFomat), texture->width, texture->height, 0,
                      internalFormat, textureType, &faceData.uint8Array());
      }
    }
  }

  auto isPot = !_this->needPOTTextures()
               || (Tools::IsExponentOfTwo(static_cast<size_t>(texture->width))
                   && Tools::IsExponentOfTwo(static_cast<size_t>(texture->height)));
  if (isPot && texture->generateMipMaps && level == 0) {
    gl.generateMipmap(GL::TEXTURE_CUBE_MAP);
  }
  _this->_bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);

  // resetTextureCache();
  texture->isReady = true;
}

InternalTexturePtr RawTextureExtension::createRawCubeTextureFromUrl(
  const std::string& url, Scene* scene, int size, unsigned int format, unsigned int type,
  bool noMipmap,
  const std::function<ArrayBufferViewArray(const ArrayBuffer& arrayBuffer)>& callback,
  const std::function<std::vector<ArrayBufferViewArray>(const ArrayBufferViewArray& faces)>&
    mipmapGenerator,
  const std::function<void()>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& onError,
  unsigned int samplingMode, bool invertY)
{
  auto texture
    = _this->createRawCubeTexture({}, size, format, type, !noMipmap, invertY, samplingMode);
  scene->_addPendingData(texture);
  texture->url = url;
  _this->_internalTexturesCache.emplace_back(texture);

  const auto onerror = [=](const std::string& message, const std::string& exception) -> void {
    scene->_removePendingData(texture);
    if (onError) {
      onError(message, exception);
    }
  };

  const auto internalCallback = [=](const std::variant<std::string, ArrayBufferView>& data,
                                    const std::string& /*responseURL*/) -> void {
    auto width = texture->width;

    if (!std::holds_alternative<ArrayBufferView>(data)) {
      return;
    }

    auto faceDataArrays = callback(std::get<ArrayBufferView>(data).uint8Array());

    if (faceDataArrays.empty()) {
      return;
    }

    if (mipmapGenerator) {
      auto textureType        = _this->_getWebGLTextureType(type);
      auto internalFormat     = _this->_getInternalFormat(format);
      auto internalSizedFomat = _this->_getRGBABufferInternalSizedFormat(type);

      auto needConversion = false;
      if (internalFormat == GL::RGB) {
        internalFormat = GL::RGBA;
        needConversion = true;
      }

      _this->_bindTextureDirectly(GL::TEXTURE_CUBE_MAP, texture, true);
      _this->_unpackFlipY(false);

      const auto& mipData = mipmapGenerator(faceDataArrays);
      for (size_t level = 0; level < mipData.size(); level++) {
        auto mipSize = width >> level;

        for (unsigned int faceIndex = 0; faceIndex < 6; ++faceIndex) {
          auto mipFaceData = mipData[level][faceIndex];
          if (needConversion) {
            mipFaceData = _convertRGBtoRGBATextureData(mipFaceData, mipSize, mipSize, type);
          }
          _this->_gl->texImage2D(faceIndex, static_cast<int>(level),
                                 static_cast<int>(internalSizedFomat), mipSize, mipSize, 0,
                                 internalFormat, textureType, &mipFaceData.uint8Array());
        }
      }

      _this->_bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);
    }
    else {
      _this->updateRawCubeTexture(texture, faceDataArrays, format, type, invertY);
    }

    texture->isReady = true;
    // resetTextureCache();
    scene->_removePendingData(texture);

    if (onLoad) {
      onLoad();
    }
  };

  _this->_loadFile(
    url,
    [=](const std::variant<std::string, ArrayBufferView>& data,
        const std::string& responseURL) -> void { internalCallback(data, responseURL); },
    nullptr, true, onerror);

  return texture;
}

InternalTexturePtr RawTextureExtension::_createRawTexture(const ArrayBufferView& data, int width,
                                                          int height, int depth,
                                                          unsigned int format, bool generateMipMaps,
                                                          bool invertY, unsigned int samplingMode,
                                                          const std::string& compression,
                                                          unsigned int textureType, bool is3D)
{
  auto target         = is3D ? GL::TEXTURE_3D : GL::TEXTURE_2D_ARRAY;
  auto source         = is3D ? InternalTextureSource::Raw3D : InternalTextureSource::Raw2DArray;
  auto texture        = InternalTexture::New(_this, source);
  texture->baseWidth  = width;
  texture->baseHeight = height;
  texture->baseDepth  = depth;
  texture->width      = width;
  texture->height     = height;
  texture->depth      = depth;
  texture->format     = format;
  texture->type       = textureType;
  texture->generateMipMaps = generateMipMaps;
  texture->samplingMode    = samplingMode;
  if (is3D) {
    texture->is3D = true;
  }
  else {
    texture->is2DArray = true;
  }

  if (!_this->_doNotHandleContextLost) {
    texture->_bufferView = data.uint8Array();
  }

  if (is3D) {
    _this->updateRawTexture3D(texture, data, format, invertY, compression, textureType);
  }
  else {
    _this->updateRawTexture2DArray(texture, data, format, invertY, compression, textureType);
  }
  _this->_bindTextureDirectly(target, texture, true);

  // Filters
  auto filters = _this->_getSamplingParameters(samplingMode, generateMipMaps);

  _this->_gl->texParameteri(target, GL::TEXTURE_MAG_FILTER, filters.mag);
  _this->_gl->texParameteri(target, GL::TEXTURE_MIN_FILTER, filters.min);

  if (generateMipMaps) {
    _this->_gl->generateMipmap(target);
  }

  _this->_bindTextureDirectly(target, nullptr);

  _this->_internalTexturesCache.emplace_back(texture);

  return texture;
}

InternalTexturePtr
RawTextureExtension::createRawTexture3D(const ArrayBufferView& data, int width, int height,
                                        int depth, unsigned int format, bool generateMipMaps,
                                        bool invertY, unsigned int samplingMode,
                                        const std::string& compression, unsigned int textureType)
{
  return _createRawTexture(data, width, height, depth, format, generateMipMaps, invertY,
                           samplingMode, compression, textureType, true);
}

InternalTexturePtr RawTextureExtension::createRawTexture2DArray(
  const ArrayBufferView& data, int width, int height, int depth, unsigned int format,
  bool generateMipMaps, bool invertY, unsigned int samplingMode, const std::string& compression,
  unsigned int textureType)
{
  return _createRawTexture(data, width, height, depth, format, generateMipMaps, invertY,
                           samplingMode, compression, textureType, false);
}

void RawTextureExtension::_updateRawTexture(const InternalTexturePtr& texture,
                                            const ArrayBufferView& data, unsigned int format,
                                            bool invertY, const std::string& compression,
                                            unsigned int textureType, bool is3D)
{
  const auto& _data = data.uint8Array();

  auto target             = is3D ? GL::TEXTURE_3D : GL::TEXTURE_2D_ARRAY;
  auto internalType       = _this->_getWebGLTextureType(textureType);
  auto internalFormat     = _this->_getInternalFormat(format);
  auto internalSizedFomat = _this->_getRGBABufferInternalSizedFormat(textureType, format);

  _this->_bindTextureDirectly(target, texture, true);
  _this->_unpackFlipY(invertY);

  if (!_this->_doNotHandleContextLost) {
    texture->_bufferView  = _data;
    texture->format       = format;
    texture->invertY      = invertY;
    texture->_compression = compression;
  }

  if (texture->width % 4 != 0) {
    _this->_gl->pixelStorei(GL::UNPACK_ALIGNMENT, 1);
  }

  if (!compression.empty() && !_data.empty()) {
    // Not implemented yet
  }
  else {
    _this->_gl->texImage3D(target, 0, static_cast<int>(internalSizedFomat), texture->width,
                           texture->height, texture->depth, 0, internalFormat, internalType, _data);
  }

  if (texture->generateMipMaps) {
    _this->_gl->generateMipmap(target);
  }
  _this->_bindTextureDirectly(target, nullptr);
  // resetTextureCache();
  texture->isReady = true;
}

void RawTextureExtension::updateRawTexture3D(const InternalTexturePtr& texture,
                                             const ArrayBufferView& data, unsigned int format,
                                             bool invertY, const std::string& compression,
                                             unsigned int textureType)
{
  return _updateRawTexture(texture, data, format, invertY, compression, textureType, true);
}

void RawTextureExtension::updateRawTexture2DArray(const InternalTexturePtr& texture,
                                                  const ArrayBufferView& data, unsigned int format,
                                                  bool invertY, const std::string& compression,
                                                  unsigned int textureType)
{
  return _updateRawTexture(texture, data, format, invertY, compression, textureType, false);
}

ArrayBufferView RawTextureExtension::_convertRGBtoRGBATextureData(const ArrayBufferView& rgbData,
                                                                  int width, int height,
                                                                  unsigned int textureType)
{
  // Create new RGBA data container.
  if (textureType == Constants::TEXTURETYPE_FLOAT) {
    Float32Array rgbaData(static_cast<size_t>(width * height * 4));
    const auto rgbDataFloat32Array = rgbData.float32Array();
    // Convert each pixel.
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        auto index    = static_cast<size_t>((y * width + x) * 3);
        auto newIndex = static_cast<size_t>((y * width + x) * 4);

        // Map Old Value to new value.
        rgbaData[newIndex + 0] = rgbDataFloat32Array[index + 0];
        rgbaData[newIndex + 1] = rgbDataFloat32Array[index + 1];
        rgbaData[newIndex + 2] = rgbDataFloat32Array[index + 2];

        // Add fully opaque alpha channel.
        rgbaData[newIndex + 3] = 1.f;
      }
    }
    return ArrayBufferView(rgbaData);
  }
  else {
    Uint32Array rgbaData(static_cast<size_t>(width * height * 4));
    const auto rgbDataUint32Array = rgbData.uint32Array();
    // Convert each pixel.
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        auto index    = static_cast<size_t>((y * width + x) * 3);
        auto newIndex = static_cast<size_t>((y * width + x) * 4);

        // Map Old Value to new value.
        rgbaData[newIndex + 0] = rgbDataUint32Array[index + 0];
        rgbaData[newIndex + 1] = rgbDataUint32Array[index + 1];
        rgbaData[newIndex + 2] = rgbDataUint32Array[index + 2];

        // Add fully opaque alpha channel.
        rgbaData[newIndex + 3] = 1;
      }
    }
    return ArrayBufferView(rgbaData);
  }
}

} // end of namespace BABYLON
