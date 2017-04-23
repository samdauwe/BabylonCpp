#include <babylon/materials/effect.h>

#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/effect_includes_shaders_store.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector4.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

std::size_t Effect::_uniqueIdSeed = 0;

Effect::Effect(
  const std::string& baseName, const std::vector<std::string>& attributesNames,
  const std::vector<std::string>& uniformsNames,
  const std::vector<std::string>& samplers, Engine* engine,
  const std::string& iDefines, EffectFallbacks* fallbacks,
  const std::function<void(Effect* effect)>& iOnCompiled,
  const std::function<void(Effect* effect, const std::string& errors)> _onError,
  const std::unordered_map<std::string, unsigned int>& indexParameters)
    : name{baseName}
    , defines{iDefines}
    , onCompiled{iOnCompiled}
    , onError{_onError}
    , uniqueId{Effect::_uniqueIdSeed++}
    , _engine{engine}
    , _uniformsNames{uniformsNames}
    , _samplers{samplers}
    , _isReady{false}
    , _compilationError{""}
    , _attributesNames{attributesNames}
    , _indexParameters{indexParameters}
{
  std_util::concat(_uniformsNames, samplers);

  std::string vertexSource   = baseName;
  std::string fragmentSource = baseName;

  _loadVertexShader(vertexSource, [this, &fragmentSource,
                                   &fallbacks](const std::string& vertexCode) {
    _processIncludes(vertexCode, [this, &fragmentSource, &fallbacks](
                                   const std::string& vertexCodeWithIncludes) {
      _loadFragmentShader(
        fragmentSource, [this, &vertexCodeWithIncludes,
                         &fallbacks](const std::string& fragmentCode) {
          _processIncludes(
            fragmentCode, [this, &vertexCodeWithIncludes, &fallbacks](
                            const std::string& fragmentCodeWithIncludes) {
              _prepareEffect(vertexCodeWithIncludes, fragmentCodeWithIncludes,
                             _attributesNames, defines, fallbacks);
            });
        });
    });
  });
}

Effect::Effect(
  const std::unordered_map<std::string, std::string>& baseName,
  const std::vector<std::string>& attributesNames,
  const std::vector<std::string>& uniformsNames,
  const std::vector<std::string>& samplers, Engine* engine,
  const std::string& iDefines, EffectFallbacks* fallbacks,
  const std::function<void(Effect* effect)>& iOnCompiled,
  const std::function<void(Effect* effect, const std::string& errors)> _onError,
  const std::unordered_map<std::string, unsigned int>& indexParameters)
    : defines{iDefines}
    , onCompiled{iOnCompiled}
    , onError{_onError}
    , _engine{engine}
    , _uniformsNames{uniformsNames}
    , _samplers{samplers}
    , _isReady{false}
    , _compilationError{""}
    , _attributesNames{attributesNames}
    , _indexParameters{indexParameters}
{
  std_util::concat(_uniformsNames, samplers);

  std::string vertexSource   = "";
  std::string fragmentSource = "";

  if (std_util::contains(baseName, "vertexElement")) {
    vertexSource = baseName.at("vertexElement");
  }
  else if (std_util::contains(baseName, "vertex")) {
    vertexSource = baseName.at("vertex");
  }

  if (std_util::contains(baseName, "fragmentElement")) {
    fragmentSource = baseName.at("fragmentElement");
  }
  else if (std_util::contains(baseName, "fragment")) {
    fragmentSource = baseName.at("fragment");
  }

  if (vertexSource.empty() || fragmentSource.empty()) {
    return;
  }

  name = fragmentSource;

  _loadVertexShader(vertexSource, [this, &fragmentSource,
                                   &fallbacks](const std::string& vertexCode) {
    _processIncludes(vertexCode, [this, &fragmentSource, &fallbacks](
                                   const std::string& vertexCodeWithIncludes) {
      _loadFragmentShader(
        fragmentSource, [this, &vertexCodeWithIncludes,
                         &fallbacks](const std::string& fragmentCode) {
          _processIncludes(
            fragmentCode, [this, &vertexCodeWithIncludes, &fallbacks](
                            const std::string& fragmentCodeWithIncludes) {
              _prepareEffect(vertexCodeWithIncludes, fragmentCodeWithIncludes,
                             _attributesNames, defines, fallbacks);
            });
        });
    });
  });
}

