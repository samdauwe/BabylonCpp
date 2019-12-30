#ifndef BABYLON_MATERIALS_EFFECT_H
#define BABYLON_MATERIALS_EFFECT_H

#include <unordered_map>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class BaseTexture;
class Color3;
class Color4;
class Effect;
struct IEffectCreationOptions;
class EffectFallbacks;
class Engine;
class InternalTexture;
class IPipelineContext;
class Matrix;
class PostProcess;
class RenderTargetTexture;
class Vector2;
class Vector3;
class Vector4;
class WebGLDataBuffer;
using BaseTexturePtr         = std::shared_ptr<BaseTexture>;
using EffectPtr              = std::shared_ptr<Effect>;
using InternalTexturePtr     = std::shared_ptr<InternalTexture>;
using IPipelineContextPtr    = std::shared_ptr<IPipelineContext>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;
using WebGLDataBufferPtr     = std::shared_ptr<WebGLDataBuffer>;

namespace GL {
class IGLProgram;
class IGLUniformLocation;
} // end of namespace GL

/**
 * @brief Effect containing vertex and fragment shader that can be executed on an object.
 */
class BABYLON_SHARED_EXPORT Effect : public IDisposable {

  friend class Engine;

public:
  /**
   * Gets or sets the relative url used to load shaders if using the engine in non-minified mode
   */
  static std::string ShadersRepository;

  /**
   * Store of each shader (The can be looked up using effect.key)
   */
  static std::unordered_map<std::string, std::string>& ShadersStore();

  /**
   * Store of each included file for a shader (The can be looked up using effect.key)
   */
  static std::unordered_map<std::string, std::string>& IncludesShadersStore();

public:
  template <typename... Ts>
  static EffectPtr New(Ts&&... args)
  {
    return std::shared_ptr<Effect>(new Effect(std::forward<Ts>(args)...));
  }
  ~Effect() override; // = default

  /** Properties **/

  /**
   * Unique key for this effect
   */
  [[nodiscard]] std::string key() const;

  /**
   * @brief If the effect has been compiled and prepared.
   * @returns if the effect is compiled and prepared.
   */
  [[nodiscard]] bool isReady() const;

  /**
   * @brief The engine the effect was initialized with.
   * @returns the engine.
   */
  [[nodiscard]] Engine* getEngine() const;

  /**
   * @brief The pipeline context for this effect.
   * @returns the associated pipeline context
   */
  IPipelineContextPtr& getPipelineContext();

  /**
   * @brief The set of names of attribute variables for the shader.
   * @returns An array of attribute names.
   */
  std::vector<std::string>& getAttributesNames();

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
  int getAttributeLocationByName(const std::string& name);

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
  int getUniformIndex(const std::string& uniformName);

  /**
   * @brief Returns the attribute based on the name of the variable.
   * @param uniformName of the uniform to look up.
   * @returns the location of the uniform.
   */
  GL::IGLUniformLocation* getUniform(const std::string& uniformName);

  /**
   * @brief Returns an array of sampler variable names
   * @returns The array of sampler variable neames.
   */
  std::vector<std::string>& getSamplers();

  /**
   * @brief The error from the last compilation.
   * @returns the error string.
   */
  std::string getCompilationError();

  /** Methods **/

  /**
   * @brief Adds a callback to the onCompiled observable and call the callback immediatly if already
   * ready.
   * @param func The callback to be used.
   */
  void executeWhenCompiled(const std::function<void(Effect* effect)>& func);

  /**
   * @brief Hidden
   */
  void _loadVertexShader(const std::string& vertex,
                         const std::function<void(const std::string&)>& callback);

  /**
   * @brief Hidden
   */
  void _loadFragmentShader(const std::string& fragment,
                           const std::function<void(const std::string&)>& callback);

  /**
   * @brief Hidden
   */
  void _dumpShadersSource(std::string vertexCode, std::string fragmentCode,
                          const std::string& defines);

  /**
   * @brief Recompiles the webGL program
   * @param vertexSourceCode The source code for the vertex shader.
   * @param fragmentSourceCode The source code for the fragment shader.
   * @param onCompiled Callback called when completed.
   * @param onError Callback called on error.
   * Hidden
   */
  void
  _rebuildProgram(const std::string& vertexSourceCode, const std::string& fragmentSourceCode,
                  const std::function<void(const IPipelineContextPtr& pipelineContext)>& onCompiled,
                  const std::function<void(const std::string& message)>& onError);

