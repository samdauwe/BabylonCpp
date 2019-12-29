#ifndef BABYLON_ENGINES_THIN_ENGINE_H
#define BABYLON_ENGINES_THIN_ENGINE_H

#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/core/structs.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine_capabilities.h>
#include <babylon/engines/engine_options.h>
#include <babylon/meshes/buffer_pointer.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class AlphaState;
class BaseTexture;
class DepthCullingState;
class Effect;
struct EffectCreationOptions;
class ICanvasRenderingContext2D;
struct IInternalTextureLoader;
class InternalTexture;
class IPipelineContext;
struct IShaderProcessor;
struct ISize;
class Scene;
class StencilState;
class Texture;
class UniformBuffer;
class VertexBuffer;
class WebGLDataBuffer;
class WebGLPipelineContext;
using BaseTexturePtr            = std::shared_ptr<BaseTexture>;
using EffectPtr                 = std::shared_ptr<Effect>;
using IInternalTextureLoaderPtr = std::shared_ptr<IInternalTextureLoader>;
using InternalTexturePtr        = std::shared_ptr<InternalTexture>;
using IPipelineContextPtr       = std::shared_ptr<IPipelineContext>;
using IShaderProcessorPtr       = std::shared_ptr<IShaderProcessor>;
using VertexBufferPtr           = std::shared_ptr<VertexBuffer>;
using WebGLBufferPtr            = std::shared_ptr<GL::IGLBuffer>;
using WebGLDataBufferPtr        = std::shared_ptr<WebGLDataBuffer>;
using WebGLFramebufferPtr       = std::shared_ptr<GL::IGLFramebuffer>;
using WebGLPipelineContextPtr   = std::shared_ptr<WebGLPipelineContext>;
using WebGLProgramPtr           = std::shared_ptr<GL::IGLProgram>;
using WebGLRenderbufferPtr      = std::shared_ptr<GL::IGLRenderbuffer>;
using WebGLRenderingContext     = GL::IGLRenderingContext;
using WebGLShaderPtr            = std::shared_ptr<GL::IGLShader>;
using WebGLTexturePtr           = std::shared_ptr<GL::IGLTexture>;
using WebGLVertexArrayObjectPtr = std::shared_ptr<GL::IGLVertexArrayObject>;
using WebGLUniformLocationPtr   = std::shared_ptr<GL::IGLUniformLocation>;

/**
 * @brief The base engine class (root of all engines).
 */
class BABYLON_SHARED_EXPORT ThinEngine {

public:
  ThinEngine();
  virtual ~ThinEngine();

  /** Shaders **/

  /**
   * @brief Hidden
   */
  void _releaseEffect(Effect* effect);

  /**
   * @brief Hidden
   */
  void _deletePipelineContext(const IPipelineContextPtr& pipelineContext);

  /**
   * @brief Create a new effect (used to store vertex/fragment shaders).
   * @param baseName defines the base name of the effect (The name of file without .fragment.fx or
   * .vertex.fx)
   * @param attributesNamesOrOptions defines either a list of attribute names or an
   * IEffectCreationOptions object
   * @param uniformsNamesOrEngine defines either a list of uniform names or the engine to use
   * @param samplers defines an array of string used to represent textures
   * @param defines defines the string containing the defines to use to compile the shaders
   * @param fallbacks defines the list of potential fallbacks to use if shader conmpilation fails
   * @param onCompiled defines a function to call when the effect creation is successful
   * @param onError defines a function to call when the effect creation has failed
   * @param indexParameters defines an object containing the index values to use to compile shaders
   * (like the maximum number of simultaneous lights)
   * @returns the new Effect
   */
  EffectPtr createEffect(
    const std::variant<std::string, std::unordered_map<std::string, std::string>>& baseName,
    EffectCreationOptions& options, ThinEngine* engine,
    const std::function<void(const EffectPtr& effect)>& onCompiled = nullptr);

  /**
   * @brief Directly creates a webGL program.
   * @param pipelineContext  defines the pipeline context to attach to
   * @param vertexCode defines the vertex shader code to use
   * @param fragmentCode defines the fragment shader code to use
   * @param context defines the webGL context to use (if not set, the current one will be used)
   * @param transformFeedbackVaryings defines the list of transform feedback varyings to use
   * @returns the new webGL program
   */
  WebGLProgramPtr
  createRawShaderProgram(const IPipelineContextPtr& pipelineContext, const std::string& vertexCode,
                         const std::string& fragmentCode, WebGLRenderingContext* context = nullptr,
                         const std::vector<std::string>& transformFeedbackVaryings = {});

