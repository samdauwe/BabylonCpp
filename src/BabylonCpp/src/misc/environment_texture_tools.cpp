#include <babylon/misc/environment_texture_tools.h>

#include <sstream>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/math/scalar.h>
#include <babylon/math/spherical_polynomial.h>
#include <babylon/math/vector3.h>
#include <babylon/misc/environment_texture_info.h>
#include <babylon/misc/tools.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/postprocesses/post_process_manager.h>

namespace BABYLON {

std::array<uint8_t, 8> EnvironmentTextureTools::_MagicBytes
  = {0x86, 0x16, 0x87, 0x96, 0xf6, 0xd6, 0x96, 0x36};

EnvironmentTextureTools::EnvironmentTextureTools() = default;

EnvironmentTextureTools::~EnvironmentTextureTools() = default;

EnvironmentTextureInfoPtr EnvironmentTextureTools::GetEnvInfo(const ArrayBuffer& data)
{
  const auto& dataView = data;
  size_t pos           = 0;

  for (unsigned int i = 0; i < EnvironmentTextureTools::_MagicBytes.size(); ++i) {
    if (dataView[pos++] != EnvironmentTextureTools::_MagicBytes[i]) {
      BABYLON_LOG_ERROR("EnvironmentTextureTools", "Not a babylon environment map")
      return nullptr;
    }
  }

  // Read json manifest - collect characters up to null terminator
  std::ostringstream manifestString;
  char charCode = static_cast<char>(dataView[pos++]);
  while (charCode) {
    manifestString << String::fromCharCode(charCode);
    charCode = static_cast<char>(dataView[pos++]);
  }

  // Parse JSON string
  auto parsedManifest = json::parse(manifestString.str());
  auto manifest       = EnvironmentTextureInfo::Parse(parsedManifest);
  if (manifest->specular) {
    // Extend the header with the position of the payload.
    manifest->specular->specularDataPosition = pos;
    // Fallback to 0.8 exactly if lodGenerationScale is not defined for backward
    // compatibility.
    manifest->specular->lodGenerationScale = manifest->specular->lodGenerationScale.has_value() ?
                                               *manifest->specular->lodGenerationScale :
                                               0.8f;
  }

  return manifest;
}

void EnvironmentTextureTools::UploadEnvLevels(const InternalTexturePtr& texture,
                                              const ArrayBuffer& arrayBuffer,
                                              const EnvironmentTextureInfo& info)
{
  if (info.version != 1) {
    throw std::runtime_error("Unsupported babylon environment map version "
                             + std::to_string(info.version));
  }

  auto& specularInfo = info.specular;
  if (!specularInfo) {
    // Nothing else parsed so far
    return;
  }

  // Double checks the enclosed info
  auto _mipmapsCount = Scalar::Log2(info.width);
  auto mipmapsCount  = static_cast<size_t>(std::round(_mipmapsCount) + 1);
  if (specularInfo->mipmaps.size() != 6 * mipmapsCount) {
    throw std::runtime_error("Unsupported specular mipmaps number "
                             + std::to_string(specularInfo->mipmaps.size()));
  }

  texture->_lodGenerationScale = *specularInfo->lodGenerationScale;

  std::vector<std::vector<ArrayBuffer>> imageData(mipmapsCount);
  for (size_t i = 0; i < mipmapsCount; ++i) {
    imageData[i] = std::vector<ArrayBuffer>(6);
    for (size_t face = 0; face < 6; ++face) {
      auto imageInfo     = specularInfo->mipmaps[i * 6 + face];
      imageData[i][face] = stl_util::to_array<uint8_t>(
        arrayBuffer, *specularInfo->specularDataPosition + imageInfo.position, imageInfo.length);
    }
  }

  return EnvironmentTextureTools::UploadLevels(texture, imageData);
}

void EnvironmentTextureTools::UploadLevels(const InternalTexturePtr& texture,
                                           const std::vector<std::vector<ArrayBuffer>>& imageData)
{
  if (!Tools::IsExponentOfTwo(static_cast<size_t>(texture->width))) {
    throw std::runtime_error("Texture size must be a power of two");
  }

  const auto mipmapsCount = static_cast<size_t>(std::round(Scalar::Log2(texture->width)) + 1);

  // Gets everything ready.
  auto engine                    = texture->getEngine();
  auto expandTexture             = false;
  auto generateNonLODTextures    = false;
  PostProcessPtr rgbdPostProcess = nullptr;
  InternalTexturePtr cubeRtt     = nullptr;
  std::unordered_map<size_t, BaseTexturePtr> lodTextures;
  const auto& caps = engine->getCaps();

  texture->format          = Constants::TEXTUREFORMAT_RGBA;
  texture->type            = Constants::TEXTURETYPE_UNSIGNED_INT;
  texture->generateMipMaps = true;
  engine->updateTextureSamplingMode(Constants::TEXTURE_TRILINEAR_SAMPLINGMODE, texture);

  // Add extra process if texture lod is not supported
  if (!caps.textureLOD) {
    expandTexture          = false;
    generateNonLODTextures = true;
    lodTextures            = {};
  }
  // in webgl 1 there are no ways to either render or copy lod level information
  // for float textures.
  else if (engine->webGLVersion() < 2) {
    expandTexture = false;
  }
  // If half float available we can uncompress the texture
  else if (caps.textureHalfFloatRender && caps.textureHalfFloatLinearFiltering) {
    expandTexture = true;
    texture->type = Constants::TEXTURETYPE_HALF_FLOAT;
  }
  // If full float available we can uncompress the texture
  else if (caps.textureFloatRender && caps.textureFloatLinearFiltering) {
    expandTexture = true;
    texture->type = Constants::TEXTURETYPE_FLOAT;
  }

  // Expand the texture if possible
  if (expandTexture) {
    // Simply run through the decode PP
    rgbdPostProcess = PostProcess::New("rgbdDecode", "rgbdDecode", {}, {}, 1.f, nullptr,
                                       Constants::TEXTURE_TRILINEAR_SAMPLINGMODE, engine, false, "",
                                       texture->type, "", {}, false);

    texture->_isRGBD = false;
    texture->invertY = false;

    RenderTargetCreationOptions options;
    options.generateDepthBuffer   = false;
    options.generateMipMaps       = true;
    options.generateStencilBuffer = false;
    options.samplingMode          = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE;
    options.type                  = texture->type;
    options.format                = Constants::TEXTUREFORMAT_RGBA;

    cubeRtt = engine->createRenderTargetCubeTexture(texture->width, options);
  }
  else {
    texture->_isRGBD = true;
    texture->invertY = true;

    // In case of missing support, applies the same patch than DDS files.
    if (generateNonLODTextures) {
      auto mipSlices = 3u;
      auto scale     = texture->_lodGenerationScale;
      auto offset    = texture->_lodGenerationOffset;

      for (unsigned int i = 0; i < mipSlices; i++) {
        // compute LOD from even spacing in smoothness (matching shader
        // calculation)
        auto smoothness = static_cast<float>(i) / static_cast<float>(mipSlices - 1);
        auto roughness  = 1.f - smoothness;

        auto minLODIndex = offset; // roughness = 0
        auto maxLODIndex
          = (mipmapsCount - 1) * scale + offset; // roughness = 1 (mipmaps start from 0)

        auto lodIndex = minLODIndex + (maxLODIndex - minLODIndex) * roughness;
        auto mipmapIndex
          = static_cast<size_t>(std::round(std::min(std::max(lodIndex, 0.f), maxLODIndex)));

        auto glTextureFromLod     = InternalTexture::New(engine, InternalTexture::DATASOURCE_TEMP);
        glTextureFromLod->isCube  = true;
        glTextureFromLod->invertY = true;
        glTextureFromLod->generateMipMaps = false;
        engine->updateTextureSamplingMode(Constants::TEXTURE_LINEAR_LINEAR, glTextureFromLod);

        // Wrap in a base texture for easy binding.
        auto lodTexture          = BaseTexture::New(nullptr);
        lodTexture->isCube       = true;
        lodTexture->_texture     = glTextureFromLod;
        lodTextures[mipmapIndex] = lodTexture;

        switch (i) {
          case 0:
            texture->_lodTextureLow = lodTexture;
            break;
          case 1:
            texture->_lodTextureMid = lodTexture;
            break;
          case 2:
            texture->_lodTextureHigh = lodTexture;
            break;
        }
      }
    }
  }

  std::vector<std::function<void()>> promises;
  // All mipmaps up to provided number of images
  for (size_t i = 0; i < imageData.size(); ++i) {
    // All faces
    for (unsigned int face = 0; face < 6; ++face) {
      // Enqueue promise to upload to the texture.
      const auto promise = [=]() -> void {
        // Constructs an image element from image data
        const auto& bytes = imageData[i][face];
        auto image        = Tools::ArrayBufferToImage(bytes);

        // Upload to the texture.
        if (expandTexture) {
          auto tempTexture = engine->createTexture(
            "", true, true, nullptr, Constants::TEXTURE_NEAREST_SAMPLINGMODE, nullptr,
            [](const std::string& message, const std::string& /*exception*/) {
              throw std::runtime_error(message);
            },
            image);

          rgbdPostProcess->getEffect()->executeWhenCompiled([&](Effect* /*effect*/) {
            // Uncompress the data to a RTT
            rgbdPostProcess->onApply = [&](Effect* effect, EventState& /*es*/) {
              effect->_bindTexture("textureSampler", tempTexture);
              effect->setFloat2("scale", 1, 1);
            };

            engine->scenes[0]->postProcessManager->directRender({rgbdPostProcess}, cubeRtt, true,
                                                                face, static_cast<int>(i));

            // Cleanup
            engine->restoreDefaultFramebuffer();
            tempTexture->dispose();
          });
        }
        else {
          engine->_uploadImageToTexture(texture, image, face, static_cast<int>(i));

          // Upload the face to the non lod texture support
          if (generateNonLODTextures) {
            auto lodTexture = lodTextures.at(i);
            if (lodTexture) {
              engine->_uploadImageToTexture(lodTexture->_texture, image, face, 0);
            }
          }
        }
      };
      promises.emplace_back(promise);
    }
  }

  // Fill remaining mipmaps with black textures.
  if (imageData.size() < mipmapsCount) {
    ArrayBufferView data;
    auto size       = std::pow(2, mipmapsCount - 1 - imageData.size());
    auto dataLength = static_cast<size_t>(size * size * 4);
    switch (texture->type) {
      case Constants::TEXTURETYPE_UNSIGNED_INT: {
        data = Uint8Array(dataLength);
        break;
      }
      case Constants::TEXTURETYPE_HALF_FLOAT: {
        data = Uint16Array(dataLength);
        break;
      }
      case Constants::TEXTURETYPE_FLOAT: {
        data = Float32Array(dataLength);
        break;
      }
    }
    for (size_t i = imageData.size(); i < mipmapsCount; ++i) {
      for (unsigned int face = 0; face < 6; face++) {
        engine->_uploadArrayBufferViewToTexture(texture, data.uint8Array, face,
                                                static_cast<int>(i));
      }
    }
  }

  // Once all done, finishes the cleanup and return
  for (const auto& promise : promises) {
    promise();
  }

  // Release temp RTT.
  if (cubeRtt) {
    engine->_releaseFramebufferObjects(cubeRtt.get());
    engine->_releaseTexture(texture.get());
    cubeRtt->_swapAndDie(texture);
  }
  // Release temp Post Process.
  if (rgbdPostProcess) {
    rgbdPostProcess->dispose();
  }
  // Flag internal texture as ready in case they are in use.
  if (generateNonLODTextures) {
    if (texture->_lodTextureHigh && texture->_lodTextureHigh->_texture) {
      texture->_lodTextureHigh->_texture->isReady = true;
    }
    if (texture->_lodTextureMid && texture->_lodTextureMid->_texture) {
      texture->_lodTextureMid->_texture->isReady = true;
    }
    if (texture->_lodTextureLow && texture->_lodTextureLow->_texture) {
      texture->_lodTextureLow->_texture->isReady = true;
    }
  }
}

void EnvironmentTextureTools::UploadEnvSpherical(const InternalTexturePtr& texture,
                                                 const EnvironmentTextureInfo& info)
{
  if (info.version != 1) {
    BABYLON_LOGF_WARN("EnvironmentTextureTools",
                      "Unsupported babylon environment map version \"%u\"", info.version)
  }

  auto irradianceInfo = info.irradiance;
  if (!irradianceInfo) {
    return;
  }

  auto sp = std::make_shared<SphericalPolynomial>();
  Vector3::FromArrayToRef(irradianceInfo->x, 0, sp->x);
  Vector3::FromArrayToRef(irradianceInfo->y, 0, sp->y);
  Vector3::FromArrayToRef(irradianceInfo->z, 0, sp->z);
  Vector3::FromArrayToRef(irradianceInfo->xx, 0, sp->xx);
  Vector3::FromArrayToRef(irradianceInfo->yy, 0, sp->yy);
  Vector3::FromArrayToRef(irradianceInfo->zz, 0, sp->zz);
  Vector3::FromArrayToRef(irradianceInfo->yz, 0, sp->yz);
  Vector3::FromArrayToRef(irradianceInfo->zx, 0, sp->zx);
  Vector3::FromArrayToRef(irradianceInfo->xy, 0, sp->xy);
  texture->_sphericalPolynomial = sp;
}

} // end of namespace BABYLON
