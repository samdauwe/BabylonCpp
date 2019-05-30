#include <babylon/materials/effect.h>

#include <sstream>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/effect_includes_shaders_store.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/material.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector4.h>
#include <babylon/shaders/shadersinclude/glsl_version_3.h>
#include <babylon/tools/tools.h>
#include <babylon/utils/base64.h>

namespace BABYLON {

std::unordered_map<std::string, std::string>& Effect::ShadersStore()
{
  return EffectShadersStore().shaders();
}

std::unordered_map<std::string, std::string>& Effect::IncludesShadersStore()
{
  return EffectIncludesShadersStore().shaders();
}

std::size_t Effect::_uniqueIdSeed = 0;
std::unordered_map<unsigned int, GL::IGLBuffer*> Effect::_baseCache{};

Effect::Effect(const std::string& baseName, EffectCreationOptions& options,
               Engine* engine)
    : name{baseName}
    , defines{options.defines}
    , onCompiled{options.onCompiled}
    , onError{options.onError}
    , onBind{nullptr}
    , uniqueId{Effect::_uniqueIdSeed++}
    , _bonesComputationForcedToCPU{false}
    , onBindObservable{this, &Effect::get_onBindObservable}
    , _program{nullptr}
    , _onCompileObserver{nullptr}
    , _engine{engine}
    , _uniformsNames{options.uniformsNames}
    , _samplers{options.samplers}
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

  const auto& vertexSource   = baseName;
  const auto& fragmentSource = baseName;

