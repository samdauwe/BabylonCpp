#ifndef BABYLON_MATERIALS_EFFECT_H
#define BABYLON_MATERIALS_EFFECT_H

#include <babylon/babylon_global.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief Effect containing vertex and fragment shader that can be executed on
 * an object.
 */
class BABYLON_SHARED_EXPORT Effect {

  friend class Engine;

public:
  /**
   * @brief Instantiates an effect.
   * An effect can be used to create/manage/execute vertex and fragment shaders.
   * @param baseName Name of the effect.
   * @param attributesNamesOrOptions List of attribute names that will be passed
   * to the shader or set of all options to create the effect.
   * @param uniformsNamesOrEngine List of uniform variable names that will be
   * passed to the shader or the engine that will be used to render effect.
   * @param samplers List of sampler variables that will be passed to the
   * shader.
   * @param engine Engine to be used to render the effect
   * @param defines Define statements to be added to the shader.
   * @param fallbacks Possible fallbacks for this effect to improve performance
   * when needed.
   * @param onCompiled Callback that will be called when the shader is compiled.
   * @param onError Callback that will be called if an error occurs during
   * shader compilation.
   * @param indexParameters Parameters to be used with Babylons include syntax
   * to iterate over an array (eg. {lights: 10})
   */
  Effect(const string_t& baseName, EffectCreationOptions& options,
         Engine* engine);
  Effect(const unordered_map_t<string_t, string_t>& baseName,
         EffectCreationOptions& options, Engine* engine);
  ~Effect();

  /** Properties **/

  /**
   * Unique key for this effect
   */
  string_t key() const;

  /**
   * @brief If the effect has been compiled and prepared.
   * @returns if the effect is compiled and prepared.
   */
  bool isReady() const;

  /**
   * @brief The engine the effect was initialized with.
   * @returns the engine.
   */
  Engine* getEngine() const;

  /**
   * @brief The compiled webGL program for the effect
   * @returns the webGL program.
   */
  GL::IGLProgram* getProgram();

  /**
   * @brief The set of names of attribute variables for the shader.
   * @returns An array of attribute names.
   */
  vector_t<string_t>& getAttributesNames();

  /**
   * @brief Returns the attribute at the given index.
   * @param index The index of the attribute.
   * @returns The location of the attribute.
   */
  int getAttributeLocation(unsigned int index);

  /**
   * @brief Returns the attribute based on the name of the variable.
   * @param name of the attribute to look up.
   * @returns the attribute location.
   */
  int getAttributeLocationByName(const string_t& name);

  /**
   * @brief The number of attributes.
   * @returns the numnber of attributes.
   */
  size_t getAttributesCount();

  /**
   * @brief Gets the index of a uniform variable.
   * @param uniformName of the uniform to look up.
   * @returns the index.
   */
  int getUniformIndex(const string_t& uniformName);

  /**
   * @brief Returns the attribute based on the name of the variable.
   * @param uniformName of the uniform to look up.
   * @returns the location of the uniform.
   */
  GL::IGLUniformLocation* getUniform(const string_t& uniformName);

  /**
   * @brief Returns an array of sampler variable names
   * @returns The array of sampler variable neames.
   */
  vector_t<string_t>& getSamplers();

  /**
   * @brief The error from the last compilation.
   * @returns the error string.
   */
  string_t getCompilationError();

  /** Methods **/

  /**
   * @brief Adds a callback to the onCompiled observable and call the callback
   * immediatly if already ready.
   * @param func The callback to be used.
   */
  void executeWhenCompiled(const ::std::function<void(Effect* effect)>& func);

  string_t _loadVertexShaderAsync(const string_t& vertex);
  string_t _loadFragmentShaderAsync(const string_t& fragment);

  /**
   * @brief Recompiles the webGL program
   * @param vertexSourceCode The source code for the vertex shader.
   * @param fragmentSourceCode The source code for the fragment shader.
   * @param onCompiled Callback called when completed.
   * @param onError Callback called on error.
   */
  void _rebuildProgram(
    const string_t& vertexSourceCode, const string_t& fragmentSourceCode,
    const ::std::function<void(GL::IGLProgram* program)>& onCompiled,
    const ::std::function<void(const string_t& message)>& onError);

