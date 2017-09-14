#include <babylon/materials/effect.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/effect_includes_shaders_store.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector4.h>
#include <babylon/tools/tools.h>
#include <babylon/utils/base64.h>

namespace BABYLON {

std::size_t Effect::_uniqueIdSeed = 0;
std::unordered_map<unsigned int, GL::IGLBuffer*> Effect::_baseCache{};

Effect::Effect(const string_t& baseName, EffectCreationOptions& options,
               Engine* engine)
    : name{baseName}
    , defines{options.defines}
    , onCompiled{options.onCompiled}
    , onError{options.onError}
    , uniqueId{Effect::_uniqueIdSeed++}
    , _engine{engine}
    , _uniformsNames{options.uniformsNames}
    , _samplers{options.samplers}
    , _isReady{false}
    , _compilationError{""}
    , _attributesNames{options.attributes}
    , _indexParameters{options.indexParameters}
    , _fallbacks{::std::move(options.fallbacks)}
{
  stl_util::concat(_uniformsNames, options.samplers);

  if (!options.uniformBuffersNames.empty()) {
    for (unsigned int i = 0; i < options.uniformBuffersNames.size(); ++i) {
      _uniformBuffersNames[options.uniformBuffersNames[i]] = i;
    }
  }

  const string_t vertexSource   = baseName;
  const string_t fragmentSource = baseName;

  _loadVertexShader(vertexSource, [this, &fragmentSource,
                                   &baseName](const string_t& vertexCode) {
    _processIncludes(vertexCode, [this, &fragmentSource, &baseName](
                                   const string_t& vertexCodeWithIncludes) {
      _processShaderConversion(
        vertexCodeWithIncludes, false,
        [this, &fragmentSource, &baseName](const string_t& migratedVertexCode) {
          _loadFragmentShader(
            fragmentSource, [this, &migratedVertexCode,
                             &baseName](const string_t& fragmentCode) {
              _processIncludes(
                fragmentCode, [this, &migratedVertexCode, &baseName](
                                const string_t& fragmentCodeWithIncludes) {
                  _processShaderConversion(
                    fragmentCodeWithIncludes, true,
                    [this, &migratedVertexCode,
                     &baseName](const string_t& migratedFragmentCode) {
                      if (!baseName.empty()) {
                        _vertexSourceCode
                          = "#define SHADER_NAME vertex:" + baseName + "\n"
                            + migratedVertexCode;
                        _fragmentSourceCode
                          = "#define SHADER_NAME fragment:" + baseName + "\n"
                            + migratedFragmentCode;
                      }
                      else {
                        _vertexSourceCode   = migratedVertexCode;
                        _fragmentSourceCode = migratedFragmentCode;
                      }
                      _prepareEffect();
                    });
                });
            });
        });
    });
  });
}

Effect::Effect(const std::unordered_map<string_t, string_t>& baseName,
               EffectCreationOptions& options, Engine* engine)
    : defines{options.defines}
    , onCompiled{options.onCompiled}
    , onError{options.onError}
    , uniqueId{Effect::_uniqueIdSeed++}
    , _engine{engine}
    , _uniformsNames{options.uniformsNames}
    , _samplers{options.samplers}
    , _isReady{false}
    , _compilationError{""}
    , _attributesNames{options.attributes}
    , _indexParameters{options.indexParameters}
    , _fallbacks{::std::move(options.fallbacks)}
{
  stl_util::concat(_uniformsNames, options.samplers);

  if (!options.uniformBuffersNames.empty()) {
    for (unsigned int i = 0; i < options.uniformBuffersNames.size(); ++i) {
      _uniformBuffersNames[options.uniformBuffersNames[i]] = i;
    }
  }

  string_t vertexSource   = "";
  string_t fragmentSource = "";

  if (stl_util::contains(baseName, "vertexElement")) {
    vertexSource = baseName.at("vertexElement");
  }
  else if (stl_util::contains(baseName, "vertex")) {
    vertexSource = baseName.at("vertex");
  }

  if (stl_util::contains(baseName, "fragmentElement")) {
    fragmentSource = baseName.at("fragmentElement");
  }
  else if (stl_util::contains(baseName, "fragment")) {
    fragmentSource = baseName.at("fragment");
  }

  if (vertexSource.empty() || fragmentSource.empty()) {
    return;
  }

  name = fragmentSource;

  _loadVertexShader(vertexSource, [this, &fragmentSource,
                                   &vertexSource](const string_t& vertexCode) {
    _processIncludes(vertexCode, [this, &fragmentSource, &vertexSource](
                                   const string_t& vertexCodeWithIncludes) {
      _processShaderConversion(
        vertexCodeWithIncludes, false,
        [this, &fragmentSource,
         &vertexSource](const string_t& migratedVertexCode) {
          _loadFragmentShader(
            fragmentSource, [this, &migratedVertexCode, &fragmentSource,
                             &vertexSource](const string_t& fragmentCode) {
              _processIncludes(
                fragmentCode,
                [this, &migratedVertexCode, &fragmentSource,
                 &vertexSource](const string_t& fragmentCodeWithIncludes) {
                  _processShaderConversion(
                    fragmentCodeWithIncludes, true,
                    [this, &migratedVertexCode, &fragmentSource,
                     &vertexSource](const string_t& migratedFragmentCode) {
                      if (!vertexSource.empty()) {
                        _vertexSourceCode
                          = "#define SHADER_NAME vertex:" + vertexSource + "\n"
                            + migratedVertexCode;
                        _fragmentSourceCode
                          = "#define SHADER_NAME fragment:" + fragmentSource
                            + "\n" + migratedFragmentCode;
                      }
                      else {
                        _vertexSourceCode   = migratedVertexCode;
                        _fragmentSourceCode = migratedFragmentCode;
                      }
                      _prepareEffect();
                    });
                });
            });
        });
    });
  });
}

Effect::~Effect()
{
}

string_t Effect::key() const
{
  return _key;
}

bool Effect::isReady() const
{
  return _isReady;
}

Engine* Effect::getEngine() const
{
  return _engine;
}

GL::IGLProgram* Effect::getProgram()
{
  return _program.get();
}

vector_t<string_t>& Effect::getAttributesNames()
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

int Effect::getAttributeLocationByName(const string_t& _name)
{
  int index = stl_util::index_of(_attributesNames, _name);

  return (index != -1) ? _attributes[static_cast<size_t>(index)] : -1;
}

size_t Effect::getAttributesCount()
{
  return _attributes.size();
}

int Effect::getUniformIndex(const string_t& uniformName)
{
  return stl_util::index_of(_uniformsNames, uniformName);
}

GL::IGLUniformLocation* Effect::getUniform(const string_t& uniformName)
{
  if (stl_util::contains(_uniforms, uniformName)) {
    return _uniforms[uniformName].get();
  }

  return nullptr;
}

vector_t<string_t>& Effect::getSamplers()
{
  return _samplers;
}

string_t Effect::getCompilationError()
{
  return _compilationError;
}

void Effect::executeWhenCompiled(
  const ::std::function<void(Effect* effect)>& func)
{
  if (isReady()) {
    func(this);
    return;
  }

  onCompileObservable.add([&](Effect* effect) { func(effect); });
}

void Effect::_loadVertexShader(
  const string_t& vertex, ::std::function<void(const string_t& data)> callback)
{
  // Base64 encoded ?
  if (vertex.substr(0, 7) == "base64:") {
    auto vertexBinary = base64_atob(vertex.substr(7));
    callback(vertexBinary);
    return;
  }

  // Is in local store ?
  const string_t vertexShaderName = vertex + "VertexShader";
  if (stl_util::contains(EffectShadersStore::Shaders, vertexShaderName)) {
    callback(string_t(EffectShadersStore::Shaders[vertexShaderName]));
    return;
  }

  string_t vertexShaderUrl;

  if ((vertex.at(0) == '.') || (vertex.at(0) == '/')
      || ((vertex.find("http") != string_t::npos))) {
    vertexShaderUrl = vertex;
  }
  else {
    vertexShaderUrl = Engine::ShadersRepository + vertex;
  }

  // Vertex shader
  Tools::LoadFile(vertexShaderUrl + ".vertex.fx", callback);
}

void Effect::_loadFragmentShader(
  const string_t& fragment,
  ::std::function<void(const string_t& data)> callback)
{
  // Base64 encoded ?
  if (fragment.substr(0, 7) == "base64:") {
    auto vertexBinary = base64_atob(fragment.substr(7));
    callback(vertexBinary);
    return;
  }

  // Is in local store ?
  string_t fragmentShaderName = fragment + "PixelShader";
  if (stl_util::contains(EffectShadersStore::Shaders, fragmentShaderName)) {
    callback(string_t(EffectShadersStore::Shaders[fragmentShaderName]));
    return;
  }

  fragmentShaderName = fragment + "FragmentShader";
  if (stl_util::contains(EffectShadersStore::Shaders, fragmentShaderName)) {
    callback(string_t(EffectShadersStore::Shaders[fragmentShaderName]));
    return;
  }

  string_t fragmentShaderUrl;

  if ((fragment.at(0) == '.') || (fragment.at(0) == '/')
      || ((fragment.find("http") != string_t::npos))) {
    fragmentShaderUrl = fragment;
  }
  else {
    fragmentShaderUrl = Engine::ShadersRepository + fragment;
  }

  // Fragment shader
  Tools::LoadFile(fragmentShaderUrl + ".fragment.fx", callback);
}

void Effect::_dumpShadersSource(string_t vertexCode, string_t fragmentCode,
                                string_t defines)
{
  // Rebuild shaders source code
  auto shaderVersion
    = (_engine->webGLVersion() > 1.f) ? "#version 300 es\n" : "";
  auto prefix  = shaderVersion + (!defines.empty() ? defines + "\n" : "");
  vertexCode   = prefix + vertexCode;
  fragmentCode = prefix + fragmentCode;

  // Number lines of shaders source code
  unsigned int i = 2;
  const ::std::regex regex("\n", ::std::regex::optimize);
  auto formattedVertexCode
    = "\n1\t"
      + String::regexReplace(vertexCode, regex,
                             [&i](const ::std::smatch& /*m*/) {
                               return "\n" + ::std::to_string(i++) + "\t";
                             });
  i = 2;
  auto formattedFragmentCode
    = "\n1\t"
      + String::regexReplace(fragmentCode, regex,
                             [&i](const ::std::smatch& /*m*/) {
                               return "\n" + ::std::to_string(i++) + "\t";
                             });

  // Dump shaders name and formatted source code
  BABYLON_LOGF_ERROR("Effect", "Vertex shader: %s%s", name.c_str(),
                     formattedVertexCode.c_str());
  BABYLON_LOGF_ERROR("Effect", "Fragment shader: %s%s", name.c_str(),
                     formattedFragmentCode.c_str());
}

void Effect::_processShaderConversion(
  const string_t& sourceCode, bool isFragment,
  const ::std::function<void(const string_t& data)>& callback)
{

  auto preparedSourceCode = _processPrecision(sourceCode);

  if (_engine->webGLVersion() == 1.f) {
    callback(preparedSourceCode);
    return;
  }

  // Already converted
  if (String::contains(preparedSourceCode, "#version 3")) {
    String::replaceInPlace(preparedSourceCode, "#version 300 es", "");
    callback(preparedSourceCode);
    return;
  }

  // Remove extensions
  // #extension GL_OES_standard_derivatives : enable
  // #extension GL_EXT_shader_texture_lod : enable
  // #extension GL_EXT_frag_depth : enable
  const string_t regex(
    "#extension.+(GL_OES_standard_derivatives|GL_EXT_shader_texture_lod|GL_"
    "EXT_frag_depth).+enable");
  string_t result = String::regexReplace(preparedSourceCode, regex, "");

  // Migrate to GLSL v300
  result = String::regexReplace(preparedSourceCode, "varying(?![\n\r])\\s",
                                isFragment ? "in " : "out ");
  result = String::regexReplace(preparedSourceCode, "attribute[ \t]", "in ");
  result = String::regexReplace(preparedSourceCode, "[ \t]attribute", " in");

  if (isFragment) {
    const vector_t<std::pair<string_t, string_t>> fragMappings{
      ::std::make_pair("texture2DLodEXT\\(", "textureLod("),   //
      ::std::make_pair("textureCubeLodEXT\\(", "textureLod("), //
      ::std::make_pair("texture2D\\(", "texture("),            //
      ::std::make_pair("textureCube\\(", "texture("),          //
      ::std::make_pair("gl_FragDepthEXT", "gl_FragDepth"),     //
      ::std::make_pair("gl_FragColor", "glFragColor"),         //
      ::std::make_pair("void\\s+?main\\(",                     //
                       "out vec4 glFragColor;\nvoid main("),   //
    };
    for (const auto& fragMapping : fragMappings) {
      result = String::regexReplace(preparedSourceCode, fragMapping.first,
                                    fragMapping.second);
    }
  }

  callback(result);
}

void Effect::_processIncludes(
  const string_t& sourceCode,
  const ::std::function<void(const string_t& data)>& callback)
{
  const ::std::regex regex("#include<(.+)>(\\((.*)\\))*(\\[(.*)\\])*",
                           ::std::regex::optimize);
  auto lines = String::split(sourceCode, '\n');

  std::ostringstream returnValue;

  for (auto& line : lines) {
    auto match = String::regexMatch(line, regex);
    if (match.size() != 6) {
      returnValue << line << ::std::endl;
      continue;
    }

    auto includeFile = match[1];

    // Uniform declaration
    if (String::contains(includeFile, "__decl__")) {
      String::replaceInPlace(includeFile, "__decl__", "");
      if (_engine->webGLVersion() != 1.f) {
        String::replaceInPlace(includeFile, "Vertex", "Ubo");
        String::replaceInPlace(includeFile, "Fragment", "Ubo");
      }
      includeFile += "Declaration";
    }

    if (stl_util::contains(EffectIncludesShadersStore::Shaders, includeFile)) {
      // Substitution
      string_t includeContent
        = EffectIncludesShadersStore::Shaders[includeFile];
      if (!match[2].empty()) {
        auto splits = String::split(match[3], ',');

        for (std::size_t index = 0; index < splits.size(); index += 2) {
          const string_t source{splits[index]};
          const string_t dest{splits[index + 1]};

          includeContent = String::regexReplace(includeContent, source, dest);
        }
      }

      if (!match[4].empty()) {
        auto indexString = match[5];

        if (String::contains(indexString, "..")) {
          String::replaceInPlace(indexString, "..", ".");
          auto indexSplits = String::split(indexString, '.');
          if (indexSplits.size() == 2) {
            auto minIndex             = indexSplits[0];
            auto maxIndex             = indexSplits[1];
            auto sourceIncludeContent = includeContent;
            includeContent            = "";

            if ((!String::isDigit(maxIndex))
                && stl_util::contains(_indexParameters, maxIndex)) {
              maxIndex = ::std::to_string(_indexParameters[maxIndex]);
            }

            if (String::isDigit(minIndex) && String::isDigit(maxIndex)) {
              const size_t _minIndex = ::std::stoul(minIndex, nullptr, 0);
              const size_t _maxIndex = ::std::stoul(maxIndex, nullptr, 0);
              for (size_t i = _minIndex; i < _maxIndex; ++i) {
                const auto istr = ::std::to_string(i);
                if (_engine->webGLVersion() == 1.f) {
                  // Ubo replacement
                  const auto callback = [](const ::std::smatch& m) {
                    if (m.size() == 2) {
                      return m.str(1) + "{X}";
                    }
                    return m.str(0);
                  };
                  const ::std::regex regex{"light\\{X\\}.(\\w*)",
                                           ::std::regex::optimize};
                  sourceIncludeContent = String::regexReplace(
                    sourceIncludeContent, regex, callback);
                }
                includeContent
                  += String::regexReplace(sourceIncludeContent, "\\{X\\}", istr)
                     + "\n";
              }
            }
          }
        }
        else {
          if (_engine->webGLVersion() == 1.f) {
            // Ubo replacement
            const auto callback = [](const ::std::smatch& m) {
              if (m.size() == 2) {
                return m.str(1) + "{X}";
              }
              return m.str(0);
            };
            const ::std::regex regex{"light\\{X\\}.(\\w*)",
                                     ::std::regex::optimize};
            includeContent
              = String::regexReplace(includeContent, regex, callback);
          }
          includeContent
            = String::regexReplace(includeContent, "\\{X\\}", indexString);
        }
      }

      // Replace
      String::replaceInPlace(line, match[0], includeContent);
      returnValue << line << ::std::endl;
    }
    else {
      // Load from file
    }
  }

  callback(returnValue.str());
}

string_t Effect::_processPrecision(string_t source)
{
  if (!String::contains(source, "precision highp float")
      && !String::contains(source, "precision mediump float")) {
    if (!_engine->getCaps().highPrecisionShaderSupported) {
      source = "precision mediump float;\n" + source;
    }
    else {
      source = "precision highp float;\n" + source;
    }
  }
  else {
    if (!_engine->getCaps().highPrecisionShaderSupported) {
      // Moving highp to mediump
      String::replaceInPlace(source, "precision highp float",
                             "precision mediump float");
    }
  }

  // Add GL_ES define
  // -- precision mediump float
  const string_t mediump{"#ifdef GL_ES\nprecision mediump float;\n#endif\n"};
  if (String::contains(source, "precision mediump float;")
      && !String::contains(source, mediump)) {
    String::replaceInPlace(source, "precision mediump float;", mediump);
  }

  // -- precision highp float
  const string_t highp{"#ifdef GL_ES\nprecision highp float;\n#endif\n"};
  if (String::contains(source, "precision highp float;")
      && !String::contains(source, highp)) {
    String::replaceInPlace(source, "precision highp float;", highp);
  }

  return source;
}

void Effect::_prepareEffect()
{
  auto attributesNames = _attributesNames;
  auto& _defines       = defines;
  auto& fallbacks      = _fallbacks;
  _valueCache.clear();

  auto engine = _engine;

  try {
    _program = engine->createShaderProgram(_vertexSourceCode,
                                           _fragmentSourceCode, _defines);

    if (engine->webGLVersion() > 1.f) {
      for (auto& item : _uniformBuffersNames) {
        bindUniformBlock(name, item.second);
      }
    }

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
    onCompileObservable.notifyObservers(this);
    onCompileObservable.clear();
  }
  catch (const ::std::exception& e) {
    _compilationError = e.what();

    // Let's go through fallbacks then
    BABYLON_LOG_ERROR("Effect", "Unable to compile effect: ");
    BABYLON_LOGF_ERROR("Effect", "Defines: %s", defines.c_str());
    _dumpShadersSource(_vertexSourceCode, _fragmentSourceCode, defines);
    BABYLON_LOGF_ERROR("Effect", "Uniforms: %s",
                       String::join(_uniformsNames, ' ').c_str());
    BABYLON_LOGF_ERROR("Effect", "Attributes: %s",
                       String::join(attributesNames, ' ').c_str());
    BABYLON_LOGF_ERROR("Effect", "Error: %s", _compilationError.c_str());

    if (!fallbacks && fallbacks->isMoreFallbacks()) {
      BABYLON_LOG_ERROR("Effect", "Trying next fallback.");
      defines = fallbacks->reduce(defines);
      _prepareEffect();
    }
    else { // Sorry we did everything we can
      if (onError) {
        onError(this, _compilationError);
      }
      onErrorObservable.notifyObservers(this);
      onErrorObservable.clear();
    }
  }
}

bool Effect::isSupported() const
{
  return _compilationError.empty();
}

void Effect::_bindTexture(const string_t& channel, InternalTexture* texture)
{
  _engine->_bindTexture(stl_util::index_of(_samplers, channel), texture);
}

void Effect::setTexture(const string_t& channel, BaseTexture* texture)
{
  _engine->setTexture(stl_util::index_of(_samplers, channel),
                      getUniform(channel), texture);
}

void Effect::setTextureArray(const string_t& channel,
                             const vector_t<BaseTexture*>& textures)
{
  if (stl_util::index_of(_samplers, channel + "Ex") == -1) {
    auto initialPos = stl_util::index_of(_samplers, channel);
    for (unsigned int index = 1; index < textures.size(); ++index) {
      stl_util::splice(_samplers, initialPos + static_cast<int>(index), 0,
                       {channel + "Ex"});
    }
  }

  _engine->setTextureArray(stl_util::index_of(_samplers, channel),
                           getUniform(channel), textures);
}

void Effect::setTextureFromPostProcess(const string_t& channel,
                                       PostProcess* postProcess)
{
  _engine->setTextureFromPostProcess(stl_util::index_of(_samplers, channel),
                                     postProcess);
}

bool Effect::_cacheMatrix(const string_t& uniformName, const Matrix& matrix)
{
  auto flag = matrix.updateFlag;
  if (stl_util::contains(_valueCache, uniformName)
      && !_valueCache[uniformName].empty()
      && static_cast<int>(_valueCache[uniformName][0]) == flag) {
    return false;
  }

  _valueCache[uniformName].emplace_back(static_cast<float>(flag));

  return true;
}

bool Effect::_cacheFloat2(const string_t& uniformName, float x, float y)
{
  if (!stl_util::contains(_valueCache, uniformName)) {
    _valueCache[uniformName] = {x, y};
    return true;
  }

  bool changed = false;
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

bool Effect::_cacheFloat3(const string_t& uniformName, float x, float y,
                          float z)
{
  if (!stl_util::contains(_valueCache, uniformName)) {
    _valueCache[uniformName] = {x, y, z};
    return true;
  }

  bool changed = false;
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

bool Effect::_cacheFloat4(const string_t& uniformName, float x, float y,
                          float z, float w)
{
  if (!stl_util::contains(_valueCache, uniformName)) {
    _valueCache[uniformName] = {x, y, z, w};
    return true;
  }

  bool changed = false;
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

void Effect::bindUniformBuffer(GL::IGLBuffer* _buffer, const string_t& name)
{
  if (stl_util::contains(_uniformBuffersNames, name)) {
    if (stl_util::contains(Effect::_baseCache, _uniformBuffersNames[name])
        && Effect::_baseCache[_uniformBuffersNames[name]] == _buffer) {
      return;
    }
  }
  else {
    _uniformBuffersNames[name] = 0;
  }

  Effect::_baseCache[_uniformBuffersNames[name]] = _buffer;
  _engine->bindUniformBufferBase(_buffer, _uniformBuffersNames[name]);
}

void Effect::bindUniformBlock(const string_t& blockName, unsigned index)
{
  _engine->bindUniformBlock(_program.get(), blockName, index);
}

Effect& Effect::setIntArray(const string_t& uniformName,
                            const Int32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setIntArray(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setIntArray2(const string_t& uniformName,
                             const Int32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setIntArray2(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setIntArray3(const string_t& uniformName,
                             const Int32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setIntArray3(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setIntArray4(const string_t& uniformName,
                             const Int32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setIntArray4(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setFloatArray(const string_t& uniformName,
                              const Float32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setFloatArray(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setFloatArray2(const string_t& uniformName,
                               const Float32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setFloatArray2(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setFloatArray3(const string_t& uniformName,
                               const Float32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setFloatArray3(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setFloatArray4(const string_t& uniformName,
                               const Float32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setFloatArray4(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setArray(const string_t& uniformName, Float32Array array)
{
  _valueCache.erase(uniformName);
  _engine->setArray(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setArray2(const string_t& uniformName, Float32Array array)
{
  _valueCache.erase(uniformName);
  _engine->setArray2(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setArray3(const string_t& uniformName, Float32Array array)
{
  _valueCache.erase(uniformName);
  _engine->setArray3(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setArray4(const string_t& uniformName, Float32Array array)
{
  _valueCache.erase(uniformName);
  _engine->setArray4(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setMatrices(const string_t& uniformName, Float32Array matrices)
{
  if (matrices.empty()) {
    return *this;
  }

  _valueCache.erase(uniformName);
  _engine->setMatrices(getUniform(uniformName), matrices);

  return *this;
}

Effect& Effect::setMatrix(const string_t& uniformName, const Matrix& matrix)
{
  if (_cacheMatrix(uniformName, matrix)) {
    _engine->setMatrix(getUniform(uniformName), matrix);
  }

  return *this;
}

Effect& Effect::setMatrix3x3(const string_t& uniformName,
                             const Float32Array& matrix)
{
  _valueCache.erase(uniformName);
  _engine->setMatrix3x3(getUniform(uniformName), matrix);

  return *this;
}

Effect& Effect::setMatrix2x2(const string_t& uniformName,
                             const Float32Array& matrix)
{
  _valueCache.erase(uniformName);
  _engine->setMatrix2x2(getUniform(uniformName), matrix);

  return *this;
}

Effect& Effect::setFloat(const string_t& uniformName, float value)
{
  if (stl_util::contains(_valueCache, uniformName)
      && stl_util::almost_equal(_valueCache[uniformName][0], value)) {
    return *this;
  }

  _valueCache[uniformName] = {value};

  _engine->setFloat(getUniform(uniformName), value);

  return *this;
}

Effect& Effect::setBool(const string_t& uniformName, bool _bool)
{
  if (stl_util::contains(_valueCache, uniformName)
      && stl_util::almost_equal(_valueCache[uniformName][0],
                                _bool ? 1.f : 0.f)) {
    return *this;
  }

  _valueCache[uniformName] = {_bool ? 1.f : 0.f};

  _engine->setBool(getUniform(uniformName), _bool ? 1 : 0);

  return *this;
}

Effect& Effect::setVector2(const string_t& uniformName, const Vector2& vector2)
{
  if (_cacheFloat2(uniformName, vector2.x, vector2.y)) {
    _engine->setFloat2(getUniform(uniformName), vector2.x, vector2.y);
  }

  return *this;
}

Effect& Effect::setFloat2(const string_t& uniformName, float x, float y)
{
  if (_cacheFloat2(uniformName, x, y)) {
    _engine->setFloat2(getUniform(uniformName), x, y);
  }

  return *this;
}

Effect& Effect::setVector3(const string_t& uniformName, const Vector3& vector3)
{
  if (_cacheFloat3(uniformName, vector3.x, vector3.y, vector3.z)) {
    _engine->setFloat3(getUniform(uniformName), vector3.x, vector3.y,
                       vector3.z);
  }

  return *this;
}

Effect& Effect::setFloat3(const string_t& uniformName, float x, float y,
                          float z)
{
  if (_cacheFloat3(uniformName, x, y, z)) {
    _engine->setFloat3(getUniform(uniformName), x, y, z);
  }

  return *this;
}

Effect& Effect::setVector4(const string_t& uniformName, const Vector4& vector4)
{
  if (_cacheFloat4(uniformName, vector4.x, vector4.y, vector4.z, vector4.w)) {
    _engine->setFloat4(getUniform(uniformName), vector4.x, vector4.y, vector4.z,
                       vector4.w);
  }

  return *this;
}

Effect& Effect::setFloat4(const string_t& uniformName, float x, float y,
                          float z, float w)
{
  if (_cacheFloat4(uniformName, x, y, z, w)) {
    _engine->setFloat4(getUniform(uniformName), x, y, z, w);
  }

  return *this;
}

Effect& Effect::setColor3(const string_t& uniformName, const Color3& color3)
{
  if (_cacheFloat3(uniformName, color3.r, color3.g, color3.b)) {
    _engine->setColor3(getUniform(uniformName), color3);
  }

  return *this;
}

Effect& Effect::setColor4(const string_t& uniformName, const Color3& color3,
                          float alpha)
{
  if (_cacheFloat4(uniformName, color3.r, color3.g, color3.b, alpha)) {
    _engine->setColor4(getUniform(uniformName), color3, alpha);
  }

  return *this;
}

void Effect::ResetCache()
{
  Effect::_baseCache.clear();
}

} // end of namespace BABYLON
