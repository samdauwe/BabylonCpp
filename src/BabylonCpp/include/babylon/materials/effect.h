#ifndef BABYLON_MATERIALS_EFFECT_H
#define BABYLON_MATERIALS_EFFECT_H

#include <babylon/babylon_global.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Effect {

public:
  using ChildShaderNode = std::pair<std::size_t, std::string>;
  struct ShaderNode {
    std::string condition;
    std::string define;
    bool ndef;
    std::vector<ChildShaderNode> children;
    std::size_t parent;
  }; // end of struct ShaderNode

public:
  Effect(const std::string& baseName, EffectCreationOptions& options,
         Engine* engine);
  Effect(const std::unordered_map<std::string, std::string>& baseName,
         EffectCreationOptions& options, Engine* engine);
  ~Effect();

  /** Properties **/
  std::string key() const;
  bool isReady() const;
  GL::IGLProgram* getProgram();
  std::vector<std::string>& getAttributesNames();
  int getAttributeLocation(unsigned int index);
  int getAttributeLocationByName(const std::string& name);
  size_t getAttributesCount();
  int getUniformIndex(const std::string& uniformName);
  GL::IGLUniformLocation* getUniform(const std::string& uniformName);
  std::vector<std::string>& getSamplers();
  std::string getCompilationError();
  std::string getVertexShaderSource();
  std::string getFragmentShaderSource();

  /** Methods **/
  void executeWhenCompiled(const std::function<void(Effect* effect)>& func);
  void _loadVertexShader(const std::string& vertex,
                         std::function<void(const std::string& data)> callback);
  void
  _loadFragmentShader(const std::string& fragment,
                      std::function<void(const std::string& data)> callback);
  bool isSupported() const;
  void _bindTexture(const std::string& channel, GL::IGLTexture* texture);
  void setTexture(const std::string& channel, BaseTexture* texture);
  void setTextureArray(const std::string& channel,
                       const std::vector<BaseTexture*>& textures);
  void setTextureFromPostProcess(const std::string& channel,
                                 PostProcess* postProcess);
  bool _cacheMatrix(const std::string& uniformName, const Matrix& matrix);
  bool _cacheFloat2(const std::string& uniformName, float x, float y);
  bool _cacheFloat3(const std::string& uniformName, float x, float y, float z);
  bool _cacheFloat4(const std::string& uniformName, float x, float y, float z,
                    float w);
  void bindUniformBuffer(GL::IGLBuffer* _buffer, const std::string& name);
  void bindUniformBlock(const std::string& blockName, unsigned index);
  Effect& setIntArray(const std::string& uniformName, const Int32Array& array);
  Effect& setIntArray2(const std::string& uniformName, const Int32Array& array);
  Effect& setIntArray3(const std::string& uniformName, const Int32Array& array);
  Effect& setIntArray4(const std::string& uniformName, const Int32Array& array);
  Effect& setFloatArray(const std::string& uniformName,
                        const Float32Array& array);
  Effect& setFloatArray2(const std::string& uniformName,
                         const Float32Array& array);
  Effect& setFloatArray3(const std::string& uniformName,
                         const Float32Array& array);
  Effect& setFloatArray4(const std::string& uniformName,
                         const Float32Array& array);
  Effect& setArray(const std::string& uniformName, std::vector<float> array);
  Effect& setArray2(const std::string& uniformName, std::vector<float> array);
  Effect& setArray3(const std::string& uniformName, std::vector<float> array);
  Effect& setArray4(const std::string& uniformName, std::vector<float> array);
  Effect& setMatrices(const std::string& uniformName, Float32Array matrices);
  Effect& setMatrix(const std::string& uniformName, const Matrix& matrix);
  Effect& setMatrix3x3(const std::string& uniformName,
                       const Float32Array& matrix);
  Effect& setMatrix2x2(const std::string& uniformName,
                       const Float32Array& matrix);
  Effect& setFloat(const std::string& uniformName, float value);
  Effect& setBool(const std::string& uniformName, bool _bool);
  Effect& setVector2(const std::string& uniformName, const Vector2& vector2);
  Effect& setFloat2(const std::string& uniformName, float x, float y);
  Effect& setVector3(const std::string& uniformName, const Vector3& vector3);
  Effect& setFloat3(const std::string& uniformName, float x, float y, float z);
  Effect& setVector4(const std::string& uniformName, const Vector4& vector4);
  Effect& setFloat4(const std::string& uniformName, float x, float y, float z,
                    float w);
  Effect& setColor3(const std::string& uniformName, const Color3& color3);
  Effect& setColor4(const std::string& uniformName, const Color3& color3,
                    float alpha);

private:
  void _dumpShadersSource(std::string vertexCode, std::string fragmentCode,
                          std::string defines);
  void _processShaderConversion(
    const std::string& sourceCode, bool isFragment,
    const std::function<void(const std::string& data)>& callback);
  void _processIncludes(
    const std::string& sourceCode,
    const std::function<void(const std::string& data)>& callback);
  std::string _processPrecision(std::string source);
  void _prepareEffect(const std::string& vertexSourceCode,
                      const std::string& fragmentSourceCode,
                      const std::vector<std::string>& attributesNames,
                      const std::string& defines, EffectFallbacks* fallbacks);
  std::string _recombineShader(const std::vector<ShaderNode>& shaderNodes,
                               std::size_t rootNodeId = 0);
  std::string _evaluateDefinesOnString(const std::string& shaderString);

public:
  std::string name;
  std::string defines;
  std::function<void(Effect* effect)> onCompiled;
  std::function<void(Effect* effect, const std::string& errors)> onError;
  std::function<void(Effect* effect)> onBind;
  std::size_t uniqueId;
  Observable<Effect> onCompileObservable;
  Observable<Effect> onErrorObservable;
  Observable<Effect> onBindObservable;
  std::string _key;

private:
  Observer<Effect>::Ptr _onCompileObserver;
  static std::size_t _uniqueIdSeed;
  Engine* _engine;
  std::unordered_map<std::string, unsigned int> _uniformBuffersNames;
  std::vector<std::string> _uniformsNames;
  std::vector<std::string> _samplers;
  bool _isReady;
  std::string _compilationError;
  std::vector<std::string> _attributesNames;
  Int32Array _attributes;
  std::unordered_map<std::string, std::unique_ptr<GL::IGLUniformLocation>>
    _uniforms;
  std::unordered_map<std::string, unsigned int> _indexParameters;
  std::unique_ptr<EffectFallbacks> _fallbacks;
  std::unique_ptr<GL::IGLProgram> _program;
  std::unordered_map<std::string, Float32Array> _valueCache;
  static std::unordered_map<unsigned int, GL::IGLBuffer*> _baseCache;

}; // end of class Effect

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_H