Effect::~Effect()
{
}

std::string Effect::key() const
{
  return _key;
}

bool Effect::isReady() const
{
  return _isReady;
}

GL::IGLProgram* Effect::getProgram()
{
  return _program.get();
}

std::vector<std::string>& Effect::getAttributesNames()
{
  return _attributesNames;
}

int Effect::getAttributeLocation(unsigned int index)
{
  if (index < _attributes.size()) {
    return _attributes[index];
  }

  return -1;
}

int Effect::getAttributeLocationByName(const std::string& _name)
{
  int index = std_util::index_of(_attributesNames, _name);

  return (index != -1) ? _attributes[static_cast<size_t>(index)] : -1;
}

size_t Effect::getAttributesCount()
{
  return _attributes.size();
}

int Effect::getUniformIndex(const std::string& uniformName)
{
  return std_util::index_of(_uniformsNames, uniformName);
}

GL::IGLUniformLocation* Effect::getUniform(const std::string& uniformName)
{
  if (std_util::contains(_uniforms, uniformName)) {
    return _uniforms[uniformName].get();
  }

  return nullptr;
}

std::vector<std::string>& Effect::getSamplers()
{
  return _samplers;
}

std::string Effect::getCompilationError()
{
  return _compilationError;
}

std::string Effect::getVertexShaderSource()
{
  return _evaluateDefinesOnString(
    _engine->getVertexShaderSource(_program.get()));
}

std::string Effect::getFragmentShaderSource()
{
  return _evaluateDefinesOnString(
    _engine->getFragmentShaderSource(_program.get()));
}

void Effect::_loadVertexShader(
  const std::string& vertex,
  std::function<void(const std::string& data)> callback)
{
  // Base64 encoded ?
  if (vertex.substr(0, 7) == "base64:") {
    auto vertexBinary = vertex.substr(7);
    callback(vertexBinary);
    return;
  }

  // Is in local store ?
  const std::string vertexShaderName = vertex + "VertexShader";
  if (std_util::contains(EffectShadersStore::Shaders, vertexShaderName)) {
    callback(std::string(EffectShadersStore::Shaders[vertexShaderName]));
    return;
  }

  std::string vertexShaderUrl;

  if ((vertex.at(0) == '.') || (vertex.at(0) == '/')
      || ((vertex.find("http") != std::string::npos))) {
    vertexShaderUrl = vertex;
  }
  else {
    vertexShaderUrl = Engine::ShadersRepository + vertex;
  }

  // Vertex shader
  Tools::LoadFile(vertexShaderUrl + ".vertex.fx", callback);
}

void Effect::_loadFragmentShader(
  const std::string& fragment,
  std::function<void(const std::string& data)> callback)
{
  // Base64 encoded ?
  if (fragment.substr(0, 7) == "base64:") {
    auto vertexBinary = fragment.substr(7);
    callback(vertexBinary);
    return;
  }

  // Is in local store ?
  std::string fragmentShaderName = fragment + "PixelShader";
  if (std_util::contains(EffectShadersStore::Shaders, fragmentShaderName)) {
    callback(std::string(EffectShadersStore::Shaders[fragmentShaderName]));
    return;
  }

  fragmentShaderName = fragment + "FragmentShader";
  if (std_util::contains(EffectShadersStore::Shaders, fragmentShaderName)) {
    callback(std::string(EffectShadersStore::Shaders[fragmentShaderName]));
    return;
  }

  std::string fragmentShaderUrl;

  if ((fragment.at(0) == '.') || (fragment.at(0) == '/')
      || ((fragment.find("http") != std::string::npos))) {
    fragmentShaderUrl = fragment;
  }
  else {
    fragmentShaderUrl = Engine::ShadersRepository + fragment;
  }

  // Fragment shader
  Tools::LoadFile(fragmentShaderUrl + ".fragment.fx", callback);
}

