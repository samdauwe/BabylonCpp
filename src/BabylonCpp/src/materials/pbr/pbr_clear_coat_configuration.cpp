#include <babylon/materials/pbr/pbr_clear_coat_configuration.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_flags.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/pbr/imaterial_clear_coat_defines.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/uniform_buffer.h>

namespace BABYLON {

PBRClearCoatConfiguration::PBRClearCoatConfiguration(
  const std::function<void()>& markAllSubMeshesAsTexturesDirty)
    : isEnabled{this, &PBRClearCoatConfiguration::get_isEnabled,
                &PBRClearCoatConfiguration::set_isEnabled}
    , intensity{1.f}
    , roughness{1.f}
    , indexOfRefraction{this, &PBRClearCoatConfiguration::get_indexOfRefraction,
                        &PBRClearCoatConfiguration::set_indexOfRefraction}
    , texture{this, &PBRClearCoatConfiguration::get_texture,
              &PBRClearCoatConfiguration::set_texture}
    , bumpTexture{this, &PBRClearCoatConfiguration::get_bumpTexture,
                  &PBRClearCoatConfiguration::set_bumpTexture}
    , isTintEnabled{this, &PBRClearCoatConfiguration::get_isTintEnabled,
                    &PBRClearCoatConfiguration::set_isTintEnabled}
    , tintColor{Color3::White()}
    , tintColorAtDistance{1.f}
    , tintThickness{1.f}
    , tintTexture{this, &PBRClearCoatConfiguration::get_tintTexture,
                  &PBRClearCoatConfiguration::set_tintTexture}
    , _isEnabled{false}
    , _indexOfRefraction{PBRClearCoatConfiguration::_DefaultIndexOfRefraction}
    , _texture{nullptr}
    , _bumpTexture{nullptr}
    , _isTintEnabled{false}
    , _tintTexture{nullptr}
    , _internalMarkAllSubMeshesAsTexturesDirty{markAllSubMeshesAsTexturesDirty}
{
}

PBRClearCoatConfiguration::~PBRClearCoatConfiguration() = default;

bool PBRClearCoatConfiguration::get_isEnabled() const
{
  return _isEnabled;
}

void PBRClearCoatConfiguration::set_isEnabled(bool value)
{
  if (_isEnabled == value) {
    return;
  }

  _isEnabled = value;
  _markAllSubMeshesAsTexturesDirty();
}

float PBRClearCoatConfiguration::get_indexOfRefraction() const
{
  return _indexOfRefraction;
}

void PBRClearCoatConfiguration::set_indexOfRefraction(float value)
{
  if (stl_util::almost_equal(_indexOfRefraction, value)) {
    return;
  }

  _indexOfRefraction = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRClearCoatConfiguration::get_texture()
{
  return _texture;
}

void PBRClearCoatConfiguration::set_texture(const BaseTexturePtr& value)
{
  if (_texture == value) {
    return;
  }

  _texture = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRClearCoatConfiguration::get_bumpTexture()
{
  return _bumpTexture;
}

void PBRClearCoatConfiguration::set_bumpTexture(const BaseTexturePtr& value)
{
  if (_bumpTexture == value) {
    return;
  }

  _bumpTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRClearCoatConfiguration::get_isTintEnabled() const
{
  return _isTintEnabled;
}

void PBRClearCoatConfiguration::set_isTintEnabled(bool value)
{
  if (_isTintEnabled == value) {
    return;
  }

  _isTintEnabled = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRClearCoatConfiguration::get_tintTexture()
{
  return _tintTexture;
}

void PBRClearCoatConfiguration::set_tintTexture(const BaseTexturePtr& value)
{
  if (_tintTexture == value) {
    return;
  }

  _tintTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

void PBRClearCoatConfiguration::_markAllSubMeshesAsTexturesDirty()
{
  _internalMarkAllSubMeshesAsTexturesDirty();
}

bool PBRClearCoatConfiguration::isReadyForSubMesh(
  const MaterialDefines& defines, Scene* scene, Engine* engine,
  bool disableBumpMap) const
{
  if (defines._areTexturesDirty) {
    if (scene->texturesEnabled()) {
      if (_texture && MaterialFlags::ClearCoatTextureEnabled()) {
        if (!_texture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      if (engine->getCaps().standardDerivatives && _bumpTexture
          && MaterialFlags::ClearCoatBumpTextureEnabled() && !disableBumpMap) {
        // Bump texture cannot be not blocking.
        if (!_bumpTexture->isReady()) {
          return false;
        }
      }

      if (_isTintEnabled && _tintTexture
          && MaterialFlags::ClearCoatTintTextureEnabled()) {
        if (!_tintTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }
    }
  }

  return true;
}

void PBRClearCoatConfiguration::prepareDefines(MaterialDefines& defines,
                                               Scene* scene)
{
  if (_isEnabled) {
    defines.boolDef["CLEARCOAT"] = true;

    if (defines._areTexturesDirty) {
      if (scene->texturesEnabled()) {
        if (_texture && MaterialFlags::ClearCoatTextureEnabled()) {
          MaterialHelper::PrepareDefinesForMergedUV(_texture, defines,
                                                    "CLEARCOAT_TEXTURE");
        }
        else {
          defines.boolDef["CLEARCOAT_TEXTURE"] = false;
        }

        if (_bumpTexture && MaterialFlags::ClearCoatBumpTextureEnabled()) {
          MaterialHelper::PrepareDefinesForMergedUV(_bumpTexture, defines,
                                                    "CLEARCOAT_BUMP");
        }
        else {
          defines.boolDef["CLEARCOAT_BUMP"] = false;
        }

        defines.boolDef["CLEARCOAT_DEFAULTIOR"] = stl_util::almost_equal(
          _indexOfRefraction,
          PBRClearCoatConfiguration::_DefaultIndexOfRefraction);

        if (_isTintEnabled) {
          defines.boolDef["CLEARCOAT_TINT"] = true;
          if (_tintTexture && MaterialFlags::ClearCoatTintTextureEnabled()) {
            MaterialHelper::PrepareDefinesForMergedUV(_tintTexture, defines,
                                                      "CLEARCOAT_TINT_TEXTURE");
          }
          else {
            defines.boolDef["CLEARCOAT_TINT_TEXTURE"] = false;
          }
        }
        else {
          defines.boolDef["CLEARCOAT_TINT"]         = false;
          defines.boolDef["CLEARCOAT_TINT_TEXTURE"] = false;
        }
      }
    }
  }
  else {
    defines.boolDef["CLEARCOAT"]              = false;
    defines.boolDef["CLEARCOAT_TEXTURE"]      = false;
    defines.boolDef["CLEARCOAT_BUMP"]         = false;
    defines.boolDef["CLEARCOAT_TINT"]         = false;
    defines.boolDef["CLEARCOAT_TINT_TEXTURE"] = false;
  }
}

void PBRClearCoatConfiguration::bindForSubMesh(UniformBuffer& uniformBuffer,
                                               Scene* scene, Engine* engine,
                                               bool disableBumpMap,
                                               bool isFrozen,
                                               bool invertNormalMapX,
                                               bool invertNormalMapY)
{
  if (!uniformBuffer.useUbo() || !isFrozen || !uniformBuffer.isSync()) {
    if (_texture && MaterialFlags::ClearCoatTextureEnabled()) {
      uniformBuffer.updateFloat2("vClearCoatInfos",
                                 static_cast<float>(_texture->coordinatesIndex),
                                 _texture->level, "");
      MaterialHelper::BindTextureMatrix(*_texture, uniformBuffer, "clearCoat");
    }

    if (_bumpTexture && engine->getCaps().standardDerivatives
        && MaterialFlags::ClearCoatTextureEnabled() && !disableBumpMap) {
      uniformBuffer.updateFloat2(
        "vClearCoatBumpInfos",
        static_cast<float>(_bumpTexture->coordinatesIndex), _bumpTexture->level,
        "");
      MaterialHelper::BindTextureMatrix(*_bumpTexture, uniformBuffer,
                                        "clearCoatBump");

      if (scene->_mirroredCameraPosition) {
        uniformBuffer.updateFloat2("vClearCoatTangentSpaceParams",
                                   invertNormalMapX ? 1.f : -1.f,
                                   invertNormalMapY ? 1.f : -1.f, "");
      }
      else {
        uniformBuffer.updateFloat2("vClearCoatTangentSpaceParams",
                                   invertNormalMapX ? -1.f : 1.f,
                                   invertNormalMapY ? -1.f : 1.f, "");
      }
    }

    if (_tintTexture && MaterialFlags::ClearCoatTintTextureEnabled()) {
      uniformBuffer.updateFloat2(
        "vClearCoatTintInfos",
        static_cast<float>(_tintTexture->coordinatesIndex), _tintTexture->level,
        "");
      MaterialHelper::BindTextureMatrix(*_tintTexture, uniformBuffer,
                                        "clearCoatTint");
    }

    // Clear Coat General params
    uniformBuffer.updateFloat2("vClearCoatParams", intensity, roughness, "");

    // Clear Coat Refraction params
    const auto a = 1.f - _indexOfRefraction;
    const auto b = 1.f + _indexOfRefraction;
    // Schlicks approx: (ior1 - ior2) / (ior1 + ior2) where ior2 for air is
    // close to vacuum = 1.
    const auto f0  = std::pow((-a / b), 2.f);
    const auto eta = 1.f / _indexOfRefraction;
    uniformBuffer.updateFloat4("vClearCoatRefractionParams", f0, eta, a, b, "");

    if (_isTintEnabled) {
      uniformBuffer.updateFloat4("vClearCoatTintParams", tintColor.r,
                                 tintColor.g, tintColor.b,
                                 std::max(0.00001f, tintThickness), "");
      uniformBuffer.updateFloat("clearCoatColorAtDistance",
                                std::max(0.00001f, tintColorAtDistance));
    }
  }

  // Textures
  if (scene->texturesEnabled()) {
    if (_texture && MaterialFlags::ClearCoatTextureEnabled()) {
      uniformBuffer.setTexture("clearCoatSampler", _texture);
    }

    if (_bumpTexture && engine->getCaps().standardDerivatives
        && MaterialFlags::ClearCoatBumpTextureEnabled() && !disableBumpMap) {
      uniformBuffer.setTexture("clearCoatBumpSampler", _bumpTexture);
    }

    if (_isTintEnabled && _tintTexture
        && MaterialFlags::ClearCoatTintTextureEnabled()) {
      uniformBuffer.setTexture("clearCoatTintSampler", _tintTexture);
    }
  }
}

bool PBRClearCoatConfiguration::hasTexture(const BaseTexturePtr& iTexture) const
{
  if (_texture == iTexture) {
    return true;
  }

  if (_bumpTexture == iTexture) {
    return true;
  }

  if (_tintTexture == iTexture) {
    return true;
  }

  return false;
}

void PBRClearCoatConfiguration::getActiveTextures(
  std::vector<BaseTexturePtr>& activeTextures)
{
  if (_texture) {
    activeTextures.emplace_back(_texture);
  }

  if (_bumpTexture) {
    activeTextures.emplace_back(_bumpTexture);
  }

  if (_tintTexture) {
    activeTextures.emplace_back(_tintTexture);
  }
}

void PBRClearCoatConfiguration::getAnimatables(
  std::vector<IAnimatablePtr>& animatables)
{
  if (_texture && !_texture->animations.empty()) {
    animatables.emplace_back(_texture);
  }

  if (_bumpTexture && !_bumpTexture->animations.empty()) {
    animatables.emplace_back(_bumpTexture);
  }

  if (_tintTexture && !_tintTexture->animations.empty()) {
    animatables.emplace_back(_tintTexture);
  }
}

void PBRClearCoatConfiguration::dispose(bool forceDisposeTextures)
{
  if (forceDisposeTextures) {
    if (_texture) {
      _texture->dispose();
    }

    if (_bumpTexture) {
      _bumpTexture->dispose();
    }

    if (_tintTexture) {
      _tintTexture->dispose();
    }
  }
}

const std::string PBRClearCoatConfiguration::getClassName() const
{
  return "PBRClearCoatConfiguration";
}

unsigned int PBRClearCoatConfiguration::AddFallbacks(
  const IMaterialClearCoatDefines& defines, EffectFallbacks& fallbacks,
  unsigned int currentRank)
{
  if (defines["CLEARCOAT_BUMP"]) {
    fallbacks.addFallback(currentRank++, "CLEARCOAT_BUMP");
  }
  if (defines["CLEARCOAT_TINT"]) {
    fallbacks.addFallback(currentRank++, "CLEARCOAT_TINT");
  }
  if (defines["CLEARCOAT"]) {
    fallbacks.addFallback(currentRank++, "CLEARCOAT");
  }
  return currentRank;
}

void PBRClearCoatConfiguration::AddUniforms(std::vector<std::string>& uniforms)
{
  stl_util::concat(
    uniforms, {"vClearCoatTangentSpaceParams", "vClearCoatParams",
               "vClearCoatRefractionParams", "vClearCoatTintParams",
               "clearCoatColorAtDistance", "clearCoatMatrix",
               "clearCoatBumpMatrix", "clearCoatTintMatrix", "vClearCoatInfos",
               "vClearCoatBumpInfos", "vClearCoatTintInfos"});
}

void PBRClearCoatConfiguration::AddSamplers(std::vector<std::string>& samplers)
{
  stl_util::concat(samplers, {"clearCoatSampler", "clearCoatBumpSampler",
                              "clearCoatTintSampler"});
}

void PBRClearCoatConfiguration::PrepareUniformBuffer(
  UniformBuffer& uniformBuffer)
{
  uniformBuffer.addUniform("vClearCoatParams", 2);
  uniformBuffer.addUniform("vClearCoatRefractionParams", 4);
  uniformBuffer.addUniform("vClearCoatInfos", 2);
  uniformBuffer.addUniform("clearCoatMatrix", 16);
  uniformBuffer.addUniform("vClearCoatBumpInfos", 2);
  uniformBuffer.addUniform("vClearCoatTangentSpaceParams", 2);
  uniformBuffer.addUniform("clearCoatBumpMatrix", 16);
  uniformBuffer.addUniform("vClearCoatTintParams", 4);
  uniformBuffer.addUniform("clearCoatColorAtDistance", 1);
  uniformBuffer.addUniform("vClearCoatTintInfos", 2);
  uniformBuffer.addUniform("clearCoatTintMatrix", 16);
}

void PBRClearCoatConfiguration::copyTo(
  PBRClearCoatConfiguration& /*clearCoatConfiguration*/)
{
}

json PBRClearCoatConfiguration::serialize() const
{
  return nullptr;
}

void PBRClearCoatConfiguration::parse(const json& /*source*/, Scene* /*scene*/,
                                      const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
