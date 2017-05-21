#include <babylon/materials/shader_material.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

ShaderMaterial::ShaderMaterial(const std::string& iName, Scene* scene,
                               const std::string& shaderPath,
                               const ShaderMaterialOptions& options)
    : Material{iName, scene}, _shaderPath{shaderPath}, _renderId{-1}
{
  _options.needAlphaBlending = options.needAlphaBlending;
  _options.needAlphaTesting  = options.needAlphaTesting;
  _options.attributes        = options.attributes;
  _options.uniforms          = options.uniforms;
  _options.uniformBuffers    = options.uniformBuffers;
  _options.samplers          = options.samplers;
  _options.defines           = options.defines;
}

ShaderMaterial::~ShaderMaterial()
{
}

const char* ShaderMaterial::getClassName() const
{
  return "ShaderMaterial";
}

IReflect::Type ShaderMaterial::type() const
{
  return IReflect::Type::SHADERMATERIAL;
}

bool ShaderMaterial::needAlphaBlending()
{
  return _options.needAlphaBlending;
}

bool ShaderMaterial::needAlphaTesting()
{
  return _options.needAlphaTesting;
}

void ShaderMaterial::_checkUniform(const std::string& uniformName)
{
  if (!stl_util::contains(_options.uniforms, uniformName)) {
    _options.uniforms.emplace_back(uniformName);
  }
}

ShaderMaterial& ShaderMaterial::setTexture(const std::string& iName,
                                           Texture* texture)
{
  if (!stl_util::contains(_options.samplers, iName)) {
    _options.samplers.emplace_back(iName);
  }
  _textures[name] = texture;

  return *this;
}

ShaderMaterial&
ShaderMaterial::setTextureArray(const std::string& iName,
                                const std::vector<BaseTexture*>& textures)
{
  if (!stl_util::contains(_options.samplers, iName)) {
    _options.samplers.emplace_back(iName);
  }

  _checkUniform(name);

  _textureArrays[name] = textures;

  return *this;
}