  /**
   * @brief Creates a webGL program.
   * @param pipelineContext  defines the pipeline context to attach to
   * @param vertexCode  defines the vertex shader code to use
   * @param fragmentCode defines the fragment shader code to use
   * @param defines defines the string containing the defines to use to compile the shaders
   * @param context defines the webGL context to use (if not set, the current one will be used)
   * @param transformFeedbackVaryings defines the list of transform feedback varyings to use
   * @returns the new webGL program
   */
  virtual WebGLProgramPtr
  createShaderProgram(const IPipelineContextPtr& pipelineContext, const std::string& vertexCode,
                      const std::string& fragmentCode, const std::string& defines,
                      WebGLRenderingContext* context                            = nullptr,
                      const std::vector<std::string>& transformFeedbackVaryings = {});

  /**
   * @brief Creates a new pipeline context
   * @returns the new pipeline
   */
  IPipelineContextPtr createPipelineContext();

  /**
   * @brief Hidden
   */
  void _preparePipelineContext(const IPipelineContextPtr& pipelineContext,
                               const std::string& vertexSourceCode,
                               const std::string& fragmentSourceCode, bool createAsRaw,
                               bool rebuildRebind, const std::string& defines,
                               const std::vector<std::string>& transformFeedbackVaryings);

  /**
   * @brief Hidden
   */
  bool _isRenderingStateCompiled(const IPipelineContextPtr& pipelineContext);

  /**
   * @brief Hidden
   */
  void _executeWhenRenderingStateIsCompiled(const IPipelineContextPtr& pipelineContext,
                                            const std::function<void()>& action);

  /**
   * @brief Gets the list of webGL uniform locations associated with a specific program based on a
   * list of uniform names.
   * @param pipelineContext defines the pipeline context to use
   * @param uniformsNames defines the list of uniform names
   * @returns an array of webGL uniform locations
   */
  virtual std::unordered_map<std::string, WebGLUniformLocationPtr>
  getUniforms(const IPipelineContextPtr& pipelineContext,
              const std::vector<std::string>& uniformsNames);

  /**
   * @brief Gets the lsit of active attributes for a given webGL program.
   * @param pipelineContext defines the pipeline context to use
   * @param attributesNames defines the list of attribute names to get
   * @returns an array of indices indicating the offset of each attribute
   */
  virtual Int32Array getAttributes(const IPipelineContextPtr& pipelineContext,
                                   const std::vector<std::string>& attributesNames);

  /**
   * @brief Activates an effect, mkaing it the current one (ie. the one used for rendering).
   * @param effect defines the effect to activate
   */
  virtual void enableEffect(const EffectPtr& effect);

  /**
   * @brief Set the value of an uniform to a number (int).
   * @param uniform defines the webGL uniform location where to store the value
   * @param value defines the int number to store
   */
  virtual void setInt(GL::IGLUniformLocation* uniform, int value);

  /**
   * @brief Set the value of an uniform to an array of int32.
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   */
  virtual void setIntArray(GL::IGLUniformLocation* uniform, const Int32Array& array);

  /**
   * @brief Set the value of an uniform to an array of int32 (stored as vec2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   */
  virtual void setIntArray2(GL::IGLUniformLocation* uniform, const Int32Array& array);

  /**
   * @brief Set the value of an uniform to an array of int32 (stored as vec3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   */
  virtual void setIntArray3(GL::IGLUniformLocation* uniform, const Int32Array& array);

  /**
   * @brief Set the value of an uniform to an array of int32 (stored as vec4).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   */
  virtual void setIntArray4(GL::IGLUniformLocation* uniform, const Int32Array& array);