  /**
   * @brief Prepares the effect
   * Hidden
   */
  void _prepareEffect();

  /**
   * @brief Checks if the effect is supported. (Must be called after
   * compilation)
   */
  [[nodiscard]] bool isSupported() const;

  /**
   * @brief Binds a texture to the engine to be used as output of the shader.
   * @param channel Name of the output variable.
   * @param texture Texture to bind.
   * Hidden
   */
  void _bindTexture(const std::string& channel, const InternalTexturePtr& texture);

  /**
   * @brief Sets a texture on the engine to be used in the shader.
   * @param channel Name of the sampler variable.
   * @param texture Texture to set.
   */
  void setTexture(const std::string& channel, const BaseTexturePtr& texture);

  /**
   * @brief Sets a depth stencil texture from a render target on the engine to be used in the
   * shader.
   * @param channel Name of the sampler variable.
   * @param texture Texture to set.
   */
  void setDepthStencilTexture(const std::string& channel, const RenderTargetTexturePtr& texture);

  /**
   * @brief Sets an array of textures on the engine to be used in the shader.
   * @param channel Name of the variable.
   * @param textures Textures to set.
   */
  void setTextureArray(const std::string& channel, const std::vector<BaseTexturePtr>& textures);

  /**
   * @brief Sets a texture to be the input of the specified post process. (To use the output, pass
   * in the next post process in the pipeline).
   * @param channel Name of the sampler variable.
   * @param postProcess Post process to get the input texture from.
   */
  void setTextureFromPostProcess(const std::string& channel, PostProcess* postProcess);

  /**
   * @brief (Warning! setTextureFromPostProcessOutput may be desired instead) Sets the input texture
   * of the passed in post process to be input of this effect. (To use the output of the passed in
   * post process use setTextureFromPostProcessOutput)
   * @param channel Name of the sampler variable.
   * @param postProcess Post process to get the output texture from.
   */
  void setTextureFromPostProcessOutput(const std::string& channel, PostProcess* postProcess);

  bool _cacheMatrix(const std::string& uniformName, const Matrix& matrix);
  bool _cacheFloat2(const std::string& uniformName, float x, float y);
  bool _cacheFloat3(const std::string& uniformName, float x, float y, float z);
  bool _cacheFloat4(const std::string& uniformName, float x, float y, float z, float w);

  /**
   * @brief Binds a buffer to a uniform.
   * @param buffer Buffer to bind.
   * @param name Name of the uniform variable to bind to.
   */
  void bindUniformBuffer(const WebGLDataBufferPtr& buffer, const std::string& name);

  /**
   * @brief Binds block to a uniform.
   * @param blockName Name of the block to bind.
   * @param index Index to bind.
   */
  void bindUniformBlock(const std::string& blockName, unsigned index);

  /**
   * @brief Sets an interger value on a uniform variable.
   * @param uniformName Name of the variable.
   * @param value Value to be set.
   * @returns this effect.
   */
  Effect& setInt(const std::string& uniformName, int value);

  /**
   * @brief Sets an int array on a uniform variable.
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setIntArray(const std::string& uniformName, const Int32Array& array);

  /**
   * @brief Sets an int array 2 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4] will result in [[1,2],[3,4]] in the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setIntArray2(const std::string& uniformName, const Int32Array& array);

  /**
   * @brief Sets an int array 3 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6] will result in [[1,2,3],[4,5,6]] in the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setIntArray3(const std::string& uniformName, const Int32Array& array);

  /**
   * @brief Sets an int array 4 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6,7,8] will result in [[1,2,3,4],[5,6,7,8]] in the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setIntArray4(const std::string& uniformName, const Int32Array& array);

  /**
   * @brief Sets an float array on a uniform variable.
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setFloatArray(const std::string& uniformName, const Float32Array& array);

  /**
   * @brief Sets an float array 2 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4] will result in [[1,2],[3,4]] in the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setFloatArray2(const std::string& uniformName, const Float32Array& array);

  /**
   * @brief Sets an float array 3 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6] will result in [[1,2,3],[4,5,6]] in the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setFloatArray3(const std::string& uniformName, const Float32Array& array);

  /**
   * @brief Sets an float array 4 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6,7,8] will result in [[1,2,3,4],[5,6,7,8]] in the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setFloatArray4(const std::string& uniformName, const Float32Array& array);

  /**
   * @brief Sets an array on a uniform variable.
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setArray(const std::string& uniformName, Float32Array array);

  /**
   * @brief Sets an array 2 on a uniform variable. (Array is specified as single array eg. [1,2,3,4]
   * will result in [[1,2],[3,4]] in the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setArray2(const std::string& uniformName, Float32Array array);

  /**
   * @brief Sets an array 3 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6] will result in [[1,2,3],[4,5,6]] in the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setArray3(const std::string& uniformName, Float32Array array);

  /**
   * @brief Sets an array 4 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6,7,8] will result in [[1,2,3,4],[5,6,7,8]] in the shader)
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  Effect& setArray4(const std::string& uniformName, Float32Array array);

  /**
   * @brief Sets matrices on a uniform variable.
   * @param uniformName Name of the variable.
   * @param matrices matrices to be set.
   * @returns this effect.
   */
  Effect& setMatrices(const std::string& uniformName, Float32Array matrices);

