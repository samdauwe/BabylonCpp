#include <babylon/materials/effect.h>

#include <sstream>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/ipipeline_context.h>
#include <babylon/engines/processors/shader_processing_options.h>
#include <babylon/engines/processors/shader_processor.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/effect_includes_shaders_store.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/vector2.h>
#include <babylon/maths/vector4.h>
#include <babylon/misc/string_tools.h>
#include <babylon/misc/tools.h>
#include <babylon/utils/base64.h>

namespace BABYLON {

std::string Effect::ShadersRepository        = "src/Shaders/";
bool Effect::LogShaderCodeOnCompilationError = true;

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
  IEffectCreationOptions& options, ThinEngine* engine)
    : name{baseName}
    , onBind{nullptr}
    , _wasPreviouslyReady{false}
    , _bonesComputationForcedToCPU{false}
    , _multiTarget{false}
    , onBindObservable{this, &Effect::get_onBindObservable}
    , _pipelineContext{nullptr}
    , vertexSourceCode{this, &Effect::get_vertexSourceCode}
    , fragmentSourceCode{this, &Effect::get_fragmentSourceCode}
    , rawVertexSourceCode{this, &Effect::get_rawVertexSourceCode}
    , rawFragmentSourceCode{this, &Effect::get_rawFragmentSourceCode}
    , _onCompileObserver{nullptr}
    , _isReady{false}
    , _compilationError{""}
    , _allFallbacksProcessed{false}