ShaderMaterial& ShaderMaterial::setFloat(const std::string& iName, float value)
{
  _checkUniform(iName);
  _floats[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setFloats(const std::string& iName,
                                          const std::vector<float>& value)
{
  _checkUniform(iName);
  _floatsArrays[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setColor3(const std::string& iName,
                                          const Color3& value)
{
  _checkUniform(iName);
  _colors3[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setColor4(const std::string& iName,
                                          const Color4& value)
{
  _checkUniform(iName);
  _colors4[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setVector2(const std::string& iName,
                                           const Vector2& value)
{
  _checkUniform(iName);
  _vectors2[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setVector3(const std::string& iName,
                                           const Vector3& value)
{
  _checkUniform(iName);
  _vectors3[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setVector4(const std::string& iName,
                                           const Vector4& value)
{
  _checkUniform(iName);
  _vectors4[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setMatrix(const std::string& iName,
                                          const Matrix& value)
{
  _checkUniform(iName);
  _matrices[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setMatrix3x3(const std::string& iName,
                                             const Float32Array& value)
{
  _checkUniform(iName);
  _matrices3x3[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setMatrix2x2(const std::string& iName,
                                             const Float32Array& value)
{
  _checkUniform(iName);
  _matrices2x2[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setArray3(const std::string& iName,
                                          const Float32Array& value)
{
  _checkUniform(iName);
  _vectors3Arrays[iName] = value;

  return *this;
}

bool ShaderMaterial::_checkCache(Scene* /*scene*/, AbstractMesh* mesh,
                                 bool useInstances)
{
  if (!mesh) {
    return true;
  }

  if (_effect
      && (_effect->defines.find("#define INSTANCES") != std::string::npos)
           != useInstances) {
    return false;
  }

  return false;
}

bool ShaderMaterial::isReady(AbstractMesh* mesh, bool useInstances)
{
  auto scene  = getScene();
  auto engine = scene->getEngine();

  if (!checkReadyOnEveryCall) {
    if (_renderId == scene->getRenderId()) {
      if (_checkCache(scene, mesh, useInstances)) {
        return true;
      }
    }
  }

  // Instances
  std::vector<std::string> defines;
  auto fallbacks = std::make_unique<EffectFallbacks>();
  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
  }

  for (auto& _define : _options.defines) {
    defines.emplace_back(_define);
  }

  // Bones
  if (mesh && mesh->useBones() && mesh->computeBonesUsingShaders()) {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS "
                         + std::to_string(mesh->numBoneInfluencers()));
    defines.emplace_back("#define BonesPerMesh "
                         + std::to_string(mesh->skeleton()->bones.size() + 1));
    fallbacks->addCPUSkinningFallback(0, mesh);
  }

  // Textures
  for (auto& item : _textures) {
    if (!item.second->isReady()) {
      return false;
    }
  }

  // Alpha test
  if (engine->getAlphaTesting()) {
    defines.emplace_back("#define ALPHATEST");
  }

  auto previousEffect = _effect;
  auto join           = String::join(defines, '\n');

  EffectCreationOptions options;
  options.attributes          = _options.attributes;
  options.uniformsNames       = _options.uniforms;
  options.uniformBuffersNames = _options.uniformBuffers;
  options.samplers            = _options.samplers;
  options.defines             = std::move(join);
  options.fallbacks           = std::move(fallbacks);
  options.onCompiled          = onCompiled;
  options.onError             = onError;

  _effect = engine->createEffect(_shaderPath, options, engine);

  if (!_effect->isReady()) {
    return false;
  }

  if (previousEffect != _effect) {
    scene->resetCachedMaterial();
  }

  _renderId = scene->getRenderId();

  return true;
}

void ShaderMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  auto scene = getScene();

  if (!stl_util::contains(_options.uniforms, "world")) {
    _effect->setMatrix("world", world);
  }

  if (!stl_util::contains(_options.uniforms, "worldView")) {
    world.multiplyToRef(scene->getViewMatrix(), _cachedWorldViewMatrix);
    _effect->setMatrix("worldView", _cachedWorldViewMatrix);
  }

  if (!stl_util::contains(_options.uniforms, "worldViewProjection")) {
    auto transformMatrix = scene->getTransformMatrix();
    _effect->setMatrix("worldViewProjection", world.multiply(transformMatrix));
  }
}

void ShaderMaterial::bind(Matrix* world, Mesh* mesh)
{
  // Std values
  bindOnlyWorldMatrix(*world);

  if (getScene()->getCachedMaterial() != this) {
    if (!stl_util::contains(_options.uniforms, "view")) {
      _effect->setMatrix("view", getScene()->getViewMatrix());
    }

    if (!stl_util::contains(_options.uniforms, "projection")) {
      _effect->setMatrix("projection", getScene()->getProjectionMatrix());
    }

    if (!stl_util::contains(_options.uniforms, "viewProjection")) {
      _effect->setMatrix("viewProjection", getScene()->getTransformMatrix());
    }

    // Bones
    MaterialHelper::BindBonesParameters(mesh, _effect);

    // Texture
    for (auto& kv : _textures) {
      _effect->setTexture(kv.first, kv.second);
    }

    // Texture arrays
    for (auto& kv : _textureArrays) {
      _effect->setTextureArray(kv.first, kv.second);
    }

    // Float
    for (auto& kv : _floats) {
      _effect->setFloat(kv.first, kv.second);
    }

    // Floats
    for (auto& kv : _floatsArrays) {
      _effect->setArray(kv.first, kv.second);
    }

    // Color3
    for (auto& kv : _colors3) {
      _effect->setColor3(kv.first, kv.second);
    }

    // Color4
    for (auto& kv : _colors4) {
      Color4& color = kv.second;
      _effect->setFloat4(kv.first, color.r, color.g, color.b, color.a);
    }

    // Vector2
    for (auto& kv : _vectors2) {
      _effect->setVector2(kv.first, kv.second);
    }

    // Vector3
    for (auto& kv : _vectors3) {
      _effect->setVector3(kv.first, kv.second);
    }

    // Vector4
    for (auto& kv : _vectors4) {
      _effect->setVector4(kv.first, kv.second);
    }

    // Matrix
    for (auto& kv : _matrices) {
      _effect->setMatrix(kv.first, kv.second);
    }

    // Matrix 3x3
    for (auto& kv : _matrices3x3) {
      _effect->setMatrix3x3(kv.first, kv.second);
    }

    // Matrix 2x2
    for (auto& kv : _matrices2x2) {
      _effect->setMatrix2x2(kv.first, kv.second);
    }

    // Vector3Array
    for (auto& kv : _vectors3Arrays) {
      _effect->setArray3(kv.first, kv.second);
    }
  }

  _afterBind(mesh);
}

Material* ShaderMaterial::clone(const std::string& iName,
                                bool /*cloneChildren*/) const
{
  return ShaderMaterial::New(iName, getScene(), _shaderPath, _options);
}

void ShaderMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (forceDisposeTextures) {
    for (auto& kv : _textures) {
      kv.second->dispose();
    }

    for (auto& kv : _textureArrays) {
      for (auto& textureArray : kv.second) {
        textureArray->dispose();
      }
    }
  }

  _textures.clear();

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Json::object ShaderMaterial::serialize() const
{
  return Json::object();
}

std::unique_ptr<ShaderMaterial>
ShaderMaterial::Parse(const Json::value& /*source*/, Scene* /*scene*/,
                      const std::string& /*url*/)
{
  return nullptr;
}

} // end of namespace BABYLON