  /**
   * @brief Sets matrix on a uniform variable.
   * @param uniformName Name of the variable.
   * @param matrix matrix to be set.
   * @returns this effect.
   */
  Effect& setMatrix(const std::string& uniformName, const Matrix& matrix);

  /**
   * @brief Sets a 3x3 matrix on a uniform variable. (Speicified as [1,2,3,4,5,6,7,8,9] will result
   * in [1,2,3][4,5,6][7,8,9] matrix)
   * @param uniformName Name of the variable.
   * @param matrix matrix to be set.
   * @returns this effect.
   */
  Effect& setMatrix3x3(const std::string& uniformName, const Float32Array& matrix);

  /**
   * @brief Sets a 2x2 matrix on a uniform variable. (Speicified as [1,2,3,4] will result in
   * [1,2][3,4] matrix)
   * @param uniformName Name of the variable.
   * @param matrix matrix to be set.
   * @returns this effect.
   */
  Effect& setMatrix2x2(const std::string& uniformName, const Float32Array& matrix);

  /**
   * @brief Sets a float on a uniform variable.
   * @param uniformName Name of the variable.
   * @param value value to be set.
   * @returns this effect.
   */
  Effect& setFloat(const std::string& uniformName, float value);

  /**
   * @brief Sets a boolean on a uniform variable.
   * @param uniformName Name of the variable.
   * @param bool value to be set.
   * @returns this effect.
   */
  Effect& setBool(const std::string& uniformName, bool _bool);

  /**
   * @brief Sets a Vector2 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param vector2 vector2 to be set.
   * @returns this effect.
   */
  Effect& setVector2(const std::string& uniformName, const Vector2& vector2);

  /**
   * @brief Sets a float2 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First float in float2.
   * @param y Second float in float2.
   * @returns this effect.
   */
  Effect& setFloat2(const std::string& uniformName, float x, float y);

  /**
   * @brief Sets a Vector3 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param vector3 Value to be set.
   * @returns this effect.
   */
  Effect& setVector3(const std::string& uniformName, const Vector3& vector3);

  /**
   * @brief Sets a float3 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First float in float3.
   * @param y Second float in float3.
   * @param z Third float in float3.
   * @returns this effect.
   */
  Effect& setFloat3(const std::string& uniformName, float x, float y, float z);

  /**
   * @brief Sets a Vector4 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param vector4 Value to be set.
   * @returns this effect.
   */
  Effect& setVector4(const std::string& uniformName, const Vector4& vector4);

  /**
   * @brief Sets a float4 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First float in float4.
   * @param y Second float in float4.
   * @param z Third float in float4.
   * @param w Fourth float in float4.
   * @returns this effect.
   */
  Effect& setFloat4(const std::string& uniformName, float x, float y, float z, float w);

  /**
   * @brief Sets a Color3 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param color3 Value to be set.
   * @returns this effect.
   */
  Effect& setColor3(const std::string& uniformName, const Color3& color3);

  /**
   * @brief Sets a Color4 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param color3 Value to be set.
   * @param alpha Alpha value to be set.
   * @returns this effect.
   */
  Effect& setColor4(const std::string& uniformName, const Color3& color3, float alpha);

  /**
   * @brief Sets a Color4 on a uniform variable.
   * @param uniformName defines the name of the variable
   * @param color4 defines the value to be set
   * @returns this effect.
   */
  Effect& setDirectColor4(const std::string& uniformName, const Color4& color4);

  /**
   * @brief Release all associated resources.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  // Statics

  /**
   * @brief This function will add a new shader to the shader store.
   * @param name the name of the shader
   * @param pixelShader optional pixel shader content
   * @param vertexShader optional vertex shader content
   */
  static void RegisterShader(const std::string& name,
                             const std::optional<std::string>& pixelShader  = std::nullopt,
                             const std::optional<std::string>& vertexShader = std::nullopt);

