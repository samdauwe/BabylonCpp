#include <babylon/materials/pbr/pbr_anisotropic_configuration.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_flags.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/pbr/imaterial_anisotropic_defines.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {

PBRAnisotropicConfiguration::PBRAnisotropicConfiguration(
  const std::function<void()>& markAllSubMeshesAsTexturesDirty)
    : isEnabled{this, &PBRAnisotropicConfiguration::get_isEnabled,
                &PBRAnisotropicConfiguration::set_isEnabled}
    , intensity{1.f}
    , direction{Vector2(1.f, 0.f)}
    , texture{this, &PBRAnisotropicConfiguration::get_texture,
              &PBRAnisotropicConfiguration::set_texture}
    , _isEnabled{false}
    , _texture{nullptr}
    , _internalMarkAllSubMeshesAsTexturesDirty{markAllSubMeshesAsTexturesDirty}
{
  isEnabled = false;
}

PBRAnisotropicConfiguration::~PBRAnisotropicConfiguration()
{
}

bool PBRAnisotropicConfiguration::get_isEnabled() const
{
  return _isEnabled;
}

void PBRAnisotropicConfiguration::set_isEnabled(bool value)
{
  if (_isEnabled == value) {
    return;
  }

  _isEnabled = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRAnisotropicConfiguration::get_texture()
{
  return _texture;
}

void PBRAnisotropicConfiguration::set_texture(const BaseTexturePtr& value)
{
  if (_texture == value) {
    return;
  }

  _texture = value;
  _markAllSubMeshesAsTexturesDirty();
}

void PBRAnisotropicConfiguration::_markAllSubMeshesAsTexturesDirty()
{
  _internalMarkAllSubMeshesAsTexturesDirty();
}

bool PBRAnisotropicConfiguration::isReadyForSubMesh(
  const IMaterialAnisotropicDefines& defines, Scene* scene) const
{
  if (defines._areTexturesDirty) {
    if (scene->texturesEnabled()) {
      if (_texture && MaterialFlags::AnisotropicTextureEnabled()) {
        if (!_texture->isReadyOrNotBlocking()) {
          return false;
        }
      }
    }
  }

  return true;
}

void PBRAnisotropicConfiguration::prepareDefines(
  IMaterialAnisotropicDefines& defines, const AbstractMeshPtr& mesh,
  Scene* scene)
{
  if (_isEnabled) {
    defines.boolDef["ANISOTROPIC"] = _isEnabled;
    if (_isEnabled && !mesh->isVerticesDataPresent(VertexBuffer::TangentKind)) {
      defines._needUVs           = true;
      defines.boolDef["MAINUV1"] = true;
    }

    if (defines._areTexturesDirty) {
      if (scene->texturesEnabled) {
        if (_texture && MaterialFlags::AnisotropicTextureEnabled()) {
          MaterialHelper::PrepareDefinesForMergedUV(_texture, defines,
                                                    "ANISOTROPIC_TEXTURE");
        }
        else {
          defines.boolDef["ANISOTROPIC_TEXTURE"] = false;
        }
      }
    }
  }
  else {
    defines.boolDef["ANISOTROPIC"]         = false;
    defines.boolDef["ANISOTROPIC_TEXTURE"] = false;
  }
}

void PBRAnisotropicConfiguration::bindForSubMesh(UniformBuffer& uniformBuffer,
                                                 Scene* scene, bool isFrozen)
{
  if (!uniformBuffer.useUbo() || !isFrozen || !uniformBuffer.isSync()) {
    if (_texture && MaterialFlags::AnisotropicTextureEnabled()) {
      uniformBuffer.updateFloat2("vAnisotropyInfos", _texture->coordinatesIndex,
                                 _texture->level, "");
      MaterialHelper::BindTextureMatrix(*_texture, uniformBuffer, "anisotropy");
    }

    // Anisotropy
    uniformBuffer.updateFloat3("vAnisotropy", direction.x, direction.y,
                               intensity, "");
  }

  // Textures
  if (scene->texturesEnabled()) {
    if (_texture && MaterialFlags::AnisotropicTextureEnabled()) {
      uniformBuffer.setTexture("anisotropySampler", _texture);
    }
  }
}

bool PBRAnisotropicConfiguration::hasTexture(
  const BaseTexturePtr& texture) const
{
  if (_texture == texture) {
    return true;
  }

  return false;
}

void PBRAnisotropicConfiguration::getActiveTextures(
  std::vector<BaseTexturePtr>& activeTextures)
{
  if (_texture) {
    activeTextures.emplace_back(_texture);
  }
}

void PBRAnisotropicConfiguration::getAnimatables(
  std::vector<IAnimatablePtr>& animatables)
{
  if (_texture && !_texture->animations.empty()) {
    animatables.emplace_back(_texture);
  }
}

void PBRAnisotropicConfiguration::dispose(bool forceDisposeTextures)
{
  if (forceDisposeTextures) {
    if (_texture) {
      _texture->dispose();
    }
  }
}

const std::string PBRAnisotropicConfiguration::getClassName() const
{
  return "PBRAnisotropicConfiguration";
}

unsigned int PBRAnisotropicConfiguration::AddFallbacks(
  const IMaterialAnisotropicDefines& defines, EffectFallbacks& fallbacks,
  unsigned int currentRank)
{
  if (defines["ANISOTROPIC"]) {
    fallbacks.addFallback(currentRank++, "ANISOTROPIC");
  }
  return currentRank;
}

void PBRAnisotropicConfiguration::AddUniforms(
  std::vector<std::string>& uniforms)
{
  stl_util::concat(uniforms,
                   {"vAnisotropy", "vAnisotropyInfos", "anisotropyMatrix"});
}

void PBRAnisotropicConfiguration::PrepareUniformBuffer(
  UniformBuffer& uniformBuffer)
{
  uniformBuffer.addUniform("vAnisotropy", 3);
  uniformBuffer.addUniform("vAnisotropyInfos", 2);
  uniformBuffer.addUniform("anisotropyMatrix", 16);
}

void PBRAnisotropicConfiguration::AddSamplers(
  std::vector<std::string>& samplers)
{
  samplers.emplace_back("anisotropySampler");
}

void PBRAnisotropicConfiguration::copyTo(
  PBRAnisotropicConfiguration& /*anisotropicConfiguration*/)
{
}

json PBRAnisotropicConfiguration::serialize() const
{
  return nullptr;
}

void PBRAnisotropicConfiguration::parse(const json& /*source*/)
{
}

} // end of namespace BABYLON
