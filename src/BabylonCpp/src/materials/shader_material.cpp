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
#include <babylon/mesh/vertex_buffer.h>
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
  _textures[iName] = texture;

  return *this;
}

ShaderMaterial&
ShaderMaterial::setTextureArray(const std::string& iName,
                                const std::vector<BaseTexture*>& textures)
{
  if (!stl_util::contains(_options.samplers, iName)) {
    _options.samplers.emplace_back(iName);
  }

  _checkUniform(iName);

  _textureArrays[iName] = textures;

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

ShaderMaterial& ShaderMaterial::setColor3Array(const std::string& iName,
                                               const std::vector<Color3>& value)
{
  _checkUniform(iName);
  _colors3Arrays[name].clear();
  for (const auto& color : value) {
    Float32Array arr(3);
    color.toArray(arr, 0);
    stl_util::concat(_colors3Arrays[name], arr);
  }

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
  std::vector<std::string> attribs;
  auto fallbacks = ::std::make_unique<EffectFallbacks>();
  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
  }

  for (auto& _define : _options.defines) {
    defines.emplace_back(_define);
  }

  for (std::size_t index = 0; index < _options.attributes.size(); ++index) {
    attribs.emplace_back(_options.attributes[index]);
  }

  if (mesh && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
    attribs.emplace_back(VertexBuffer::ColorKindChars);
    defines.emplace_back("#define VERTEXCOLOR");
  }

  // Bones
  if (mesh && mesh->useBones() && mesh->computeBonesUsingShaders()) {
    attribs.emplace_back(VertexBuffer::MatricesIndicesKindChars);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKindChars);
    if (mesh->numBoneInfluencers() > 4) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKindChars);
      attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKindChars);
    }
    defines.emplace_back("#define NUM_BONE_INFLUENCERS "
                         + ::std::to_string(mesh->numBoneInfluencers()));
    defines.emplace_back(
      "#define BonesPerMesh "
      + ::std::to_string(mesh->skeleton()->bones.size() + 1));
    fallbacks->addCPUSkinningFallback(0, mesh);

    if (!stl_util::contains(_options.uniforms, "mBones")) {
      _options.uniforms.emplace_back("mBones");
    }
  }
  else {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
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
  options.attributes          = ::std::move(attribs);
  options.uniformsNames       = _options.uniforms;
  options.uniformBuffersNames = _options.uniformBuffers;
  options.samplers            = _options.samplers;
  options.defines             = ::std::move(join);
  options.fallbacks           = ::std::move(fallbacks);
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

  if (stl_util::contains(_options.uniforms, "world")) {
    _effect->setMatrix("world", world);
  }

  if (stl_util::contains(_options.uniforms, "worldView")) {
    world.multiplyToRef(scene->getViewMatrix(), _cachedWorldViewMatrix);
    _effect->setMatrix("worldView", _cachedWorldViewMatrix);
  }

  if (stl_util::contains(_options.uniforms, "worldViewProjection")) {
    auto transformMatrix = scene->getTransformMatrix();
    _effect->setMatrix("worldViewProjection", world.multiply(transformMatrix));
  }
}

void ShaderMaterial::bind(Matrix* world, Mesh* mesh)
{
  // Std values
  bindOnlyWorldMatrix(*world);

  if (getScene()->getCachedMaterial() != this) {
    if (stl_util::contains(_options.uniforms, "view")) {
      _effect->setMatrix("view", getScene()->getViewMatrix());
    }

    if (stl_util::contains(_options.uniforms, "projection")) {
      _effect->setMatrix("projection", getScene()->getProjectionMatrix());
    }

    if (stl_util::contains(_options.uniforms, "viewProjection")) {
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

    // Color3 array
    for (auto& kv : _colors3Arrays) {
      _effect->setArray3(kv.first, kv.second);
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

std::vector<BaseTexture*> ShaderMaterial::getActiveTextures() const
{
  auto activeTextures = Material::getActiveTextures();

  for (auto& textureItem : _textures) {
    activeTextures.emplace_back(textureItem.second);
  }

  for (auto& textureArrayItem : _textureArrays) {
    const auto& array = textureArrayItem.second;
    for (std::size_t index = 0; index < array.size(); ++index) {
      activeTextures.emplace_back(array[index]);
    }
  }

  return activeTextures;
}

bool ShaderMaterial::hasTexture(BaseTexture* texture) const
{
  if (Material::hasTexture(texture)) {
    return true;
  }

  auto it1
    = ::std::find_if(_textures.begin(), _textures.end(),
                     [&texture](const std::pair<std::string, Texture*>& tex) {
                       return tex.second == texture;
                     });
  if (it1 != _textures.end()) {
    return true;
  }

  auto it2 = ::std::find_if(
    _textureArrays.begin(), _textureArrays.end(),
    [&texture](
      const std::pair<std::string, std::vector<BaseTexture*>>& textures) {
      return stl_util::contains(textures.second, texture);
    });
  if (it2 != _textureArrays.end()) {
    return true;
  }

  return false;
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
