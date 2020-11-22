#ifndef BABYLON_ENGINES_THIN_ENGINE_H
#define BABYLON_ENGINES_THIN_ENGINE_H

#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/core/delegates/delegate.h>
#include <babylon/core/structs.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine_capabilities.h>
#include <babylon/engines/engine_options.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/maths/vector4.h>
#include <babylon/maths/viewport.h>
#include <babylon/meshes/buffer_pointer.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class AlphaExtension;
class AlphaState;
class BaseTexture;
class CubeTextureExtension;
struct CubeTextureData;
class DepthCullingState;
struct DepthTextureCreationOptions;
class DynamicBufferExtension;
class DynamicTextureExtension;
class Color4;
class Effect;
class ICanvasRenderingContext2D;
struct IEffectCreationOptions;
struct IFileRequest;
struct IInternalTextureLoader;
struct IMultiRenderTargetOptions;
struct InstancingAttributeInfo;
class InternalTexture;
class IPipelineContext;
struct RenderTargetCreationOptions;
struct IRenderTargetOptions;
struct IShaderProcessor;
struct ISize;
struct LoaderOptions;
class MultiRenderExtension;
class ProgressEvent;
class RawTextureExtension;
class ReadTextureExtension;
class RenderTargetCubeExtension;
class RenderTargetExtension;
class Scene;
class StencilState;
class Texture;
class ThinTexture;
class UniformBuffer;
class UniformBufferExtension;
class VertexBuffer;
class WebGLDataBuffer;
class WebGLPipelineContext;
using ArrayBufferViewArray      = std::vector<ArrayBufferView>;
using BaseTexturePtr            = std::shared_ptr<BaseTexture>;
using EffectPtr                 = std::shared_ptr<Effect>;
using IInternalTextureLoaderPtr = std::shared_ptr<IInternalTextureLoader>;
using InternalTexturePtr        = std::shared_ptr<InternalTexture>;
using IPipelineContextPtr       = std::shared_ptr<IPipelineContext>;
using IShaderProcessorPtr       = std::shared_ptr<IShaderProcessor>;
using LoaderOptionsPtr          = std::shared_ptr<LoaderOptions>;
using ThinTexturePtr            = std::shared_ptr<ThinTexture>;
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

struct FramebufferDimensionsObject {
  int framebufferWidth  = 0;
  int framebufferHeight = 0;
}; // end of struct framebufferDimensionsObject

/**
 * @brief Information about the current host.
 */
struct HostInformation {
  /**
   * Defines if the current host is a mobile
   */
  bool isMobile = false;
}; // end of struct HostInformation

/**
 * @brief The base engine class (root of all engines).
 */
class BABYLON_SHARED_EXPORT ThinEngine {

public:
  /**
   * Type used to define a render target texture size (either with a number or with a rect width and
   * height)
   */
  using RenderTargetTextureSize = std::variant<int, RenderTargetSize, float>;

public:
  static std::vector<IInternalTextureLoaderPtr> _TextureLoaders;

  /**
   * @brief Returns the current version of the framework.
   */
  static std::string Version();

  /**
   * @brief Returns a string describing the current engine.
   */
  std::string description() const;

  // Updatable statics so stick with vars here

  /**
   * Gets or sets the epsilon value used by collision engine
   */
  static float CollisionsEpsilon;

  /**
   * @brief Gets the relative url used to load shaders if using the engine in non-minified mode
   */
  static std::string ShadersRepository();

  /**
   * @brief Sets the relative url used to load shaders if using the engine in non-minified mode
   */
  static void setShadersRepository(const std::string& value);

public:
  template <typename... Ts>
  static std::unique_ptr<ThinEngine> New(Ts&&... args)
  {
    std::unique_ptr<ThinEngine> engine(new ThinEngine(std::forward<Ts>(args)...));
    return engine;
  }
  virtual ~ThinEngine(); // = default

  /**
   * @brief Gets a boolean indicating if all created effects are ready.
   * @returns true if all effects are ready
   */
  virtual bool areAllEffectsReady() const;

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "Engine" string
   */
  std::string getClassName() const;

  /**
   * @brief Hidden
   */
  void _prepareWorkingCanvas();

  /**
   * @brief Reset the texture cache to empty state.
   */
  void resetTextureCache();

  /**
   * @brief Gets an object containing information about the current webGL context.
   * @returns an object containing the vender, the renderer and the version of the current webGL
   * context
   */
  GL::GLInfo getGlInfo();

  /**
   * @brief Defines the hardware scaling level.
   * By default the hardware scaling level is computed from the window device ratio.
   * if level = 1 then the engine will render at the exact resolution of the canvas. If level = 0.5
   * then the engine will render at twice the size of the canvas.
   * @param level defines the level to use
   */
  void setHardwareScalingLevel(float level);

  /**
   * @brief Gets the current hardware scaling level.
   * By default the hardware scaling level is computed from the window device ratio.
   * if level = 1 then the engine will render at the exact resolution of the canvas. If level = 0.5
   * then the engine will render at twice the size of the canvas.
   * @returns a number indicating the current hardware scaling level
   */
  virtual float getHardwareScalingLevel() const;

  /**
   * @brief Gets the list of loaded textures.
   * @returns an array containing all loaded textures
   */
  std::vector<InternalTexturePtr>& getLoadedTexturesCache();

  /**
   * @brief Gets the object containing all engine capabilities.
   * @returns the EngineCapabilities object
   */
  EngineCapabilities& getCaps();

  /**
   * @brief Stop executing a render loop function and remove it from the execution array.
   */
  void stopRenderLoop();

  /**
   * @brief Stop executing a render loop function and remove it from the execution array.
   * @param renderFunction defines the function to be removed. If not provided all functions will be
   * removed.
   */
  void stopRenderLoop(const SA::delegate<void()>& renderFunction);

  /**
   * @brief Hidden
   */
  virtual void _renderLoop();

  /**
   * @brief Gets the HTML canvas attached with the current webGL context.
   * @returns a HTML canvas
   */
  ICanvas* getRenderingCanvas();

  /**
   * @brief Gets the current render width.
   * @param useScreen defines if screen size must be used (or the current render target if any)
   * @returns a number defining the current render width
   */
  virtual int getRenderWidth(bool useScreen = false) const;

  /**
   * @brief Gets the current render height.
   * @param useScreen defines if screen size must be used (or the current render target if any)
   * @returns a number defining the current render height
   */
  virtual int getRenderHeight(bool useScreen = false) const;

  /**
   * @brief Register and execute a render loop. The engine can have more than one render function.
   * @param renderFunction defines the function to continuously execute
   */
  void runRenderLoop(const std::function<void()>& renderFunction);

  /**
   * @brief Execute the render funtion.
   * @param renderFunction defines the function to execute
   */
  void renderFunction(const std::function<void()>& renderFunction);

  /**
   * @brief Clear the current render buffer or the current render target (if any is set up)
   * @param color defines the color to use
   * @param backBuffer defines if the back buffer must be cleared
   * @param depth defines if the depth buffer must be cleared
   * @param stencil defines if the stencil buffer must be cleared
   */
  virtual void clear(const std::optional<Color4>& color, bool backBuffer, bool depth,
                     bool stencil = false);