void Effect::_dumpShadersName()
{
  BABYLON_LOG_ERROR("Materials::Effect", "Vertex shader:", name);
  BABYLON_LOG_ERROR("Materials::Effect", "Fragment shader:", name);
}

void Effect::_processShaderConversion(
  const std::string& sourceCode, bool isFragment,
  const std::function<void(const std::string& data)>& callback)
{

  auto preparedSourceCode = _processPrecision(sourceCode);

  if (_engine->webGLVersion() == 1.f) {
    callback(preparedSourceCode);
    return;
  }

  // Already converted
  if (String::contains(preparedSourceCode, "#version 3")) {
    callback(preparedSourceCode);
    return;
  }

  // Remove extensions
  // #extension GL_OES_standard_derivatives : enable
  // #extension GL_EXT_shader_texture_lod : enable
  // #extension GL_EXT_frag_depth : enable
  std::string regex(
    "/#extension.+(GL_OES_standard_derivatives|GL_EXT_shader_texture_lod|GL_"
    "EXT_frag_depth).+enable/g");
  std::string result = String::regexReplace(preparedSourceCode, regex, "");

  // Migrate to GLSL v300
  result = String::regexReplace(preparedSourceCode, "/varying\\s/g",
                                isFragment ? "in " : "out ");
  result = String::regexReplace(preparedSourceCode, "/attribute[ \t]/g", "in ");
  result = String::regexReplace(preparedSourceCode, "/[ \t]attribute/g", " in");

  if (isFragment) {
    result = String::regexReplace(preparedSourceCode, "/texture2DLodEXT\\(/g",
                                  "textureLod(");
    result = String::regexReplace(preparedSourceCode, "/textureCubeLodEXT\\(/g",
                                  "textureLod(");
    result
      = String::regexReplace(preparedSourceCode, "/texture2D\\(/g", "texture(");
    result = String::regexReplace(preparedSourceCode, "/textureCube\\(/g",
                                  "texture(");
    result = String::regexReplace(preparedSourceCode, "/gl_FragDepthEXT/g",
                                  "gl_FragDepth");
    result = String::regexReplace(preparedSourceCode, "/gl_FragColor/g",
                                  "glFragColor");
    result = String::regexReplace(preparedSourceCode, "/void\\s+?main\\(\\/g",
                                  "out vec4 glFragColor;\nvoid main(");
  }

  callback(result);
}

void Effect::_processIncludes(
  const std::string& sourceCode,
  const std::function<void(const std::string& data)>& callback)
{
  std::ostringstream returnValue;
  auto lines = String::split(sourceCode, '\n');
  std::regex regex;
  std::smatch match;

  for (const auto& line : lines) {
    // Shader includes
    regex = std::regex("#include<(.+)>");
    std::string includeFile;
    if (std::regex_search(line, match, regex) && (match.size() == 2)) {
      // Shader include found
      includeFile = match.str(1);
    }
    else {
      // Line without shader include
      returnValue << line << std::endl;
      continue;
    }

    if (std_util::contains(EffectIncludesShadersStore::Shaders, includeFile)) {
      // Substitution
      auto includeContent = EffectIncludesShadersStore::Shaders[includeFile];
      // Instanced includes
      regex = std::regex("#include<(.+)>\\[(.*)]");
      if (std::regex_search(line, match, regex) && (match.size() == 3)) {
        // Perform instanced includes
        std::string indexString(match.str(2));
        if (indexString.find("..") != std::string::npos) {
          String::replaceInPlace(indexString, "..", ".");
          std::vector<std::string> indexSplits
            = String::split(indexString, '.');
          if (indexSplits.size() == 2) {
            auto minIndex = indexSplits[0];
            auto maxIndex = indexSplits[1];
            std::ostringstream includeContentStream;

            if ((!String::isDigit(maxIndex))
                && std_util::contains(_indexParameters, maxIndex)) {
              maxIndex = std::to_string(_indexParameters[maxIndex]);
            }

            if (String::isDigit(minIndex) && String::isDigit(maxIndex)) {
              size_t _minIndex = std::stoul(minIndex, nullptr, 0);
              size_t _maxIndex = std::stoul(maxIndex, nullptr, 0);
              for (size_t i = _minIndex; i <= _maxIndex; ++i) {
                std::string _includeContent(includeContent);
                String::replaceInPlace(_includeContent, "{X}",
                                       std::to_string(i));
                includeContentStream << _includeContent << std::endl;
              }
            }

            returnValue << includeContentStream.str();
          }
        }
      }
      else {
        // No instanced includes
        returnValue << includeContent;
      }
    }
  }

  callback(returnValue.str());
}

