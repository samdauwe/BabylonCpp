#ifndef BABYLON_MATERIALS_EFFECT_H
#define BABYLON_MATERIALS_EFFECT_H

#include <babylon/babylon_global.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Effect {

  friend class Engine;

public:
  Effect(const string_t& baseName, EffectCreationOptions& options,
         Engine* engine);
  Effect(const std::unordered_map<string_t, string_t>& baseName,
         EffectCreationOptions& options, Engine* engine);
  ~Effect();

  /** Properties **/
  string_t key() const;
  bool isReady() const;
  Engine* getEngine() const;
  GL::IGLProgram* getProgram();
  vector_t<string_t>& getAttributesNames();
  int getAttributeLocation(unsigned int index);
  int getAttributeLocationByName(const string_t& name);
  size_t getAttributesCount();
  int getUniformIndex(const string_t& uniformName);
  GL::IGLUniformLocation* getUniform(const string_t& uniformName);
  vector_t<string_t>& getSamplers();
  string_t getCompilationError();

  /** Methods **/
  void executeWhenCompiled(const ::std::function<void(Effect* effect)>& func);
  void _loadVertexShader(const string_t& vertex,
                         ::std::function<void(const string_t& data)> callback);
  void
  _loadFragmentShader(const string_t& fragment,
                      ::std::function<void(const string_t& data)> callback);
  bool isSupported() const;
  void _bindTexture(const string_t& channel, InternalTexture* texture);
  void setTexture(const string_t& channel, BaseTexture* texture);
  void setTextureArray(const string_t& channel,
                       const vector_t<BaseTexture*>& textures);
  void setTextureFromPostProcess(const string_t& channel,
                                 PostProcess* postProcess);
  bool _cacheMatrix(const string_t& uniformName, const Matrix& matrix);
  bool _cacheFloat2(const string_t& uniformName, float x, float y);
  bool _cacheFloat3(const string_t& uniformName, float x, float y, float z);
  bool _cacheFloat4(const string_t& uniformName, float x, float y, float z,
                    float w);
  void bindUniformBuffer(GL::IGLBuffer* _buffer, const string_t& name);
  void bindUniformBlock(const string_t& blockName, unsigned index);
  Effect& setIntArray(const string_t& uniformName, const Int32Array& array);
  Effect& setIntArray2(const string_t& uniformName, const Int32Array& array);
  Effect& setIntArray3(const string_t& uniformName, const Int32Array& array);
  Effect& setIntArray4(const string_t& uniformName, const Int32Array& array);
  Effect& setFloatArray(const string_t& uniformName, const Float32Array& array);
  Effect& setFloatArray2(const string_t& uniformName,
                         const Float32Array& array);
  Effect& setFloatArray3(const string_t& uniformName,
                         const Float32Array& array);
  Effect& setFloatArray4(const string_t& uniformName,
                         const Float32Array& array);
  Effect& setArray(const string_t& uniformName, Float32Array array);
  Effect& setArray2(const string_t& uniformName, Float32Array array);
  Effect& setArray3(const string_t& uniformName, Float32Array array);
  Effect& setArray4(const string_t& uniformName, Float32Array array);
  Effect& setMatrices(const string_t& uniformName, Float32Array matrices);
  Effect& setMatrix(const string_t& uniformName, const Matrix& matrix);
  Effect& setMatrix3x3(const string_t& uniformName, const Float32Array& matrix);
  Effect& setMatrix2x2(const string_t& uniformName, const Float32Array& matrix);
  Effect& setFloat(const string_t& uniformName, float value);
  Effect& setBool(const string_t& uniformName, bool _bool);
  Effect& setVector2(const string_t& uniformName, const Vector2& vector2);
  Effect& setFloat2(const string_t& uniformName, float x, float y);
  Effect& setVector3(const string_t& uniformName, const Vector3& vector3);
  Effect& setFloat3(const string_t& uniformName, float x, float y, float z);
  Effect& setVector4(const string_t& uniformName, const Vector4& vector4);
  Effect& setFloat4(const string_t& uniformName, float x, float y, float z,
                    float w);
  Effect& setColor3(const string_t& uniformName, const Color3& color3);
  Effect& setColor4(const string_t& uniformName, const Color3& color3,
                    float alpha);

  // Statics
  static void ResetCache();

private:
  void _dumpShadersSource(string_t vertexCode, string_t fragmentCode,
                          string_t defines);
  void _processShaderConversion(
    const string_t& sourceCode, bool isFragment,
    const ::std::function<void(const string_t& data)>& callback);
  void
  _processIncludes(const string_t& sourceCode,
                   const ::std::function<void(const string_t& data)>& callback);
  string_t _processPrecision(string_t source);
  void _prepareEffect();

public:
  string_t name;
  string_t defines;
  ::std::function<void(Effect* effect)> onCompiled;
  ::std::function<void(Effect* effect, const string_t& errors)> onError;
  ::std::function<void(Effect* effect)> onBind;
  std::size_t uniqueId;
  Observable<Effect> onCompileObservable;
  Observable<Effect> onErrorObservable;
  Observable<Effect> onBindObservable;
  string_t _key;

private:
  Observer<Effect>::Ptr _onCompileObserver;
  static std::size_t _uniqueIdSeed;
  Engine* _engine;
  std::unordered_map<string_t, unsigned int> _uniformBuffersNames;
  vector_t<string_t> _uniformsNames;
  vector_t<string_t> _samplers;
  bool _isReady;
  string_t _compilationError;
  vector_t<string_t> _attributesNames;
  Int32Array _attributes;
  std::unordered_map<string_t, std::unique_ptr<GL::IGLUniformLocation>>
    _uniforms;
  std::unordered_map<string_t, unsigned int> _indexParameters;
  std::unique_ptr<EffectFallbacks> _fallbacks;
  string_t _vertexSourceCode;
  string_t _fragmentSourceCode;
  std::unique_ptr<GL::IGLProgram> _program;
  std::unordered_map<string_t, Float32Array> _valueCache;
  static std::unordered_map<unsigned int, GL::IGLBuffer*> _baseCache;

}; // end of class Effect

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_H