  _loadVertexShader(vertexSource, [this, &fragmentSource,
                                   &baseName](const std::string& vertexCode) {
    _processIncludes(vertexCode, [this, &fragmentSource, &baseName](
                                   const std::string& vertexCodeWithIncludes) {
      _processShaderConversion(
        vertexCodeWithIncludes, false,
        [this, &fragmentSource,
         &baseName](const std::string& migratedVertexCode) {
          _loadFragmentShader(
            fragmentSource, [this, &migratedVertexCode,
                             &baseName](const std::string& fragmentCode) {
              _processIncludes(
                fragmentCode, [this, &migratedVertexCode, &baseName](
                                const std::string& fragmentCodeWithIncludes) {
                  _processShaderConversion(
                    fragmentCodeWithIncludes, true,
                    [this, &migratedVertexCode,
                     &baseName](const std::string& migratedFragmentCode) {
                      if (!baseName.empty()) {
                        const auto& vertex   = baseName;
                        const auto& fragment = baseName;

                        _vertexSourceCode
                          = "#define SHADER_NAME vertex:" + vertex + "\n"
                            + migratedVertexCode;
                        _fragmentSourceCode
                          = "#define SHADER_NAME fragment:" + fragment + "\n"
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

Effect::Effect(const std::unordered_map<std::string, std::string>& baseName,
               EffectCreationOptions& options, Engine* engine)
    : defines{options.defines}
    , onCompiled{options.onCompiled}
    , onError{options.onError}
    , onBind{nullptr}
    , uniqueId{Effect::_uniqueIdSeed++}
    , _bonesComputationForcedToCPU{false}
    , onBindObservable{this, &Effect::get_onBindObservable}
    , _program{nullptr}
    , _onCompileObserver{nullptr}
    , _engine{engine}
    , _uniformsNames{options.uniformsNames}
    , _samplers{options.samplers}
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

  std::string vertexSource   = "";
  std::string fragmentSource = "";

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

  _loadVertexShader(vertexSource, [this, &fragmentSource, &vertexSource](
                                    const std::string& vertexCode) {
    _processIncludes(vertexCode, [this, &fragmentSource, &vertexSource](
                                   const std::string& vertexCodeWithIncludes) {
      _processShaderConversion(
        vertexCodeWithIncludes, false,
        [this, &fragmentSource,
         &vertexSource](const std::string& migratedVertexCode) {
          _loadFragmentShader(
            fragmentSource, [this, &migratedVertexCode, &fragmentSource,
                             &vertexSource](const std::string& fragmentCode) {
              _processIncludes(
                fragmentCode,
                [this, &migratedVertexCode, &fragmentSource,
                 &vertexSource](const std::string& fragmentCodeWithIncludes) {
                  _processShaderConversion(
                    fragmentCodeWithIncludes, true,
                    [this, &migratedVertexCode, &fragmentSource,
                     &vertexSource](const std::string& migratedFragmentCode) {
                      if (!vertexSource.empty()) {
                        const auto& vertex   = vertexSource;
                        const auto& fragment = fragmentSource;

                        _vertexSourceCode
                          = "#define SHADER_NAME vertex:" + vertex + "\n"
                            + migratedVertexCode;
                        _fragmentSourceCode
                          = "#define SHADER_NAME fragment:" + fragment + "\n"
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

Effect::~Effect()
{
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
#if 0
  if (!_isReady && _program && _program->isParallelCompiled) {
    return _engine->_isProgramCompiled(_program.get());
  }
#endif
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
  return _samplers;
}

std::string Effect::getCompilationError()
{
  return _compilationError;
}

void Effect::executeWhenCompiled(
  const std::function<void(Effect* effect)>& func)
{
  if (isReady()) {
    func(this);
    return;
  }

  onCompileObservable.add([&](Effect* effect, EventState&) { func(effect); });
}

void Effect::_loadVertexShader(
  const std::string& vertex,
  const std::function<void(const std::string&)>& callback)
{
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
    vertexShaderUrl = Engine::ShadersRepository + vertex;
  }

  // Vertex shader
  // Tools::LoadFile(vertexShaderUrl + ".vertex.fx", callback);
}

void Effect::_loadFragmentShader(
  const std::string& fragment,
  const std::function<void(const std::string&)>& callback)
{
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
    fragmentShaderUrl = Engine::ShadersRepository + fragment;
  }

  // Fragment shader
  // Tools::LoadFile(fragmentShaderUrl + ".fragment.fx", callback);
}

void Effect::_dumpShadersSource(std::string vertexCode,
                                std::string fragmentCode,
                                const std::string& iDefines)
{
  // Rebuild shaders source code
  auto shaderVersion
    = (_engine->webGLVersion() > 1.f) ? BABYLONCPP_GLSL_VERSION_3 : "";
  auto prefix  = shaderVersion + (!iDefines.empty() ? iDefines + "\n" : "");
  vertexCode   = prefix + vertexCode;
  fragmentCode = prefix + fragmentCode;

  // Number lines of shaders source code
  unsigned int i = 2;
  const std::regex regex("\n", std::regex::optimize);
  auto formattedVertexCode
    = "\n1\t"
      + String::regexReplace(vertexCode, regex, [&i](const std::smatch& /*m*/) {
          return "\n" + std::to_string(i++) + "\t";
        });
  i = 2;
  auto formattedFragmentCode
    = "\n1\t"
      + String::regexReplace(fragmentCode, regex,
                             [&i](const std::smatch& /*m*/) {
                               return "\n" + std::to_string(i++) + "\t";
                             });

  // Dump shaders name and formatted source code
  BABYLON_LOGF_ERROR("Effect", "Vertex shader: %s%s", name.c_str(),
                     formattedVertexCode.c_str())
  BABYLON_LOGF_ERROR("Effect", "Fragment shader: %s%s", name.c_str(),
                     formattedFragmentCode.c_str())
}

void Effect::_processShaderConversion(
  const std::string& sourceCode, bool isFragment,
  const std::function<void(const std::string&)>& callback)
{

  auto preparedSourceCode = _processPrecision(sourceCode);

  if (_engine->webGLVersion() == 1.f) {
    callback(preparedSourceCode);
    return;
  }

  // Already converted
  if (String::contains(preparedSourceCode, "#version 3")) {
    String::replaceInPlace(preparedSourceCode, BABYLONCPP_GLSL_VERSION_3, "");
    callback(preparedSourceCode);
    return;
  }

  const auto hasDrawBuffersExtension = String::contains(
    preparedSourceCode, "#extension.+GL_EXT_draw_buffers.+require");

  // Remove extensions
  // #extension GL_OES_standard_derivatives : enable
  // #extension GL_EXT_shader_texture_lod : enable
  // #extension GL_EXT_frag_depth : enable
  // #extension GL_EXT_draw_buffers : require
  const std::string regex(
    "#extension.+(GL_OES_standard_derivatives|GL_EXT_shader_texture_lod|GL_EXT_"
    "frag_depth|GL_EXT_draw_buffers).+(enable|require)");
  auto result = String::regexReplace(preparedSourceCode, regex, "");

  // Migrate to GLSL v300
  result = String::regexReplace(result, "varying(?![\n\r])\\s",
                                isFragment ? "in " : "out ");
  result = String::regexReplace(result, "attribute[ \t]", "in ");
  result = String::regexReplace(result, "[ \t]attribute", " in");

  if (isFragment) {
    const std::vector<std::pair<std::string, std::string>> fragmentMappings{
      std::make_pair("texture2DLodEXT\\s*\\(", "textureLod("),   //
      std::make_pair("textureCubeLodEXT\\s*\\(", "textureLod("), //
      std::make_pair("texture2D\\s*\\(", "texture("),            //
      std::make_pair("textureCube\\s*\\(", "texture("),          //
      std::make_pair("gl_FragDepthEXT", "gl_FragDepth"),         //
      std::make_pair("gl_FragColor", "glFragColor"),             //
      std::make_pair("gl_FragData", "glFragData"),
      std::make_pair(
        "void\\s+?main\\s*\\(", //
        String::concat(
          (hasDrawBuffersExtension ? "" : "out vec4 glFragColor;\n"),
          "void main(")),

    };
    for (const auto& fragmentMapping : fragmentMappings) {
      result = String::regexReplace(result, fragmentMapping.first,
                                    fragmentMapping.second);
    }
  }

  callback(result);
}

void Effect::_processIncludes(
  const std::string& sourceCode,
  const std::function<void(const std::string&)>& callback)
{
  const std::regex regex("#include<(.+)>(\\((.*)\\))*(\\[(.*)\\])*",
                         std::regex::optimize);
  auto lines = String::split(sourceCode, '\n');

  std::ostringstream returnValue;

  for (auto& line : lines) {
    auto match = String::regexMatch(line, regex);
    if (match.size() != 6) {
      returnValue << line << std::endl;
      continue;
    }

    auto includeFile = match[1];

    // Uniform declaration
    if (String::contains(includeFile, "__decl__")) {
      String::replaceInPlace(includeFile, "__decl__", "");
      if (_engine->supportsUniformBuffers()) {
        String::replaceInPlace(includeFile, "Vertex", "Ubo");
        String::replaceInPlace(includeFile, "Fragment", "Ubo");
      }
      includeFile += "Declaration";
    }

    if (stl_util::contains(Effect::IncludesShadersStore(), includeFile)) {
      // Substitution
      std::string includeContent = Effect::IncludesShadersStore()[includeFile];
      if (!match[2].empty()) {
        auto splits = String::split(match[3], ',');

        for (std::size_t index = 0; index < splits.size(); index += 2) {
          const std::string source{splits[index]};
          const std::string dest{splits[index + 1]};

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
              maxIndex = std::to_string(_indexParameters[maxIndex]);
            }

            if (String::isDigit(minIndex) && String::isDigit(maxIndex)) {
              const size_t _minIndex = std::stoul(minIndex, nullptr, 0);
              const size_t _maxIndex = std::stoul(maxIndex, nullptr, 0);
              for (size_t i = _minIndex; i < _maxIndex; ++i) {
                const auto istr = std::to_string(i);
                if (!_engine->supportsUniformBuffers()) {
                  // Ubo replacement
                  const auto iCallback = [](const std::smatch& m) {
                    if (m.size() == 2) {
                      return m.str(1) + "{X}";
                    }
                    return m.str(0);
                  };
                  const std::regex iRegex{"light\\{X\\}.(\\w*)",
                                          std::regex::optimize};
                  sourceIncludeContent = String::regexReplace(
                    sourceIncludeContent, iRegex, iCallback);
                }
                includeContent
                  += String::regexReplace(sourceIncludeContent, "\\{X\\}", istr)
                     + "\n";
              }
            }
          }
        }
        else {
          if (!_engine->supportsUniformBuffers()) {
            // Ubo replacement
            const auto iCallback = [](const std::smatch& m) {
              if (m.size() == 2) {
                return m.str(1) + "{X}";
              }
              return m.str(0);
            };
            const std::regex iRegex{"light\\{X\\}.(\\w*)",
                                    std::regex::optimize};
            includeContent
              = String::regexReplace(includeContent, iRegex, iCallback);
          }
          includeContent
            = String::regexReplace(includeContent, "\\{X\\}", indexString);
        }
      }

      // Replace
      String::replaceInPlace(line, match[0], includeContent);
      returnValue << line << std::endl;
    }
    else {
      // Load from file
    }
  }

  callback(returnValue.str());
}

std::string Effect::_processPrecision(std::string source)
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

  // Add GL_ES defines
  // -- precision mediump float
  std::ostringstream mediumpStream;
  mediumpStream << "#ifdef GL_ES\n";
  mediumpStream << "precision mediump float;\n";
  mediumpStream << "precision mediump sampler2DShadow;\n";
  mediumpStream << "#endif\n";
  const std::string mediump{mediumpStream.str()};
  if (String::contains(source, "precision mediump float;")
      && !String::contains(source, mediump)) {
    if (String::contains(source,
                         "#ifdef GL_ES\nprecision mediump float;\n#endif")) {
      String::replaceInPlace(source,
                             "#ifdef GL_ES\nprecision mediump float;\n#endif",
                             "precision mediump float;");
    }
    String::replaceInPlace(source, "precision mediump float;", mediump);
  }

  // -- precision highp float
  std::ostringstream highpStream;
  highpStream << "#ifdef GL_ES\n";
  highpStream << "precision highp float;\n";
  highpStream << "precision highp sampler2DShadow;\n";
  highpStream << "#endif\n";
  const std::string highp{highpStream.str()};
  if (String::contains(source, "precision highp float;")
      && !String::contains(source, highp)) {
    if (String::contains(source,
                         "#ifdef GL_ES\nprecision highp float;\n#endif")) {
      String::replaceInPlace(source,
                             "#ifdef GL_ES\nprecision highp float;\n#endif",
                             "precision highp float;");
    }
    String::replaceInPlace(source, "precision highp float;", highp);
  }

  return source;
}

void Effect::_rebuildProgram(
  const std::string& vertexSourceCode, const std::string& fragmentSourceCode,
  const std::function<void(GL::IGLProgram* program)>& iOnCompiled,
  const std::function<void(const std::string& message)>& iOnError)
{
  _isReady = false;

  _vertexSourceCodeOverride   = vertexSourceCode;
  _fragmentSourceCodeOverride = fragmentSourceCode;
  onError = [&](const Effect* /*effect*/, const std::string& error) {
    if (iOnError) {
      iOnError(error);
    }
  };
  this->onCompiled = [&](const Effect* /*effect*/) {
    for (auto& scene : getEngine()->scenes) {
      scene->markAllMaterialsAsDirty(Material::TextureDirtyFlag);
    }

    if (onCompiled) {
      iOnCompiled(_program.get());
    }
  };
  _fallbacks = nullptr;
  _prepareEffect();
}

std::unordered_map<std::string, std::unique_ptr<GL::IGLUniformLocation>>
Effect::getSpecificUniformLocations(const std::vector<std::string>& names)
{
  return _engine->getUniforms(_program.get(), names);
}

void Effect::_prepareEffect()
{
  auto attributesNames = _attributesNames;
  auto& _defines       = defines;
  auto& fallbacks      = _fallbacks;
  _valueCache.clear();

  auto& previousProgram = _program;

  try {
    auto engine = _engine;

    if (previousProgram) {
      getEngine()->_deleteProgram(previousProgram.get());
    }

    if (!_vertexSourceCodeOverride.empty()
        && !_fragmentSourceCodeOverride.empty()) {
      _program = engine->createRawShaderProgram(
        _vertexSourceCodeOverride, _fragmentSourceCodeOverride, nullptr,
        _transformFeedbackVaryings);
    }
    else {
      _program = engine->createShaderProgram(
        _vertexSourceCode, _fragmentSourceCode, defines, nullptr,
        _transformFeedbackVaryings);
    }
    _program->__SPECTOR_rebuildProgram
      = [this](const std::string& vertexSourceCode,
               const std::string& fragmentSourceCode,
               const std::function<void(GL::IGLProgram * program)>& onCompiled,
               const std::function<void(const std::string& message)>& onError) {
          _rebuildProgram(vertexSourceCode, fragmentSourceCode, onCompiled,
                          onError);
        };

    _program = engine->createShaderProgram(_vertexSourceCode,
                                           _fragmentSourceCode, _defines);

    if (engine->supportsUniformBuffers()) {
      for (auto& item : _uniformBuffersNames) {
        bindUniformBlock(item.first, item.second);
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
  }
  catch (const std::exception& e) {
    _compilationError = e.what();

    // Let's go through fallbacks then
    BABYLON_LOG_ERROR("Effect", "Unable to compile effect: ")
    BABYLON_LOGF_ERROR("Effect", "Defines: %s", defines.c_str())
    // _dumpShadersSource(_vertexSourceCode, _fragmentSourceCode, defines);
    BABYLON_LOGF_ERROR("Effect", "Uniforms: %s",
                       String::join(_uniformsNames, ' ').c_str())
    BABYLON_LOGF_ERROR("Effect", "Attributes: %s",
                       String::join(attributesNames, ' ').c_str())
    BABYLON_LOGF_ERROR("Effect", "Error: %s", _compilationError.c_str())
    if (previousProgram) {
      _program = std::move(previousProgram);
      _isReady = true;
      if (onError) {
        onError(this, _compilationError);
      }
      onErrorObservable.notifyObservers(this);
    }

    if (!fallbacks && fallbacks->isMoreFallbacks()) {
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

void Effect::_bindTexture(const std::string& channel,
                          const InternalTexturePtr& texture)
{
  _engine->_bindTexture(stl_util::index_of(_samplers, channel), texture);
}

void Effect::setTexture(const std::string& channel,
                        const BaseTexturePtr& texture)
{
  _engine->setTexture(stl_util::index_of(_samplers, channel),
                      getUniform(channel), texture);
}

void Effect::setDepthStencilTexture(const std::string& channel,
                                    const RenderTargetTexturePtr& texture)
{
  _engine->setDepthStencilTexture(stl_util::index_of(_samplers, channel),
                                  getUniform(channel), texture);
}

void Effect::setTextureArray(const std::string& channel,
                             const std::vector<BaseTexturePtr>& textures)
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

void Effect::setTextureFromPostProcess(const std::string& channel,
                                       PostProcess* postProcess)
{
  _engine->setTextureFromPostProcess(stl_util::index_of(_samplers, channel),
                                     postProcess);
}

void Effect::setTextureFromPostProcessOutput(const std::string& channel,
                                             PostProcess* postProcess)
{
  _engine->setTextureFromPostProcessOutput(
    stl_util::index_of(_samplers, channel), postProcess);
}

bool Effect::_cacheMatrix(const std::string& uniformName, const Matrix& matrix)
{
  auto flag = matrix.updateFlag;
  if (stl_util::contains(_valueCache, uniformName)
      && !_valueCache[uniformName].empty()
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

bool Effect::_cacheFloat3(const std::string& uniformName, float x, float y,
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

bool Effect::_cacheFloat4(const std::string& uniformName, float x, float y,
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

void Effect::bindUniformBuffer(GL::IGLBuffer* _buffer, const std::string& iName)
{
  if (stl_util::contains(_uniformBuffersNames, iName)) {
    const auto& bufferName = _uniformBuffersNames[iName];
    if (stl_util::contains(Effect::_baseCache, bufferName)
        && Effect::_baseCache[bufferName] == _buffer) {
      return;
    }
  }
  else {
    _uniformBuffersNames[iName] = 0;
  }

  const auto& bufferName         = _uniformBuffersNames[iName];
  Effect::_baseCache[bufferName] = _buffer;
  _engine->bindUniformBufferBase(_buffer, bufferName);
}

void Effect::bindUniformBlock(const std::string& blockName, unsigned index)
{
  _engine->bindUniformBlock(_program.get(), blockName, index);
}

Effect& Effect::setInt(const std::string& uniformName, int value)
{
  Float32Array _value{static_cast<float>(value)};
  if (stl_util::contains(_valueCache, uniformName)
      && (_valueCache[uniformName] == _value)) {
    return *this;
  }

  _valueCache[uniformName] = _value;

  _engine->setInt(getUniform(uniformName), value);

  return *this;
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

Effect& Effect::setMatrices(const std::string& uniformName,
                            Float32Array matrices)
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
      && stl_util::almost_equal(_valueCache[uniformName][0],
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

Effect& Effect::setDirectColor4(const std::string& uniformName,
                                const Color4& color4)
{
  if (_cacheFloat4(uniformName, color4.r, color4.g, color4.b, color4.a)) {
    _engine->setDirectColor4(getUniform(uniformName), color4);
  }

  return *this;
}

void Effect::RegisterShader(const std::string& name,
                            const std::optional<std::string>& pixelShader,
                            const std::optional<std::string>& vertexShader)
{
  if (pixelShader.has_value()) {
    Effect::ShadersStore()[String::concat(name, "PixelShader")]
      = (*pixelShader).c_str();
  }

  if (vertexShader) {
    Effect::ShadersStore()[String::concat(name, "VertexShader")]
      = (*vertexShader).c_str();
  }
}

void Effect::ResetCache()
{
  Effect::_baseCache.clear();
}

} // end of namespace BABYLON
