#include <babylon/materials/shader_material.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/color4.h>
#include <babylon/maths/vector2.h>
#include <babylon/maths/vector3.h>
#include <babylon/maths/vector4.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/string_tools.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

ShaderMaterial::ShaderMaterial(const std::string& iName, Scene* scene,
                               const std::string& shaderPath, const IShaderMaterialOptions& options)
    : Material{iName, scene}
    , shaderPath{this, &ShaderMaterial::get_shaderPath, &ShaderMaterial::set_shaderPath}
    , _shaderPath{shaderPath}
    , _renderId{-1}
    , _multiview{false}
{
  _options.needAlphaBlending = options.needAlphaBlending;
  _options.needAlphaTesting  = options.needAlphaTesting;
  _options.attributes        = options.attributes;
  _options.uniforms          = options.uniforms;
  _options.uniformBuffers    = options.uniformBuffers;
  _options.samplers          = options.samplers;
  _options.defines           = options.defines;
}

ShaderMaterial::~ShaderMaterial() = default;

std::string ShaderMaterial::get_shaderPath() const
{
  return _shaderPath;
}

void ShaderMaterial::set_shaderPath(const std::string iShaderPath)
{
  _shaderPath = iShaderPath;
}

IShaderMaterialOptions& ShaderMaterial::options()
{
  return _options;
}

std::string ShaderMaterial::getClassName() const
{
  return "ShaderMaterial";
}

Type ShaderMaterial::type() const
{
  return Type::SHADERMATERIAL;
}

bool ShaderMaterial::needAlphaBlending() const
{
  return (alpha < 1.f) || _options.needAlphaBlending;
}

bool ShaderMaterial::needAlphaTesting() const
{
  return _options.needAlphaTesting;
}

void ShaderMaterial::_checkUniform(const std::string& uniformName)
{
  if (!stl_util::contains(_options.uniforms, uniformName)) {
    _options.uniforms.emplace_back(uniformName);
  }
}

ShaderMaterial& ShaderMaterial::setTexture(const std::string& iName, const BaseTexturePtr& texture)
{
  if (!stl_util::contains(_options.samplers, iName)) {
    _options.samplers.emplace_back(iName);
  }
  _textures[iName] = texture;

  return *this;
}

ShaderMaterial& ShaderMaterial::setTextureArray(const std::string& iName,
                                                const std::vector<BaseTexturePtr>& textures)
{
  if (!stl_util::contains(_options.samplers, iName)) {
    _options.samplers.emplace_back(iName);
  }

  _checkUniform(iName);

  _textureArrays[iName] = textures;

  return *this;
}