  /**
   * @brief Gets the uniform locations of the the specified variable names
   * @param names THe names of the variables to lookup.
   * @returns Array of locations in the same order as variable names.
   */
  unordered_map_t<string_t, unique_ptr_t<GL::IGLUniformLocation>>
  getSpecificUniformLocations(const vector_t<string_t>& names);

  /**
   * @brief Prepares the effect
   */
  void _prepareEffect();

  /**
   * @brief Checks if the effect is supported. (Must be called after
   * compilation)
   */
  bool isSupported() const;

  /**
   * @brief Binds a texture to the engine to be used as output of the shader.
   * @param channel Name of the output variable.
   * @param texture Texture to bind.
   */
  void _bindTexture(const string_t& channel, InternalTexture* texture);

  /**
   * @brief Sets a texture on the engine to be used in the shader.
   * @param channel Name of the sampler variable.
   * @param texture Texture to set.
   */
  void setTexture(const string_t& channel, BaseTexture* texture);

  /**
   * @brief Sets a depth stencil texture from a render target on the engine to
   * be used in the shader.
   * @param channel Name of the sampler variable.
   * @param texture Texture to set.
   */
  void setDepthStencilTexture(const string_t& channel,
                              RenderTargetTexture* texture);

  /**
   * @brief Sets an array of textures on the engine to be used in the shader.
   * @param channel Name of the variable.
   * @param textures Textures to set.
   */
  void setTextureArray(const string_t& channel,
                       const vector_t<BaseTexture*>& textures);

  /**
   * @brief Sets a texture to be the input of the specified post process. (To
   * use the output, pass in the next post process in the pipeline).
   * @param channel Name of the sampler variable.
   * @param postProcess Post process to get the input texture from.
   */
  void setTextureFromPostProcess(const string_t& channel,
                                 PostProcess* postProcess);

  /**
   * @brief (Warning! setTextureFromPostProcessOutput may be desired instead)
   * Sets the input texture of the passed in post process to be input of this
   * effect. (To use the output of the passed in post process use
   * setTextureFromPostProcessOutput)
   * @param channel Name of the sampler variable.
   * @param postProcess Post process to get the output texture from.
   */
  void setTextureFromPostProcessOutput(const string_t& channel,
                                       PostProcess* postProcess);

  bool _cacheMatrix(const string_t& uniformName, const Matrix& matrix);
  bool _cacheFloat2(const string_t& uniformName, float x, float y);
  bool _cacheFloat3(const string_t& uniformName, float x, float y, float z);
  bool _cacheFloat4(const string_t& uniformName, float x, float y, float z,
                    float w);

  /**
   * @brief Binds a buffer to a uniform.
   * @param buffer Buffer to bind.
   * @param name Name of the uniform variable to bind to.
   */
  void bindUniformBuffer(GL::IGLBuffer* _buffer, const string_t& name);

  /**
   * @brief Binds block to a uniform.
   * @param blockName Name of the block to bind.
   * @param index Index to bind.
   */
  void bindUniformBlock(const string_t& blockName, unsigned index);

  /**
   * @brief Sets an interger value on a uniform variable.
   * @param uniformName Name of the variable.
   * @param value Value to be set.
   * @returns this effect.
   */
  Effect& setInt(const string_t& uniformName, int value);