  /**
   * @brief Set the value of an uniform to an array of number.
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   */
  virtual void setArray(GL::IGLUniformLocation* uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of number (stored as vec2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   */
  virtual void setArray2(GL::IGLUniformLocation* uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of number (stored as vec3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   */
  virtual void setArray3(GL::IGLUniformLocation* uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of number (stored as vec4).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   */
  virtual void setArray4(GL::IGLUniformLocation* uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of float32 (stored as
   * matrices).
   * @param uniform defines the webGL uniform location where to store the value
   * @param matrices defines the array of float32 to store
   */
  virtual void setMatrices(GL::IGLUniformLocation* uniform, const Float32Array& matrices);

  /**
   * @brief Set the value of an uniform to a matrix (3x3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param matrix defines the Float32Array representing the 3x3 matrix to store
   */
  virtual void setMatrix3x3(GL::IGLUniformLocation* uniform, const Float32Array& matrix);

  /**
   * @brief Set the value of an uniform to a matrix (2x2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param matrix defines the Float32Array representing the 2x2 matrix to store
   */
  virtual void setMatrix2x2(GL::IGLUniformLocation* uniform, const Float32Array& matrix);

  /**
   * @brief Set the value of an uniform to a number (float).
   * @param uniform defines the webGL uniform location where to store the value
   * @param value defines the float number to store
   */
  virtual void setFloat(GL::IGLUniformLocation* uniform, float value);

  /**
   * @brief Set the value of an uniform to a vec2.
   * @param uniform defines the webGL uniform location where to store the value
   * @param x defines the 1st component of the value
   * @param y defines the 2nd component of the value
   */
  virtual void setFloat2(GL::IGLUniformLocation* uniform, float x, float y);

  /**
   * @brief Set the value of an uniform to a vec3.
   * @param uniform defines the webGL uniform location where to store the value
   * @param x defines the 1st component of the value
   * @param y defines the 2nd component of the value
   * @param z defines the 3rd component of the value
   */
  virtual void setFloat3(GL::IGLUniformLocation* uniform, float x, float y, float z);

  /**
   * @brief Set the value of an uniform to a vec4.
   * @param uniform defines the webGL uniform location where to store the value
   * @param x defines the 1st component of the value
   * @param y defines the 2nd component of the value
   * @param z defines the 3rd component of the value
   * @param w defines the 4th component of the value
   */
  virtual void setFloat4(GL::IGLUniformLocation* uniform, float x, float y, float z, float w);

  // States

  /**
   * @brief Apply all cached states (depth, culling, stencil and alpha).
   */
  void applyStates();

  /**
   * @brief Enable or disable color writing.
   * @param enable defines the state to set
   */
  void setColorWrite(bool enable);

  /**
   * @brief Gets a boolean indicating if color writing is enabled.
   * @returns the current color writing state
   */
  bool getColorWrite() const;

  /** Textures **/

  /**
   * @brief Clears the list of texture accessible through engine.
   * This can help preventing texture load conflict due to name collision.
   */
  void clearInternalTexturesCache();

  /**
   * @brief Force the entire cache to be cleared.
   * You should not have to use this function unless your engine needs to share the webGL context
   * with another engine
   * @param bruteForce defines a boolean to force clearing ALL caches (including stencil, detoh and
   * alpha states)
   */
  virtual void wipeCaches(bool bruteForce = false);

  /**
   * @brief Hidden
   */
  SamplingParameters _getSamplingParameters(unsigned int samplingMode, bool generateMipMaps);

  /**
   * @brief Hidden
   */
  virtual WebGLTexturePtr _createTexture();

  /**
   * @brief Usually called from Texture.ts.
   * Passed information to create a WebGLTexture
   * @param urlArg defines a value which contains one of the following:
   * * A conventional http URL, e.g. 'http://...' or 'file://...'
   * * A base64 string of in-line texture data, e.g. 'data:image/jpg;base64,/...'
   * * An indicator that data being passed using the buffer parameter, e.g. 'data:mytexture.jpg'
   * @param noMipmap defines a boolean indicating that no mipmaps shall be generated.  Ignored for
   * compressed textures.  They must be in the file
   * @param invertY when true, image is flipped when loaded.  You probably want true. Certain
   * compressed textures may invert this if their default is inverted (eg. ktx)
   * @param scene needed for loading to the correct scene
   * @param samplingMode mode with should be used sample / access the texture (Default:
   * Texture.TRILINEAR_SAMPLINGMODE)
   * @param onLoad optional callback to be called upon successful completion
   * @param onError optional callback to be called upon failure
   * @param buffer a source of a file previously fetched as either a base64 string, an ArrayBuffer
   * (compressed or image format), HTMLImageElement (image format), or a Blob
   * @param fallback an internal argument in case the function must be called again, due to etc1 not
   * having alpha capabilities
   * @param format internal format.  Default: RGB when extension is '.jpg' else RGBA.  Ignored for
   * compressed textures
   * @param forcedExtension defines the extension to use to pick the right loader
   * @param excludeLoaders array of texture loaders that should be excluded when picking a loader
   * for the texture (default: empty array)
   * @param mimeType defines an optional mime type
   * @returns a InternalTexture for assignment back into BABYLON.Texture
   */
  virtual InternalTexturePtr createTexture(
    const std::string& urlArg, bool noMipmap, bool invertY, Scene* scene,
    unsigned int samplingMode = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE,
    const std::function<void(InternalTexture*, EventState&)>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr,
    const std::optional<std::variant<std::string, ArrayBuffer, ArrayBufferView, Image>>& buffer
    = std::nullopt,
    const InternalTexturePtr& fallBack                           = nullptr,
    const std::optional<unsigned int>& format                    = std::nullopt,
    const std::string& forcedExtension                           = "",
    const std::vector<IInternalTextureLoaderPtr>& excludeLoaders = {},
    const std::string& mimeType                                  = "");

  /**
   * @brief Rescales a texture.
   * @param source input texutre
   * @param destination destination texture
   * @param scene scene to use to render the resize
   * @param internalFormat format to use when resizing
   * @param onComplete callback to be called when resize has completed
   */
  void _rescaleTexture(const InternalTexturePtr& source, const InternalTexturePtr& destination,
                       Scene* scene, unsigned int internalFormat,
                       const std::function<void()>& onComplete);

  /**
   * @brief Creates a raw texture.
   * @param data defines the data to store in the texture
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @param format defines the format of the data
   * @param generateMipMaps defines if the engine should generate the mip levels
   * @param invertY defines if data must be stored with Y axis inverted
   * @param samplingMode defines the required sampling mode (Texture.NEAREST_SAMPLINGMODE by
   * default)
   * @param compression defines the compression used (null by default)
   * @param type defines the type fo the data (Engine.TEXTURETYPE_UNSIGNED_INT by default)
   * @returns the raw texture inside an InternalTexture
   */
  InternalTexturePtr createRawTexture(const Uint8Array& data, int width, int height,
                                      unsigned int format, bool generateMipMaps, bool invertY,
                                      unsigned int samplingMode,
                                      const std::string& compression = "",
                                      unsigned int type = Constants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief Creates a new raw cube texture.
   * @param data defines the array of data to use to create each face
   * @param size defines the size of the textures
   * @param format defines the format of the data
   * @param type defines the type of the data (like Engine.TEXTURETYPE_UNSIGNED_INT)
   * @param generateMipMaps  defines if the engine should generate the mip levels
   * @param invertY defines if data must be stored with Y axis inverted
   * @param samplingMode defines the required sampling mode (like Texture.NEAREST_SAMPLINGMODE)
   * @param compression defines the compression used (null by default)
   * @returns the cube texture as an InternalTexture
   */
  InternalTexturePtr createRawCubeTexture(const std::vector<ArrayBufferView>& data, int size,
                                          unsigned int format, unsigned int type,
                                          bool generateMipMaps, bool invertY,
                                          unsigned int samplingMode,
                                          const std::string& compression = "");

  /**
   * @brief Creates a new raw 3D texture.
   * @param data defines the data used to create the texture
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @param depth defines the depth of the texture
   * @param format defines the format of the texture
   * @param generateMipMaps defines if the engine must generate mip levels
   * @param invertY defines if data must be stored with Y axis inverted
   * @param samplingMode defines the required sampling mode (like Texture.NEAREST_SAMPLINGMODE)
   * @param compression defines the compressed used (can be null)
   * @param textureType defines the compressed used (can be null)
   * @returns a new raw 3D texture (stored in an InternalTexture)
   */
  InternalTexturePtr
  createRawTexture3D(const ArrayBufferView& data, int width, int height, int depth,
                     unsigned int format, bool generateMipMaps, bool invertY,
                     unsigned int samplingMode, const std::string& compression = "",
                     unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief Creates a new raw 2D array texture.
   * @param data defines the data used to create the texture
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @param depth defines the number of layers of the texture
   * @param format defines the format of the texture
   * @param generateMipMaps defines if the engine must generate mip levels
   * @param invertY defines if data must be stored with Y axis inverted
   * @param samplingMode defines the required sampling mode (like Texture.NEAREST_SAMPLINGMODE)
   * @param compression defines the compressed used (can be null)
   * @param textureType defines the compressed used (can be null)
   * @returns a new raw 2D array texture (stored in an InternalTexture)
   */
  InternalTexturePtr
  createRawTexture2DArray(const ArrayBufferView& data, int width, int height, int depth,
                          unsigned int format, bool generateMipMaps, bool invertY,
                          unsigned int samplingMode, const std::string& compression = "",
                          unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief hidden
   */
  virtual void _unpackFlipY(bool value);

  /**
   * @brief hidden
   */
  virtual int _getUnpackAlignement();

  /**
   * @brief Update the sampling mode of a given texture.
   * @param samplingMode defines the required sampling mode
   * @param texture defines the texture to update
   * @param generateMipMaps defines whether to generate mipmaps for the texture
   */
  virtual void updateTextureSamplingMode(unsigned int samplingMode,
                                         const InternalTexturePtr& texture,
                                         bool generateMipMaps = false);

  /**
   * @brief Update the sampling mode of a given texture.
   * @param texture defines the texture to update
   * @param wrapU defines the texture wrap mode of the u coordinates
   * @param wrapV defines the texture wrap mode of the v coordinates
   * @param wrapR defines the texture wrap mode of the r coordinates
   */
  void updateTextureWrappingMode(InternalTexture* texture, std::optional<int> wrapU,
                                 std::optional<int> wrapV = std::nullopt,
                                 std::optional<int> wrapR = std::nullopt);

  /**
   * @brief Hidden
   */
  void _setupDepthStencilTexture(InternalTexture* internalTexture,
                                 const std::variant<int, ISize>& size, bool generateStencil,
                                 bool bilinearFiltering, int comparisonFunction);

  /**
   * @brief Hidden
   */
  virtual void _uploadCompressedDataToTextureDirectly(const InternalTexturePtr& texture,
                                                      unsigned int internalFormat, int width,
                                                      int height, const Uint8Array& data,
                                                      unsigned int faceIndex = 0, int lod = 0);

  /**
   * @brief Hidden
   */
  virtual void _uploadDataToTextureDirectly(const InternalTexturePtr& texture,
                                            const ArrayBufferView& imageData,
                                            unsigned int faceIndex = 0, int lod = 0,
                                            int babylonInternalFormat     = -1,
                                            bool useTextureWidthAndHeight = false);

  /**
   * @brief Hidden
   */
  virtual void _uploadArrayBufferViewToTexture(const InternalTexturePtr& texture,
                                               const Uint8Array& imageData,
                                               unsigned int faceIndex = 0, int lod = 0);

  /**
   * @brief Hidden
   */
  WebGLRenderbufferPtr _setupFramebufferDepthAttachments(bool generateStencilBuffer,
                                                         bool generateDepthBuffer, int width,
                                                         int height, int samples = 1);

  /**
   * @brief Hidden
   */
  void _releaseFramebufferObjects(InternalTexture* texture);

  /**
   * @brief Hidden
   */
  virtual void _releaseTexture(InternalTexture* texture);

  /**
   * @brief Binds an effect to the webGL context.
   * @param effect defines the effect to bind
   */
  virtual void bindSamplers(Effect& effect);

  /**
   * @brief Hidden
   */
  virtual bool _bindTextureDirectly(unsigned int target, const InternalTexturePtr& texture,
                                    bool forTextureDataUpdate = false, bool force = false);

  /**
   * @brief Hidden
   */
  virtual void _bindTexture(int channel, const InternalTexturePtr& texture);

  /**
   * @brief Unbind all textures from the webGL context.
   */
  void unbindAllTextures();

  /**
   * @brief Sets a texture to the according uniform.
   * @param channel The texture channel
   * @param uniform The uniform to set
   * @param texture The texture to apply
   */
  void setTexture(int channel, const WebGLUniformLocationPtr& uniform,
                  const BaseTexturePtr& texture);

  /**
   * @brief Sets an array of texture to the webGL context
   * @param channel defines the channel where the texture array must be set
   * @param uniform defines the associated uniform location
   * @param textures defines the array of textures to bind
   */
  void setTextureArray(int channel, const WebGLUniformLocationPtr& uniform,
                       const std::vector<BaseTexturePtr>& textures);

  /**
   * @brief Hidden
   */
  void _setAnisotropicLevel(unsigned int target, const BaseTexturePtr& texture);

  /**
   * @brief Unbind all vertex attributes from the webGL context.
   */
  void unbindAllAttributes();

  /**
   * @brief Force the engine to release all cached effects. This means that next effect compilation
   * will have to be done completely even if a similar effect was already compiled
   */
  virtual void releaseEffects();

  /**
   * @brief Dispose and release all associated resources.
   */
  void dispose();

  /**
   * @brief Get the current error code of the webGL context.
   * @returns the error code
   * @see https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getError
   */
  virtual unsigned int getError() const;

  /**
   * @brief Hidden
   */
  unsigned int _getWebGLTextureType(unsigned int type) const;

  /**
   * @brief Hidden
   */
  unsigned int _getInternalFormat(unsigned int format) const;

  /**
   * @brief Hidden
   */
  unsigned int _getRGBABufferInternalSizedFormat(unsigned int type,
                                                 const std::optional<unsigned int>& format
                                                 = std::nullopt) const;

  /**
   * @brief Hidden
   */
  unsigned int _getRGBAMultiSampleBufferFormat(unsigned int type) const;

  /**
   * @brief Reads pixels from the current frame buffer. Please note that this function can be slow
   * @param x defines the x coordinate of the rectangle where pixels must be read
   * @param y defines the y coordinate of the rectangle where pixels must be read
   * @param width defines the width of the rectangle where pixels must be read
   * @param height defines the height of the rectangle where pixels must be read
   * @param hasAlpha defines wether the output should have alpha or not (defaults to true)
   * @returns a Uint8Array containing RGBA colors
   */
  Uint8Array readPixels(int x, int y, int width, int height, bool hasAlpha = true);

  // Statics

  /**
   * @brief Gets a boolean indicating if the engine can be instanciated (ie. if a webGL context can
   * be found)
   * @returns true if the engine can be created
   */
  static bool isSupported();

  /**
   * @brief Find the next highest power of two.
   * @param x Number to start search from.
   * @return Next highest power of two.
   */
  static int CeilingPOT(int x);

  /**
   * @brief Find the next lowest power of two.
   * @param x Number to start search from.
   * @return Next lowest power of two.
   */
  static int FloorPOT(int x);

  /**
   * @brief Find the nearest power of two.
   * @param x Number to start search from.
   * @return Next nearest power of two.
   */
  static int NearestPOT(int x);

  /**
   * @brief Get the closest exponent of two.
   * @param value defines the value to approximate
   * @param max defines the maximum value to return
   * @param mode defines how to define the closest value
   * @returns closest exponent of two of the given value
   */
  static int GetExponentOfTwo(int value, int max, unsigned int mode = Constants::SCALEMODE_NEAREST);

protected:
  /**
   * Gets the depth culling state manager
   */
  std::unique_ptr<DepthCullingState>& get_depthCullingState();

  /**
   * Gets the alpha state manager
   */
  std::unique_ptr<AlphaState>& get_alphaState();

  /**
   * Gets the stencil state manager
   */
  std::unique_ptr<StencilState>& get_stencilState();

  static std::string _ConcatenateShader(const std::string& source, const std::string& defines,
                                        const std::string& shaderVersion);
  WebGLProgramPtr
  _createShaderProgram(const WebGLPipelineContextPtr& pipelineContext,
                       const WebGLShaderPtr& vertexShader, const WebGLShaderPtr& fragmentShader,
                       WebGLRenderingContext* context,
                       const std::vector<std::string>& transformFeedbackVaryings = {});
  void _finalizePipelineContext(const WebGLPipelineContextPtr& pipelineContext);
  void _prepareWebGLTextureContinuation(const InternalTexturePtr& texture, Scene* scene,
                                        bool noMipmap, bool isCompressed,
                                        unsigned int samplingMode);
  void _deleteTexture(const WebGLTexturePtr& texture);
  void _setProgram(const WebGLProgramPtr& program);
  bool _setTexture(int channel, const BaseTexturePtr& texture, bool isPartOfTextureArray = false,
                   bool depthStencilTexture = false);

private:
  WebGLShaderPtr _compileShader(const std::string& source, const std::string& type,
                                const std::string& defines, const std::string& shaderVersion);
  WebGLShaderPtr _compileRawShader(const std::string& source, const std::string& type);
  unsigned int _getTextureTarget(InternalTexture* texture) const;
  void _prepareWebGLTexture(
    const InternalTexturePtr& texture, Scene* scene, int width, int height,
    std::optional<bool> invertY, bool noMipmap, bool isCompressed,
    const std::function<bool(int width, int height,
                             const std::function<void()>& continuationCallback)>& processFunction,
    unsigned int samplingMode = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE);
  WebGLRenderbufferPtr _getDepthStencilBuffer(int width, int height, int samples,
                                              unsigned int internalFormat,
                                              unsigned int msInternalFormat,
                                              unsigned int attachment);
  void _activateCurrentTexture();
  void _bindSamplerUniformToChannel(int sourceSlot, int destination);
  unsigned int _getTextureWrapMode(unsigned int mode) const;
  void _setTextureParameterFloat(unsigned int target, unsigned int parameter, float value,
                                 const InternalTexturePtr& texture);
  void _setTextureParameterInteger(unsigned int target, unsigned int parameter, int value,
                                   const InternalTexturePtr& texture = nullptr);
  bool _canRenderToFloatFramebuffer();
  bool _canRenderToHalfFloatFramebuffer();
  // Thank you :
  // http://stackoverflow.com/questions/28827511/webgl-ios-render-to-floating-point-texture
  bool _canRenderToFramebuffer(unsigned int type);

public:
  /** @hidden */
  IShaderProcessorPtr _shaderProcessor = nullptr;

  /**
   * Gets or sets a boolean that indicates if textures must be forced to power of 2 size even if not
   * required
   */
  bool forcePOTTextures = false;

  /**
   * Gets a boolean indicating if the engine is currently rendering in fullscreen mode
   */
  bool isFullscreen = false;

  /**
   * Gets or sets a boolean indicating if back faces must be culled (true by default)
   */
  bool cullBackFaces = true;

  /**
   * Gets or sets a boolean indicating if the engine must keep rendering even if the window is not
   * in foregroun
   */
  bool renderEvenInBackground = true;

  /**
   * Gets or sets a boolean indicating that cache can be kept between frames
   */
  bool preventCacheWipeBetweenFrames = false;

  /** Gets or sets a boolean indicating if the engine should validate programs after compilation */
  bool validateShaderPrograms = false;

  /**
   * Gets or sets a boolean indicating if depth buffer should be reverse, going from far to near.
   * This can provide greater z depth for distant objects.
   */
  bool useReverseDepthBuffer = false;
  // Uniform buffers list

  /**
   * Gets or sets a boolean indicating that uniform buffers must be disabled even if they are
   * supported
   */
  bool disableUniformBuffers = false;

  /** @hidden */
  std::vector<UniformBuffer*> _uniformBuffers;

  /** @hidden */
  WebGLRenderingContext* _gl;

  /** @hidden */
  bool _badOS = false;

  /** @hidden */
  bool _badDesktopOS = false;

  /** @hidden */
  EngineCapabilities _caps;

  /** @hidden */
  bool _videoTextureSupported = false;

  // Lost context
  /**
   * Observable signaled when a context lost event is raised
   */
  Observable<ThinEngine> onContextLostObservable;
  /**
   * Observable signaled when a context restored event is raised
   */
  Observable<ThinEngine> onContextRestoredObservable;

  /** @hidden */
  bool _doNotHandleContextLost = false;

  /**
   * Gets or sets a boolean indicating that vertex array object must be disabled even if they are
   * supported
   */
  bool disableVertexArrayObjects = false;

  /** @hidden */
  std::unique_ptr<AlphaState> _alphaState;
  /** @hidden */
  unsigned int _alphaMode = Constants::ALPHA_ADD;
  /** @hidden */
  unsigned int _alphaEquation = Constants::ALPHA_DISABLE;

  // Cache
  /** @hidden */
  std::vector<InternalTexturePtr> _internalTexturesCache;

  /** @hidden */
  InternalTexturePtr _currentRenderTarget = nullptr;
  /** @hidden */
  ICanvas* _workingCanvas = nullptr;
  /** @hidden */
  ICanvasRenderingContext2D* _workingContext = nullptr;

  /**
   * Gets the depth culling state manager
   */
  ReadOnlyProperty<ThinEngine, std::unique_ptr<DepthCullingState>> depthCullingState;

  /**
   * Gets the alpha state manager
   */
  ReadOnlyProperty<ThinEngine, std::unique_ptr<AlphaState>> alphaState;

  /**
   * Gets the stencil state manager
   */
  ReadOnlyProperty<ThinEngine, std::unique_ptr<StencilState>> stencilState;

  /**
   * In case you are sharing the context with other applications, it might
   * be interested to not cache the unpack flip y state to ensure a consistent
   * value would be set.
   */
  bool enableUnpackFlipYCached = true;

protected:
  /**
   * Gets or sets the textures that the engine should not attempt to load as compressed
   */
  std::vector<std::string> _excludedCompressedTextures;

  ICanvas* _renderingCanvas = nullptr;
  bool _windowIsBackground  = false;
  float _webGLVersion       = 1.f;
  EngineOptions _creationOptions;

  bool _highPrecisionShadersAllowed = true;
  bool _renderingQueueLaunched      = false;
  std::vector<std::function<void()>> _activeRenderLoops;
  bool _contextWasLost = false;

  // States
  /** @hidden */
  bool _colorWrite = true;
  /** @hidden */
  bool _colorWriteChanged = true;
  /** @hidden */
  std::unique_ptr<DepthCullingState> _depthCullingState = nullptr;
  /** @hidden */
  std::unique_ptr<StencilState> _stencilState = nullptr;
  /** @hidden */
  int _activeChannel = 0;
  /** @hidden */
  std::unordered_map<int, InternalTexturePtr> _boundTexturesCache;
  /** @hidden */
  EffectPtr _currentEffect = nullptr;
  /** @hidden */
  WebGLProgramPtr _currentProgram = nullptr;
  /** @hidden */
  std::optional<Viewport> _cachedViewport = std::nullopt;
  /** @hidden */
  std::unordered_map<std::string, VertexBufferPtr> _cachedVertexBuffers;
  /** @hidden */
  WebGLDataBufferPtr _cachedIndexBuffer = nullptr;
  /** @hidden */
  EffectPtr _cachedEffectForVertexBuffers = nullptr;
  /** @hidden */
  std::vector<WebGLBufferPtr> _currentBoundBuffer;
  /** @hidden */
  WebGLFramebufferPtr _currentFramebuffer = nullptr;
  /** @hidden */
  int _frameHandler = -1;
  // Hardware supported Compressed Textures
  std::vector<std::string> _texturesSupported;

  /**
   * Defines whether the engine has been created with the premultipliedAlpha option on or not.
   */
  const bool premultipliedAlpha = true;

  /**
   * Observable event triggered before each texture is initialized
   */
  Observable<Texture> onBeforeTextureInitObservable;

  /** @hidden */
  std::unordered_map<int, WebGLUniformLocationPtr> _boundUniforms;

private:
  float _hardwareScalingLevel = 1.f;
  bool _isStencilEnable       = false;

  std::string _glVersion;
  std::string _glRendere;
  std::string _glVendor;

  std::function<void(Event&& evt)> _onContextLost     = nullptr;
  std::function<void(Event&& evt)> _onContextRestored = nullptr;

  int _currentTextureChannel = -1;

  std::unordered_map<std::string, EffectPtr> _compiledEffects;
  std::vector<bool> _vertexAttribArraysEnabled;
  WebGLVertexArrayObjectPtr _cachedVertexArrayObject = nullptr;
  bool _uintIndicesCurrentlySet                      = false;

  std::unordered_map<unsigned int, BufferPointer> _currentBufferPointers;
  Int32Array _currentInstanceLocations;
  std::vector<WebGLDataBufferPtr> _currentInstanceBuffers;
  Int32Array _textureUnits;

  bool _vaoRecordInProgress      = false;
  bool _mustWipeVertexAttributes = false;

  InternalTexturePtr _emptyTexture        = nullptr;
  InternalTexturePtr _emptyCubeTexture    = nullptr;
  InternalTexturePtr _emptyTexture3D      = nullptr;
  InternalTexturePtr _emptyTexture2DArray = nullptr;

  Int32Array _nextFreeTextureSlots;
  unsigned int _maxSimultaneousTextures = 0;

  std::optional<bool> _unpackFlipYCached = std::nullopt;

}; // end of class ThinEngine

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_THIN_ENGINE_H