ShaderMaterial& ShaderMaterial::setInt(const std::string& iName, int value)
{
  _checkUniform(iName);
  _ints[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setFloat(const std::string& iName, float value)
{
  _checkUniform(iName);
  _floats[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setFloats(const std::string& iName, const Float32Array& value)
{
  _checkUniform(iName);
  _floatsArrays[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setColor3(const std::string& iName, const Color3& value)
{
  _checkUniform(iName);
  _colors3[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setColor3Array(const std::string& iName,
                                               const std::vector<Color3>& value)
{
  _checkUniform(iName);
  _colors3Arrays[iName] = {};
  for (const auto& color : value) {
    Float32Array arr(3);
    color.toArray(arr, 0);
    stl_util::concat(_colors3Arrays[iName], arr);
  }

  return *this;
}

ShaderMaterial& ShaderMaterial::setColor4(const std::string& iName, const Color4& value)
{
  _checkUniform(iName);
  _colors4[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setColor4Array(const std::string& iName,
                                               const std::vector<Color4>& value)
{
  _checkUniform(iName);
  _colors4Arrays[iName] = {};
  for (const auto& color : value) {
    Float32Array arr(4);
    color.toArray(arr, 0);
    stl_util::concat(_colors4Arrays[iName], arr);
  }

  return *this;
}

ShaderMaterial& ShaderMaterial::setVector2(const std::string& iName, const Vector2& value)
{
  _checkUniform(iName);
  _vectors2[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setVector3(const std::string& iName, const Vector3& value)
{
  _checkUniform(iName);
  _vectors3[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setVector4(const std::string& iName, const Vector4& value)
{
  _checkUniform(iName);
  _vectors4[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setMatrix(const std::string& iName, const Matrix& value)
{
  _checkUniform(iName);
  _matrices[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setMatrices(const std::string& iName,
                                            const std::vector<Matrix>& value)
{
  _checkUniform(iName);

  auto float32Array = Float32Array(value.size() * 16, 0.f);

  for (unsigned int index = 0; index < value.size(); ++index) {
    const auto& matrix = value[index];

    matrix.copyToArray(float32Array, index * 16);
  }

  _matrixArrays[name] = float32Array;

  return *this;
}

ShaderMaterial& ShaderMaterial::setMatrix3x3(const std::string& iName, const Float32Array& value)
{
  _checkUniform(iName);
  _matrices3x3[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setMatrix2x2(const std::string& iName, const Float32Array& value)
{
  _checkUniform(iName);
  _matrices2x2[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setArray2(const std::string& iName, const Float32Array& value)
{
  _checkUniform(iName);
  _vectors2Arrays[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setArray3(const std::string& iName, const Float32Array& value)
{
  _checkUniform(iName);
  _vectors3Arrays[iName] = value;

  return *this;
}

ShaderMaterial& ShaderMaterial::setArray4(const std::string& iName, const Float32Array& value)
{
  _checkUniform(iName);
  _vectors4Arrays[iName] = value;

  return *this;
}

bool ShaderMaterial::_checkCache(AbstractMesh* mesh, bool useInstances)
{
  if (!mesh) {
    return true;
  }

  if (_effect
      && (_effect->defines.find("#define INSTANCES") != std::string::npos) != useInstances) {
    return false;
  }

  return true;
}

bool ShaderMaterial::isReadyForSubMesh(AbstractMesh* mesh, SubMesh* /*subMesh*/, bool useInstances)
{
  return isReady(mesh, useInstances);
}

bool ShaderMaterial::isReady(AbstractMesh* mesh, bool useInstances)
{
  if (_effect && isFrozen()) {
    if (_effect->_wasPreviouslyReady) {
      return true;
    }
  }

  auto scene  = getScene();
  auto engine = scene->getEngine();

  if (!checkReadyOnEveryCall) {
    if (_renderId == scene->getRenderId()) {
      if (_checkCache(mesh, useInstances)) {
        return true;
      }
    }
  }

  // Instances
  std::vector<std::string> defines;
  std::vector<std::string> attribs;
  auto fallbacks = std::make_unique<EffectFallbacks>();

  // global multiview
  if (engine->getCaps().multiview && scene->activeCamera()
      && scene->activeCamera()->outputRenderTarget
      && scene->activeCamera()->outputRenderTarget->getViewCount() > 1) {
    _multiview = true;
    defines.emplace_back("#define MULTIVIEW");
    if (stl_util::index_of(_options.uniforms, "viewProjection") != -1
        && stl_util::index_of(_options.uniforms, "viewProjectionR") == -1) {
      _options.uniforms.emplace_back("viewProjectionR");
    }
  }

  for (const auto& _define : _options.defines) {
    defines.emplace_back(_define);
  }

  for (const auto& _attrib : _options.attributes) {
    attribs.emplace_back(_attrib);
  }

  if (mesh && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
    attribs.emplace_back(VertexBuffer::ColorKind);
    defines.emplace_back("#define VERTEXCOLOR");
  }

  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
    MaterialHelper::PushAttributesForInstances(attribs);
    if (mesh->hasThinInstances()) {
      defines.emplace_back("#define THIN_INSTANCES");
    }
  }

  // Bones
  auto numInfluencers = 0u;

  if (mesh && mesh->useBones() && mesh->computeBonesUsingShaders() && mesh->skeleton()) {
    attribs.emplace_back(VertexBuffer::MatricesIndicesKind);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKind);
    if (mesh->numBoneInfluencers() > 4) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKind);
      attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKind);
    }

    const auto& skeleton = mesh->skeleton();

    numInfluencers = mesh->numBoneInfluencers();

    defines.emplace_back("#define NUM_BONE_INFLUENCERS " + std::to_string(numInfluencers));
    fallbacks->addCPUSkinningFallback(0, mesh);

    if (skeleton->isUsingTextureForMatrices()) {
      defines.emplace_back("#define BONETEXTURE");

      if (!stl_util::contains(_options.uniforms, "boneTextureWidth")) {
        _options.uniforms.emplace_back("boneTextureWidth");
      }

      if (!stl_util::contains(_options.samplers, "boneSampler")) {
        _options.samplers.emplace_back("boneSampler");
      }
    }
    else {
      defines.emplace_back("#define BonesPerMesh " + std::to_string(skeleton->bones.size() + 1));

      if (!stl_util::contains(_options.uniforms, "mBones")) {
        _options.uniforms.emplace_back("mBones");
      }
    }
  }
  else {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
  }

  // Textures
  for (const auto& item : _textures) {
    if (!item.second->isReady()) {
      return false;
    }
  }

  // Alpha test
  if (mesh && _shouldTurnAlphaTestOn(mesh)) {
    defines.emplace_back("#define ALPHATEST");
  }

  auto shaderName     = _shaderPath;
  auto uniforms       = _options.uniforms;
  auto uniformBuffers = _options.uniformBuffers;
  auto samplers       = _options.samplers;

  if (customShaderNameResolve) {
    shaderName = customShaderNameResolve(shaderName, uniforms, uniformBuffers, samplers, nullptr,
                                         &defines, attribs, nullptr);
  }

  auto previousEffect = _effect;
  auto join           = StringTools::join(defines, '\n');

  if (_cachedDefines != join) {
    _cachedDefines = join;

    IEffectCreationOptions options;
    options.attributes          = std::move(attribs);
    options.uniformsNames       = _options.uniforms;
    options.uniformBuffersNames = _options.uniformBuffers;
    options.samplers            = samplers;
    options.defines             = join;
    options.fallbacks           = std::move(fallbacks);
    options.onCompiled          = onCompiled;
    options.onError             = onError;
    options.indexParameters     = {{"maxSimultaneousMorphTargets", numInfluencers}};

    _effect = engine->createEffect(shaderName, options, engine);

    /* if (_onEffectCreatedObservable()) */ {
      onCreatedEffectParameters.effect = _effect.get();
      _onEffectCreatedObservable.notifyObservers(&onCreatedEffectParameters);
    }
  }

  if (!_effect->isReady()) {
    return false;
  }

  if (previousEffect != _effect) {
    scene->resetCachedMaterial();
  }

  _renderId                    = scene->getRenderId();
  _effect->_wasPreviouslyReady = true;

  return true;
}

void ShaderMaterial::bindOnlyWorldMatrix(Matrix& world, const EffectPtr& effectOverride)
{
  auto scene = getScene();

  auto effect = effectOverride ? effectOverride : _effect;

  if (!effect) {
    return;
  }

  if (stl_util::contains(_options.uniforms, "world")) {
    effect->setMatrix("world", world);
  }

  if (stl_util::contains(_options.uniforms, "worldView")) {
    world.multiplyToRef(scene->getViewMatrix(), _cachedWorldViewMatrix);
    effect->setMatrix("worldView", _cachedWorldViewMatrix);
  }

  if (stl_util::contains(_options.uniforms, "worldViewProjection")) {
    world.multiplyToRef(scene->getTransformMatrix(), _cachedWorldViewProjectionMatrix);
    effect->setMatrix("worldViewProjection", world.multiply(_cachedWorldViewProjectionMatrix));
  }
}

void ShaderMaterial::bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh)
{
  bind(world, mesh, subMesh->_effectOverride);
}

void ShaderMaterial::bind(Matrix& world, Mesh* mesh, const EffectPtr& effectOverride)
{
  // Std values
  bindOnlyWorldMatrix(world, effectOverride);

  auto effect = effectOverride ? effectOverride : _effect;

  if (effect && getScene()->getCachedMaterial() != this) {
    if (stl_util::contains(_options.uniforms, "view")) {
      effect->setMatrix("view", getScene()->getViewMatrix());
    }

    if (stl_util::contains(_options.uniforms, "projection")) {
      effect->setMatrix("projection", getScene()->getProjectionMatrix());
    }

    if (stl_util::contains(_options.uniforms, "viewProjection")) {
      effect->setMatrix("viewProjection", getScene()->getTransformMatrix());
      if (_multiview) {
        effect->setMatrix("viewProjectionR", getScene()->_transformMatrixR);
      }
    }

    if (getScene()->activeCamera() && stl_util::contains(_options.uniforms, "cameraPosition")) {
      effect->setVector3("cameraPosition", getScene()->activeCamera()->globalPosition());
    }

    // Bones
    MaterialHelper::BindBonesParameters(mesh, effect.get());

    // Texture
    for (const auto& [channel, texture] : _textures) {
      effect->setTexture(channel, texture);
    }

    // Texture arrays
    for (const auto& [channel, textureArray] : _textureArrays) {
      effect->setTextureArray(channel, textureArray);
    }

    // Int
    for (const auto& [channel, intValue] : _ints) {
      effect->setInt(channel, intValue);
    }

    // Float
    for (const auto& [channel, floatValue] : _floats) {
      effect->setFloat(channel, floatValue);
    }

    // Floats
    for (const auto& [channel, floatsArray] : _floatsArrays) {
      effect->setArray(channel, floatsArray);
    }

    // Color3
    for (const auto& [channel, colors3] : _colors3) {
      effect->setColor3(channel, colors3);
    }

    // Color3Array
    for (const auto& [channel, array3] : _colors3Arrays) {
      effect->setArray3(channel, array3);
    }

    // Color4
    for (const auto& [channel, color] : _colors4) {
      effect->setFloat4(channel, color.r, color.g, color.b, color.a);
    }

    // Color4Array
    for (const auto& [channel, array4] : _colors4Arrays) {
      effect->setArray4(channel, array4);
    }

    // Vector2
    for (const auto& [channel, vectors2] : _vectors2) {
      effect->setVector2(channel, vectors2);
    }

    // Vector3
    for (const auto& [channel, vector3] : _vectors3) {
      effect->setVector3(channel, vector3);
    }

    // Vector4
    for (const auto& [channel, vector4] : _vectors4) {
      effect->setVector4(channel, vector4);
    }

    // Matrix
    for (const auto& [channel, matrix] : _matrices) {
      effect->setMatrix(channel, matrix);
    }

    // MatrixArray
    for (const auto& [channel, matrixArray] : _matrixArrays) {
      effect->setMatrices(channel, matrixArray);
    }

    // Matrix 3x3
    for (const auto& [channel, matrices3x3] : _matrices3x3) {
      effect->setMatrix3x3(channel, matrices3x3);
    }

    // Matrix 2x2
    for (const auto& [channel, matrix2x2] : _matrices2x2) {
      effect->setMatrix2x2(channel, matrix2x2);
    }

    // Vector2Array
    for (const auto& [channel, array2] : _vectors2Arrays) {
      effect->setArray2(channel, array2);
    }

    // Vector3Array
    for (const auto& [channel, array3] : _vectors3Arrays) {
      effect->setArray3(channel, array3);
    }

    // Vector4Array
    for (const auto& [channel, array4] : _vectors4Arrays) {
      effect->setArray4(channel, array4);
    }
  }

  const auto seffect = _effect;

  _effect = effect; // make sure the active effect is the right one if there are some observers for
                    // onBind that would need to get the current effect
  _afterBind(mesh);
  _effect = seffect;
}

void ShaderMaterial::_afterBind(Mesh* mesh, const EffectPtr& /*effect*/)
{
  Material::_afterBind(mesh);
  getScene()->_cachedEffect = _effect;
}

std::vector<BaseTexturePtr> ShaderMaterial::getActiveTextures() const
{
  auto activeTextures = Material::getActiveTextures();

  for (const auto& textureItem : _textures) {
    activeTextures.emplace_back(textureItem.second);
  }

  for (const auto& textureArrayItem : _textureArrays) {
    const auto& array = textureArrayItem.second;
    for (const auto& index : array) {
      activeTextures.emplace_back(index);
    }
  }

  return activeTextures;
}

bool ShaderMaterial::hasTexture(const BaseTexturePtr& texture) const
{
  if (Material::hasTexture(texture)) {
    return true;
  }

  auto it1 = std::find_if(_textures.begin(), _textures.end(),
                          [&texture](const std::pair<std::string, BaseTexturePtr>& tex) {
                            return tex.second == texture;
                          });
  if (it1 != _textures.end()) {
    return true;
  }

  auto it2
    = std::find_if(_textureArrays.begin(), _textureArrays.end(),
                   [&texture](const std::pair<std::string, std::vector<BaseTexturePtr>>& textures) {
                     return stl_util::contains(textures.second, texture);
                   });
  return it2 != _textureArrays.end();
}

MaterialPtr ShaderMaterial::clone(const std::string& iName, bool /*cloneChildren*/) const
{
  auto result = ShaderMaterial::New(iName, getScene(), _shaderPath, _options);

  result->name = name;
  result->id   = name;

  // Texture
  for (const auto& [channel, texture] : _textures) {
    result->setTexture(channel, texture);
  }

  // Float
  for (const auto& [channel, floatValue] : _floats) {
    result->setFloat(channel, floatValue);
  }

  // Floats
  for (const auto& [channel, floatsArray] : _floatsArrays) {
    result->setFloats(channel, floatsArray);
  }

  // Color3
  for (const auto& [channel, colors3] : _colors3) {
    result->setColor3(channel, colors3);
  }

  // Color4
  for (const auto& [channel, colors4] : _colors4) {
    result->setColor4(channel, colors4);
  }

  // Vector2
  for (const auto& [channel, vectors2] : _vectors2) {
    result->setVector2(channel, vectors2);
  }

  // Vector3
  for (const auto& [channel, vector3] : _vectors3) {
    result->setVector3(channel, vector3);
  }

  // Vector4
  for (const auto& [channel, vector4] : _vectors4) {
    result->setVector4(channel, vector4);
  }

  // Matrix
  for (const auto& [channel, matrix] : _matrices) {
    result->setMatrix(channel, matrix);
  }

  // Matrix 3x3
  for (const auto& [channel, matrix3x3] : _matrices3x3) {
    result->setMatrix3x3(channel, matrix3x3);
  }

  // Matrix 2x2
  for (const auto& [channel, matrix2x2] : _matrices2x2) {
    result->setMatrix2x2(channel, matrix2x2);
  }

  return result;
}

void ShaderMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures,
                             bool notBoundToMesh)
{
  if (forceDisposeTextures) {
    for (const auto& kv : _textures) {
      kv.second->dispose();
    }

    for (const auto& kv : _textureArrays) {
      for (const auto& textureArray : kv.second) {
        textureArray->dispose();
      }
    }
  }

  _textures.clear();

  Material::dispose(forceDisposeEffect, forceDisposeTextures, notBoundToMesh);
}

json ShaderMaterial::serialize() const
{
  return nullptr;
}

std::unique_ptr<ShaderMaterial> ShaderMaterial::Parse(const json& /*source*/, Scene* /*scene*/,
                                                      const std::string& /*url*/)
{
  return nullptr;
}

} // end of namespace BABYLON
