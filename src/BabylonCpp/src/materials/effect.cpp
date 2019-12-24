#include <babylon/materials/effect.h>

#include <sstream>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/ipipeline_context.h>
#include <babylon/engines/processors/processing_options.h>
#include <babylon/engines/processors/shader_processor.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/effect_includes_shaders_store.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/material.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/vector2.h>
#include <babylon/maths/vector4.h>
#include <babylon/misc/tools.h>
#include <babylon/utils/base64.h>

namespace BABYLON {

std::string Effect::ShadersRepository = "src/Shaders/";

std::unordered_map<std::string, std::string>& Effect::ShadersStore()
{
  return EffectShadersStore().shaders();
}

std::unordered_map<std::string, std::string>& Effect::IncludesShadersStore()
{
  return EffectIncludesShadersStore().shaders();
}

std::size_t Effect::_uniqueIdSeed = 0;
std::unordered_map<unsigned int, WebGLDataBufferPtr> Effect::_baseCache{};

Effect::Effect(
  const std::variant<std::string, std::unordered_map<std::string, std::string>>& baseName,
  EffectCreationOptions& options, Engine* engine)
    : name{baseName}
    , defines{options.defines}
    , onCompiled{options.onCompiled}
    , onError{options.onError}
    , onBind{nullptr}
    , _bonesComputationForcedToCPU{false}
    , onBindObservable{this, &Effect::get_onBindObservable}
    , _pipelineContext{nullptr}
    , _onCompileObserver{nullptr}
    , _engine{engine}
    , _uniformsNames{options.uniformsNames}
    , _samplerList{options.samplers}
    , _isReady{false}
    , _compilationError{""}
    , _attributesNames{options.attributes}
    , _indexParameters{options.indexParameters}
    , _fallbacks{std::move(options.fallbacks)}
    , _transformFeedbackVaryings{options.transformFeedbackVaryings}
{
  stl_util::concat(_uniformsNames, options.samplers);

  if (!options.uniformBuffersNames.empty()) {
    for (unsigned int i = 0; i < options.uniformBuffersNames.size(); ++i) {
      _uniformBuffersNames[options.uniformBuffersNames[i]] = i;
    }
  }

  uniqueId = Effect::_uniqueIdSeed++;

  std::string vertexSource;
  std::string fragmentSource;

  if (std::holds_alternative<std::unordered_map<std::string, std::string>>(baseName)) {
    auto& iBaseName = std::get<std::unordered_map<std::string, std::string>>(baseName);
    if (stl_util::contains(iBaseName, "vertexSource")) {
      vertexSource = "source:" + iBaseName.at("vertexSource");
    }
    else if (stl_util::contains(iBaseName, "vertexElement")) {
      vertexSource = iBaseName.at("vertexElement");
    }
    else if (stl_util::contains(iBaseName, "vertex")) {
      vertexSource = iBaseName.at("vertex");
    }

    if (stl_util::contains(iBaseName, "fragmentSource")) {
      fragmentSource = "source:" + iBaseName.at("fragmentSource");
    }
    else if (stl_util::contains(iBaseName, "fragmentElement")) {
      fragmentSource = iBaseName.at("fragmentElement");
    }
    else if (stl_util::contains(iBaseName, "fragment")) {
      fragmentSource = iBaseName.at("fragment");
    }
  }
  else if (std::holds_alternative<std::string>(baseName)) {
    vertexSource   = std::get<std::string>(baseName);
    fragmentSource = std::get<std::string>(baseName);
  }

  ProcessingOptions processorOptions;
  processorOptions.defines                      = String::split(defines, '\n');
  processorOptions.indexParameters              = _indexParameters;
  processorOptions.isFragment                   = false;
  processorOptions.shouldUseHighPrecisionShader = _engine->_shouldUseHighPrecisionShader;
  processorOptions.processor                    = _engine->_shaderProcessor;
  processorOptions.supportsUniformBuffers       = _engine->supportsUniformBuffers();
  processorOptions.shadersRepository            = Effect::ShadersRepository;
  processorOptions.includesShadersStore         = Effect::IncludesShadersStore();
  processorOptions.version      = std::to_string(static_cast<int>(_engine->webGLVersion() * 100));
  processorOptions.platformName = _engine->webGLVersion() >= 2 ? "WEBGL2" : "WEBGL1";

  _loadVertexShader(
    vertexSource,
    [this, &fragmentSource, &processorOptions, &baseName](const std::string& vertexCode) -> void {
      _loadFragmentShader(
        fragmentSource,
        [this, &vertexCode, &processorOptions, &baseName](const std::string& fragmentCode) -> void {
          ShaderProcessor::Process(
            vertexCode, processorOptions,
            [this, &fragmentCode, &processorOptions,
             &baseName](const std::string& migratedVertexCode) -> void {
              processorOptions.isFragment = true;
              ShaderProcessor::Process(
                fragmentCode, processorOptions,
                [this, &migratedVertexCode, &baseName](const std::string& migratedFragmentCode)
                  -> void { _useFinalCode(migratedVertexCode, migratedFragmentCode, baseName); });
            });
        });
    });
}

Effect::~Effect() = default;

void Effect::_useFinalCode(
  const std::string& migratedVertexCode, const std::string& migratedFragmentCode,
  const std::variant<std::string, std::unordered_map<std::string, std::string>>& baseName)
{
  std::string vertex;
  std::string fragment;
  if (std::holds_alternative<std::unordered_map<std::string, std::string>>(baseName)) {
    auto& iBaseName = std::get<std::unordered_map<std::string, std::string>>(baseName);
    if (stl_util::contains(iBaseName, "vertexElement")) {
      vertex = iBaseName.at("vertexElement");
    }
    else if (stl_util::contains(iBaseName, "vertex")) {
      vertex = iBaseName.at("vertex");
    }
    else if (stl_util::contains(iBaseName, "spectorName")) {
      vertex = iBaseName.at("spectorName");
    }

    if (stl_util::contains(iBaseName, "fragmentElement")) {
      fragment = iBaseName.at("fragmentElement");
    }
    else if (stl_util::contains(iBaseName, "fragment")) {
      fragment = iBaseName.at("fragment");
    }
    else if (stl_util::contains(iBaseName, "spectorName")) {
      fragment = iBaseName.at("spectorName");
    }
  }
  else if (std::holds_alternative<std::string>(baseName)) {
    vertex   = std::get<std::string>(baseName);
    fragment = std::get<std::string>(baseName);
  }

  if (!vertex.empty() && !fragment.empty()) {
    _vertexSourceCode   = "#define SHADER_NAME vertex:" + vertex + "\n" + migratedVertexCode;
    _fragmentSourceCode = "#define SHADER_NAME fragment:" + fragment + "\n" + migratedFragmentCode;
  }
  else {
    _vertexSourceCode   = migratedVertexCode;
    _fragmentSourceCode = migratedFragmentCode;
  }
  _prepareEffect();
}

Observable<Effect>& Effect::get_onBindObservable()
{
  return _onBindObservable;
}

std::string Effect::key() const
{
  return _key;
}

bool Effect::isReady() const
{
  return _isReadyInternal();
}

bool Effect::_isReadyInternal() const
{
  if (_isReady) {
    return true;
  }
  if (_pipelineContext) {
    return _pipelineContext->isReady();
  }
  return false;
}

Engine* Effect::getEngine() const
{
  return _engine;
}

IPipelineContextPtr& Effect::getPipelineContext()
{
  return _pipelineContext;
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
  int index = stl_util::index_of(_attributesNames, _name);

  return (index != -1) ? _attributes[static_cast<size_t>(index)] : -1;
}

size_t Effect::getAttributesCount()
{
  return _attributes.size();
}

int Effect::getUniformIndex(const std::string& uniformName)
{
  return stl_util::index_of(_uniformsNames, uniformName);
}

GL::IGLUniformLocation* Effect::getUniform(const std::string& uniformName)
{
  if (stl_util::contains(_uniforms, uniformName)) {
    return _uniforms[uniformName].get();
  }

  return nullptr;
}

std::vector<std::string>& Effect::getSamplers()
{
  return _samplerList;
}

std::string Effect::getCompilationError()
{
  return _compilationError;
}

void Effect::executeWhenCompiled(const std::function<void(Effect* effect)>& func)
{
  if (isReady()) {
    func(this);
    return;
  }

  onCompileObservable.add([&](Effect* effect, EventState&) { func(effect); });

  if (!_pipelineContext || _pipelineContext->isAsync()) {
    _checkIsReady();
  }
}

void Effect::_checkIsReady()
{
  _isReadyInternal();
}

void Effect::_loadVertexShader(const std::string& vertex,
                               const std::function<void(const std::string&)>& callback)
{
  // Direct source ?
  if (vertex.substr(0, 7) == "source:") {
    callback(vertex.substr(7));
    return;
  }

  // Base64 encoded ?
  if (vertex.substr(0, 7) == "base64:") {
    auto vertexBinary = Base64::atob(vertex.substr(7));
    callback(vertexBinary);
    return;
  }

  // Is in local store ?
  const std::string vertexShaderName = vertex + "VertexShader";
  if (stl_util::contains(Effect::ShadersStore(), vertexShaderName)) {
    callback(Effect::ShadersStore()[vertexShaderName]);
    return;
  }

  std::string vertexShaderUrl;

  if ((vertex.at(0) == '.') || (vertex.at(0) == '/')
      || ((vertex.find("http") != std::string::npos))) {
    vertexShaderUrl = vertex;
  }
  else {
    vertexShaderUrl = Effect::ShadersRepository + vertex;
  }

  // Vertex shader
  _engine->_loadFile(vertexShaderUrl + ".vertex.fx",
                     [callback](const std::variant<std::string, ArrayBuffer>& data,
                                const std::string& /*responseURL*/) {
                       if (std::holds_alternative<std::string>(data)) {
                         callback(std::get<std::string>(data));
                       }
                     });
}

void Effect::_loadFragmentShader(const std::string& fragment,
                                 const std::function<void(const std::string&)>& callback)
{
  // Direct source ?
  if (fragment.substr(0, 7) == "source:") {
    callback(fragment.substr(7));
    return;
  }

  // Base64 encoded ?
  if (fragment.substr(0, 7) == "base64:") {
    auto fragmentBinary = Base64::atob(fragment.substr(7));
    callback(fragmentBinary);
    return;
  }

  // Is in local store ?
  std::string fragmentShaderName = fragment + "PixelShader";
  if (stl_util::contains(Effect::ShadersStore(), fragmentShaderName)) {
    callback(Effect::ShadersStore()[fragmentShaderName]);
    return;
  }

  fragmentShaderName = fragment + "FragmentShader";
  if (stl_util::contains(Effect::ShadersStore(), fragmentShaderName)) {
    callback(Effect::ShadersStore()[fragmentShaderName]);
    return;
  }

  std::string fragmentShaderUrl;

  if ((fragment.at(0) == '.') || (fragment.at(0) == '/')
      || ((fragment.find("http") != std::string::npos))) {
    fragmentShaderUrl = fragment;
  }
  else {
    fragmentShaderUrl = Effect::ShadersRepository + fragment;
  }

  // Fragment shader
  _engine->_loadFile(fragmentShaderUrl + ".fragment.fx",
                     [callback](const std::variant<std::string, ArrayBuffer>& data,
                                const std::string& /*responseURL*/) {
                       if (std::holds_alternative<std::string>(data)) {
                         callback(std::get<std::string>(data));
                       }
                     });
}

void Effect::_dumpShadersSource(std::string vertexCode, std::string fragmentCode,
                                const std::string& iDefines)
{
  // Rebuild shaders source code
  auto shaderVersion = (_engine->webGLVersion() > 1.f) ? "#version 300 es\n#define WEBGL2 \n" : "";
  auto prefix        = shaderVersion + (!iDefines.empty() ? iDefines + "\n" : "");
  vertexCode         = prefix + vertexCode;
  fragmentCode       = prefix + fragmentCode;

  // Number lines of shaders source code
  unsigned int i = 2;
  const std::regex regex("\n", std::regex::optimize);
  auto formattedVertexCode
    = "\n1\t" + String::regexReplace(vertexCode, regex, [&i](const std::smatch& /*m*/) {
        return "\n" + std::to_string(i++) + "\t";
      });
  i = 2;
  auto formattedFragmentCode
    = "\n1\t" + String::regexReplace(fragmentCode, regex, [&i](const std::smatch& /*m*/) {
        return "\n" + std::to_string(i++) + "\t";
      });

  // Dump shaders name and formatted source code
  std::string vertexShaderName;
  std::string fragmentShaderName;
  if (std::holds_alternative<std::unordered_map<std::string, std::string>>(name)) {
    const auto& iName = std::get<std::unordered_map<std::string, std::string>>(name);
    // Vertex shader
    if (stl_util::contains(iName, "vertexElement")) {
      vertexShaderName = iName.at("vertexElement");
    }
    else if (stl_util::contains(iName, "vertex")) {
      vertexShaderName = iName.at("vertex");
    }
    // Fragment shader
    if (stl_util::contains(iName, "fragmentElement")) {
      fragmentShaderName = iName.at("fragmentElement");
    }
    else if (stl_util::contains(iName, "fragment")) {
      fragmentShaderName = iName.at("fragment");
    }
  }
  else if (std::holds_alternative<std::string>(name)) {
    vertexShaderName   = std::get<std::string>(name);
    fragmentShaderName = std::get<std::string>(name);
  }

  BABYLON_LOGF_ERROR("Effect", "Vertex shader: %s%s", vertexShaderName.c_str(),
                     formattedVertexCode.c_str())
  BABYLON_LOGF_ERROR("Effect", "Fragment shader: %s%s", fragmentShaderName.c_str(),
                     formattedFragmentCode.c_str())
}

void Effect::_rebuildProgram(
  const std::string& vertexSourceCode, const std::string& fragmentSourceCode,
  const std::function<void(const IPipelineContextPtr& pipelineContext)>& iOnCompiled,
  const std::function<void(const std::string& message)>& iOnError)
{
  _isReady = false;

  _vertexSourceCodeOverride   = vertexSourceCode;
  _fragmentSourceCodeOverride = fragmentSourceCode;
  onError                     = [&](const Effect* /*effect*/, const std::string& error) {
    if (iOnError) {
      iOnError(error);
    }
  };
  this->onCompiled = [&](const Effect* /*effect*/) {
    for (const auto& scene : getEngine()->scenes) {
      scene->markAllMaterialsAsDirty(Constants::MATERIAL_AllDirtyFlag);
    }

    if (_pipelineContext) {
      _pipelineContext->_handlesSpectorRebuildCallback(iOnCompiled);
    }
  };
  _fallbacks = nullptr;
  _prepareEffect();
}

void Effect::_prepareEffect()
{
  _valueCache.clear();

  auto previousPipelineContext = _pipelineContext;

  try {
    auto engine = _engine;

    _pipelineContext = engine->createPipelineContext();

    auto rebuildRebind = false; // _rebuildProgram.bind(this);
    if (!_vertexSourceCodeOverride.empty() && !_fragmentSourceCodeOverride.empty()) {
      engine->_preparePipelineContext(_pipelineContext, _vertexSourceCodeOverride,
                                      _fragmentSourceCodeOverride, true, rebuildRebind, nullptr,
                                      _transformFeedbackVaryings);
    }
    else {
      engine->_preparePipelineContext(_pipelineContext, _vertexSourceCode, _fragmentSourceCode,
                                      false, rebuildRebind, defines, _transformFeedbackVaryings);
    }

    engine->_executeWhenRenderingStateIsCompiled(
      _pipelineContext, [this, previousPipelineContext]() -> void {
        auto attributesNames = _attributesNames;
        auto engine          = _engine;
        if (engine->supportsUniformBuffers()) {
          for (const auto& [uniformBuffersName, value] : _uniformBuffersNames) {
            bindUniformBlock(uniformBuffersName, value);
          }
        }

        auto uniforms = engine->getUniforms(_pipelineContext, _uniformsNames);
        for (auto& [uniformsName, uniformLocation] : uniforms) {
          _uniforms[uniformsName] = std::move(uniformLocation);
        }

        _attributes = engine->getAttributes(_pipelineContext, attributesNames);

        for (unsigned int index = 0; index < _samplerList.size(); ++index) {
          auto sampler = getUniform(_samplerList[index]);

          if (!sampler) {
            stl_util::splice(_samplerList, static_cast<int>(index), 1);
            --index;
          }
        }

        for (unsigned int index = 0; index < _samplerList.size(); ++index) {
          _samplers[_samplerList[index]] = static_cast<int>(index);
        }

        engine->bindSamplers(*this);

        _compilationError.clear();
        _isReady = true;
        if (onCompiled) {
          onCompiled(this);
        }
        onCompileObservable.notifyObservers(this);
        onCompileObservable.clear();

        // Unbind mesh reference in fallbacks
        if (_fallbacks) {
          _fallbacks->unBindMesh();
        }

        if (previousPipelineContext) {
          getEngine()->_deletePipelineContext(previousPipelineContext);
        }
      });

    if (_pipelineContext->isAsync()) {
      _checkIsReady();
    }
  }
  catch (const std::exception& e) {
    _processCompilationErrors(e, previousPipelineContext);
  }
}

void Effect::_processCompilationErrors(const std::exception& e,
                                       const IPipelineContextPtr& previousPipelineContext)
{
  _compilationError           = e.what();
  const auto& attributesNames = _attributesNames;
  const auto& fallbacks       = _fallbacks;

  // Let's go through fallbacks then
  BABYLON_LOG_ERROR("Effect", "Unable to compile effect: ")
  BABYLON_LOGF_ERROR("Effect", "Uniforms: %s", String::join(_uniformsNames, ' ').c_str())
  BABYLON_LOGF_ERROR("Effect", "Attributes: %s", String::join(attributesNames, ' ').c_str())
  BABYLON_LOGF_ERROR("Effect", "Defines: %s", defines.c_str())
  BABYLON_LOGF_ERROR("Effect", "Error: %s", _compilationError.c_str())

  if (previousPipelineContext) {
    _pipelineContext = previousPipelineContext;
    _isReady         = true;
    if (onError) {
      onError(this, _compilationError);
    }
    onErrorObservable.notifyObservers(this);
  }

  if (fallbacks) {
    _pipelineContext = nullptr;
    if (fallbacks->hasMoreFallbacks()) {
      BABYLON_LOG_ERROR("Effect", "Trying next fallback.")
      defines = fallbacks->reduce(defines, this);
      _prepareEffect();
    }
    else { // Sorry we did everything we can
      if (onError) {
        onError(this, _compilationError);
      }
      onErrorObservable.notifyObservers(this);
      onErrorObservable.clear();

      // Unbind mesh reference in fallbacks
      if (_fallbacks) {
        _fallbacks->unBindMesh();
      }
    }
  }
}

bool Effect::isSupported() const
{
  return _compilationError.empty();
}

int Effect::_getChannel(const std::string& channel)
{
  return stl_util::contains(_samplers, channel) ? _samplers[channel] : -1;
}

void Effect::_bindTexture(const std::string& channel, const InternalTexturePtr& texture)
{
  _engine->_bindTexture(_getChannel(channel), texture);
}

void Effect::setTexture(const std::string& channel, const BaseTexturePtr& texture)
{
  _engine->setTexture(_getChannel(channel), getUniform(channel), texture);
}

void Effect::setDepthStencilTexture(const std::string& channel,
                                    const RenderTargetTexturePtr& texture)
{
  _engine->setDepthStencilTexture(_getChannel(channel), getUniform(channel), texture);
}

void Effect::setTextureArray(const std::string& channel,
                             const std::vector<BaseTexturePtr>& textures)
{
  const auto exName = channel + "Ex";
  if (!stl_util::contains(_samplerList, exName + "0")) {
    auto initialPos = stl_util::index_of(_samplerList, channel);
    for (size_t index = 1; index < textures.size(); index++) {
      const auto currentExName = exName + std::to_string(index - 1);
      stl_util::splice(_samplerList, initialPos + static_cast<int>(index), 0, {currentExName});
    }

    // Reset every channels
    int channelIndex = 0;
    for (const auto& key : _samplerList) {
      _samplers[key] = channelIndex;
      channelIndex += 1;
    }
  }

  _engine->setTextureArray(_samplers[channel], getUniform(channel), textures);
}

void Effect::setTextureFromPostProcess(const std::string& channel, PostProcess* postProcess)
{
  _engine->setTextureFromPostProcess(_getChannel(channel), postProcess);
}

void Effect::setTextureFromPostProcessOutput(const std::string& channel, PostProcess* postProcess)
{
  _engine->setTextureFromPostProcessOutput(_getChannel(channel), postProcess);
}

bool Effect::_cacheMatrix(const std::string& uniformName, const Matrix& matrix)
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

bool Effect::_cacheFloat2(const std::string& uniformName, float x, float y)
{
  if (!stl_util::contains(_valueCache, uniformName)) {
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

bool Effect::_cacheFloat3(const std::string& uniformName, float x, float y, float z)
{
  if (!stl_util::contains(_valueCache, uniformName)) {
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

bool Effect::_cacheFloat4(const std::string& uniformName, float x, float y, float z, float w)
{
  if (!stl_util::contains(_valueCache, uniformName)) {
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

void Effect::bindUniformBuffer(const WebGLDataBufferPtr& buffer, const std::string& iName)
{
  if (stl_util::contains(_uniformBuffersNames, iName)) {
    const auto& bufferName = _uniformBuffersNames[iName];
    if (stl_util::contains(Effect::_baseCache, bufferName)
        && Effect::_baseCache[bufferName] == buffer) {
      return;
    }
  }
  else {
    _uniformBuffersNames[iName] = 0;
  }

  const auto& bufferName         = _uniformBuffersNames[iName];
  Effect::_baseCache[bufferName] = buffer;
  _engine->bindUniformBufferBase(buffer, bufferName);
}

void Effect::bindUniformBlock(const std::string& blockName, unsigned index)
{
  _engine->bindUniformBlock(_pipelineContext, blockName, index);
}

Effect& Effect::setInt(const std::string& uniformName, int value)
{
  Float32Array _value{static_cast<float>(value)};
  if (stl_util::contains(_valueCache, uniformName) && (_valueCache[uniformName] == _value)) {
    return *this;
  }

  _valueCache[uniformName] = _value;

  _engine->setInt(getUniform(uniformName), value);

  return *this;
}

Effect& Effect::setIntArray(const std::string& uniformName, const Int32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setIntArray(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setIntArray2(const std::string& uniformName, const Int32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setIntArray2(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setIntArray3(const std::string& uniformName, const Int32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setIntArray3(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setIntArray4(const std::string& uniformName, const Int32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setIntArray4(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setFloatArray(const std::string& uniformName, const Float32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setFloatArray(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setFloatArray2(const std::string& uniformName, const Float32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setFloatArray2(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setFloatArray3(const std::string& uniformName, const Float32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setFloatArray3(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setFloatArray4(const std::string& uniformName, const Float32Array& array)
{
  _valueCache.erase(uniformName);
  _engine->setFloatArray4(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setArray(const std::string& uniformName, Float32Array array)
{
  _valueCache.erase(uniformName);
  _engine->setArray(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setArray2(const std::string& uniformName, Float32Array array)
{
  _valueCache.erase(uniformName);
  _engine->setArray2(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setArray3(const std::string& uniformName, Float32Array array)
{
  _valueCache.erase(uniformName);
  _engine->setArray3(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setArray4(const std::string& uniformName, Float32Array array)
{
  _valueCache.erase(uniformName);
  _engine->setArray4(getUniform(uniformName), array);

  return *this;
}

Effect& Effect::setMatrices(const std::string& uniformName, Float32Array matrices)
{
  if (matrices.empty()) {
    return *this;
  }

  _valueCache.erase(uniformName);
  _engine->setMatrices(getUniform(uniformName), matrices);

  return *this;
}

Effect& Effect::setMatrix(const std::string& uniformName, const Matrix& matrix)
{
  if (_cacheMatrix(uniformName, matrix)) {
    _engine->setMatrices(getUniform(uniformName), matrix.toArray());
  }

  return *this;
}

Effect& Effect::setMatrix3x3(const std::string& uniformName, const Float32Array& matrix)
{
  _valueCache.erase(uniformName);
  _engine->setMatrix3x3(getUniform(uniformName), matrix);

  return *this;
}

Effect& Effect::setMatrix2x2(const std::string& uniformName, const Float32Array& matrix)
{
  _valueCache.erase(uniformName);
  _engine->setMatrix2x2(getUniform(uniformName), matrix);

  return *this;
}

Effect& Effect::setFloat(const std::string& uniformName, float value)
{
  if (stl_util::contains(_valueCache, uniformName)
      && stl_util::almost_equal(_valueCache[uniformName][0], value)) {
    return *this;
  }

  _valueCache[uniformName] = {value};

  _engine->setFloat(getUniform(uniformName), value);

  return *this;
}

Effect& Effect::setBool(const std::string& uniformName, bool _bool)
{
  if (stl_util::contains(_valueCache, uniformName)
      && stl_util::almost_equal(_valueCache[uniformName][0], _bool ? 1.f : 0.f)) {
    return *this;
  }

  _valueCache[uniformName] = {_bool ? 1.f : 0.f};

  _engine->setBool(getUniform(uniformName), _bool ? 1 : 0);

  return *this;
}

Effect& Effect::setVector2(const std::string& uniformName, const Vector2& vector2)
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

Effect& Effect::setVector3(const std::string& uniformName, const Vector3& vector3)
{
  if (_cacheFloat3(uniformName, vector3.x, vector3.y, vector3.z)) {
    _engine->setFloat3(getUniform(uniformName), vector3.x, vector3.y, vector3.z);
  }

  return *this;
}

Effect& Effect::setFloat3(const std::string& uniformName, float x, float y, float z)
{
  if (_cacheFloat3(uniformName, x, y, z)) {
    _engine->setFloat3(getUniform(uniformName), x, y, z);
  }

  return *this;
}

Effect& Effect::setVector4(const std::string& uniformName, const Vector4& vector4)
{
  if (_cacheFloat4(uniformName, vector4.x, vector4.y, vector4.z, vector4.w)) {
    _engine->setFloat4(getUniform(uniformName), vector4.x, vector4.y, vector4.z, vector4.w);
  }

  return *this;
}

Effect& Effect::setFloat4(const std::string& uniformName, float x, float y, float z, float w)
{
  if (_cacheFloat4(uniformName, x, y, z, w)) {
    _engine->setFloat4(getUniform(uniformName), x, y, z, w);
  }

  return *this;
}

Effect& Effect::setColor3(const std::string& uniformName, const Color3& color3)
{
  if (_cacheFloat3(uniformName, color3.r, color3.g, color3.b)) {
    _engine->setFloat3(getUniform(uniformName), color3.r, color3.g, color3.b);
  }

  return *this;
}

Effect& Effect::setColor4(const std::string& uniformName, const Color3& color3, float alpha)
{
  if (_cacheFloat4(uniformName, color3.r, color3.g, color3.b, alpha)) {
    _engine->setFloat4(getUniform(uniformName), color3.r, color3.g, color3.b, alpha);
  }

  return *this;
}

Effect& Effect::setDirectColor4(const std::string& uniformName, const Color4& color4)
{
  if (_cacheFloat4(uniformName, color4.r, color4.g, color4.b, color4.a)) {
    _engine->setDirectColor4(getUniform(uniformName), color4);
  }

  return *this;
}

void Effect::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  _engine->_releaseEffect(this);
}

void Effect::RegisterShader(const std::string& name, const std::optional<std::string>& pixelShader,
                            const std::optional<std::string>& vertexShader)
{
  if (pixelShader.has_value()) {
    Effect::ShadersStore()[String::concat(name, "PixelShader")] = *pixelShader;
  }

  if (vertexShader) {
    Effect::ShadersStore()[String::concat(name, "VertexShader")] = *vertexShader;
  }
}

void Effect::ResetCache()
{
  Effect::_baseCache.clear();
}

} // end of namespace BABYLON