std::string Effect::_processPrecision(const std::string& source)
{
  std::string _source = source;
  if (_source.find("precision highp float") != std::string::npos) {
    if (!_engine->getCaps().highPrecisionShaderSupported) {
      _source = "precision mediump float;\n" + _source;
    }
    else {
      _source = "#ifdef GL_ES\nprecision highp float;\n#endif\n" + _source;
    }
  }
  else {
    if (!_engine->getCaps().highPrecisionShaderSupported) {
      // Moving highp to mediump
      String::replaceInPlace(_source, "precision highp float",
                             "precision mediump float");
    }
  }

  return source;
}

void Effect::_prepareEffect(const std::string& vertexSourceCode,
                            const std::string& fragmentSourceCode,
                            const std::vector<std::string>& attributesNames,
                            const std::string& iDefines,
                            EffectFallbacks* fallbacks)
{

  auto engine = _engine;

  try {
    _program = engine->createShaderProgram(vertexSourceCode, fragmentSourceCode,
                                           iDefines);

    _uniforms   = engine->getUniforms(_program.get(), _uniformsNames);
    _attributes = engine->getAttributes(_program.get(), attributesNames);

    for (unsigned int index = 0; index < _samplers.size(); ++index) {
      auto sampler = getUniform(_samplers[index]);
      if (!sampler) {
        _samplers.erase(_samplers.begin() + index,
                        _samplers.begin() + index + 1);
        --index;
      }
    }

    engine->bindSamplers(this);

    _compilationError.clear();
    _isReady = true;
    if (onCompiled) {
      onCompiled(this);
    }
  }
  catch (const std::exception& e) {
    _compilationError = e.what();

    // Let's go through fallbacks then
    BABYLON_LOG_ERROR("Effect", "Unable to compile effect: ");
    BABYLON_LOGF_ERROR("Effect", "Defines: %s", defines.c_str());
    BABYLON_LOGF_ERROR("Effect", "Error: %s", _compilationError.c_str());
    _dumpShadersName();

    if (!fallbacks && fallbacks->isMoreFallbacks()) {
      BABYLON_LOG_ERROR("Effect", "Trying next fallback.");
      defines = fallbacks->reduce(defines);
      _prepareEffect(vertexSourceCode, fragmentSourceCode, attributesNames,
                     defines, fallbacks);
    }
    else { // Sorry we did everything we can
      if (onError) {
        onError(this, _compilationError);
      }
    }
  }
}

bool Effect::isSupported() const
{
  return _compilationError.empty();
}

void Effect::_bindTexture(const std::string& channel, GL::IGLTexture* texture)
{
  _engine->_bindTexture(std_util::index_of(_samplers, channel), texture);
}

void Effect::setTexture(const std::string& channel, BaseTexture* texture)
{
  _engine->setTexture(std_util::index_of(_samplers, channel),
                      getUniform(channel), texture);
}

void Effect::setTextureArray(const std::string& channel,
                             const std::vector<BaseTexture*>& textures)
{
  if (std_util::index_of(_samplers, channel + "Ex") == -1) {
    int initialPos = std_util::index_of(_samplers, channel);
    for (unsigned int index = 1; index < textures.size(); ++index) {
      std_util::splice(_samplers, initialPos + static_cast<int>(index), 0,
                       {channel + "Ex"});
    }
  }

  _engine->setTextureArray(std_util::index_of(_samplers, channel),
                           getUniform(channel), textures);
}

void Effect::setTextureFromPostProcess(const std::string& channel,
                                       PostProcess* postProcess)
{
  _engine->setTextureFromPostProcess(std_util::index_of(_samplers, channel),
                                     postProcess);
}