  /**
   * @brief Sets an int array on a uniform variable.
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setIntArray(const string_t& uniformName, const Int32Array& array);

  /**
   * @brief Sets an int array 2 on a uniform variable. (Array is specified as
   * single array eg. [1,2,3,4] will result in [[1,2],[3,4]] in the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setIntArray2(const string_t& uniformName, const Int32Array& array);

  /**
   * @brief Sets an int array 3 on a uniform variable. (Array is specified as
   * single array eg. [1,2,3,4,5,6] will result in [[1,2,3],[4,5,6]] in the
   * shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setIntArray3(const string_t& uniformName, const Int32Array& array);

  /**
   * @brief Sets an int array 4 on a uniform variable. (Array is specified as
   * single array eg. [1,2,3,4,5,6,7,8] will result in [[1,2,3,4],[5,6,7,8]] in
   * the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setIntArray4(const string_t& uniformName, const Int32Array& array);

  /**
   * @brief Sets an float array on a uniform variable.
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setFloatArray(const string_t& uniformName, const Float32Array& array);

  /**
   * @brief Sets an float array 2 on a uniform variable. (Array is specified as
   * single array eg. [1,2,3,4] will result in [[1,2],[3,4]] in the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setFloatArray2(const string_t& uniformName,
                         const Float32Array& array);

  /**
   * @brief Sets an float array 3 on a uniform variable. (Array is specified as
   * single array eg. [1,2,3,4,5,6] will result in [[1,2,3],[4,5,6]] in the
   * shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setFloatArray3(const string_t& uniformName,
                         const Float32Array& array);

  /**
   * @brief Sets an float array 4 on a uniform variable. (Array is specified as
   * single array eg. [1,2,3,4,5,6,7,8] will result in [[1,2,3,4],[5,6,7,8]] in
   * the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setFloatArray4(const string_t& uniformName,
                         const Float32Array& array);

  /**
   * @brief Sets an array on a uniform variable.
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setArray(const string_t& uniformName, Float32Array array);

  /**
   * @brief Sets an array 2 on a uniform variable. (Array is specified as single
   * array eg. [1,2,3,4] will result in [[1,2],[3,4]] in the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setArray2(const string_t& uniformName, Float32Array array);

  /**
   * @brief Sets an array 3 on a uniform variable. (Array is specified as single
   * array eg. [1,2,3,4,5,6] will result in [[1,2,3],[4,5,6]] in the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setArray3(const string_t& uniformName, Float32Array array);

  /**
   * @brief Sets an array 4 on a uniform variable. (Array is specified as single
   * array eg. [1,2,3,4,5,6,7,8] will result in [[1,2,3,4],[5,6,7,8]] in the
   * shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setArray4(const string_t& uniformName, Float32Array array);

  /**
   * @brief Sets matrices on a uniform variable.
   * @param uniformName Name of the variable.
   * @param matrices matrices to be set.
   * @returns this effect.
   */
  Effect& setMatrices(const string_t& uniformName, Float32Array matrices);

  /**
   * @brief Sets matrix on a uniform variable.
   * @param uniformName Name of the variable.
   * @param matrix matrix to be set.
   * @returns this effect.
   */
  Effect& setMatrix(const string_t& uniformName, const Matrix& matrix);

  /**
   * @brief Sets a 3x3 matrix on a uniform variable. (Speicified as
   * [1,2,3,4,5,6,7,8,9] will result in [1,2,3][4,5,6][7,8,9] matrix)
   * @param uniformName Name of the variable.
   * @param matrix matrix to be set.
   * @returns this effect.
   */
  Effect& setMatrix3x3(const string_t& uniformName, const Float32Array& matrix);

  /**
   * @brief Sets a 2x2 matrix on a uniform variable. (Speicified as [1,2,3,4]
   * will result in [1,2][3,4] matrix)
   * @param uniformName Name of the variable.
   * @param matrix matrix to be set.
   * @returns this effect.
   */
  Effect& setMatrix2x2(const string_t& uniformName, const Float32Array& matrix);

  /**
   * @brief Sets a float on a uniform variable.
   * @param uniformName Name of the variable.
   * @param value value to be set.
   * @returns this effect.
   */
  Effect& setFloat(const string_t& uniformName, float value);

  /**
   * @brief Sets a boolean on a uniform variable.
   * @param uniformName Name of the variable.
   * @param bool value to be set.
   * @returns this effect.
   */
  Effect& setBool(const string_t& uniformName, bool _bool);

  /**
   * @brief Sets a Vector2 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param vector2 vector2 to be set.
   * @returns this effect.
   */
  Effect& setVector2(const string_t& uniformName, const Vector2& vector2);

  /**
   * @brief Sets a float2 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First float in float2.
   * @param y Second float in float2.
   * @returns this effect.
   */
  Effect& setFloat2(const string_t& uniformName, float x, float y);