  /**
   * @brief Resets the cache of effects.
   */
  static void ResetCache();

protected:
  /**
   * @brief Instantiates an effect.
   * An effect can be used to create/manage/execute vertex and fragment shaders.
   * @param baseName Name of the effect.
   * @param attributesNamesOrOptions List of attribute names that will be passed to the shader or
   * set of all options to create the effect.
   * @param uniformsNamesOrEngine List of uniform variable names that will be passed to the shader
   * or the engine that will be used to render effect.
   * @param samplers List of sampler variables that will be passed to the shader.
   * @param engine Engine to be used to render the effect
   * @param defines Define statements to be added to the shader.
   * @param fallbacks Possible fallbacks for this effect to improve performance when needed.
   * @param onCompiled Callback that will be called when the shader is compiled.
   * @param onError Callback that will be called if an error occurs during shader compilation.
   * @param indexParameters Parameters to be used with Babylons include syntax to iterate over an
   * array (eg. {lights: 10})
   */
  Effect(const std::variant<std::string, std::unordered_map<std::string, std::string>>& baseName,
         IEffectCreationOptions& options, Engine* engine);

  /**
   * @brief Observable that will be called when effect is bound.
   */
  Observable<Effect>& get_onBindObservable();

private:
  void _useFinalCode(
    const std::string& migratedVertexCode, const std::string& migratedFragmentCode,
    const std::variant<std::string, std::unordered_map<std::string, std::string>>& baseName);
  bool _isReadyInternal() const;
  void _checkIsReady();
  void _processCompilationErrors(const std::exception& e,
                                 const IPipelineContextPtr& previousPipelineContext);
  int _getChannel(const std::string& channel);

public:
  /**
   * Name of the effect.
   */
  std::variant<std::string, std::unordered_map<std::string, std::string>> name;
  /**
   * String container all the define statements that should be set on the
   * shader.
   */
  std::string defines;
  /**
   * Callback that will be called when the shader is compiled.
   */
  std::function<void(Effect* effect)> onCompiled;
  /**
   * Callback that will be called if an error occurs during shader compilation.
   */
  std::function<void(Effect* effect, const std::string& errors)> onError;
  /**
   * Callback that will be called when effect is bound.
   */
  std::function<void(Effect* effect)> onBind;
  /**
   * Unique ID of the effect.
   */
  std::size_t uniqueId;
  /**
   * Observable that will be called when the shader is compiled.
   * It is recommended to use executeWhenCompile() or to make sure that scene.isReady() is called to
   * get this observable raised.
   */
  Observable<Effect> onCompileObservable;
  /**
   * Observable that will be called if an error occurs during shader
   * compilation.
   */
  Observable<Effect> onErrorObservable;
  /**
   * Hidden
   */
  Observable<Effect> _onBindObservable;
  /** Hidden */
  bool _bonesComputationForcedToCPU;
  /**
   * Observable that will be called when effect is bound.
   */
  ReadOnlyProperty<Effect, Observable<Effect>> onBindObservable;
  /**
   * Key for the effect.
   * Hidden
   */
  std::string _key;
  /**
   * Compiled shader to webGL program.
   * Hidden
   */
  IPipelineContextPtr _pipelineContext;

private:
  Observer<Effect>::Ptr _onCompileObserver;
  static std::size_t _uniqueIdSeed;
  Engine* _engine;
  std::unordered_map<std::string, unsigned int> _uniformBuffersNames;
  std::vector<std::string> _uniformsNames;
  std::vector<std::string> _samplerList;
  std::unordered_map<std::string, int> _samplers;
  bool _isReady;
  std::string _compilationError;
  std::vector<std::string> _attributesNames;
  Int32Array _attributes;
  std::unordered_map<std::string, std::unique_ptr<GL::IGLUniformLocation>> _uniforms;
  std::unordered_map<std::string, unsigned int> _indexParameters;
  std::unique_ptr<EffectFallbacks> _fallbacks;
  std::string _vertexSourceCode;
  std::string _fragmentSourceCode;
  std::string _vertexSourceCodeOverride;
  std::string _fragmentSourceCodeOverride;
  std::vector<std::string> _transformFeedbackVaryings;
  std::unordered_map<std::string, Float32Array> _valueCache;
  static std::unordered_map<unsigned int, WebGLDataBufferPtr> _baseCache;

}; // end of class Effect

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_H