bool Effect::_cacheMatrix(const std::string& uniformName, const Matrix& matrix)
{
  bool changed = false;
  if (!std_util::contains(_valueCache, uniformName)) {
    changed                  = true;
    _valueCache[uniformName] = Float32Array(16);
  }

  Float32Array matrixElements(matrix.m.begin(), matrix.m.end());
  if (_valueCache[uniformName] != matrixElements) {
    changed                  = true;
    _valueCache[uniformName] = matrixElements;
  }

  return changed;
}

bool Effect::_cacheFloat2(const std::string& uniformName, float x, float y)
{
  if (!std_util::contains(_valueCache, uniformName)) {
    _valueCache[uniformName] = {x, y};
    return true;
  }

  bool changed = false;
  auto& cache  = _valueCache[uniformName];
  if (!std_util::almost_equal(cache[0], x)) {
    cache[0] = x;
    changed  = true;
  }
  if (!std_util::almost_equal(cache[1], y)) {
    cache[1] = y;
    changed  = true;
  }

  return changed;
}

bool Effect::_cacheFloat3(const std::string& uniformName, float x, float y,
                          float z)
{
  if (!std_util::contains(_valueCache, uniformName)) {
    _valueCache[uniformName] = {x, y, z};
    return true;
  }

  bool changed = false;
  auto& cache  = _valueCache[uniformName];
  if (!std_util::almost_equal(cache[0], x)) {
    cache[0] = x;
    changed  = true;
  }
  if (!std_util::almost_equal(cache[1], y)) {
    cache[1] = y;
    changed  = true;
  }
  if (!std_util::almost_equal(cache[2], z)) {
    cache[2] = z;
    changed  = true;
  }

  return changed;
}

bool Effect::_cacheFloat4(const std::string& uniformName, float x, float y,
                          float z, float w)
{
  if (!std_util::contains(_valueCache, uniformName)) {
    _valueCache[uniformName] = {x, y, z, w};
    return true;
  }

  bool changed = false;
  auto& cache  = _valueCache[uniformName];
  if (!std_util::almost_equal(cache[0], x)) {
    cache[0] = x;
    changed  = true;
  }
  if (!std_util::almost_equal(cache[1], y)) {
    cache[1] = y;
    changed  = true;
  }
  if (!std_util::almost_equal(cache[2], z)) {
    cache[2] = z;
    changed  = true;
  }
  if (!std_util::almost_equal(cache[3], w)) {
    cache[3] = w;
    changed  = true;
  }

  return changed;
}

void Effect::bindUniformBuffer(GL::IGLBuffer* /*_buffer*/,
                               const std::string& /*name*/)
{
}

