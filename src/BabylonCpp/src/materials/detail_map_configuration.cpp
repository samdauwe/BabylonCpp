#include <babylon/materials/detail_map_configuration.h>

#include <babylon/animations/ianimatable.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/imaterial_detail_map_defines.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_flags.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/uniform_buffer.h>

namespace BABYLON {

DetailMapConfiguration::DetailMapConfiguration(
  const std::function<void()>& markAllSubMeshesAsTexturesDirty)
    : texture{this, &DetailMapConfiguration::get_texture, &DetailMapConfiguration::set_texture}
    , diffuseBlendLevel{0.5f}
    , roughnessBlendLevel{0.5f}
    , bumpLevel{1.f}
    , normalBlendMethod{this, &DetailMapConfiguration::get_normalBlendMethod,
                        &DetailMapConfiguration::set_normalBlendMethod}
    , isEnabled{this, &DetailMapConfiguration::get_isEnabled,
                &DetailMapConfiguration::set_isEnabled}
    , _texture{nullptr}
    , _normalBlendMethod{Material::MATERIAL_NORMALBLENDMETHOD_WHITEOUT}
    , _isEnabled{false}
{
  _internalMarkAllSubMeshesAsTexturesDirty = markAllSubMeshesAsTexturesDirty;
}

DetailMapConfiguration::~DetailMapConfiguration() = default;

BaseTexturePtr& DetailMapConfiguration::get_texture()
{
  return _texture;
}

void DetailMapConfiguration::set_texture(const BaseTexturePtr& value)
{
  if (_texture == value) {
    return;
  }

  _texture = value;
  _markAllSubMeshesAsTexturesDirty();
}

unsigned int DetailMapConfiguration::get_normalBlendMethod() const
{
  return _normalBlendMethod;
}

void DetailMapConfiguration::set_normalBlendMethod(unsigned int value)
{
  if (_normalBlendMethod == value) {
    return;
  }

  _normalBlendMethod = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool DetailMapConfiguration::get_isEnabled() const
{
  return _isEnabled;
}

void DetailMapConfiguration::set_isEnabled(bool value)
{
  if (_isEnabled == value) {
    return;
  }

  _isEnabled = value;
  _markAllSubMeshesAsTexturesDirty();
}

void DetailMapConfiguration::_markAllSubMeshesAsTexturesDirty()
{
  if (_internalMarkAllSubMeshesAsTexturesDirty) {
    _internalMarkAllSubMeshesAsTexturesDirty();
  }
}

bool DetailMapConfiguration::isReadyForSubMesh(IMaterialDetailMapDefines& defines, Scene* scene)
{
  const auto engine = scene->getEngine();

  if (defines._areTexturesDirty && scene->texturesEnabled()) {
    if (engine->getCaps().standardDerivatives && _texture
        && MaterialFlags::DetailTextureEnabled()) {
      // Detail texture cannot be not blocking.
      if (!_texture->isReady()) {
        return false;
      }
    }
  }

  return true;
}

void DetailMapConfiguration::prepareDefines(IMaterialDetailMapDefines& defines, Scene* scene)
{
  if (_isEnabled) {
    defines.intDef["DETAIL_NORMALBLENDMETHOD"] = static_cast<int>(_normalBlendMethod);

    const auto engine = scene->getEngine();

    if (defines._areTexturesDirty) {
      if (engine->getCaps().standardDerivatives && _texture && MaterialFlags::DetailTextureEnabled()
          && _isEnabled) {
        MaterialHelper::PrepareDefinesForMergedUV(_texture, defines, "DETAIL");
        defines.intDef["DETAIL_NORMALBLENDMETHOD"] = static_cast<int>(_normalBlendMethod);
      }
      else {
        defines.boolDef["DETAIL"] = false;
      }
    }
  }
  else {
    defines.boolDef["DETAIL"] = false;
  }
}

void DetailMapConfiguration::bindForSubMesh(UniformBuffer& uniformBuffer, Scene* scene,
                                            bool isFrozen)
{
  if (!_isEnabled) {
    return;
  }

  if (!uniformBuffer.useUbo() || !isFrozen || !uniformBuffer.isSync()) {
    if (_texture && MaterialFlags::DetailTextureEnabled()) {
      uniformBuffer.updateFloat4("vDetailInfos", static_cast<float>(_texture->coordinatesIndex),
                                 diffuseBlendLevel, bumpLevel, roughnessBlendLevel, "");
      MaterialHelper::BindTextureMatrix(*_texture, uniformBuffer, "detail");
    }
  }

  // Textures
  if (scene->texturesEnabled()) {
    if (_texture && MaterialFlags::DetailTextureEnabled()) {
      uniformBuffer.setTexture("detailSampler", _texture);
    }
  }
}

bool DetailMapConfiguration::hasTexture(const BaseTexturePtr& iTexture) const
{
  if (_texture == iTexture) {
    return true;
  }

  return false;
}

void DetailMapConfiguration::getActiveTextures(std::vector<BaseTexturePtr>& activeTextures)
{
  if (_texture) {
    activeTextures.emplace_back(_texture);
  }
}

void DetailMapConfiguration::getAnimatables(std::vector<IAnimatablePtr>& animatables)
{
  if (_texture && !_texture->animations.empty()) {
    animatables.emplace_back(_texture);
  }
}

void DetailMapConfiguration::dispose(bool forceDisposeTextures)
{
  if (forceDisposeTextures && _texture) {
    _texture->dispose();
  }
}

std::string DetailMapConfiguration::getClassName() const
{
  return "DetailMap";
}

void DetailMapConfiguration::AddUniforms(std::vector<std::string>& uniforms)
{
  uniforms.emplace_back("vDetailInfos");
}

void DetailMapConfiguration::AddSamplers(std::vector<std::string>& samplers)
{
  samplers.emplace_back("detailSampler");
}

void DetailMapConfiguration::PrepareUniformBuffer(UniformBuffer& uniformBuffer)
{
  uniformBuffer.addUniform("vDetailInfos", 4);
  uniformBuffer.addUniform("detailMatrix", 16);
}

void DetailMapConfiguration::copyTo(DetailMapConfiguration& detailMap)
{
  detailMap.texture             = texture();
  detailMap.diffuseBlendLevel   = diffuseBlendLevel;
  detailMap.roughnessBlendLevel = roughnessBlendLevel;
  detailMap.bumpLevel           = bumpLevel;
  detailMap.normalBlendMethod   = normalBlendMethod();
  detailMap.isEnabled           = isEnabled();

  detailMap._internalMarkAllSubMeshesAsTexturesDirty = _internalMarkAllSubMeshesAsTexturesDirty;
}

json DetailMapConfiguration::serialize() const
{
  return nullptr;
}

void DetailMapConfiguration::parse(const json& /*source*/, Scene* /*scene*/,
                                   const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