  /**
   * @brief Sets a Vector3 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param vector3 Value to be set.
   * @returns this effect.
   */
  Effect& setVector3(const string_t& uniformName, const Vector3& vector3);

  /**
   * @brief Sets a float3 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First float in float3.
   * @param y Second float in float3.
   * @param z Third float in float3.
   * @returns this effect.
   */
  Effect& setFloat3(const string_t& uniformName, float x, float y, float z);

  /**
   * @brief Sets a Vector4 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param vector4 Value to be set.
   * @returns this effect.
   */
  Effect& setVector4(const string_t& uniformName, const Vector4& vector4);

  /**
   * @brief Sets a float4 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First float in float4.
   * @param y Second float in float4.
   * @param z Third float in float4.
   * @param w Fourth float in float4.
   * @returns this effect.
   */
  Effect& setFloat4(const string_t& uniformName, float x, float y, float z,
                    float w);

  /**
   * @brief Sets a Color3 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param color3 Value to be set.
   * @returns this effect.
   */
  Effect& setColor3(const string_t& uniformName, const Color3& color3);

  /**
   * @brief Sets a Color4 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param color3 Value to be set.
   * @param alpha Alpha value to be set.
   * @returns this effect.
   */
  Effect& setColor4(const string_t& uniformName, const Color3& color3,
                    float alpha);

  /**
   * @brief Sets a Color4 on a uniform variable.
   * @param uniformName defines the name of the variable
   * @param color4 defines the value to be set
   * @returns this effect.
   */
  Effect& setDirectColor4(const string_t& uniformName, const Color4& color4);

  // Statics

  /**
   * @brief Resets the cache of effects.
   */
  static void ResetCache();

private:
  void _dumpShadersSource(string_t vertexCode, string_t fragmentCode,
                          string_t defines);
  string_t _processShaderConversion(const string_t& sourceCode,
                                    bool isFragment);
  string_t _processIncludesAsync(const string_t& sourceCode);
  string_t _processPrecision(string_t source);

public:
  /**
   * Name of the effect.
   */
  string_t name;
  /**
   * String container all the define statements that should be set on the
   * shader.
   */
  string_t defines;
  /**
   * Callback that will be called when the shader is compiled.
   */
  ::std::function<void(Effect* effect)> onCompiled;
  /**
   * Callback that will be called if an error occurs during shader compilation.
   */
  ::std::function<void(Effect* effect, const string_t& errors)> onError;
  /**
   * Callback that will be called when effect is bound.
   */
  ::std::function<void(Effect* effect)> onBind;
  /**
   * Unique ID of the effect.
   */
  std::size_t uniqueId;
  /**
   * Observable that will be called when the shader is compiled.
   */
  Observable<Effect> onCompileObservable;
  /**
   * Observable that will be called if an error occurs during shader
   * compilation.
   */
  Observable<Effect> onErrorObservable;
  /**
   * Observable that will be called when effect is bound.
   */
  Observable<Effect> onBindObservable;
  /**
   * Key for the effect.
   */
  string_t _key;
  /**
   * Compiled shader to webGL program.
   */
  unique_ptr_t<GL::IGLProgram> _program;

private:
  Observer<Effect>::Ptr _onCompileObserver;
  static std::size_t _uniqueIdSeed;
  Engine* _engine;
  unordered_map_t<string_t, unsigned int> _uniformBuffersNames;
  vector_t<string_t> _uniformsNames;
  vector_t<string_t> _samplers;
  bool _isReady;
  string_t _compilationError;
  vector_t<string_t> _attributesNames;
  Int32Array _attributes;
  unordered_map_t<string_t, unique_ptr_t<GL::IGLUniformLocation>> _uniforms;
  unordered_map_t<string_t, unsigned int> _indexParameters;
  unique_ptr_t<EffectFallbacks> _fallbacks;
  string_t _vertexSourceCode;
  string_t _fragmentSourceCode;
  string_t _vertexSourceCodeOverride;
  string_t _fragmentSourceCodeOverride;
  vector_t<string_t> _transformFeedbackVaryings;
  unordered_map_t<string_t, Float32Array> _valueCache;
  static unordered_map_t<unsigned int, GL::IGLBuffer*> _baseCache;

}; // end of class Effect

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_H