Effect& Effect::setIntArray(const std::string& uniformName,
                            const Int32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setIntArray(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setIntArray2(const std::string& uniformName,
                             const Int32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setIntArray2(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setIntArray3(const std::string& uniformName,
                             const Int32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setIntArray3(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setIntArray4(const std::string& uniformName,
                             const Int32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setIntArray4(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setFloatArray(const std::string& uniformName,
                              const Float32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setFloatArray(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setFloatArray2(const std::string& uniformName,
                               const Float32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setFloatArray2(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setFloatArray3(const std::string& uniformName,
                               const Float32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setFloatArray3(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setFloatArray4(const std::string& uniformName,
                               const Float32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setFloatArray4(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setArray(const std::string& uniformName,
                         std::vector<float> array)
{
  _valueCache.erase(uniformName);
  _engine->setArray(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setArray2(const std::string& uniformName,
                          std::vector<float> array)
{
  _valueCache.erase(uniformName);
  _engine->setArray2(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setArray3(const std::string& uniformName,
                          std::vector<float> array)
{
  _valueCache.erase(uniformName);
  _engine->setArray3(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setArray4(const std::string& uniformName,
                          std::vector<float> array)
{
  _valueCache.erase(uniformName);
  _engine->setArray4(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setMatrices(const std::string& uniformName,
                            Float32Array matrices)
{
  _valueCache.erase(uniformName);
  _engine->setMatrices(getUniform(uniformName), matrices);

  return *this;
}

Effect& Effect::setMatrix(const std::string& uniformName, const Matrix& matrix)
{
  if (_cacheMatrix(uniformName, matrix)) {
    _engine->setMatrix(getUniform(uniformName), matrix);
  }

  return *this;
}

Effect& Effect::setMatrix3x3(const std::string& uniformName,
                             const Float32Array& matrix)
{
  _valueCache.erase(uniformName);
  _engine->setMatrix3x3(getUniform(uniformName), matrix);

  return *this;
}

Effect& Effect::setMatrix2x2(const std::string& uniformName,
                             const Float32Array& matrix)
{
  _valueCache.erase(uniformName);
  _engine->setMatrix2x2(getUniform(uniformName), matrix);

  return *this;
}

Effect& Effect::setFloat(const std::string& uniformName, float value)
{
  if (std_util::contains(_valueCache, uniformName)
      && std_util::almost_equal(_valueCache[uniformName][0], value)) {
    return *this;
  }

  _valueCache[uniformName] = {value};

  _engine->setFloat(getUniform(uniformName), value);

  return *this;
}

Effect& Effect::setBool(const std::string& uniformName, bool _bool)
{
  if (std_util::contains(_valueCache, uniformName)
      && std_util::almost_equal(_valueCache[uniformName][0],
                                _bool ? 1.f : 0.f)) {
    return *this;
  }

  _valueCache[uniformName] = {_bool ? 1.f : 0.f};

  _engine->setBool(getUniform(uniformName), _bool ? 1 : 0);

  return *this;
}

Effect& Effect::setVector2(const std::string& uniformName,
                           const Vector2& vector2)
{
  if (_cacheFloat2(uniformName, vector2.x, vector2.y)) {
    _engine->setFloat2(getUniform(uniformName), vector2.x, vector2.y);
  }

  return *this;
}

Effect& Effect::setFloat2(const std::string& uniformName, float x, float y)
{
  if (_cacheFloat2(uniformName, x, y)) {
    _engine->setFloat2(getUniform(uniformName), x, y);
  }

  return *this;
}

Effect& Effect::setVector3(const std::string& uniformName,
                           const Vector3& vector3)
{
  if (_cacheFloat3(uniformName, vector3.x, vector3.y, vector3.z)) {
    _engine->setFloat3(getUniform(uniformName), vector3.x, vector3.y,
                       vector3.z);
  }

  return *this;
}

Effect& Effect::setFloat3(const std::string& uniformName, float x, float y,
                          float z)
{
  if (_cacheFloat3(uniformName, x, y, z)) {
    _engine->setFloat3(getUniform(uniformName), x, y, z);
  }

  return *this;
}

Effect& Effect::setVector4(const std::string& uniformName,
                           const Vector4& vector4)
{
  if (_cacheFloat4(uniformName, vector4.x, vector4.y, vector4.z, vector4.w)) {
    _engine->setFloat4(getUniform(uniformName), vector4.x, vector4.y, vector4.z,
                       vector4.w);
  }

  return *this;
}

Effect& Effect::setFloat4(const std::string& uniformName, float x, float y,
                          float z, float w)
{
  if (_cacheFloat4(uniformName, x, y, z, w)) {
    _engine->setFloat4(getUniform(uniformName), x, y, z, w);
  }

  return *this;
}

Effect& Effect::setColor3(const std::string& uniformName, const Color3& color3)
{
  if (_cacheFloat3(uniformName, color3.r, color3.g, color3.b)) {
    _engine->setColor3(getUniform(uniformName), color3);
  }

  return *this;
}

Effect& Effect::setColor4(const std::string& uniformName, const Color3& color3,
                          float alpha)
{
  if (_cacheFloat4(uniformName, color3.r, color3.g, color3.b, alpha)) {
    _engine->setColor4(getUniform(uniformName), color3, alpha);
  }

  return *this;
}

std::string Effect::_recombineShader(const std::string& node)
{
  return node;
}

std::string Effect::_evaluateDefinesOnString(const std::string& shaderString)
{
  return shaderString;
}

} // end of namespace BABYLON