  /**
   * @brief Hidden
   */
  void _viewport(float x, float y, float width, float height);

  /**
   * @brief Set the WebGL's viewport.
   * @param viewport defines the viewport element to be used
   * @param requiredWidth defines the width required for rendering. If not provided the rendering
   * canvas' width is used
   * @param requiredHeight defines the height required for rendering. If not provided the rendering
   * canvas' height is used
   */
  virtual void setViewport(const Viewport& viewport,
                           const std::optional<int>& requiredWidth  = std::nullopt,
                           const std::optional<int>& requiredHeight = std::nullopt);

  /**
   * @brief Begin a new frame.
   */
  virtual void beginFrame();

  /**
   * @brief End the current frame.
   */
  virtual void endFrame();

  /**
   * @brief Resize the view according to the canvas' size.
   */
  virtual void resize();

  /**
   * @brief Force a specific size of the canvas.
   * @param width defines the new canvas' width
   * @param height defines the new canvas' height
   * @returns true if the size was changed
   */
  virtual bool setSize(int width = 0, int height = 0);

  /**
   * @brief Binds the frame buffer to the specified texture.
   * @param texture The texture to render to or null for the default canvas
   * @param faceIndex The face of the texture to render to in case of cube texture
   * @param requiredWidth The width of the target to render to
   * @param requiredHeight The height of the target to render to
   * @param forceFullscreenViewport Forces the viewport to be the entire texture/screen if true
   * @param lodLevel defines the lod level to bind to the frame buffer
   * @param layer defines the 2d array index to bind to frame buffer to
   */
  virtual void bindFramebuffer(const InternalTexturePtr& texture, unsigned int faceIndex = 0,
                               std::optional<int> requiredWidth            = std::nullopt,
                               std::optional<int> requiredHeight           = std::nullopt,
                               std::optional<bool> forceFullscreenViewport = std::nullopt,
                               int lodLevel = 0, int layer = 0);

  /**
   * @brief Hidden
   */
  void _bindUnboundFramebuffer(const WebGLFramebufferPtr& framebuffer);

  /**
   * @brief Unbind the current render target texture from the webGL context
   * @param texture defines the render target texture to unbind
   * @param disableGenerateMipMaps defines a boolean indicating that mipmaps must not be generated
   * @param onBeforeUnbind defines a function which will be called before the effective unbind
   */
  virtual void unBindFramebuffer(const InternalTexturePtr& texture,
                                 bool disableGenerateMipMaps                 = false,
                                 const std::function<void()>& onBeforeUnbind = nullptr);

  /**
   * @brief Force a webGL flush (ie. a flush of all waiting webGL commands).
   */
  void flushFramebuffer();

  /**
   * @brief Unbind the current render target and bind the default framebuffer.
   */
  void restoreDefaultFramebuffer();

  /**
   * @brief Creates a vertex buffer.
   * @param data the data for the vertex buffer
   * @returns the new WebGL static buffer
   */
  virtual WebGLDataBufferPtr createVertexBuffer(const Float32Array& data);

  /**
   * @brief Creates a dynamic vertex buffer.
   * @param data the data for the dynamic vertex buffer
   * @returns the new WebGL dynamic buffer
   */
  virtual WebGLDataBufferPtr createDynamicVertexBuffer(const Float32Array& data);

  /**
   * @brief Creates a new index buffer.
   * @param indices defines the content of the index buffer
   * @param updatable defines if the index buffer must be updatable
   * @returns a new webGL buffer
   */
  virtual WebGLDataBufferPtr createIndexBuffer(const IndicesArray& indices, bool updatable = false);

  /**
   * @brief Bind a webGL buffer to the webGL context.
   * @param buffer defines the buffer to bind
   */
  void bindArrayBuffer(const WebGLDataBufferPtr& buffer);

  /**
   * @brief Update the bound buffer with the given data.
   * @param data defines the data to update
   */
  void updateArrayBuffer(const Float32Array& data);

  /**
   * @brief Hidden
   */
  void _bindIndexBufferWithCache(const WebGLDataBufferPtr& indexBuffer);

  /**
   * @brief Records a vertex array object.
   * @see https://doc.babylonjs.com/features/webgl2#vertex-array-objects
   * @param vertexBuffers defines the list of vertex buffers to store
   * @param indexBuffer defines the index buffer to store
   * @param effect defines the effect to store
   * @returns the new vertex array object
   */
  WebGLVertexArrayObjectPtr
  recordVertexArrayObject(const std::unordered_map<std::string, VertexBufferPtr>& vertexBuffers,
                          const WebGLDataBufferPtr& indexBuffer, const EffectPtr& effect);

  /**
   * @brief Bind a specific vertex array object.
   * @see https://doc.babylonjs.com/features/webgl2#vertex-array-objects
   * @param vertexArrayObject defines the vertex array object to bind
   * @param indexBuffer defines the index buffer to bind
   */
  void bindVertexArrayObject(const WebGLVertexArrayObjectPtr& vertexArrayObject,
                             const WebGLDataBufferPtr& indexBuffer);

  /**
   * @brief Bind webGl buffers directly to the webGL context.
   * @param vertexBuffer defines the vertex buffer to bind
   * @param indexBuffer defines the index buffer to bind
   * @param vertexDeclaration defines the vertex declaration to use with the vertex buffer
   * @param vertexStrideSize defines the vertex stride of the vertex buffer
   * @param effect defines the effect associated with the vertex buffer
   */
  void bindBuffersDirectly(const WebGLDataBufferPtr& vertexBuffer,
                           const WebGLDataBufferPtr& indexBuffer,
                           const Float32Array& vertexDeclaration, int vertexStrideSize = 3,
                           const EffectPtr& effect = nullptr);

  /**
   * @brief Bind a list of vertex buffers to the webGL context.
   * @param vertexBuffers defines the list of vertex buffers to bind
   * @param indexBuffer defines the index buffer to bind
   * @param effect defines the effect associated with the vertex buffers
   */
  virtual void bindBuffers(const std::unordered_map<std::string, VertexBufferPtr>& vertexBuffers,
                           const WebGLDataBufferPtr& indexBuffer, const EffectPtr& effect);

  /**
   * @brief Unbind all instance attributes.
   */
  void unbindInstanceAttributes();

  /**
   * @brief Release and free the memory of a vertex array object
   * @param vao defines the vertex array object to delete
   */
  void releaseVertexArrayObject(const WebGLVertexArrayObjectPtr& vao);

  /**
   * @brief Hidden
   */
  bool _releaseBuffer(const WebGLDataBufferPtr& buffer);

  /**
   * @brief Update the content of a webGL buffer used with instanciation and bind it to the webGL
   * context
   * @param instancesBuffer defines the webGL buffer to update and bind
   * @param data defines the data to store in the buffer
   * @param offsetLocations defines the offsets or attributes information used to determine where
   * data must be stored in the buffer
   */
  void updateAndBindInstancesBuffer(
    const WebGLDataBufferPtr& instancesBuffer, const Float32Array& data,
    std::variant<Uint32Array, std::vector<InstancingAttributeInfo>>& offsetLocations);

