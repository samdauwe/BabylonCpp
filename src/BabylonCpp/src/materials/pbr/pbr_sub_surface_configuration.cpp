#include <babylon/materials/pbr/pbr_sub_surface_configuration.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_flags.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/pbr/imaterial_sub_surface_defines.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/uniform_buffer.h>

namespace BABYLON {

PBRSubSurfaceConfiguration::PBRSubSurfaceConfiguration(
  const std::function<void()>& markAllSubMeshesAsTexturesDirty)
    : isRefractionEnabled{this,
                          &PBRSubSurfaceConfiguration::get_isRefractionEnabled,
                          &PBRSubSurfaceConfiguration::set_isRefractionEnabled}
    , isTranslucencyEnabled{this,
                            &PBRSubSurfaceConfiguration::
                              get_isTranslucencyEnabled,
                            &PBRSubSurfaceConfiguration::
                              set_isTranslucencyEnabled}
    , refractionIntensity{1.f}
    , translucencyIntensity{1.f}
    , scatteringIntensity{1.f}
    , thicknessTexture{this, &PBRSubSurfaceConfiguration::get_thicknessTexture,
                       &PBRSubSurfaceConfiguration::set_thicknessTexture}
    , refractionTexture{this,
                        &PBRSubSurfaceConfiguration::get_refractionTexture,
                        &PBRSubSurfaceConfiguration::set_refractionTexture}
    , indexOfRefraction{this,
                        &PBRSubSurfaceConfiguration::get_indexOfRefraction,
                        &PBRSubSurfaceConfiguration::set_indexOfRefraction}
    , invertRefractionY{this,
                        &PBRSubSurfaceConfiguration::get_invertRefractionY,
                        &PBRSubSurfaceConfiguration::set_invertRefractionY}
    , linkRefractionWithTransparency{this,
                                     &PBRSubSurfaceConfiguration::
                                       get_linkRefractionWithTransparency,
                                     &PBRSubSurfaceConfiguration::
                                       set_linkRefractionWithTransparency}
    , minimumThickness{0.f}
    , maximumThickness{1.f}
    , tintColor{Color3::White()}
    , tintColorAtDistance{1.f}
    , diffusionDistance{Color3::White()}
    , useMaskFromThicknessTexture{this,
                                  &PBRSubSurfaceConfiguration::
                                    get_useMaskFromThicknessTexture,
                                  &PBRSubSurfaceConfiguration::
                                    set_useMaskFromThicknessTexture}
    , _isRefractionEnabled{false}
    , _isTranslucencyEnabled{false}
    , _isScatteringEnabled{false}
    , _thicknessTexture{nullptr}
    , _refractionTexture{nullptr}
    , _indexOfRefraction{1.f}
    , _invertRefractionY{false}
    , _linkRefractionWithTransparency{false}
    , _useMaskFromThicknessTexture{false}
    , _internalMarkAllSubMeshesAsTexturesDirty{markAllSubMeshesAsTexturesDirty}
{
}

PBRSubSurfaceConfiguration::~PBRSubSurfaceConfiguration() = default;

bool PBRSubSurfaceConfiguration::get_isRefractionEnabled() const
{
  return _isRefractionEnabled;
}

void PBRSubSurfaceConfiguration::set_isRefractionEnabled(bool value)
{
  if (_isRefractionEnabled == value) {
    return;
  }

  _isRefractionEnabled = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRSubSurfaceConfiguration::get_isTranslucencyEnabled() const
{
  return _isTranslucencyEnabled;
}

void PBRSubSurfaceConfiguration::set_isTranslucencyEnabled(bool value)
{
  if (_isTranslucencyEnabled == value) {
    return;
  }

  _isTranslucencyEnabled = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRSubSurfaceConfiguration::get_thicknessTexture()
{
  return _thicknessTexture;
}

void PBRSubSurfaceConfiguration::set_thicknessTexture(
  const BaseTexturePtr& value)
{
  if (_thicknessTexture == value) {
    return;
  }

  _thicknessTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRSubSurfaceConfiguration::get_refractionTexture()
{
  return _refractionTexture;
}

void PBRSubSurfaceConfiguration::set_refractionTexture(
  const BaseTexturePtr& value)
{
  if (_refractionTexture == value) {
    return;
  }

  _refractionTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

float PBRSubSurfaceConfiguration::get_indexOfRefraction() const
{
  return _indexOfRefraction;
}

void PBRSubSurfaceConfiguration::set_indexOfRefraction(float value)
{
  if (stl_util::almost_equal(_indexOfRefraction, value)) {
    return;
  }

  _indexOfRefraction = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRSubSurfaceConfiguration::get_invertRefractionY() const
{
  return _invertRefractionY;
}

void PBRSubSurfaceConfiguration::set_invertRefractionY(bool value)
{
  if (_invertRefractionY == value) {
    return;
  }

  _invertRefractionY = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRSubSurfaceConfiguration::get_linkRefractionWithTransparency() const
{
  return _linkRefractionWithTransparency;
}

void PBRSubSurfaceConfiguration::set_linkRefractionWithTransparency(bool value)
{
  if (_linkRefractionWithTransparency == value) {
    return;
  }

  _linkRefractionWithTransparency = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRSubSurfaceConfiguration::get_useMaskFromThicknessTexture() const
{
  return _useMaskFromThicknessTexture;
}

void PBRSubSurfaceConfiguration::set_useMaskFromThicknessTexture(bool value)
{
  if (_useMaskFromThicknessTexture == value) {
    return;
  }

  _useMaskFromThicknessTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

void PBRSubSurfaceConfiguration::_markAllSubMeshesAsTexturesDirty()
{
  _internalMarkAllSubMeshesAsTexturesDirty();
}

bool PBRSubSurfaceConfiguration::isReadyForSubMesh(
  const MaterialDefines& defines, Scene* scene) const
{
  if (defines._areTexturesDirty) {
    if (scene->texturesEnabled()) {
      if (_thicknessTexture && MaterialFlags::ThicknessTextureEnabled()) {
        if (!_thicknessTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      auto iRefractionTexture = _getRefractionTexture(scene);
      if (iRefractionTexture && MaterialFlags::RefractionTextureEnabled()) {
        if (!iRefractionTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }
    }
  }

  return true;
}

void PBRSubSurfaceConfiguration::prepareDefines(MaterialDefines& defines,
                                                Scene* scene)
{
  if (defines._areTexturesDirty) {
    defines.boolDef["SUBSURFACE"] = false;

    defines.boolDef["SS_TRANSLUCENCY"]                 = _isTranslucencyEnabled;
    defines.boolDef["SS_SCATERRING"]                   = _isScatteringEnabled;
    defines.boolDef["SS_THICKNESSANDMASK_TEXTURE"]     = false;
    defines.boolDef["SS_MASK_FROM_THICKNESS_TEXTURE"]  = false;
    defines.boolDef["SS_REFRACTION"]                   = false;
    defines.boolDef["SS_REFRACTIONMAP_3D"]             = false;
    defines.boolDef["SS_GAMMAREFRACTION"]              = false;
    defines.boolDef["SS_RGBDREFRACTION"]               = false;
    defines.boolDef["SS_LINEARSPECULARREFRACTION"]     = false;
    defines.boolDef["SS_REFRACTIONMAP_OPPOSITEZ"]      = false;
    defines.boolDef["SS_LODINREFRACTIONALPHA"]         = false;
    defines.boolDef["SS_LINKREFRACTIONTOTRANSPARENCY"] = false;

    if (_isRefractionEnabled || _isTranslucencyEnabled
        || _isScatteringEnabled) {
      defines.boolDef["SUBSURFACE"] = true;

      if (defines._areTexturesDirty) {
        if (scene->texturesEnabled()) {
          if (_thicknessTexture && MaterialFlags::ThicknessTextureEnabled()) {
            MaterialHelper::PrepareDefinesForMergedUV(
              _thicknessTexture, defines, "SS_THICKNESSANDMASK_TEXTURE");
          }
        }
      }

      defines.boolDef["SS_MASK_FROM_THICKNESS_TEXTURE"]
        = _useMaskFromThicknessTexture;
    }

    if (_isRefractionEnabled) {
      if (scene->texturesEnabled()) {
        auto refractTexture = _getRefractionTexture(scene);
        if (refractTexture && MaterialFlags::RefractionTextureEnabled()) {
          defines.boolDef["SS_REFRACTION"]       = true;
          defines.boolDef["SS_REFRACTIONMAP_3D"] = refractTexture->isCube;
          defines.boolDef["SS_GAMMAREFRACTION"]  = refractTexture->gammaSpace;
          defines.boolDef["SS_RGBDREFRACTION"]   = refractTexture->isRGBD;
          defines.boolDef["SS_LINEARSPECULARREFRACTION"]
            = refractTexture->linearSpecularLOD();
          defines.boolDef["SS_REFRACTIONMAP_OPPOSITEZ"]
            = refractTexture->invertZ;
          defines.boolDef["SS_LODINREFRACTIONALPHA"]
            = refractTexture->lodLevelInAlpha;
          defines.boolDef["SS_LINKREFRACTIONTOTRANSPARENCY"]
            = _linkRefractionWithTransparency;
        }
      }
    }
  }
}

void PBRSubSurfaceConfiguration::bindForSubMesh(UniformBuffer& uniformBuffer,
                                                Scene* scene,
                                                Engine* /*engine*/,
                                                bool isFrozen,
                                                bool lodBasedMicrosurface)
{
  auto refractTexture = _getRefractionTexture(scene);

  if (!uniformBuffer.useUbo() || !isFrozen || !uniformBuffer.isSync()) {
    if (_thicknessTexture && MaterialFlags::ThicknessTextureEnabled()) {
      uniformBuffer.updateFloat2(
        "vThicknessInfos",
        static_cast<float>(_thicknessTexture->coordinatesIndex),
        _thicknessTexture->level, "");
      MaterialHelper::BindTextureMatrix(*_thicknessTexture, uniformBuffer,
                                        "thickness");
    }

    uniformBuffer.updateFloat2("vThicknessParam", minimumThickness,
                               maximumThickness - minimumThickness, "");

    if (refractTexture && MaterialFlags::RefractionTextureEnabled()) {
      uniformBuffer.updateMatrix("refractionMatrix",
                                 *refractTexture->getReflectionTextureMatrix());

      auto depth = 1.f;
      if (!refractTexture->isCube) {
        /*if (refractionTexture.depth) {
            depth = refractionTexture.depth;
        }*/
      }
      uniformBuffer.updateFloat4("vRefractionInfos", refractTexture->level,
                                 1.f / _indexOfRefraction, depth,
                                 _invertRefractionY ? -1.f : 1.f, "");
      uniformBuffer.updateFloat3(
        "vRefractionMicrosurfaceInfos",
        static_cast<float>(refractTexture->getSize().width),
        refractTexture->lodGenerationScale, refractTexture->lodGenerationOffset,
        "");
    }

    uniformBuffer.updateColor3("vDiffusionDistance", diffusionDistance, "");

    uniformBuffer.updateFloat4("vTintColor", tintColor.r, tintColor.g,
                               tintColor.b, tintColorAtDistance, "");

    uniformBuffer.updateFloat3("vSubSurfaceIntensity", refractionIntensity,
                               translucencyIntensity, scatteringIntensity, "");
  }

  // Textures
  if (scene->texturesEnabled()) {
    if (_thicknessTexture && MaterialFlags::ThicknessTextureEnabled()) {
      uniformBuffer.setTexture("thicknessSampler", _thicknessTexture);
    }

    if (refractTexture && MaterialFlags::RefractionTextureEnabled()) {
      if (lodBasedMicrosurface) {
        uniformBuffer.setTexture("refractionSampler", refractTexture);
      }
      else {
        uniformBuffer.setTexture("refractionSampler",
                                 refractTexture->_lodTextureMid() ?
                                   refractTexture->_lodTextureMid() :
                                   refractTexture);
        uniformBuffer.setTexture("refractionSamplerLow",
                                 refractTexture->_lodTextureLow() ?
                                   refractTexture->_lodTextureLow() :
                                   refractTexture);
        uniformBuffer.setTexture("refractionSamplerHigh",
                                 refractTexture->_lodTextureHigh() ?
                                   refractTexture->_lodTextureHigh() :
                                   refractTexture);
      }
    }
  }
}

bool PBRSubSurfaceConfiguration::unbind(Effect* activeEffect)
{
  if (_refractionTexture && _refractionTexture->isRenderTarget) {
    activeEffect->setTexture("refractionSampler", nullptr);
    return true;
  }

  return false;
}

BaseTexturePtr
PBRSubSurfaceConfiguration::_getRefractionTexture(Scene* scene) const
{
  if (_refractionTexture) {
    return _refractionTexture;
  }

  if (_isRefractionEnabled) {
    return scene->environmentTexture();
  }

  return nullptr;
}

bool PBRSubSurfaceConfiguration::disableAlphaBlending() const
{
  return isRefractionEnabled && _linkRefractionWithTransparency;
}

void PBRSubSurfaceConfiguration::fillRenderTargetTextures(
  std::vector<RenderTargetTexturePtr>& renderTargets)
{
  if (MaterialFlags::RefractionTextureEnabled() && _refractionTexture
      && _refractionTexture->isRenderTarget) {
    renderTargets.emplace_back(
      std::static_pointer_cast<RenderTargetTexture>(_refractionTexture));
  }
}

bool PBRSubSurfaceConfiguration::hasTexture(const BaseTexturePtr& texture) const
{
  if (_thicknessTexture == texture) {
    return true;
  }

  if (_refractionTexture == texture) {
    return true;
  }

  return false;
}

bool PBRSubSurfaceConfiguration::hasRenderTargetTextures() const
{
  return MaterialFlags::RefractionTextureEnabled() && _refractionTexture

         && _refractionTexture->isRenderTarget;
}

void PBRSubSurfaceConfiguration::getActiveTextures(
  std::vector<BaseTexturePtr>& activeTextures)
{
  if (_thicknessTexture) {
    activeTextures.emplace_back(_thicknessTexture);
  }

  if (_refractionTexture) {
    activeTextures.emplace_back(_refractionTexture);
  }
}

void PBRSubSurfaceConfiguration::getAnimatables(
  std::vector<IAnimatablePtr>& animatables)
{
  if (_thicknessTexture && !_thicknessTexture->animations.empty()) {
    animatables.emplace_back(_thicknessTexture);
  }

  if (_refractionTexture && !_refractionTexture->animations.empty()) {
    animatables.emplace_back(_refractionTexture);
  }
}

void PBRSubSurfaceConfiguration::dispose(bool forceDisposeTextures)
{
  if (forceDisposeTextures) {
    if (_thicknessTexture) {
      _thicknessTexture->dispose();
    }

    if (_refractionTexture) {
      _refractionTexture->dispose();
    }
  }
}

std::string PBRSubSurfaceConfiguration::getClassName() const
{
  return "PBRSubSurfaceConfiguration";
}

unsigned int
PBRSubSurfaceConfiguration::AddFallbacks(const MaterialDefines& defines,
                                         EffectFallbacks& fallbacks,
                                         unsigned int currentRank)
{
  if (defines["SS_SCATERRING"]) {
    fallbacks.addFallback(currentRank++, "SS_SCATERRING");
  }
  if (defines["SS_TRANSLUCENCY"]) {
    fallbacks.addFallback(currentRank++, "SS_TRANSLUCENCY");
  }
  return currentRank;
}

void PBRSubSurfaceConfiguration::AddUniforms(std::vector<std::string>& uniforms)
{
  stl_util::concat(uniforms,
                   {"vDiffusionDistance", "vTintColor", "vSubSurfaceIntensity",
                    "vRefractionMicrosurfaceInfos", "vRefractionInfos",
                    "vThicknessInfos", "vThicknessParam", "refractionMatrix",
                    "thicknessMatrix"});
}

void PBRSubSurfaceConfiguration::AddSamplers(std::vector<std::string>& samplers)
{
  stl_util::concat(samplers, {"thicknessSampler", "refractionSampler",
                              "refractionSamplerLow", "refractionSamplerHigh"});
}

void PBRSubSurfaceConfiguration::PrepareUniformBuffer(
  UniformBuffer& uniformBuffer)
{
  uniformBuffer.addUniform("vRefractionMicrosurfaceInfos", 3);
  uniformBuffer.addUniform("vRefractionInfos", 4);
  uniformBuffer.addUniform("refractionMatrix", 16);
  uniformBuffer.addUniform("vThicknessInfos", 2);
  uniformBuffer.addUniform("thicknessMatrix", 16);
  uniformBuffer.addUniform("vThicknessParam", 2);
  uniformBuffer.addUniform("vDiffusionDistance", 3);
  uniformBuffer.addUniform("vTintColor", 4);
  uniformBuffer.addUniform("vSubSurfaceIntensity", 3);
}

void PBRSubSurfaceConfiguration::copyTo(
  PBRSubSurfaceConfiguration& /*configuration*/)
{
}

json PBRSubSurfaceConfiguration::serialize() const
{
  return nullptr;
}

void PBRSubSurfaceConfiguration::parse(const json& /*source*/, Scene* /*scene*/,
                                       const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