{
  name = baseName;
  _key = options.key;

  std::function<std::string(const std::string& shaderType, const std::string& code)>
    processFinalCode = nullptr;

  {
    _engine = engine;

    _attributesNames           = options.attributes;
    _uniformsNames             = options.uniformsNames;
    _samplerList               = options.samplers;
    defines                    = options.defines;
    onError                    = options.onError;
    onCompiled                 = options.onCompiled;
    _fallbacks                 = std::move(options.fallbacks);
    _indexParameters           = options.indexParameters;
    _transformFeedbackVaryings = options.transformFeedbackVaryings;
    _multiTarget               = options.multiTarget.value_or(false);

    stl_util::concat(_uniformsNames, options.samplers);

    if (!options.uniformBuffersNames.empty()) {
      _uniformBuffersNamesList = options.uniformBuffersNames;
      for (unsigned int i = 0; i < options.uniformBuffersNames.size(); ++i) {
        _uniformBuffersNames[options.uniformBuffersNames[i]] = i;
      }
    }

    processFinalCode = options.processFinalCode ? options.processFinalCode : nullptr;
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

  _processingContext
    = std::static_pointer_cast<ShaderProcessingContext>(_engine->_getShaderProcessingContext());

  ProcessingOptions processorOptions;
  processorOptions.defines                      = StringTools::split(defines, '\n');
  processorOptions.indexParameters              = _indexParameters;
  processorOptions.isFragment                   = false;
  processorOptions.shouldUseHighPrecisionShader = _engine->_shouldUseHighPrecisionShader();
  processorOptions.processor                    = _engine->_shaderProcessor;
  processorOptions.supportsUniformBuffers       = _engine->supportsUniformBuffers();
  processorOptions.shadersRepository            = Effect::ShadersRepository;
  processorOptions.includesShadersStore         = Effect::IncludesShadersStore();
  processorOptions.version           = std::to_string(static_cast<int>(_engine->version() * 100));
  processorOptions.platformName      = _engine->shaderPlatformName();
  processorOptions.processingContext = _processingContext;

  std::string shaderCodes[2] = {"", ""};
  auto shadersLoaded
    = [this, &shaderCodes, &processorOptions, &processFinalCode, &baseName]() -> void {
    if (!shaderCodes[0].empty() && !shaderCodes[1].empty()) {
      processorOptions.isFragment    = true;
      const auto& migratedVertexCode = shaderCodes[0];
      const auto& fragmentCode       = shaderCodes[1];
      ShaderProcessor::Process(
        fragmentCode, processorOptions,
        [&](const std::string& iMigratedFragmentCode) -> void {
          auto migratedFragmentCode = iMigratedFragmentCode;
          if (processFinalCode) {
            migratedFragmentCode = processFinalCode("fragment", migratedFragmentCode);
          }
          auto finalShaders
            = ShaderProcessor::Finalize(migratedVertexCode, migratedFragmentCode, processorOptions);
          _useFinalCode(finalShaders["vertexCode"], finalShaders["fragmentCode"], baseName);
        },
        _engine);
    }
  };
  _loadShader(vertexSource, "Vertex", "",
              [this, &processorOptions, &processFinalCode, &shaderCodes,
               &shadersLoaded](const std::string& vertexCode) -> void {
                ShaderProcessor::Initialize(processorOptions);
                ShaderProcessor::Process(
                  vertexCode, processorOptions,
                  [this, &vertexCode, &processFinalCode, &shaderCodes,
                   &shadersLoaded](const std::string& iMigratedVertexCode) -> void {
                    _rawVertexSourceCode    = vertexCode;
                    auto migratedVertexCode = iMigratedVertexCode;
                    if (processFinalCode) {
                      migratedVertexCode = processFinalCode("vertex", migratedVertexCode);
                    }
                    shaderCodes[0] = migratedVertexCode;
                    shadersLoaded();
                  },
                  _engine);
              });
  _loadShader(fragmentSource, "Fragment", "Pixel",
              [this, &shaderCodes, &shadersLoaded](const std::string& fragmentCode) -> void {
                _rawFragmentSourceCode = fragmentCode;
                shaderCodes[1]         = fragmentCode;
                shadersLoaded();
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

ThinEngine* Effect::getEngine() const
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
  return stl_util::contains(_attributeLocationByName, _name) ? _attributeLocationByName[_name] : -1;
}

size_t Effect::getAttributesCount()
{
  return _attributes.size();
}

int Effect::getUniformIndex(const std::string& uniformName)
{
  return stl_util::index_of(_uniformsNames, uniformName);
}

WebGLUniformLocationPtr Effect::getUniform(const std::string& uniformName)
{
  if (stl_util::contains(_uniforms, uniformName)) {
    return _uniforms[uniformName];
  }

  return nullptr;
}

std::vector<std::string>& Effect::getSamplers()
{
  return _samplerList;
}

std::vector<std::string>& Effect::getUniformNames()
{
  return _uniformsNames;
}

std::vector<std::string>& Effect::getUniformBuffersNames()
{
  return _uniformBuffersNamesList;
}

std::unordered_map<std::string, unsigned int>& Effect::getIndexParameters()
{
  return _indexParameters;
}

std::string Effect::getCompilationError()
{
  return _compilationError;
}

bool Effect::allFallbacksProcessed() const
{
  return _allFallbacksProcessed;
}

void Effect::executeWhenCompiled(const std::function<void(Effect* effect)>& func)
{
  if (isReady()) {
    func(this);
    return;
  }

  onCompileObservable.add([&](Effect* effect, EventState&) { func(effect); });

  if (!_pipelineContext || _pipelineContext->isAsync()) {
    _checkIsReady(nullptr);
  }
}

void Effect::_checkIsReady(const IPipelineContextPtr& previousPipelineContext)
{
  try {
    if (_isReadyInternal()) {
      return;
    }
  }
  catch (const std::exception& e) {
    _processCompilationErrors(e, previousPipelineContext);
    return;
  }

  _checkIsReady(previousPipelineContext);
}

void Effect::_loadShader(const std::string& shader, const std::string& key,
                         const std::string& optionalKey,
                         const std::function<void(const std::string&)>& callback)
{
  // Direct source ?
  if (shader.substr(0, 7) == "source:") {
    callback(shader.substr(7));
    return;
  }

  // Base64 encoded ?
  if (shader.substr(0, 7) == "base64:") {
    const auto shaderBinary = Base64::atob(shader.substr(7));
    callback(shaderBinary);
    return;
  }

  // Is in local store ?
  auto shaderName = shader + key + "Shader";
  if (stl_util::contains(Effect::ShadersStore(), shaderName)) {
    callback(Effect::ShadersStore()[shaderName]);
    return;
  }

  shaderName = shader + optionalKey + "Shader";
  if (!optionalKey.empty() && stl_util::contains(Effect::ShadersStore(), shaderName)) {
    callback(Effect::ShadersStore()[shaderName]);
    return;
  }

  std::string shaderUrl;

  if ((shader.at(0) == '.') || (shader.at(0) == '/')
      || ((shader.find("http") != std::string::npos))) {
    shaderUrl = shader;
  }
  else {
    shaderUrl = Effect::ShadersRepository + shader;
  }

  // Vertex shader
  _engine->_loadFile(
    StringTools::printf("%s.%s.fx", shaderUrl.c_str(), StringTools::toLowerCase(key).c_str()),
    [callback](const std::variant<std::string, ArrayBufferView>& data,
               const std::string& /*responseURL*/) {
      if (std::holds_alternative<std::string>(data)) {
        callback(std::get<std::string>(data));
      }
    });
}

std::string Effect::get_vertexSourceCode() const
{
  return !_vertexSourceCodeOverride.empty() && !_fragmentSourceCodeOverride.empty() ?
           _vertexSourceCodeOverride :
           (_pipelineContext && !_pipelineContext->_getVertexShaderCode().empty() ?
              _pipelineContext->_getVertexShaderCode() :
              _vertexSourceCode);
}

std::string Effect::get_fragmentSourceCode() const
{
  return !_vertexSourceCodeOverride.empty() && !_fragmentSourceCodeOverride.empty() ?
           _fragmentSourceCodeOverride :
           (_pipelineContext && !_pipelineContext->_getFragmentShaderCode().empty() ?
              _pipelineContext->_getFragmentShaderCode() :
              _fragmentSourceCode);
}

std::string Effect::get_rawVertexSourceCode() const
{
  return _rawVertexSourceCode;
}

std::string Effect::get_rawFragmentSourceCode() const
{
  return _rawFragmentSourceCode;
}

void Effect::_rebuildProgram(
  const std::string& iVertexSourceCode, const std::string& iFragmentSourceCode,
  const std::function<void(const IPipelineContextPtr& pipelineContext)>& iOnCompiled,
  const std::function<void(const std::string& message)>& iOnError)
{
  _isReady = false;

  _vertexSourceCodeOverride   = iVertexSourceCode;
  _fragmentSourceCodeOverride = iFragmentSourceCode;
  onError                     = [=](const Effect* /*effect*/, const std::string& error) {
    if (iOnError) {
      iOnError(error);
    }
  };
  this->onCompiled = [=](const Effect* /*effect*/) {
    auto engine = static_cast<Engine*>(getEngine());
    if (engine && !engine->scenes.empty()) {
      for (const auto& scene : engine->scenes) {
        scene->markAllMaterialsAsDirty(Constants::MATERIAL_AllDirtyFlag);
      }
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
  auto previousPipelineContext = _pipelineContext;

  _isReady = false;

  try {
    auto engine = _engine;

    _pipelineContext        = engine->createPipelineContext(_processingContext);
    _pipelineContext->_name = _key;

    auto rebuildRebind = false; // _rebuildProgram.bind(this);
    if (!_vertexSourceCodeOverride.empty() && !_fragmentSourceCodeOverride.empty()) {
      engine->_preparePipelineContext(_pipelineContext, _vertexSourceCodeOverride,
                                      _fragmentSourceCodeOverride, true, _rawVertexSourceCode,
                                      _rawFragmentSourceCode, rebuildRebind, nullptr,
                                      _transformFeedbackVaryings, _key);
    }
    else {
      engine->_preparePipelineContext(_pipelineContext, _vertexSourceCode, _fragmentSourceCode,
                                      false, _rawVertexSourceCode, _rawFragmentSourceCode,
                                      rebuildRebind, defines, _transformFeedbackVaryings, _key);
    }

    engine->_executeWhenRenderingStateIsCompiled(
      _pipelineContext, [this, previousPipelineContext]() -> void {
        auto attributesNames = _attributesNames;
        auto engine          = _engine;

        _attributes = {};
        _pipelineContext->_fillEffectInformation(this,                 // effect
                                                 _uniformBuffersNames, // uniformBuffersNames
                                                 _uniformsNames,       // uniformsNames
                                                 _uniforms,            // uniforms
                                                 _samplerList,         // samplerList
                                                 _samplers,            // samplers
                                                 attributesNames,      // attributesNames
                                                 _attributes           // attributes
        );

        // Caches attribute locations.
        if (!attributesNames.empty()) {
          for (size_t i = 0; i < attributesNames.size(); ++i) {
            _attributeLocationByName[attributesNames[i]] = _attributes[i];
          }
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
      _checkIsReady(previousPipelineContext);
    }
  }
  catch (const std::exception& e) {
    _processCompilationErrors(e, previousPipelineContext);
  }
}

std::tuple<std::string, std::string> Effect::_getShaderCodeAndErrorLine(const std::string& code,
                                                                        const std::string& error,
                                                                        bool isFragment) const
{
  const std::regex regexp(isFragment ? R"(FRAGMENT SHADER ERROR: 0:(\d+?):)" :
                                       R"(VERTEX SHADER ERROR: 0:(\d+?):)",
                          std::regex::optimize);

  std::string errorLine;

  if (!error.empty() && !code.empty()) {
    std::smatch match;
    std::regex_search(error, match, regexp);
    if (match.size() == 2) {
      const auto lineNumber = std::stoul(match.str(1));
      const auto lines      = StringTools::split(code, "\n");
      if (lines.size() >= lineNumber) {
        errorLine
          = StringTools::printf("Offending line [%lu] in %s code: %s", lineNumber,
                                isFragment ? "fragment" : "vertex", lines[lineNumber - 1].c_str());
      }
    }
  }

  return std::make_pair(code, errorLine);
}

void Effect::_processCompilationErrors(const std::exception& e,
                                       const IPipelineContextPtr& previousPipelineContext)
{
  _compilationError           = e.what();
  const auto& attributesNames = _attributesNames;
  const auto& fallbacks       = _fallbacks;

  // Let's go through fallbacks then
  BABYLON_LOG_ERROR("Effect", "Unable to compile effect: ")
  BABYLON_LOGF_ERROR("Effect", "Uniforms: %s", StringTools::join(_uniformsNames, ' ').c_str())
  BABYLON_LOGF_ERROR("Effect", "Attributes: %s", StringTools::join(attributesNames, ' ').c_str())
  BABYLON_LOGF_ERROR("Effect", "Defines: %s", defines.c_str())
  if (Effect::LogShaderCodeOnCompilationError) {
    std::string lineErrorVertex, lineErrorFragment, code;
    const auto vertexShaderCode = _pipelineContext->_getVertexShaderCode();
    if (!vertexShaderCode.empty()) {
      std::tie(code, lineErrorVertex)
        = _getShaderCodeAndErrorLine(vertexShaderCode, _compilationError, false);
      if (!code.empty()) {
        BABYLON_LOGF_ERROR("Effect", "Vertex code:\n%s", code.c_str());
      }
    }
    const auto fragmentShaderCode = _pipelineContext->_getFragmentShaderCode();
    if (!fragmentShaderCode.empty()) {
      std::tie(code, lineErrorFragment)
        = _getShaderCodeAndErrorLine(fragmentShaderCode, _compilationError, true);
      if (!code.empty()) {
        BABYLON_LOGF_ERROR("Effect", "Fragment code:\n%s", code.c_str());
      }
    }
    if (!lineErrorVertex.empty()) {
      BABYLON_LOGF_ERROR("Effect", "%s", lineErrorVertex.c_str());
    }
    if (!lineErrorFragment.empty()) {
      BABYLON_LOGF_ERROR("Effect", "%s", lineErrorFragment.c_str());
    }
  }
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
      _allFallbacksProcessed = false;
      BABYLON_LOG_ERROR("Effect", "Trying next fallback.")
      defines = fallbacks->reduce(defines, this);
      _prepareEffect();
    }
    else { // Sorry we did everything we can
      _allFallbacksProcessed = true;
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
  else {
    _allFallbacksProcessed = true;
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
  _engine->_bindTexture(_getChannel(channel), texture, channel);
}

void Effect::setTexture(const std::string& channel, const ThinTexturePtr& texture)
{
  _engine->setTexture(_getChannel(channel), getUniform(channel), texture, channel);
}

void Effect::setDepthStencilTexture(const std::string& channel,
                                    const RenderTargetTexturePtr& texture)
{
  auto engine = static_cast<Engine*>(_engine);
  if (engine) {
    engine->setDepthStencilTexture(_getChannel(channel), getUniform(channel), texture, channel);
  }
}

void Effect::setTextureArray(const std::string& channel,
                             const std::vector<ThinTexturePtr>& textures)
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

  _engine->setTextureArray(_samplers[channel], getUniform(channel), textures, channel);
}

void Effect::setTextureFromPostProcess(const std::string& channel,
                                       const PostProcessPtr& postProcess)
{
  auto engine = static_cast<Engine*>(_engine);
  if (engine) {
    engine->setTextureFromPostProcess(_getChannel(channel), postProcess, channel);
  }
}

void Effect::setTextureFromPostProcessOutput(const std::string& channel,
                                             const PostProcessPtr& postProcess)
{
  auto engine = static_cast<Engine*>(_engine);
  if (engine) {
    engine->setTextureFromPostProcessOutput(_getChannel(channel), postProcess, channel);
  }
}

void Effect::bindUniformBuffer(const WebGLDataBufferPtr& buffer, const std::string& iName)
{
  if (stl_util::contains(_uniformBuffersNames, iName)) {
    const auto& bufferName = _uniformBuffersNames[iName];
    if (stl_util::contains(Effect::_baseCache, bufferName)
        && (Effect::_baseCache[bufferName] == buffer && _engine->_features.useUBOBindingCache)) {
      return;
    }
  }
  else {
    _uniformBuffersNames[iName] = 0;
  }

  const auto& bufferName         = _uniformBuffersNames[iName];
  Effect::_baseCache[bufferName] = buffer;
  _engine->bindUniformBufferBase(buffer, bufferName, iName);
}

void Effect::bindUniformBlock(const std::string& blockName, unsigned index)
{
  _engine->bindUniformBlock(_pipelineContext, blockName, index);
}

Effect& Effect::setInt(const std::string& uniformName, int value)
{
  if (_pipelineContext) {
    _pipelineContext->setInt(uniformName, value);
  }
  return *this;
}

Effect& Effect::setInt2(const std::string& uniformName, int x, int y)
{
  if (_pipelineContext) {
    _pipelineContext->setInt2(uniformName, x, y);
  }
  return *this;
}

Effect& Effect::setInt3(const std::string& uniformName, int x, int y, int z)
{
  if (_pipelineContext) {
    _pipelineContext->setInt3(uniformName, x, y, z);
  }
  return *this;
}

Effect& Effect::setInt4(const std::string& uniformName, int x, int y, int z, int w)
{
  if (_pipelineContext) {
    _pipelineContext->setInt4(uniformName, x, y, z, w);
  }
  return *this;
}

Effect& Effect::setIntArray(const std::string& uniformName, const Int32Array& array)
{
  if (_pipelineContext) {
    _pipelineContext->setIntArray(uniformName, array);
  }
  return *this;
}

Effect& Effect::setIntArray2(const std::string& uniformName, const Int32Array& array)
{
  if (_pipelineContext) {
    _pipelineContext->setIntArray2(uniformName, array);
  }
  return *this;
}

Effect& Effect::setIntArray3(const std::string& uniformName, const Int32Array& array)
{
  if (_pipelineContext) {
    _pipelineContext->setIntArray3(uniformName, array);
  }
  return *this;
}

Effect& Effect::setIntArray4(const std::string& uniformName, const Int32Array& array)
{
  if (_pipelineContext) {
    _pipelineContext->setIntArray4(uniformName, array);
  }
  return *this;
}

Effect& Effect::setFloatArray(const std::string& uniformName, const Float32Array& array)
{
  if (_pipelineContext) {
    _pipelineContext->setArray(uniformName, array);
  }
  return *this;
}

Effect& Effect::setFloatArray2(const std::string& uniformName, const Float32Array& array)
{
  if (_pipelineContext) {
    _pipelineContext->setArray2(uniformName, array);
  }
  return *this;
}

Effect& Effect::setFloatArray3(const std::string& uniformName, const Float32Array& array)
{
  if (_pipelineContext) {
    _pipelineContext->setArray3(uniformName, array);
  }
  return *this;
}

Effect& Effect::setFloatArray4(const std::string& uniformName, const Float32Array& array)
{
  if (_pipelineContext) {
    _pipelineContext->setArray4(uniformName, array);
  }
  return *this;
}

Effect& Effect::setArray(const std::string& uniformName, Float32Array array)
{
  if (_pipelineContext) {
    _pipelineContext->setArray(uniformName, array);
  }
  return *this;
}

Effect& Effect::setArray2(const std::string& uniformName, Float32Array array)
{
  if (_pipelineContext) {
    _pipelineContext->setArray2(uniformName, array);
  }
  return *this;
}

Effect& Effect::setArray3(const std::string& uniformName, Float32Array array)
{
  if (_pipelineContext) {
    _pipelineContext->setArray3(uniformName, array);
  }
  return *this;
}

Effect& Effect::setArray4(const std::string& uniformName, Float32Array array)
{
  if (_pipelineContext) {
    _pipelineContext->setArray4(uniformName, array);
  }
  return *this;
}

Effect& Effect::setMatrices(const std::string& uniformName, Float32Array matrices)
{
  if (_pipelineContext) {
    _pipelineContext->setMatrices(uniformName, matrices);
  }
  return *this;
}

Effect& Effect::setMatrix(const std::string& uniformName, const Matrix& matrix)
{
  if (_pipelineContext) {
    _pipelineContext->setMatrix(uniformName, matrix);
  }
  return *this;
}

Effect& Effect::setMatrix3x3(const std::string& uniformName, const Float32Array& matrix)
{
  if (_pipelineContext) {
    _pipelineContext->setMatrix3x3(uniformName, matrix);
  }
  return *this;
}

Effect& Effect::setMatrix2x2(const std::string& uniformName, const Float32Array& matrix)
{
  if (_pipelineContext) {
    _pipelineContext->setMatrix2x2(uniformName, matrix);
  }
  return *this;
}

Effect& Effect::setFloat(const std::string& uniformName, float value)
{
  if (_pipelineContext) {
    _pipelineContext->setFloat(uniformName, value);
  }
  return *this;
}

Effect& Effect::setBool(const std::string& uniformName, bool _bool)
{
  if (_pipelineContext) {
    _pipelineContext->setInt(uniformName, _bool ? 1 : 0);
  }
  return *this;
}

Effect& Effect::setVector2(const std::string& uniformName, const Vector2& vector2)
{
  if (_pipelineContext) {
    _pipelineContext->setVector2(uniformName, vector2);
  }
  return *this;
}

Effect& Effect::setFloat2(const std::string& uniformName, float x, float y)
{
  if (_pipelineContext) {
    _pipelineContext->setFloat2(uniformName, x, y);
  }
  return *this;
}

Effect& Effect::setVector3(const std::string& uniformName, const Vector3& vector3)
{
  if (_pipelineContext) {
    _pipelineContext->setVector3(uniformName, vector3);
  }
  return *this;
}

Effect& Effect::setFloat3(const std::string& uniformName, float x, float y, float z)
{
  if (_pipelineContext) {
    _pipelineContext->setFloat3(uniformName, x, y, z);
  }
  return *this;
}

Effect& Effect::setVector4(const std::string& uniformName, const Vector4& vector4)
{
  if (_pipelineContext) {
    _pipelineContext->setVector4(uniformName, vector4);
  }
  return *this;
}

Effect& Effect::setFloat4(const std::string& uniformName, float x, float y, float z, float w)
{
  if (_pipelineContext) {
    _pipelineContext->setFloat4(uniformName, x, y, z, w);
  }
  return *this;
}

Effect& Effect::setColor3(const std::string& uniformName, const Color3& color3)
{
  if (_pipelineContext) {
    _pipelineContext->setColor3(uniformName, color3);
  }
  return *this;
}

Effect& Effect::setColor4(const std::string& uniformName, const Color3& color3, float alpha)
{
  if (_pipelineContext) {
    _pipelineContext->setColor4(uniformName, color3, alpha);
  }
  return *this;
}

Effect& Effect::setDirectColor4(const std::string& uniformName, const Color4& color4)
{
  if (_pipelineContext) {
    _pipelineContext->setDirectColor4(uniformName, color4);
  }
  return *this;
}

void Effect::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  if (_pipelineContext) {
    _pipelineContext->dispose();
  }
  _engine->_releaseEffect(this);
}

void Effect::RegisterShader(const std::string& name, const std::optional<std::string>& pixelShader,
                            const std::optional<std::string>& vertexShader)
{
  if (pixelShader.has_value()) {
    Effect::ShadersStore()[StringTools::concat(name, "PixelShader")] = *pixelShader;
  }

  if (vertexShader) {
    Effect::ShadersStore()[StringTools::concat(name, "VertexShader")] = *vertexShader;
  }
}

void Effect::ResetCache()
{
  Effect::_baseCache.clear();
}

} // end of namespace BABYLON