  /**
   * @brief Bind the content of a webGL buffer used with instantiation.
   * @param instancesBuffer defines the webGL buffer to bind
   * @param attributesInfo defines the offsets or attributes information used to determine where
   * data must be stored in the buffer
   * @param computeStride defines Whether to compute the strides from the info or use the default 0
   */
  void bindInstancesBuffer(const WebGLDataBufferPtr& instancesBuffer,
                           std::vector<InstancingAttributeInfo>& attributesInfo,
                           bool computeStride = true);

  /**
   * @brief Disable the instance attribute corresponding to the name in parameter.
   * @param name defines the name of the attribute to disable
   */
  void disableInstanceAttributeByName(const std::string& name);

  /**
   * @brief Disable the instance attribute corresponding to the location in parameter.
   * @param attributeLocation defines the attribute location of the attribute to disable
   */
  void disableInstanceAttribute(unsigned int attributeLocation);

  /**
   * @brief Disable the attribute corresponding to the location in parameter.
   * @param attributeLocation defines the attribute location of the attribute to disable
   */
  void disableAttributeByIndex(unsigned int attributeLocation);

  /**
   * @brief Send a draw order.
   * @param useTriangles defines if triangles must be used to draw (else wireframe will be used)
   * @param indexStart defines the starting index
   * @param indexCount defines the number of index to draw
   * @param instancesCount defines the number of instances to draw (if instanciation is enabled)
   */
  virtual void draw(bool useTriangles, int indexStart, int indexCount, int instancesCount = 0);

  /**
   * @brief Draw a list of points.
   * @param verticesStart defines the index of first vertex to draw
   * @param verticesCount defines the count of vertices to draw
   * @param instancesCount defines the number of instances to draw (if instanciation is enabled)
   */
  void drawPointClouds(int verticesStart, int verticesCount, int instancesCount = 0);

  /**
   * @brief Draw a list of unindexed primitives.
   * @param useTriangles defines if triangles must be used to draw (else wireframe will be used)
   * @param verticesStart defines the index of first vertex to draw
   * @param verticesCount defines the count of vertices to draw
   * @param instancesCount defines the number of instances to draw (if instanciation is enabled)
   */
  void drawUnIndexed(bool useTriangles, int verticesStart, int verticesCount,
                     int instancesCount = 0);

  /**
   * @brief Draw a list of indexed primitives.
   * @param fillMode defines the primitive to use
   * @param indexStart defines the starting index
   * @param indexCount defines the number of index to draw
   * @param instancesCount defines the number of instances to draw (if instanciation is enabled)
   */
  virtual void drawElementsType(unsigned int fillMode, int indexStart, int indexCount,
                                int instancesCount = 0);

  /**
   * @brief Draw a list of unindexed primitives.
   * @param fillMode defines the primitive to use
   * @param verticesStart defines the index of first vertex to draw
   * @param verticesCount defines the count of vertices to draw
   * @param instancesCount defines the number of instances to draw (if instanciation is enabled)
   */
  virtual void drawArraysType(unsigned int fillMode, int verticesStart, int verticesCount,
                              int instancesCount = 0);

  /** Shaders **/

  /**
   * @brief Hidden
   */
  void _releaseEffect(Effect* effect);

