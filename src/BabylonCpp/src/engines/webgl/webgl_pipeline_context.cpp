#include <babylon/engines/webgl/webgl_pipeline_context.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/engine.h>
#include <babylon/materials/effect.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/color4.h>

namespace BABYLON {

WebGLPipelineContext::WebGLPipelineContext()
    : IPipelineContext{}
    , engine{nullptr}
    , program{nullptr}
    , context{nullptr}
    , vertexShader{nullptr}
    , fragmentShader{nullptr}
    , isParallelCompiled{false}
    , onCompiled{nullptr}
    , transformFeedback{nullptr}
{
}

WebGLPipelineContext::~WebGLPipelineContext() = default;

bool WebGLPipelineContext::isAsync()
{
  return isParallelCompiled;
}

bool WebGLPipelineContext::isReady()
{
  if (program) {
    if (isParallelCompiled) {
      return engine->_isRenderingStateCompiled(this);
    }
    return true;
  }

  return false;
}

void WebGLPipelineContext::_handlesSpectorRebuildCallback(
  const std::function<void(const GL::IGLProgramPtr& program)>& iOnCompiled)
{
  if (iOnCompiled && program) {
    iOnCompiled(program);
  }
}

void WebGLPipelineContext::_fillEffectInformation(
  Effect* effect, const std::unordered_map<std::string, unsigned int>& uniformBuffersNames,
  const std::vector<std::string>& uniformsNames,
  std::unordered_map<std::string, WebGLUniformLocationPtr>& uniforms,
  std::vector<std::string>& samplerList, std::unordered_map<std::string, int>& samplers,
  const std::vector<std::string>& attributesNames, Int32Array& attributes)
{
  if (engine->supportsUniformBuffers()) {
    for (const auto& [name, index] : uniformBuffersNames) {
      effect->bindUniformBlock(name, index);
    }
  }

  const auto effectAvailableUniforms = engine->getUniforms(this, uniformsNames);
  for (size_t index = 0; index < effectAvailableUniforms.size(); ++index) {
    uniforms[uniformsNames[index]] = effectAvailableUniforms[index];
  }
  _uniforms = uniforms;

  stl_util::erase_remove_if(samplerList, [&effect](const std::string& uniformName) {
    return effect->getUniform(uniformName) == nullptr;
  });

  for (size_t index = 0; index < samplerList.size(); ++index) {
    samplers[samplerList[index]] = index;
  }

  for (const auto& attr : engine->getAttributes(this, attributesNames)) {
    attributes.emplace_back(attr);
  }
}

void WebGLPipelineContext::dispose()
{
  _uniforms = {};
}

WebGLUniformLocationPtr WebGLPipelineContext::getUniform(const std::string& uniformName)
{
  if (stl_util::contains(_uniforms, uniformName)) {
    return _uniforms[uniformName];
  }

  return nullptr;
}

bool WebGLPipelineContext::_cacheMatrix(const std::string& uniformName, const Matrix& matrix)
{
  auto flag = matrix.updateFlag;
  if (stl_util::contains(_valueCache, uniformName) && !_valueCache[uniformName].empty()
      && static_cast<int>(_valueCache[uniformName][0]) == flag) {
    return false;
  }

  if (_valueCache[uniformName].empty()) {
    _valueCache[uniformName].emplace_back(static_cast<float>(flag));
  }
  else {
    _valueCache[uniformName][0] = static_cast<float>(flag);
  }

  return true;
}

bool WebGLPipelineContext::_cacheFloat2(const std::string& uniformName, float x, float y)
{
  if (!stl_util::contains(_valueCache, uniformName) || _valueCache[uniformName].size() != 2) {
    _valueCache[uniformName] = {x, y};
    return true;
  }

  auto changed = false;
  auto& cache  = _valueCache[uniformName];
  if (!stl_util::almost_equal(cache[0], x)) {
    cache[0] = x;
    changed  = true;
  }
  if (!stl_util::almost_equal(cache[1], y)) {
    cache[1] = y;
    changed  = true;
  }

  return changed;
}

bool WebGLPipelineContext::_cacheFloat3(const std::string& uniformName, float x, float y, float z)
{
  if (!stl_util::contains(_valueCache, uniformName) || _valueCache[uniformName].size() != 3) {
    _valueCache[uniformName] = {x, y, z};
    return true;
  }

  auto changed = false;
  auto& cache  = _valueCache[uniformName];
  if (!stl_util::almost_equal(cache[0], x)) {
    cache[0] = x;
    changed  = true;
  }
  if (!stl_util::almost_equal(cache[1], y)) {
    cache[1] = y;
    changed  = true;
  }
  if (!stl_util::almost_equal(cache[2], z)) {
    cache[2] = z;
    changed  = true;
  }

  return changed;
}

bool WebGLPipelineContext::_cacheFloat4(const std::string& uniformName, float x, float y, float z,
                                        float w)
{
  if (!stl_util::contains(_valueCache, uniformName) || _valueCache[uniformName].size() != 4) {
    _valueCache[uniformName] = {x, y, z, w};
    return true;
  }

  auto changed = false;
  auto& cache  = _valueCache[uniformName];
  if (!stl_util::almost_equal(cache[0], x)) {
    cache[0] = x;
    changed  = true;
  }
  if (!stl_util::almost_equal(cache[1], y)) {
    cache[1] = y;
    changed  = true;
  }
  if (!stl_util::almost_equal(cache[2], z)) {
    cache[2] = z;
    changed  = true;
  }
  if (!stl_util::almost_equal(cache[3], w)) {
    cache[3] = w;
    changed  = true;
  }

  return changed;
}

void WebGLPipelineContext::setInt(const std::string& uniformName, int value)
{
  Float32Array _value{static_cast<float>(value)};
  if (stl_util::contains(_valueCache, uniformName) && (_valueCache[uniformName] == _value)) {
    return;
  }

  if (engine->setInt(getUniform(uniformName), value)) {
    _valueCache[uniformName] = _value;
  }
}

void WebGLPipelineContext::setInt2(const std::string& uniformName, int x, int y)
{
  if (_cacheFloat2(uniformName, static_cast<float>(x), static_cast<float>(y))) {
    if (!engine->setInt2(getUniform(uniformName), x, y)) {
      _valueCache.erase(uniformName);
    }
  }
}

void WebGLPipelineContext::setInt3(const std::string& uniformName, int x, int y, int z)
{
  if (_cacheFloat3(uniformName, static_cast<float>(x), static_cast<float>(y),
                   static_cast<float>(z))) {
    if (!engine->setInt3(getUniform(uniformName), x, y, z)) {
      _valueCache.erase(uniformName);
    }
  }
}

void WebGLPipelineContext::setInt4(const std::string& uniformName, int x, int y, int z, int w)
{
  if (_cacheFloat4(uniformName, static_cast<float>(x), static_cast<float>(y), static_cast<float>(z),
                   static_cast<float>(w))) {
    if (!engine->setInt4(getUniform(uniformName), x, y, z, w)) {
      _valueCache.erase(uniformName);
    }
  }
}

void WebGLPipelineContext::setIntArray(const std::string& uniformName, const Int32Array& array)
{
  _valueCache.erase(uniformName);
  engine->setIntArray(getUniform(uniformName), array);
}

void WebGLPipelineContext::setIntArray2(const std::string& uniformName, const Int32Array& array)
{
  _valueCache.erase(uniformName);
  engine->setIntArray2(getUniform(uniformName), array);
}

void WebGLPipelineContext::setIntArray3(const std::string& uniformName, const Int32Array& array)
{
  _valueCache.erase(uniformName);
  engine->setIntArray3(getUniform(uniformName), array);
}

void WebGLPipelineContext::setIntArray4(const std::string& uniformName, const Int32Array& array)
{
  _valueCache.erase(uniformName);
  engine->setIntArray4(getUniform(uniformName), array);
}

void WebGLPipelineContext::setArray(const std::string& uniformName, const Float32Array& array)
{
  _valueCache.erase(uniformName);
  engine->setArray(getUniform(uniformName), array);
}

void WebGLPipelineContext::setArray2(const std::string& uniformName, const Float32Array& array)
{
  _valueCache.erase(uniformName);
  engine->setArray2(getUniform(uniformName), array);
}

void WebGLPipelineContext::setArray3(const std::string& uniformName, const Float32Array& array)
{
  _valueCache.erase(uniformName);
  engine->setArray3(getUniform(uniformName), array);
}

void WebGLPipelineContext::setArray4(const std::string& uniformName, const Float32Array& array)
{
  _valueCache.erase(uniformName);
  engine->setArray4(getUniform(uniformName), array);
}

void WebGLPipelineContext::setMatrices(const std::string& uniformName, const Float32Array& matrices)
{
  if (matrices.empty()) {
    return;
  }

  _valueCache.erase(uniformName);
  engine->setMatrices(getUniform(uniformName), matrices);
}

void WebGLPipelineContext::setMatrix(const std::string& uniformName, const Matrix& matrix)
{
  if (_cacheMatrix(uniformName, matrix)) {
    if (!engine->setMatrices(getUniform(uniformName), matrix.toArray())) {
      _valueCache.erase(uniformName);
    }
  }
}

void WebGLPipelineContext::setMatrix3x3(const std::string& uniformName, const Float32Array& matrix)
{
  _valueCache.erase(uniformName);
  engine->setMatrix3x3(getUniform(uniformName), matrix);
}

void WebGLPipelineContext::setMatrix2x2(const std::string& uniformName, const Float32Array& matrix)
{
  _valueCache.erase(uniformName);
  engine->setMatrix2x2(getUniform(uniformName), matrix);
}

void WebGLPipelineContext::setFloat(const std::string& uniformName, float value)
{
  if (stl_util::contains(_valueCache, uniformName)
      && stl_util::almost_equal(_valueCache[uniformName][0], value)) {
    return;
  }

  if (engine->setFloat(getUniform(uniformName), value)) {
    _valueCache[uniformName] = {value};
  }
}

void WebGLPipelineContext::setVector2(const std::string& uniformName, const Vector2& vector2)
{
  if (_cacheFloat2(uniformName, vector2.x, vector2.y)) {
    if (!engine->setFloat2(getUniform(uniformName), vector2.x, vector2.y)) {
      _valueCache.erase(uniformName);
    }
  }
}

void WebGLPipelineContext::setFloat2(const std::string& uniformName, float x, float y)
{
  if (_cacheFloat2(uniformName, x, y)) {
    if (!engine->setFloat2(getUniform(uniformName), x, y)) {
      _valueCache.erase(uniformName);
    }
  }
}

void WebGLPipelineContext::setVector3(const std::string& uniformName, const Vector3& vector3)
{
  if (_cacheFloat3(uniformName, vector3.x, vector3.y, vector3.z)) {
    if (!engine->setFloat3(getUniform(uniformName), vector3.x, vector3.y, vector3.z)) {
      _valueCache.erase(uniformName);
    }
  }
}

void WebGLPipelineContext::setFloat3(const std::string& uniformName, float x, float y, float z)
{
  if (_cacheFloat3(uniformName, x, y, z)) {
    if (!engine->setFloat3(getUniform(uniformName), x, y, z)) {
      _valueCache.erase(uniformName);
    }
  }
}

void WebGLPipelineContext::setVector4(const std::string& uniformName, const Vector4& vector4)
{
  if (_cacheFloat4(uniformName, vector4.x, vector4.y, vector4.z, vector4.w)) {
    if (!engine->setFloat4(getUniform(uniformName), vector4.x, vector4.y, vector4.z, vector4.w)) {
      _valueCache.erase(uniformName);
    }
  }
}

void WebGLPipelineContext::setFloat4(const std::string& uniformName, float x, float y, float z,
                                     float w)
{
  if (_cacheFloat4(uniformName, x, y, z, w)) {
    if (!engine->setFloat4(getUniform(uniformName), x, y, z, w)) {
      _valueCache.erase(uniformName);
    }
  }
}

void WebGLPipelineContext::setColor3(const std::string& uniformName, const Color3& color3)
{
  if (_cacheFloat3(uniformName, color3.r, color3.g, color3.b)) {
    if (!engine->setFloat3(getUniform(uniformName), color3.r, color3.g, color3.b)) {
      _valueCache.erase(uniformName);
    }
  }
}

void WebGLPipelineContext::setColor4(const std::string& uniformName, const Color3& color3,
                                     float alpha)
{
  if (_cacheFloat4(uniformName, color3.r, color3.g, color3.b, alpha)) {
    if (!engine->setFloat4(getUniform(uniformName), color3.r, color3.g, color3.b, alpha)) {
      _valueCache.erase(uniformName);
    }
  }
}

void WebGLPipelineContext::setDirectColor4(const std::string& uniformName, const Color4& color4)
{
  if (_cacheFloat4(uniformName, color4.r, color4.g, color4.b, color4.a)) {
    if (!engine->setFloat4(getUniform(uniformName), color4.r, color4.g, color4.b, color4.a)) {
      _valueCache.erase(uniformName);
    }
  }
}

std::string WebGLPipelineContext::_getVertexShaderCode() const
{
  return vertexShader ? engine->_getShaderSource(vertexShader) : "";
}

std::string WebGLPipelineContext::_getFragmentShaderCode() const
{
  return fragmentShader ? engine->_getShaderSource(fragmentShader) : "";
}

} // end of namespace BABYLON