  /**
   * @brief Hidden
   */
  virtual void _deletePipelineContext(const IPipelineContextPtr& pipelineContext);

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
    IEffectCreationOptions& options, ThinEngine* engine,
    const std::function<void(const EffectPtr& effect)>& onCompiled = nullptr);

  /**
   * @brief Hidden
   */
  std::string _getShaderSource(const WebGLShaderPtr& shader);

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
  bool _isRenderingStateCompiled(IPipelineContext* pipelineContext);

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
   * @brief Set the value of an uniform to a number (int)
   * @param uniform defines the webGL uniform location where to store the value
   * @param value defines the int number to store
   * @returns true if the value was set
   */
  virtual bool setInt(const WebGLUniformLocationPtr& uniform, int value);

  /**
   * @brief Set the value of an uniform to an array of int32.
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   * @returns true if the value was set
   */
  virtual bool setIntArray(const WebGLUniformLocationPtr& uniform, const Int32Array& array);

  /**
   * @brief Set the value of an uniform to an array of int32 (stored as vec2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   * @returns true if the value was set
   */
  virtual bool setIntArray2(const WebGLUniformLocationPtr& uniform, const Int32Array& array);

  /**
   * @brief Set the value of an uniform to an array of int32 (stored as vec3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   * @returns true if the value was set
   */
  virtual bool setIntArray3(const WebGLUniformLocationPtr& uniform, const Int32Array& array);

  /**
   * @brief Set the value of an uniform to an array of int32 (stored as vec4).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   * @returns true if the value was set
   */
  virtual bool setIntArray4(const WebGLUniformLocationPtr& uniform, const Int32Array& array);

  /**
   * @brief Set the value of an uniform to an array of number.
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   * @returns true if the value was set
   */
  virtual bool setArray(const WebGLUniformLocationPtr& uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of number (stored as vec2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   * @returns true if the value was set
   */
  virtual bool setArray2(const WebGLUniformLocationPtr& uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of number (stored as vec3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   * @returns true if the value was set
   */
  virtual bool setArray3(const WebGLUniformLocationPtr& uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of number (stored as vec4).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   * @returns true if the value was set
   */
  virtual bool setArray4(const WebGLUniformLocationPtr& uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of float32 (stored as matrices).
   * @param uniform defines the webGL uniform location where to store the value
   * @param matrices defines the array of float32 to store
   * @returns true if the value was set
   */
  virtual bool setMatrices(const WebGLUniformLocationPtr& uniform, const Float32Array& matrices);

  /**
   * @brief Set the value of an uniform to a matrix (3x3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param matrix defines the Float32Array representing the 3x3 matrix to store
   * @returns true if the value was set
   */
  virtual bool setMatrix3x3(const WebGLUniformLocationPtr& uniform, const Float32Array& matrix);

  /**
   * @brief Set the value of an uniform to a matrix (2x2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param matrix defines the Float32Array representing the 2x2 matrix to store
   * @returns true if the value was set
   */
  virtual bool setMatrix2x2(const WebGLUniformLocationPtr& uniform, const Float32Array& matrix);

  /**
   * @brief Set the value of an uniform to a number (float).
   * @param uniform defines the webGL uniform location where to store the value
   * @param value defines the float number to store
   * @returns true if the value was transfered
   */
  virtual bool setFloat(const WebGLUniformLocationPtr& uniform, float value);

  /**
   * @brief Set the value of an uniform to a vec2.
   * @param uniform defines the webGL uniform location where to store the value
   * @param x defines the 1st component of the value
   * @param y defines the 2nd component of the value
   * @returns true if the value was set
   */
  virtual bool setFloat2(const WebGLUniformLocationPtr& uniform, float x, float y);

  /**
   * @brief Set the value of an uniform to a vec3.
   * @param uniform defines the webGL uniform location where to store the value
   * @param x defines the 1st component of the value
   * @param y defines the 2nd component of the value
   * @param z defines the 3rd component of the value
   * @returns true if the value was set
   */
  virtual bool setFloat3(const WebGLUniformLocationPtr& uniform, float x, float y, float z);

  /**
   * @brief Set the value of an uniform to a vec4.
   * @param uniform defines the webGL uniform location where to store the value
   * @param x defines the 1st component of the value
   * @param y defines the 2nd component of the value
   * @param z defines the 3rd component of the value
   * @param w defines the 4th component of the value
   * @returns true if the value was set
   */
  virtual bool setFloat4(const WebGLUniformLocationPtr& uniform, float x, float y, float z,
                         float w);

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
   * @param url defines a value which contains one of the following:
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
   * @param mimeType defines an optional mime type
   * @returns a InternalTexture for assignment back into BABYLON.Texture
   */
  virtual InternalTexturePtr createTexture(
    std::string url, bool noMipmap, bool invertY, Scene* scene,
    unsigned int samplingMode = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE,
    const std::function<void(InternalTexture*, EventState&)>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr,
    const std::optional<std::variant<std::string, ArrayBuffer, ArrayBufferView, Image>>& buffer
    = std::nullopt,
    const InternalTexturePtr& fallBack        = nullptr,
    const std::optional<unsigned int>& format = std::nullopt,
    const std::string& forcedExtension = "", const std::string& mimeType = "",
    const LoaderOptionsPtr& loaderOptions = nullptr);

  /**
   * @brief Create a cube texture from prefiltered data (ie. the mipmaps contain ready to use data
   * for PBR reflection)
   * @param rootUrl defines the url where the file to load is located
   * @param scene defines the current scene
   * @param lodScale defines scale to apply to the mip map selection
   * @param lodOffset defines offset to apply to the mip map selection
   * @param onLoad defines an optional callback raised when the texture is loaded
   * @param onError defines an optional callback raised if there is an issue to load the texture
   * @param format defines the format of the data
   * @param forcedExtension defines the extension to use to pick the right loader
   * @param createPolynomials defines wheter or not to create polynomails harmonics for the texture
   * @returns the cube texture as an InternalTexture
   */
  InternalTexturePtr createPrefilteredCubeTexture(
    const std::string& rootUrl, Scene* scene, float lodScale, float lodOffset,
    const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr,
    unsigned int format = 0, const std::string& forcedExtension = "",
    bool createPolynomials = true);

  /**
   * @brief Loads an image as an HTMLImageElement.
   * @param input url string
   * @param onLoad callback called when the image successfully loads
   * @param onError callback called when the image fails to load
   * @param offlineProvider offline provider for caching
   * @param mimeType optional mime type
   * @returns the HTMLImageElement of the loaded image
   * @hidden
   */
  static void _FileToolsLoadImageFromUrl(
    std::string url, const std::function<void(const Image& img)>& onLoad,
    const std::function<void(const std::string& message, const std::string& exception)>& onError,
    bool flipVertically = false, const std::string& mimeType = "");

  /**
   * @brief Loads an image as an HTMLImageElement.
   * @param input url string, ArrayBuffer, or Blob to load
   * @param onLoad callback called when the image successfully loads
   * @param onError callback called when the image fails to load
   * @param offlineProvider offline provider for caching
   * @param mimeType optional mime type
   * @returns the HTMLImageElement of the loaded image
   * @hidden
   */
  static void _FileToolsLoadImage(
    const std::variant<std::string, ArrayBuffer, ArrayBufferView, Image>& input, bool invertY,
    const std::function<void(const Image& img)>& onLoad,
    const std::function<void(const std::string& message, const std::string& exception)>& onError,
    const std::string& mimeType);

  /**
   * @brief Rescales a texture.
   * @param source input texutre
   * @param destination destination texture
   * @param scene scene to use to render the resize
   * @param internalFormat format to use when resizing
   * @param onComplete callback to be called when resize has completed
   */
  virtual void _rescaleTexture(const InternalTexturePtr& source,
                               const InternalTexturePtr& destination, Scene* scene,
                               unsigned int internalFormat,
                               const std::function<void()>& onComplete);

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
  void updateTextureWrappingMode(const InternalTexturePtr& texture,
                                 std::optional<unsigned int> wrapU,
                                 std::optional<unsigned int> wrapV = std::nullopt,
                                 std::optional<unsigned int> wrapR = std::nullopt);

  /**
   * @brief Hidden
   */
  void _setupDepthStencilTexture(const InternalTexturePtr& internalTexture,
                                 const RenderTargetTextureSize& size, bool generateStencil,
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
   * @brief Update a portion of an internal texture.
   * @param texture defines the texture to update
   * @param imageData defines the data to store into the texture
   * @param xOffset defines the x coordinates of the update rectangle
   * @param yOffset defines the y coordinates of the update rectangle
   * @param width defines the width of the update rectangle
   * @param height defines the height of the update rectangle
   * @param faceIndex defines the face index if texture is a cube (0 by default)
   * @param lod defines the lod level to update (0 by default)
   */
  virtual void updateTextureData(const InternalTexturePtr& texture,
                                 const ArrayBufferView& imageData, int xOffset, int yOffset,
                                 int width, int height, unsigned int faceIndex = 0, int lod = 0);

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
  void _releaseFramebufferObjects(const InternalTexturePtr& texture);

  /**
   * @brief Hidden
   */
  virtual void _releaseTexture(const InternalTexturePtr& texture);

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
                  const ThinTexturePtr& texture);

  /**
   * @brief Sets an array of texture to the webGL context
   * @param channel defines the channel where the texture array must be set
   * @param uniform defines the associated uniform location
   * @param textures defines the array of textures to bind
   */
  void setTextureArray(int channel, const WebGLUniformLocationPtr& uniform,
                       const std::vector<ThinTexturePtr>& textures);

  /**
   * @brief Hidden
   */
  void _setAnisotropicLevel(unsigned int target, const InternalTexturePtr& internalTexture,
                            unsigned int anisotropicFilteringLevel);

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
  virtual void dispose();

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
   * @brief Hidden
   */
  IFileRequest _loadFile(
    const std::string& url,
    const std::function<void(const std::variant<std::string, ArrayBufferView>& data,
                             const std::string& responseURL)>& onSuccess,
    const std::function<void(const ProgressEvent& event)>& onProgress = nullptr,
    bool useArrayBuffer                                               = false,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr);

  /**
   * @brief Reads pixels from the current frame buffer. Please note that this function can be slow.
   * @param x defines the x coordinate of the rectangle where pixels must be read
   * @param y defines the y coordinate of the rectangle where pixels must be read
   * @param width defines the width of the rectangle where pixels must be read
   * @param height defines the height of the rectangle where pixels must be read
   * @param hasAlpha defines whether the output should have alpha or not (defaults to true)
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

  //------------------------------------------------------------------------------------------------
  //                              Alpha Extension
  //------------------------------------------------------------------------------------------------

  /**
   * @brief Sets alpha constants used by some alpha blending modes.
   * @param r defines the red component
   * @param g defines the green component
   * @param b defines the blue component
   * @param a defines the alpha component
   */
  void setAlphaConstants(float r, float g, float b, float a);

  /**
   * @brief Sets the current alpha mode.
   * @param mode defines the mode to use (one of the Engine.ALPHA_XXX)
   * @param noDepthWriteChange defines if depth writing state should remains unchanged (false by
   * default)
   * @see https://doc.babylonjs.com/resources/transparency_and_how_meshes_are_rendered
   */
  virtual void setAlphaMode(unsigned int mode, bool noDepthWriteChange = false);

  /**
   * @brief Gets the current alpha mode.
   * @see https://doc.babylonjs.com/resources/transparency_and_how_meshes_are_rendered
   * @returns the current alpha mode
   */
  unsigned int getAlphaMode() const;

  /**
   * @brief Sets the current alpha equation.
   * @param equation defines the equation to use (one of the Engine.ALPHA_EQUATION_XXX)
   */
  void setAlphaEquation(unsigned int equation);

  /**
   * @brief Gets the current alpha equation.
   * @returns the current alpha equation
   */
  unsigned int getAlphaEquation() const;

  //------------------------------------------------------------------------------------------------
  //                              Cube Texture Extension
  //------------------------------------------------------------------------------------------------

  /**
   * @brief Creates a depth stencil cube texture.
   * This is only available in WebGL 2.
   * @param size The size of face edge in the cube texture.
   * @param options The options defining the cube texture.
   * @returns The cube texture
   */
  InternalTexturePtr _createDepthStencilCubeTexture(int size,
                                                    const DepthTextureCreationOptions& options);

  /**
   * @brief Creates a cube texture.
   * @param rootUrl defines the url where the files to load is located
   * @param scene defines the current scene
   * @param files defines the list of files to load (1 per face)
   * @param noMipmap defines a boolean indicating that no mipmaps shall be generated (false by
   * default)
   * @param onLoad defines an optional callback raised when the texture is loaded
   * @param onError defines an optional callback raised if there is an issue to load the texture
   * @param format defines the format of the data
   * @param forcedExtension defines the extension to use to pick the right loader
   * @param createPolynomials if a polynomial sphere should be created for the cube texture
   * @param lodScale defines the scale applied to environment texture. This manages the range of LOD
   * level used for IBL according to the roughness
   * @param lodOffset defines the offset applied to environment texture. This manages first LOD
   * level used for IBL according to the roughness
   * @param fallback defines texture to use while falling back when (compressed) texture file not
   * found.
   * @param loaderOptions options to be passed to the loader
   * @returns the cube texture as an InternalTexture
   */
  InternalTexturePtr createCubeTexture(
    std::string rootUrl, Scene* scene, const std::vector<std::string>& files, bool noMipmap,
    const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr,
    unsigned int format = 0, const std::string& forcedExtension = "",
    bool createPolynomials = false, float lodScale = 0.f, float lodOffset = 0.f,
    const InternalTexturePtr& fallback = nullptr, const LoaderOptionsPtr& loaderOptions = nullptr);

  /**
   * @brief Hidden
   */
  void _cascadeLoadImgs(
    const std::string& rootUrl, Scene* scene,
    const std::function<void(const std::vector<Image>& images)>& onfinish,
    const std::vector<std::string>& files,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr);

  /**
   * @brief Hidden
   */
  void _setCubeMapTextureParams(const InternalTexturePtr& texture, bool loadMipmap);

  //------------------------------------------------------------------------------------------------
  //                              Dynamic Buffer Extension
  //------------------------------------------------------------------------------------------------

  /**
   * @brief Update a dynamic index buffer.
   * @param indexBuffer defines the target index buffer
   * @param indices defines the data to update
   * @param offset defines the offset in the target index buffer where update should start
   */
  virtual void updateDynamicIndexBuffer(const WebGLDataBufferPtr& indexBuffer,
                                        const IndicesArray& indices, int offset = 0);

  /**
   * @brief Updates a dynamic vertex buffer.
   * @param vertexBuffer the vertex buffer to update
   * @param data the data used to update the vertex buffer
   * @param byteOffset the byte offset of the data
   * @param byteLength the byte length of the data
   */
  virtual void updateDynamicVertexBuffer(const WebGLDataBufferPtr& vertexBuffer,
                                         const Float32Array& data, int byteOffset = -1,
                                         int byteLength = -1);

  //------------------------------------------------------------------------------------------------
  //                              Dynamic Texture Extension
  //------------------------------------------------------------------------------------------------

  /**
   * @brief Creates a dynamic texture.
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @param generateMipMaps defines if the engine should generate the mip levels
   * @param samplingMode defines the required sampling mode (Texture.NEAREST_SAMPLINGMODE by
   * default)
   * @returns the dynamic texture inside an InternalTexture
   */
  InternalTexturePtr createDynamicTexture(int width, int height, bool generateMipMaps,
                                          unsigned int samplingMode);

  /**
   * @brief Update the content of a dynamic texture.
   * @param texture defines the texture to update
   * @param source defines the source containing the data
   * @param invertY defines if data must be stored with Y axis inverted
   * @param premulAlpha defines if alpha is stored as premultiplied
   * @param format defines the format of the data
   * @param forceBindTexture if the texture should be forced to be bound eg. after a graphics
   * context loss (Default: false)
   */
  virtual void updateDynamicTexture(const InternalTexturePtr& texture, ICanvas* source,
                                    std::optional<bool> invertY        = std::nullopt,
                                    bool premulAlpha                   = false,
                                    std::optional<unsigned int> format = std::nullopt,
                                    bool forceBindTexture              = false);

  //------------------------------------------------------------------------------------------------
  //                              Multi Render Extension
  //------------------------------------------------------------------------------------------------

  /**
   * @brief Unbind a list of render target textures from the webGL context.
   * This is used only when drawBuffer extension or webGL2 are active
   * @param textures defines the render target textures to unbind
   * @param disableGenerateMipMaps defines a boolean indicating that mipmaps must not be generated
   * @param onBeforeUnbind defines a function which will be called before the effective unbind
   */
  void unBindMultiColorAttachmentFramebuffer(const std::vector<InternalTexturePtr>& textures,
                                             bool disableGenerateMipMaps                 = false,
                                             const std::function<void()>& onBeforeUnbind = nullptr);

  /**
   * @brief Create a multi render target texture.
   * @see http://doc.babylonjs.com/features/webgl2#multiple-render-target
   * @param size defines the size of the texture
   * @param options defines the creation options
   * @returns the cube texture as an InternalTexture
   */
  std::vector<InternalTexturePtr>
  createMultipleRenderTarget(ISize size, const IMultiRenderTargetOptions& options);

  /**
   * @brief Update the sample count for a given multiple render target texture
   * @see http://doc.babylonjs.com/features/webgl2#multisample-render-targets
   * @param textures defines the textures to update
   * @param samples defines the sample count to set
   * @returns the effective sample count (could be 0 if multisample render targets are not
   * supported)
   */
  unsigned int
  updateMultipleRenderTargetTextureSampleCount(const std::vector<InternalTexturePtr>& textures,
                                               unsigned int samples);

  /**
   * @brief Select a subsets of attachments to draw to.
   * @param attachments gl attachments
   */
  void bindAttachments(const std::vector<unsigned int>& attachments);

  /**
   * @brief Creates a layout object to draw/clear on specific textures in a MRT.
   * @param textureStatus textureStatus[i] indicates if the i-th is active
   * @returns A layout to be fed to the engine, calling `bindAttachments`.
   */
  std::vector<unsigned int> buildTextureLayout(const std::vector<bool>& textureStatus) const;

  /**
   * @brief Restores the webgl state to only draw on the main color attachment.
   */
  void restoreSingleAttachment();

  //------------------------------------------------------------------------------------------------
  //                              Raw Texture Extension
  //------------------------------------------------------------------------------------------------

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
   * @brief Update a raw texture.
   * @param texture defines the texture to update
   * @param data defines the data to store in the texture
   * @param format defines the format of the data
   * @param invertY defines if data must be stored with Y axis inverted
   * @param compression defines the compression used (null by default)
   * @param type defines the type fo the data (Engine.TEXTURETYPE_UNSIGNED_INT by default)
   */
  void updateRawTexture(const InternalTexturePtr& texture, const Uint8Array& data,
                        unsigned int format, bool invertY = true,
                        const std::string& compression = "",
                        unsigned int type              = Constants::TEXTURETYPE_UNSIGNED_INT);

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
   * @brief Update a raw cube texture.
   * @param texture defines the texture to udpdate
   * @param data defines the data to store
   * @param format defines the data format
   * @param type defines the type fo the data (Engine.TEXTURETYPE_UNSIGNED_INT by default)
   * @param invertY defines if data must be stored with Y axis inverted
   * @param compression defines the compression used (null by default)
   * @param level defines which level of the texture to update
   */
  void updateRawCubeTexture(const InternalTexturePtr& texture,
                            const std::vector<ArrayBufferView>& data, unsigned int format,
                            unsigned int type, bool invertY = true,
                            const std::string& compression = "", unsigned int level = 0);

  /**
   * @brief Creates a new raw cube texture from a specified url.
   * @param url defines the url where the data is located
   * @param scene defines the current scene
   * @param size defines the size of the textures
   * @param format defines the format of the data
   * @param type defines the type fo the data (like Engine.TEXTURETYPE_UNSIGNED_INT)
   * @param noMipmap defines if the engine should avoid generating the mip levels
   * @param callback defines a callback used to extract texture data from loaded data
   * @param mipmapGenerator defines to provide an optional tool to generate mip levels
   * @param onLoad defines a callback called when texture is loaded
   * @param onError defines a callback called if there is an error
   * @param samplingMode defines the required sampling mode (like Texture.NEAREST_SAMPLINGMODE)
   * @param invertY defines if data must be stored with Y axis inverted
   * @returns the cube texture as an InternalTexture
   */
  InternalTexturePtr createRawCubeTextureFromUrl(
    const std::string& url, Scene* scene, int size, unsigned int format, unsigned int type,
    bool noMipmap,
    const std::function<ArrayBufferViewArray(const ArrayBuffer& arrayBuffer)>& callback,
    const std::function<std::vector<ArrayBufferViewArray>(const ArrayBufferViewArray& faces)>&
      mipmapGenerator,
    const std::function<void()>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE, bool invertY = false);

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
   * @brief Update a raw 3D texture.
   * @param texture defines the texture to update
   * @param data defines the data to store
   * @param format defines the data format
   * @param invertY defines if data must be stored with Y axis inverted
   * @param compression defines the used compression (can be null)
   * @param textureType defines the texture Type (Engine.TEXTURETYPE_UNSIGNED_INT,
   * Engine.TEXTURETYPE_FLOAT...)
   */
  void updateRawTexture3D(const InternalTexturePtr& texture, const ArrayBufferView& data,
                          unsigned int format, bool invertY = true,
                          const std::string& compression = "",
                          unsigned int textureType       = Constants::TEXTURETYPE_UNSIGNED_INT);

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
   * @brief Update a raw 2D array texture.
   * @param texture defines the texture to update
   * @param data defines the data to store
   * @param format defines the data format
   * @param invertY defines if data must be stored with Y axis inverted
   * @param compression defines the used compression (can be null)
   * @param textureType defines the texture Type (Engine.TEXTURETYPE_UNSIGNED_INT,
   * Engine.TEXTURETYPE_FLOAT...)
   */
  void updateRawTexture2DArray(const InternalTexturePtr& texture, const ArrayBufferView& data,
                               unsigned int format, bool invertY = true,
                               const std::string& compression = "",
                               unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT);

  //------------------------------------------------------------------------------------------------
  //                              Read Texture Extension
  //------------------------------------------------------------------------------------------------

  /**
   * @brief Hidden
   */
  ArrayBufferView _readTexturePixels(const InternalTexturePtr& texture, int width, int height,
                                     int faceIndex = -1, int level = 0,
                                     std::optional<ArrayBufferView> buffer = std::nullopt);

  //------------------------------------------------------------------------------------------------
  //                              Render Target Extension
  //------------------------------------------------------------------------------------------------

  /**
   * @brief Creates a new render target texture.
   * @param size defines the size of the texture
   * @param options defines the options used to create the texture
   * @returns a new render target texture stored in an InternalTexture
   */
  virtual InternalTexturePtr createRenderTargetTexture(const RenderTargetTextureSize& size,
                                                       const IRenderTargetOptions& options);

  /**
   * @brief Creates a depth stencil texture.
   * This is only available in WebGL 2 or with the depth texture extension available.
   * @param size The size of face edge in the texture.
   * @param options The options defining the texture.
   * @returns The texture
   */
  InternalTexturePtr createDepthStencilTexture(const RenderTargetTextureSize& size,
                                               const DepthTextureCreationOptions& options);

  /**
   * @brief Hidden
   */
  InternalTexturePtr _createDepthStencilTexture(const RenderTargetTextureSize& size,
                                                const DepthTextureCreationOptions& options);

  //------------------------------------------------------------------------------------------------
  //                              Render Target Cube Extension
  //------------------------------------------------------------------------------------------------

  /**
   * @brief Creates a new render target cube texture.
   * @param size defines the size of the texture
   * @param options defines the options used to create the texture
   * @returns a new render target cube texture stored in an InternalTexture
   */
  InternalTexturePtr createRenderTargetCubeTexture(const ISize& size,
                                                   const IRenderTargetOptions& options);

  //------------------------------------------------------------------------------------------------
  //                              Uniform Buffer Extension
  //------------------------------------------------------------------------------------------------

  /**
   * @brief Create an uniform buffer.
   * @see https://doc.babylonjs.com/features/webgl2#uniform-buffer-objets
   * @param elements defines the content of the uniform buffer
   * @returns the webGL uniform buffer
   */
  WebGLDataBufferPtr createUniformBuffer(const Float32Array& elements);

  /**
   * @brief Create a dynamic uniform buffer.
   * @see https://doc.babylonjs.com/features/webgl2#uniform-buffer-objets
   * @param elements defines the content of the uniform buffer
   * @returns the webGL uniform buffer
   */
  WebGLDataBufferPtr createDynamicUniformBuffer(const Float32Array& elements);

  /**
   * @brief Update an existing uniform buffer.
   * @see https://doc.babylonjs.com/features/webgl2#uniform-buffer-objets
   * @param uniformBuffer defines the target uniform buffer
   * @param elements defines the content to update
   * @param offset defines the offset in the uniform buffer where update should start
   * @param count defines the size of the data to update
   */
  void updateUniformBuffer(const WebGLDataBufferPtr& uniformBuffer, const Float32Array& elements,
                           int offset = -1, int count = -1);

  /**
   * @brief Bind an uniform buffer to the current webGL context
   * @param buffer defines the buffer to bind
   */
  void bindUniformBuffer(const WebGLDataBufferPtr& buffer);

  /**
   * @brief Bind a buffer to the current webGL context at a given location.
   * @param buffer defines the buffer to bind
   * @param location defines the index where to bind the buffer
   */
  void bindUniformBufferBase(const WebGLDataBufferPtr& buffer, unsigned int location);

  /**
   * @brief Bind a specific block at a given index in a specific shader program.
   * @param pipelineContext defines the pipeline context to use
   * @param blockName defines the block name
   * @param index defines the index where to bind the block
   */
  void bindUniformBlock(const IPipelineContextPtr& pipelineContext, const std::string& blockName,
                        unsigned int index);

protected:
  /**
   * @brief Creates a new engine.
   * @param canvasOrContext defines the canvas or WebGL context to use for rendering. If you provide
   * a WebGL context, Babylon.js will not hook events on the canvas (like pointers, keyboards,
   * etc...) so no event observables will be available. This is mostly used when Babylon.js is used
   * as a plugin on a system which alreay used the WebGL context
   * @param antialias defines enable antialiasing (default: false)
   * @param options defines further options to be sent to the getContext() function
   * @param adaptToDeviceRatio defines whether to adapt to the device's viewport characteristics
   * (default: false)
   */
  ThinEngine(ICanvas* canvas, const EngineOptions& options = EngineOptions());

  /**
   * @brief Gets a boolean indicating that the engine supports uniform buffers.
   * @see https://doc.babylonjs.com/features/webgl2#uniform-buffer-objets
   */
  bool get_supportsUniformBuffers() const;

  /**
   * @brief Hidden
   */
  bool get__shouldUseHighPrecisionShader() const;

  /**
   * @brief Gets a boolean indicating that only power of 2 textures are supported.
   * Please note that you can still use non power of 2 textures but in this case the engine will
   * forcefully convert them
   */
  bool get_needPOTTextures() const;

  /**
   * @brief Gets a boolean indicating if resources should be retained to be able to handle context
   * lost events.
   * @see https://doc.babylonjs.com/how_to/optimizing_your_scene#handling-webgl-context-lost
   */
  bool get_doNotHandleContextLost() const;

  /**
   * @brief Sets a boolean indicating if resources should be retained to be able to handle context
   * lost events.
   * @see https://doc.babylonjs.com/how_to/optimizing_your_scene#handling-webgl-context-lost
   */
  void set_doNotHandleContextLost(bool value);

  /**
   * @brief Hidden
   */
  virtual bool get__supportsHardwareTextureRescaling() const;

  /**
   * @brief Sets the object from which width and height will be taken from when getting render width
   * and height Will fallback to the gl object
   * @param dimensions the framebuffer width and height that will be used.
   */
  void
  set_framebufferDimensionsObject(const std::optional<FramebufferDimensionsObject>& dimensions);

  /**
   * @brief Gets the current viewport.
   */
  std::optional<Viewport>& get_currentViewport();

  /**.
   * @brief Gets the default empty texture
   */
  InternalTexturePtr& get_emptyTexture();

  /**
   * @brief Gets the default empty 3D texture.
   */
  InternalTexturePtr& get_emptyTexture3D();

  /**
   * @brief Gets the default empty 2D array texture.
   */
  InternalTexturePtr& get_emptyTexture2DArray();

  /**
   * @brief Gets the default empty cube texture.
   */
  InternalTexturePtr& get_emptyCubeTexture();

  /**
   * @brief Gets version of the current webGL context.
   */
  float get_webGLVersion() const;

  /**
   * @brief Returns true if the stencil buffer has been enabled through the creation option of the
   * context.
   */
  bool get_isStencilEnable() const;

  /**
   * @brief Gets the depth culling state manager.
   */
  std::unique_ptr<DepthCullingState>& get_depthCullingState();

  /**
   * @brief Gets the alpha state manager.
   */
  std::unique_ptr<AlphaState>& get_alphaState();

  /**
   * @brief Gets the stencil state manager.
   */
  std::unique_ptr<StencilState>& get_stencilState();

  virtual void _rebuildBuffers();
  void _initGLContext();
  /** VBOs **/
  /** @hidden */
  void _resetVertexBufferBinding();
  void _resetIndexBufferBinding();
  void _normalizeIndexData(const IndicesArray& indices, Uint16Array& uint16ArrayResult,
                           Uint32Array& uint32ArrayResult);
  void bindIndexBuffer(const WebGLDataBufferPtr& buffer);
  void _deleteBuffer(const WebGLDataBufferPtr& buffer);
  /** @hidden */
  virtual void _reportDrawCall();
  static std::string _ConcatenateShader(const std::string& source, const std::string& defines,
                                        const std::string& shaderVersion);
  virtual WebGLProgramPtr
  _createShaderProgram(const WebGLPipelineContextPtr& pipelineContext,
                       const WebGLShaderPtr& vertexShader, const WebGLShaderPtr& fragmentShader,
                       WebGLRenderingContext* context,
                       const std::vector<std::string>& transformFeedbackVaryings = {});
  void _finalizePipelineContext(WebGLPipelineContext* pipelineContext);
  void _prepareWebGLTextureContinuation(const InternalTexturePtr& texture, Scene* scene,
                                        bool noMipmap, bool isCompressed,
                                        unsigned int samplingMode);
  void _deleteTexture(const WebGLTexturePtr& texture);
  void _setProgram(const WebGLProgramPtr& program);
  bool _setTexture(int channel, const ThinTexturePtr& texture, bool isPartOfTextureArray = false,
                   bool depthStencilTexture = false);

private:
  void _rebuildInternalTextures();
  void _rebuildEffects();
  WebGLFramebufferPtr _getRealFrameBuffer(const WebGLFramebufferPtr& framebuffer);
  WebGLDataBufferPtr _createVertexBuffer(const Float32Array& data, unsigned int usage);
  void bindBuffer(const WebGLDataBufferPtr& buffer, int target);
  void _vertexAttribPointer(const WebGLDataBufferPtr& buffer, unsigned int indx, int size,
                            unsigned int type, bool normalized, int stride, int offset);
  void _bindVertexBuffersAttributes(
    const std::unordered_map<std::string, VertexBufferPtr>& vertexBuffers, const EffectPtr& effect);
  void _unbindVertexArrayObject();
  unsigned int _drawMode(unsigned int fillMode) const;
  WebGLShaderPtr _compileShader(const std::string& source, const std::string& type,
                                const std::string& defines, const std::string& shaderVersion);
  WebGLShaderPtr _compileRawShader(const std::string& source, const std::string& type);
  unsigned int _getTextureTarget(const InternalTexturePtr& texture) const;
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

  /**
   * Gets or sets a boolean indicating if the engine should validate programs after compilation
   */
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

  /**
   * Gets a boolean indicating that the engine supports uniform buffers
   * @see https://doc.babylonjs.com/features/webgl2#uniform-buffer-objets
   */
  ReadOnlyProperty<ThinEngine, bool> supportsUniformBuffers;

  /** @hidden */
  WebGLRenderingContext* _gl = nullptr;

  /** @hidden */
  float _webGLVersion = 1.f;

  /** @hidden */
  ReadOnlyProperty<ThinEngine, bool> _shouldUseHighPrecisionShader;

  /**
   * Gets a boolean indicating that only power of 2 textures are supported
   * Please note that you can still use non power of 2 textures but in this case the engine will
   * forcefully convert them
   */
  ReadOnlyProperty<ThinEngine, bool> needPOTTextures;

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
   * Gets or sets a boolean indicating if resources should be retained to be able to handle context
   * lost events
   * @see https://doc.babylonjs.com/how_to/optimizing_your_scene#handling-webgl-context-lost
   */
  Property<ThinEngine, bool> doNotHandleContextLost;

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
  WebGLFramebufferPtr _currentFramebuffer = nullptr;
  /** @hidden */
  WebGLFramebufferPtr _dummyFramebuffer = nullptr;
  /** @hidden */
  ICanvas* _workingCanvas = nullptr;
  /** @hidden */
  ICanvasRenderingContext2D* _workingContext = nullptr;

  /** @hidden */
  int _frameHandler = -1;

  /** @hidden */
  std::function<std::string(const std::string& url)> _transformTextureUrl = nullptr;

  /**
   * Gets information about the current host
   */
  HostInformation hostInformation;

  /**
   * sets the object from which width and height will be taken from when getting render width and
   * height Will fallback to the gl object
   * @param dimensions the framebuffer width and height that will be used.
   */
  WriteOnlyProperty<ThinEngine, std::optional<FramebufferDimensionsObject>>
    framebufferDimensionsObject;

  /**
   * Gets the current viewport
   */
  ReadOnlyProperty<ThinEngine, std::optional<Viewport>> currentViewport;

  /**
   * Gets the default empty texture
   */
  ReadOnlyProperty<ThinEngine, InternalTexturePtr> emptyTexture;

  /**
   * Gets the default empty 3D texture
   */
  ReadOnlyProperty<ThinEngine, InternalTexturePtr> emptyTexture3D;

  /**
   * Gets the default empty 2D array texture
   */
  ReadOnlyProperty<ThinEngine, InternalTexturePtr> emptyTexture2DArray;

  /**
   * Gets the default empty cube texture
   */
  ReadOnlyProperty<ThinEngine, InternalTexturePtr> emptyCubeTexture;

  /**
   * Defines whether the engine has been created with the premultipliedAlpha option on or not.
   */
  const bool premultipliedAlpha = true;

  /**
   * Observable event triggered before each texture is initialized
   */
  Observable<Texture> onBeforeTextureInitObservable;

  /**
   * Gets version of the current webGL context
   */
  ReadOnlyProperty<ThinEngine, float> webGLVersion;

  /**
   * Returns true if the stencil buffer has been enabled through the creation option of the context.
   */
  ReadOnlyProperty<ThinEngine, bool> isStencilEnable;

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

  /**
   * Stores the previous frameBuffer during calls to bindFrameBuffer(), so that unBindFrameBuffer
   * can restore it
   */
  std::unique_ptr<GL::IGLFramebuffer> _previousFrameBuffer = nullptr;

  /**
   * You must have a non-zero Vertex Array Object bound in an OpenGL 3.1 context without the
   * extension GL_ARB_compatibility or a core profile context
   */
  WebGLVertexArrayObjectPtr _coreContextVAO = nullptr;

protected:
  ICanvas* _renderingCanvas = nullptr;
  bool _windowIsBackground  = false;
  EngineOptions _creationOptions;

  bool _highPrecisionShadersAllowed = true;
  bool _renderingQueueLaunched      = false;
  std::vector<SA::delegate<void()>> _activeRenderLoops;
  bool _contextWasLost = false;

  // States
  /** @hidden */
  bool _colorWrite = true;
  /** @hidden */
  bool _colorWriteChanged = true;
  /** @hidden */
  std::unique_ptr<DepthCullingState> _depthCullingState;
  /** @hidden */
  std::unique_ptr<StencilState> _stencilState;
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
  WebGLDataBufferPtr _cachedVertexBuffers = nullptr;
  /** @hidden */
  std::unordered_map<std::string, VertexBufferPtr> _cachedVertexBuffersMap;
  /** @hidden */
  WebGLDataBufferPtr _cachedIndexBuffer = nullptr;
  /** @hidden */
  EffectPtr _cachedEffectForVertexBuffers = nullptr;
  /** @hidden */
  std::unordered_map<int, WebGLDataBufferPtr> _currentBoundBuffer;
  /** @hidden */
  ReadOnlyProperty<ThinEngine, bool> _supportsHardwareTextureRescaling;

  /** @hidden */
  std::unordered_map<int, WebGLUniformLocationPtr> _boundUniforms;

private:
  float _hardwareScalingLevel = 1.f;
  bool _isStencilEnable       = false;

  std::string _glVersion;
  std::string _glRenderer;
  std::string _glVendor;

  std::function<void(Event&& evt)> _onContextLost     = nullptr;
  std::function<void(Event&& evt)> _onContextRestored = nullptr;

  int _currentTextureChannel = -1;

  std::unordered_map<std::string, EffectPtr> _compiledEffects;
  std::unordered_map<unsigned int, bool> _vertexAttribArraysEnabled;
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

  std::optional<FramebufferDimensionsObject> _framebufferDimensionsObject = std::nullopt;

  Vector4 _viewportCached;

  std::optional<bool> _unpackFlipYCached = std::nullopt;

  /** Extensions */
  std::unique_ptr<AlphaExtension> _alphaExtension;
  std::unique_ptr<CubeTextureExtension> _cubeTextureExtension;
  std::unique_ptr<DynamicBufferExtension> _dynamicBufferExtension;
  std::unique_ptr<DynamicTextureExtension> _dynamicTextureExtension;
  std::unique_ptr<MultiRenderExtension> _multiRenderExtension;
  std::unique_ptr<RawTextureExtension> _rawTextureExtension;
  std::unique_ptr<ReadTextureExtension> _readTextureExtension;
  std::unique_ptr<RenderTargetExtension> _renderTargetExtension;
  std::unique_ptr<RenderTargetCubeExtension> _renderTargetCubeExtension;
  std::unique_ptr<UniformBufferExtension> _uniformBufferExtension;

  // Friend classes
  friend class DynamicBufferExtension;
  friend class ReadTextureExtension;
  friend class RenderTargetExtension;

}; // end of class ThinEngine

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_THIN_ENGINE_H
