#ifndef BABYLON_ENGINE_ENGINE_H
#define BABYLON_ENGINE_ENGINE_H

#include <future>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/core/delegates/delegate.h>
#include <babylon/core/structs.h>
#include <babylon/engine/engine_capabilities.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/engine/engine_options.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/math/size.h>
#include <babylon/math/vector4.h>
#include <babylon/math/viewport.h>
#include <babylon/mesh/buffer_pointer.h>
#include <babylon/tools/ifile_request.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/perf_counter.h>
#include <babylon/tools/performance_monitor.h>

namespace BABYLON {

class _AlphaState;
class _DepthCullingState;
class _StencilState;
struct _TimeToken;
struct ArrayBufferView;
class AudioEngine;
class BaseTexture;
class Camera;
class Color3;
class Color4;
struct CubeTextureData;
struct DepthTextureCreationOptions;
struct DummyInternalTextureTracker;
class Effect;
struct EffectCreationOptions;
class EffectFallbacks;
class ICanvasRenderingContext2D;
struct IInternalTextureLoader;
struct IInternalTextureTracker;
class ILoadingScreen;
struct InstancingAttributeInfo;
struct IMultiRenderTargetOptions;
class InternalTexture;
struct IRenderTargetOptions;
class Material;
class PassPostProcess;
class PostProcess;
class ProgressEvent;
using RenderTargetCreationOptions = IRenderTargetOptions;
class RenderTargetTexture;
class Scene;
class Texture;
class UniformBuffer;
class VertexBuffer;
using BaseTexturePtr = std::shared_ptr<BaseTexture>;
using DummyInternalTextureTrackerPtr
  = std::shared_ptr<DummyInternalTextureTracker>;
using IInternalTextureLoaderPtr  = std::shared_ptr<IInternalTextureLoader>;
using IInternalTextureTrackerPtr = std::shared_ptr<IInternalTextureTracker>;
using InternalTexturePtr         = std::shared_ptr<InternalTexture>;
using PassPostProcessPtr         = std::shared_ptr<PassPostProcess>;
using PostProcessPtr             = std::shared_ptr<PostProcess>;
using RenderTargetTexturePtr     = std::shared_ptr<RenderTargetTexture>;

/**
 * @brief The engine class is responsible for interfacing with all lower-level
 * APIs such as WebGL and Audio.
 */
class BABYLON_SHARED_EXPORT Engine {

public:
  using ArrayBufferViewArray   = std::vector<ArrayBufferView>;
  using GLBufferPtr            = std::unique_ptr<GL::IGLBuffer>;
  using GLFrameBufferPtr       = std::unique_ptr<GL::IGLFramebuffer>;
  using GLProgramPtr           = std::unique_ptr<GL::IGLProgram>;
  using GLQueryPtr             = std::unique_ptr<GL::IGLQuery>;
  using GLRenderBufferPtr      = std::unique_ptr<GL::IGLRenderbuffer>;
  using GLShaderPtr            = std::unique_ptr<GL::IGLShader>;
  using GLTexturePtr           = std::unique_ptr<GL::IGLTexture>;
  using GLTransformFeedbackPtr = std::unique_ptr<GL::IGLTransformFeedback>;
  using GLUniformLocationPtr   = std::unique_ptr<GL::IGLUniformLocation>;
  using GLVertexArrayObjectPtr = std::unique_ptr<GL::IGLVertexArrayObject>;

  friend class BaseTexture;

public:
  static std::string Version();

  // Updatable statics so stick with vars here

  /**
   * Gets or sets the epsilon value used by collision engine
   */
  static float CollisionsEpsilon;

  /**
   * Gets or sets the relative url used to load code if using the engine in
   * non-minified mode
   */
  static std::string CodeRepository;

  /**
   * Gets or sets the relative url used to load shaders if using the engine in
   * non-minified mode
   */
  static std::string ShadersRepository;

  // Engine instances

  /**
   * Gets the list of created engines
   */
  static std::vector<Engine*> Instances;

public:
  template <typename... Ts>
  static std::unique_ptr<Engine> New(Ts&&... args)
  {
    std::unique_ptr<Engine> engine(new Engine(std::forward<Ts>(args)...));
    return engine;
  }
  virtual ~Engine();

  /**
   * @brief Gets the list of texture formats supported.
   */
  std::vector<std::string>& texturesSupported();

  /**
   * @brief Gets the list of texture formats in use.
   */
  std::string textureFormatInUse() const;

  /**
   * @brief Gets the current viewport.
   */
  Viewport* currentViewport() const;

  /**
   * @brief Gets the default empty texture.
   */
  InternalTexturePtr& emptyTexture();

  /**
   * @brief Gets the default empty 3D texture.
   */
  InternalTexturePtr& emptyTexture3D();

  /**
   * @brief Gets the default empty cube texture.
   */
  InternalTexturePtr& emptyCubeTexture();

  /**
   * @brief Gets version of the current webGL context.
   */
  float webGLVersion() const;

  /**
   * @brief Gets a boolean indicating that the engine is currently in VR
   * exclusive mode for the pointers.
   * @see
   * https://docs.microsoft.com/en-us/microsoft-edge/webvr/essentials#mouse-input
   */
  bool isInVRExclusivePointerMode() const;

  /**
   * @brief Gets a boolean indicating that the engine supports uniform buffers.
   * @see http://doc.babylonjs.com/features/webgl2#uniform-buffer-objets
   */
  bool supportsUniformBuffers() const;

  /**
   * @brief Gets a boolean indicating that only power of 2 textures are
   * supported. Please note that you can still use non power of 2 textures but
   * in this case the engine will forcefully convert them
   */
  bool needPOTTextures() const;

  /**
   * @brief Gets a boolean indicating if resources should be retained to be able
   * to handle context lost events
   * @see
   * http://doc.babylonjs.com/how_to/optimizing_your_scene#handling-webgl-context-lost
   */
  bool doNotHandleContextLost() const;

  /**
   * @brief Sets a boolean indicating if resources should be retained to be able
   * to handle context lost events
   * @see
   * http://doc.babylonjs.com/how_to/optimizing_your_scene#handling-webgl-context-lost
   */
  void setDoNotHandleContextLost(bool value);

  /**
   * @brief Gets the performance monitor attached to this engine.
   * @see
   * http://doc.babylonjs.com/how_to/optimizing_your_scene#engineinstrumentation
   */
  PerformanceMonitor* performanceMonitor() const;

  /**
   * @brief Returns true if the stencil buffer has been enabled through the
   * creation option of the context.
   */
  bool isStencilEnable() const;

  /**
   * @brief Reset the texture cache to empty state.
   */
  void resetTextureCache();

  /**
   * @brief Gets a boolean indicating that the engine is running in
   * deterministic lock step mode.
   * @see http://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   * @returns true if engine is in deterministic lock step mode
   */
  bool isDeterministicLockStep() const;

  /**
   * @brief Gets the max steps when engine is running in deterministic lock
   * step.
   * @see http://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   * @returns the max steps
   */
  unsigned int getLockstepMaxSteps() const;

  /**
   * @brief Gets an object containing information about the current webGL
   * context.
   * @returns an object containing the vender, the renderer and the version of
   * the current webGL context
   */
  GL::GLInfo getGlInfo();

  /**
   * @brief Gets current aspect ratio.
   * @param camera defines the camera to use to get the aspect ratio
   * @param useScreen defines if screen size must be used (or the current render
   * target if any)
   * @returns a number defining the aspect ratio
   */
  float getAspectRatio(const Camera& camera, bool useScreen = false);

  /**
   * @brief Gets current screen aspect ratio.
   * @returns a number defining the aspect ratio
   */
  float getScreenAspectRatio() const;

  /**
   * @brief Gets the current render width.
   * @param useScreen defines if screen size must be used (or the current render
   * target if any)
   * @returns a number defining the current render width
   */
  int getRenderWidth(bool useScreen = false) const;

  /**
   * @brief Gets the current render height.
   * @param useScreen defines if screen size must be used (or the current render
   * target if any)
   * @returns a number defining the current render height
   */
  int getRenderHeight(bool useScreen = false) const;

  /**
   * @brief Gets the HTML canvas attached with the current webGL context.
   * @returns a HTML canvas
   */
  ICanvas* getRenderingCanvas();

  /**
   * @brief Gets the client rect of the HTML canvas attached with the current
   * webGL context.
   * @returns a client rectanglee
   */
  std::optional<ClientRect> getRenderingCanvasClientRect();

  /**
   * @brief Defines the hardware scaling level.
   * By default the hardware scaling level is computed from the window device
   * ratio. if level = 1 then the engine will render at the exact resolution of
   * the canvas. If level = 0.5 then the engine will render at twice the size of
   * the canvas.
   * @param level defines the level to use
   */
  void setHardwareScalingLevel(int level);

  /**
   * @brief Gets the current hardware scaling level.
   * By default the hardware scaling level is computed from the window device
   * ratio. if level = 1 then the engine will render at the exact resolution of
   * the canvas. If level = 0.5 then the engine will render at twice the size of
   * the canvas.
   * @returns a number indicating the current hardware scaling level
   */
  int getHardwareScalingLevel() const;

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
   * @brief Hidden
   */
  size_t drawCalls() const;

  /**
   * @brief Hidden
   */
  std::optional<PerfCounter> drawCallsPerfCounter();

  /** Methods **/

  /**
   * @brief Backup the GL state.
   */
  void backupGLState();

  /**
   * @brief Restore the GL state.
   */
  void restoreGLState();

  /**
   * @brief Gets the current depth function.
   * @returns a number defining the depth function
   */
  int getDepthFunction() const;

  /**
   * @brief Sets the current depth function.
   * @param depthFunc defines the function to use
   */
  void setDepthFunction(int depthFunc);

  /**
   * @brief Sets the current depth function to GREATER.
   */
  void setDepthFunctionToGreater();

  /**
   * @brief Sets the current depth function to GEQUAL.
   */
  void setDepthFunctionToGreaterOrEqual();

  /**
   * @brief Sets the current depth function to LESS.
   */
  void setDepthFunctionToLess();

  /**
   * @brief Sets the current depth function to LEQUAL.
   */
  void setDepthFunctionToLessOrEqual();

  /**
   * @brief Gets a boolean indicating if stencil buffer is enabled.
   * @returns the current stencil buffer state
   */
  bool getStencilBuffer() const;

  /**
   * @brief Enable or disable the stencil buffer.
   * @param enable defines if the stencil buffer must be enabled or disabled
   */
  void setStencilBuffer(bool enable);

  /**
   * @brief Gets the current stencil mask.
   * @returns a number defining the new stencil mask to use
   */
  unsigned int getStencilMask() const;

  /**
   * @brief Sets the current stencil mask.
   * @param mask defines the new stencil mask to use
   */
  void setStencilMask(unsigned int mask);

  /**
   * @brief Gets the current stencil function.
   * @returns a number defining the stencil function to use
   */
  unsigned int getStencilFunction() const;

  /**
   * @brief Gets the current stencil reference value.
   * @returns a number defining the stencil reference value to use
   */
  int getStencilFunctionReference() const;

  /**
   * @brief Gets the current stencil mask.
   * @returns a number defining the stencil mask to use
   */
  unsigned int getStencilFunctionMask() const;

  /**
   * @brief Sets the current stencil function.
   * @param stencilFunc defines the new stencil function to use
   */
  void setStencilFunction(unsigned int stencilFunc);

  /**
   * @brief Sets the current stencil reference.
   * @param reference defines the new stencil reference to use
   */
  void setStencilFunctionReference(int reference);

  /**
   * @brief Sets the current stencil mask.
   * @param mask defines the new stencil mask to use
   */
  void setStencilFunctionMask(unsigned int mask);

  /**
   * @brief Gets the current stencil operation when stencil fails.
   * @returns a number defining stencil operation to use when stencil fails
   */
  unsigned int getStencilOperationFail() const;

  /**
   * @brief Gets the current stencil operation when depth fails.
   * @returns a number defining stencil operation to use when depth fails
   */
  unsigned int getStencilOperationDepthFail() const;

  /**
   * @brief Gets the current stencil operation when stencil passes.
   * @returns a number defining stencil operation to use when stencil passes
   */
  unsigned int getStencilOperationPass() const;

  /**
   * @brief Sets the stencil operation to use when stencil fails.
   * @param operation defines the stencil operation to use when stencil fails
   */
  void setStencilOperationFail(unsigned int operation);

  /**
   * @brief Sets the stencil operation to use when depth fails.
   * @param operation defines the stencil operation to use when depth fails
   */
  void setStencilOperationDepthFail(unsigned int operation);

  /**
   * @brief Sets the stencil operation to use when stencil passes.
   * @param operation defines the stencil operation to use when stencil passes
   */
  void setStencilOperationPass(unsigned int operation);

  /**
   * @brief Sets a boolean indicating if the dithering state is enabled or
   * disabled.
   * @param value defines the dithering state
   */
  void setDitheringState(bool value);

  /**
   * @brief Sets a boolean indicating if the rasterizer state is enabled or
   * disabled.
   * @param value defines the rasterizer state
   */
  void setRasterizerState(bool value);

  /**
   * @brief Stop executing a render loop function and remove it from the
   * execution array.
   */
  void stopRenderLoop();

  /**
   * @brief Stop executing a render loop function and remove it from the
   * execution array.
   * @param renderFunction defines the function to be removed. If not provided
   * all functions will be removed.
   */
  void stopRenderLoop(const SA::delegate<void()>& renderFunction);

  /**
   * @brief Hidden
   */
  void _renderLoop();

  /**
   * @brief Register and execute a render loop. The engine can have more than
   * one render function.
   * @param renderFunction defines the function to continuously execute
   */
  void runRenderLoop(const std::function<void()>& renderFunction);
  void renderFunction(const std::function<void()>& renderFunction);

  /**
   * @brief Toggle full screen mode.
   * @param requestPointerLock defines if a pointer lock should be requested
   * from the user
   * @param options defines an option object to be sent to the requestFullscreen
   * function
   */
  void switchFullscreen(bool requestPointerLock);

  /**
   * @brief Clear the current render buffer or the current render target (if any
   * is set up).
   * @param depth defines if the depth buffer must be cleared
   * @param stencil defines if the stencil buffer must be cleared
   */
  void clear(bool depth, bool stencil = false);

  /**
   * @brief Clear the current render buffer or the current render target (if any
   * is set up).
   * @param color defines the color to use
   * @param backBuffer defines if the back buffer must be cleared
   * @param depth defines if the depth buffer must be cleared
   * @param stencil defines if the stencil buffer must be cleared
   */
  void clear(const Color4& color, bool backBuffer, bool depth,
             bool stencil = false);

  /**
   * @brief Executes a scissor clear (ie. a clear on a specific portion of the
   * screen).
   * @param x defines the x-coordinate of the top left corner of the clear
   * rectangle
   * @param y defines the y-coordinate of the corner of the clear rectangle
   * @param width defines the width of the clear rectangle
   * @param height defines the height of the clear rectangle
   * @param clearColor defines the clear color
   */
  void scissorClear(int x, int y, int width, int height,
                    const Color4& clearColor);

  /**
   * @brief Hidden
   */
  void _viewport(float x, float y, float width, float height);

  /**
   * @brief Set the WebGL's viewport.
   * @param viewport defines the viewport element to be used
   * @param requiredWidth defines the width required for rendering. If not
   * provided the rendering canvas' width is used
   * @param requiredHeight defines the height required for rendering. If not
   * provided the rendering canvas' height is used
   */
  void setViewport(Viewport& viewport, int requiredWidth = 0,
                   int requiredHeight = 0);

  /**
   * @brief Directly set the WebGL Viewport.
   * @param x defines the x coordinate of the viewport (in screen space)
   * @param y defines the y coordinate of the viewport (in screen space)
   * @param width defines the width of the viewport (in screen space)
   * @param height defines the height of the viewport (in screen space)
   * @return the current viewport Object (if any) that is being replaced by this
   * call. You can restore this viewport later on to go back to the original
   * state
   */
  Viewport& setDirectViewport(int x, int y, int width, int height);

  /**
   * @brief Begin a new frame.
   */
  void beginFrame();

  /**
   * @brief End the current frame.
   */
  void endFrame();

  /**
   * @brief Resize the view according to the canvas' size.
   */
  void resize();

  /**
   * @brief Force a specific size of the canvas.
   * @param width defines the new canvas' width
   * @param height defines the new canvas' height
   */
  void setSize(int width, int height);

  /**
   * @brief Binds the frame buffer to the specified texture.
   * @param texture The texture to render to or null for the default canvas
   * @param faceIndex The face of the texture to render to in case of cube
   * texture
   * @param requiredWidth The width of the target to render to
   * @param requiredHeight The height of the target to render to
   * @param forceFullscreenViewport Forces the viewport to be the entire
   * texture/screen if true
   * @param depthStencilTexture The depth stencil texture to use to render
   * @param lodLevel defines le lod level to bind to the frame buffer
   */
  void bindFramebuffer(const InternalTexturePtr& texture,
                       std::optional<unsigned int> faceIndex = std::nullopt,
                       std::optional<int> requiredWidth      = std::nullopt,
                       std::optional<int> requiredHeight     = std::nullopt,
                       std::optional<bool> forceFullscreenViewport
                       = std::nullopt,
                       InternalTexture* depthStencilTexture = nullptr,
                       int lodLevel                         = 0);

  /**
   * @brief Unbind the current render target texture from the webGL context.
   * @param texture defines the render target texture to unbind
   * @param disableGenerateMipMaps defines a boolean indicating that mipmaps
   * must not be generated
   * @param onBeforeUnbind defines a function which will be called before the
   * effective unbind
   */
  void unBindFramebuffer(const InternalTexturePtr& texture,
                         bool disableGenerateMipMaps                 = false,
                         const std::function<void()>& onBeforeUnbind = nullptr);

  /**
   * @brief Unbind a list of render target textures from the webGL context.
   * This is used only when drawBuffer extension or webGL2 are active
   * @param textures defines the render target textures to unbind
   * @param disableGenerateMipMaps defines a boolean indicating that mipmaps
   * must not be generated
   * @param onBeforeUnbind defines a function which will be called before the
   * effective unbind
   */
  void unBindMultiColorAttachmentFramebuffer(
    const std::vector<InternalTexturePtr>& textures,
    bool disableGenerateMipMaps                 = false,
    const std::function<void()>& onBeforeUnbind = nullptr);

  /**
   * @brief Force the mipmap generation for the given render target texture.
   * @param texture defines the render target texture to use
   */
  void generateMipMapsForCubemap(const InternalTexturePtr& texture);

  /**
   * @brief Force a webGL flush (ie. a flush of all waiting webGL commands).
   */
  void flushFramebuffer();

  /**
   * @brief Unbind the current render target and bind the default framebuffer.
   */
  void restoreDefaultFramebuffer();

  /** UBOs **/

  /**
   * @brief Create an uniform buffer.
   * @see http://doc.babylonjs.com/features/webgl2#uniform-buffer-objets
   * @param elements defines the content of the uniform buffer
   * @returns the webGL uniform buffer
   */
  GLBufferPtr createUniformBuffer(const Float32Array& elements);

  /**
   * @brief Create a dynamic uniform buffer.
   * @see http://doc.babylonjs.com/features/webgl2#uniform-buffer-objets
   * @param elements defines the content of the uniform buffer
   * @returns the webGL uniform buffer
   */
  GLBufferPtr createDynamicUniformBuffer(const Float32Array& elements);

  /**
   * @brief Update an existing uniform buffer.
   * @see http://doc.babylonjs.com/features/webgl2#uniform-buffer-objets
   * @param uniformBuffer defines the target uniform buffer
   * @param elements defines the content to update
   * @param offset defines the offset in the uniform buffer where update should
   * start
   * @param count defines the size of the data to update
   */
  void updateUniformBuffer(GL::IGLBuffer* uniformBuffer,
                           const Float32Array& elements, int offset = -1,
                           int count = -1);

  /** VBOs **/

  /**
   * @brief Creates a vertex buffer.
   * @param data the data for the vertex buffer
   * @returns the new WebGL static buffer
   */
  GLBufferPtr createVertexBuffer(const Float32Array& vertices);

  /**
   * @brief Creates a dynamic vertex buffer.
   * @param data the data for the dynamic vertex buffer
   * @returns the new WebGL dynamic buffer
   */
  GLBufferPtr createDynamicVertexBuffer(const Float32Array& vertices);

  /**
   * @brief Update a dynamic index buffer.
   * @param indexBuffer defines the target index buffer
   * @param indices defines the data to update
   * @param offset defines the offset in the target index buffer where update
   * should start
   */
  void updateDynamicIndexBuffer(const GLBufferPtr& indexBuffer,
                                const IndicesArray& indices, int offset = 0);

  /**
   * @brief Updates a dynamic vertex buffer.
   * @param vertexBuffer the vertex buffer to update
   * @param data the data used to update the vertex buffer
   * @param byteOffset the byte offset of the data
   * @param byteLength the byte length of the data
   */
  void updateDynamicVertexBuffer(const GLBufferPtr& vertexBuffer,
                                 const Float32Array& data, int byteOffset = -1,
                                 int byteLength = -1);

  /**
   * @brief Creates a new index buffer.
   * @param indices defines the content of the index buffer
   * @param updatable defines if the index buffer must be updatable
   * @returns a new webGL buffer
   */
  GLBufferPtr createIndexBuffer(const IndicesArray& indices,
                                bool updatable = false);

  /**
   * @brief Bind a webGL buffer to the webGL context.
   * @param buffer defines the buffer to bind
   */
  void bindArrayBuffer(GL::IGLBuffer* buffer);

  /**
   * @brief Bind an uniform buffer to the current webGL context.
   * @param buffer defines the buffer to bind
   */
  void bindUniformBuffer(GL::IGLBuffer* buffer);

  /**
   * @brief Bind a buffer to the current webGL context at a given location.
   * @param buffer defines the buffer to bind
   * @param location defines the index where to bind the buffer
   */
  void bindUniformBufferBase(GL::IGLBuffer* buffer, unsigned int location);

  /**
   * @brief Bind a specific block at a given index in a specific shader program.
   * @param shaderProgram defines the shader program
   * @param blockName defines the block name
   * @param index defines the index where to bind the block
   */
  void bindUniformBlock(GL::IGLProgram* shaderProgram,
                        const std::string blockName, unsigned int index);

  /**
   * @brief Update the bound buffer with the given data.
   * @param data defines the data to update
   */
  void updateArrayBuffer(const Float32Array& data);

  /**
   * @brief Records a vertex array object.
   * @see http://doc.babylonjs.com/features/webgl2#vertex-array-objects
   * @param vertexBuffers defines the list of vertex buffers to store
   * @param indexBuffer defines the index buffer to store
   * @param effect defines the effect to store
   * @returns the new vertex array object
   */
  GLVertexArrayObjectPtr recordVertexArrayObject(
    const std::unordered_map<std::string, VertexBuffer*>& vertexBuffers,
    GL::IGLBuffer* indexBuffer, Effect* effect);

  /**
   * @brief Bind a specific vertex array object.
   * @see http://doc.babylonjs.com/features/webgl2#vertex-array-objects
   * @param vertexArrayObject defines the vertex array object to bind
   * @param indexBuffer defines the index buffer to bind
   */
  void bindVertexArrayObject(GL::IGLVertexArrayObject* vertexArrayObject,
                             GL::IGLBuffer* indexBuffer);

  /**
   * @brief Bind webGl buffers directly to the webGL context.
   * @param vertexBuffer defines the vertex buffer to bind
   * @param indexBuffer defines the index buffer to bind
   * @param vertexDeclaration defines the vertex declaration to use with the
   * vertex buffer
   * @param vertexStrideSize defines the vertex stride of the vertex buffer
   * @param effect defines the effect associated with the vertex buffer
   */
  void bindBuffersDirectly(GL::IGLBuffer* vertexBuffer,
                           GL::IGLBuffer* indexBuffer,
                           const Float32Array& vertexDeclaration,
                           int vertexStrideSize = 3, Effect* effect = nullptr);

  /**
   * @brief Bind a list of vertex buffers to the webGL context.
   * @param vertexBuffers defines the list of vertex buffers to bind
   * @param indexBuffer defines the index buffer to bind
   * @param effect defines the effect associated with the vertex buffers
   */
  void bindBuffers(
    const std::unordered_map<std::string, VertexBuffer*>& vertexBuffers,
    GL::IGLBuffer* indexBuffer, Effect* effect);

  /**
   * @brief Unbind all instance attributes.
   */
  void unbindInstanceAttributes();

  /**
   * @brief Release and free the memory of a vertex array object.
   * @param vao defines the vertex array object to delete
   */
  void releaseVertexArrayObject(GL::IGLVertexArrayObject* vao);

  /**
   * @brief Hidden
   */
  bool _releaseBuffer(GL::IGLBuffer* buffer);

  /**
   * @brief Creates a webGL buffer to use with instanciation.
   * @param capacity defines the size of the buffer
   * @returns the webGL buffer
   */
  GLBufferPtr createInstancesBuffer(unsigned int capacity);

  /**
   * @brief Delete a webGL buffer used with instanciation.
   * @param buffer defines the webGL buffer to delete
   */
  void deleteInstancesBuffer(GL::IGLBuffer* buffer);

  /**
   * @brief Update the content of a webGL buffer used with instanciation and
   * bind it to the webGL context.
   * @param instancesBuffer defines the webGL buffer to update and bind
   * @param data defines the data to store in the buffer
   * @param offsetLocations defines the offsets or attributes information used
   * to determine where data must be stored in the buffer
   */
  void updateAndBindInstancesBuffer(GL::IGLBuffer* instancesBuffer,
                                    const Float32Array& data,
                                    const Uint32Array& offsetLocations);

  /**
   * @brief Update the content of a webGL buffer used with instanciation and
   * bind it to the webGL context.
   * @param instancesBuffer defines the webGL buffer to update and bind
   * @param data defines the data to store in the buffer
   * @param offsetLocations defines the offsets or attributes information used
   * to determine where data must be stored in the buffer
   */
  void updateAndBindInstancesBuffer(
    GL::IGLBuffer* instancesBuffer, const Float32Array& data,
    const std::vector<InstancingAttributeInfo>& offsetLocations);

  /**
   * @brief Apply all cached states (depth, culling, stencil and alpha).
   */
  void applyStates();

  /**
   * @brief Send a draw order.
   * @param useTriangles defines if triangles must be used to draw (else
   * wireframe will be used)
   * @param indexStart defines the starting index
   * @param indexCount defines the number of index to draw
   * @param instancesCount defines the number of instances to draw (if
   * instanciation is enabled)
   */
  void draw(bool useTriangles, int indexStart, int indexCount,
            int instancesCount = 0);

  /**
   * @brief Draw a list of points.
   * @param verticesStart defines the index of first vertex to draw
   * @param verticesCount defines the count of vertices to draw
   * @param instancesCount defines the number of instances to draw (if
   * instanciation is enabled)
   */
  void drawPointClouds(int verticesStart, int verticesCount,
                       int instancesCount = 0);

  /**
   * @brief Draw a list of unindexed primitives.
   * @param useTriangles defines if triangles must be used to draw (else
   * wireframe will be used)
   * @param verticesStart defines the index of first vertex to draw
   * @param verticesCount defines the count of vertices to draw
   * @param instancesCount defines the number of instances to draw (if
   * instanciation is enabled)
   */
  void drawUnIndexed(bool useTriangles, int verticesStart, int verticesCount,
                     int instancesCount = 0);

  /**
   * @brief Draw a list of indexed primitives.
   * @param fillMode defines the primitive to use
   * @param indexStart defines the starting index
   * @param indexCount defines the number of index to draw
   * @param instancesCount defines the number of instances to draw (if
   * instanciation is enabled)
   */
  void drawElementsType(unsigned int fillMode, int indexStart,
                        int verticesCount, int instancesCount = 0);

  /**
   * @brief Draw a list of unindexed primitives.
   * @param fillMode defines the primitive to use
   * @param verticesStart defines the index of first vertex to draw
   * @param verticesCount defines the count of vertices to draw
   * @param instancesCount defines the number of instances to draw (if
   * instanciation is enabled)
   */
  void drawArraysType(unsigned int fillMode, int verticesStart,
                      int verticesCount, int instancesCount = 0);

  /** Shaders **/

  /**
   * @brief Hidden
   */
  void _releaseEffect(Effect* effect);

  /**
   * @brief Hidden
   */
  void _deleteProgram(GL::IGLProgram* program);

  /**
   * @brief Create a new effect (used to store vertex/fragment shaders).
   * @param baseName defines the base name of the effect (The name of file
   * without .fragment.fx or .vertex.fx)
   * @param attributesNamesOrOptions defines either a list of attribute names or
   * an EffectCreationOptions object
   * @param uniformsNamesOrEngine defines either a list of uniform names or the
   * engine to use
   * @param samplers defines an array of string used to represent textures
   * @param defines defines the string containing the defines to use to compile
   * the shaders
   * @param fallbacks defines the list of potential fallbacks to use if shader
   * conmpilation fails
   * @param onCompiled defines a function to call when the effect creation is
   * successful
   * @param onError defines a function to call when the effect creation has
   * failed
   * @param indexParameters defines an object containing the index values to use
   * to compile shaders (like the maximum number of simultaneous lights)
   * @returns the new Effect
   */
  Effect* createEffect(const std::string& baseName,
                       EffectCreationOptions& options, Engine* engine,
                       const std::function<void(Effect* effect)>& onCompiled
                       = nullptr);

  /**
   * @brief Create a new effect (used to store vertex/fragment shaders).
   * @param baseName defines the base name of the effect (The name of file
   * without .fragment.fx or .vertex.fx)
   * @param attributesNamesOrOptions defines either a list of attribute names or
   * an EffectCreationOptions object
   * @param uniformsNamesOrEngine defines either a list of uniform names or the
   * engine to use
   * @param samplers defines an array of string used to represent textures
   * @param defines defines the string containing the defines to use to compile
   * the shaders
   * @param fallbacks defines the list of potential fallbacks to use if shader
   * conmpilation fails
   * @param onCompiled defines a function to call when the effect creation is
   * successful
   * @param onError defines a function to call when the effect creation has
   * failed
   * @param indexParameters defines an object containing the index values to use
   * to compile shaders (like the maximum number of simultaneous lights)
   * @returns the new Effect
   */
  Effect* createEffect(std::unordered_map<std::string, std::string>& baseName,
                       EffectCreationOptions& options, Engine* engine);

  /**
   * @brief Directly creates a webGL program.
   * @param vertexCode defines the vertex shader code to use
   * @param fragmentCode defines the fragment shader code to use
   * @param context defines the webGL context to use (if not set, the current
   * one will be used)
   * @param transformFeedbackVaryings defines the list of transform feedback
   * varyings to use
   * @returns the new webGL program
   */
  GLProgramPtr createRawShaderProgram(
    const std::string& vertexCode, const std::string& fragmentCode,
    GL::IGLRenderingContext* context                          = nullptr,
    const std::vector<std::string>& transformFeedbackVaryings = {});

  /**
   * @brief Creates a webGL program.
   * @param vertexCode  defines the vertex shader code to use
   * @param fragmentCode defines the fragment shader code to use
   * @param defines defines the string containing the defines to use to compile
   * the shaders
   * @param context defines the webGL context to use (if not set, the current
   * one will be used)
   * @param transformFeedbackVaryings defines the list of transform feedback
   * varyings to use
   * @returns the new webGL program
   */
  GLProgramPtr createShaderProgram(
    const std::string& vertexCode, const std::string& fragmentCode,
    const std::string& defines, GL::IGLRenderingContext* context = nullptr,
    const std::vector<std::string>& transformFeedbackVaryings = {});

  /**
   * @brief Gets the list of webGL uniform locations associated with a specific
   * program based on a list of uniform names.
   * @param shaderProgram defines the webGL program to use
   * @param uniformsNames defines the list of uniform names
   * @returns an array of webGL uniform locations
   */
  std::unordered_map<std::string, GLUniformLocationPtr>
  getUniforms(GL::IGLProgram* shaderProgram,
              const std::vector<std::string>& uniformsNames);

  /**
   * @brief Gets the lsit of active attributes for a given webGL program.
   * @param shaderProgram defines the webGL program to use
   * @param attributesNames defines the list of attribute names to get
   * @returns an array of indices indicating the offset of each attribute
   */
  Int32Array getAttributes(GL::IGLProgram* shaderProgram,
                           const std::vector<std::string>& attributesNames);

  /**
   * @brief Activates an effect, mkaing it the current one (ie. the one used for
   * rendering).
   * @param effect defines the effect to activate
   */
  void enableEffect(Effect* effect);

  /**
   * @brief Set the value of an uniform to an array of int32.
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   */
  void setIntArray(GL::IGLUniformLocation* uniform, const Int32Array& array);

  /**
   * @brief Set the value of an uniform to an array of int32 (stored as vec2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   */
  void setIntArray2(GL::IGLUniformLocation* uniform, const Int32Array& array);

  /**
   * @brief Set the value of an uniform to an array of int32 (stored as vec3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   */
  void setIntArray3(GL::IGLUniformLocation* uniform, const Int32Array& array);

  /**
   * @brief Set the value of an uniform to an array of int32 (stored as vec4).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   */
  void setIntArray4(GL::IGLUniformLocation* uniform, const Int32Array& array);

  /**
   * @brief Set the value of an uniform to an array of float32.
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of float32 to store
   */
  void setFloatArray(GL::IGLUniformLocation* uniform,
                     const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of float32 (stored as vec2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of float32 to store
   */
  void setFloatArray2(GL::IGLUniformLocation* uniform,
                      const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of float32 (stored as vec3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of float32 to store
   */
  void setFloatArray3(GL::IGLUniformLocation* uniform,
                      const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of float32 (stored as vec4).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of float32 to store
   */
  void setFloatArray4(GL::IGLUniformLocation* uniform,
                      const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of number.
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   */
  void setArray(GL::IGLUniformLocation* uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of number (stored as vec2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   */
  void setArray2(GL::IGLUniformLocation* uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of number (stored as vec3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   */
  void setArray3(GL::IGLUniformLocation* uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of number (stored as vec4).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   */
  void setArray4(GL::IGLUniformLocation* uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of float32 (stored as
   * matrices).
   * @param uniform defines the webGL uniform location where to store the value
   * @param matrices defines the array of float32 to store
   */
  void setMatrices(GL::IGLUniformLocation* uniform,
                   const Float32Array& matrices);

  /**
   * @brief Set the value of an uniform to a matrix.
   * @param uniform defines the webGL uniform location where to store the value
   * @param matrix defines the matrix to store
   */
  void setMatrix(GL::IGLUniformLocation* uniform, const Matrix& matrix);

  /**
   * @brief Set the value of an uniform to a matrix (3x3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param matrix defines the Float32Array representing the 3x3 matrix to store
   */
  void setMatrix3x3(GL::IGLUniformLocation* uniform,
                    const Float32Array& matrix);

  /**
   * @brief Set the value of an uniform to a matrix (2x2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param matrix defines the Float32Array representing the 2x2 matrix to store
   */
  void setMatrix2x2(GL::IGLUniformLocation* uniform,
                    const Float32Array& matrix);

  /**
   * @brief Set the value of an uniform to a number (int).
   * @param uniform defines the webGL uniform location where to store the value
   * @param value defines the int number to store
   */
  void setInt(GL::IGLUniformLocation* uniform, int value);

  /**
   * @brief Set the value of an uniform to a number (float).
   * @param uniform defines the webGL uniform location where to store the value
   * @param value defines the float number to store
   */
  void setFloat(GL::IGLUniformLocation* uniform, float value);

  /**
   * @brief Set the value of an uniform to a vec2.
   * @param uniform defines the webGL uniform location where to store the value
   * @param x defines the 1st component of the value
   * @param y defines the 2nd component of the value
   */
  void setFloat2(GL::IGLUniformLocation* uniform, float x, float y);

  /**
   * @brief Set the value of an uniform to a vec3.
   * @param uniform defines the webGL uniform location where to store the value
   * @param x defines the 1st component of the value
   * @param y defines the 2nd component of the value
   * @param z defines the 3rd component of the value
   */
  void setFloat3(GL::IGLUniformLocation* uniform, float x, float y, float z);

  /**
   * @brief Set the value of an uniform to a vec3.
   * @param uniform defines the webGL uniform location where to store the value
   * @param x defines the 1st component of the value
   * @param y defines the 2nd component of the value
   * @param z defines the 3rd component of the value
   */
  void setBool(GL::IGLUniformLocation* uniform, int value);

  /**
   * @brief Set the value of an uniform to a vec4.
   * @param uniform defines the webGL uniform location where to store the value
   * @param x defines the 1st component of the value
   * @param y defines the 2nd component of the value
   * @param z defines the 3rd component of the value
   * @param w defines the 4th component of the value
   */
  void setFloat4(GL::IGLUniformLocation* uniform, float x, float y, float z,
                 float w);

  /**
   * @brief Set the value of an uniform to a Color3.
   * @param uniform defines the webGL uniform location where to store the value
   * @param color3 defines the color to store
   */
  void setColor3(GL::IGLUniformLocation* uniform, const Color3& color3);

  /**
   * @brief Set the value of an uniform to a Color3 and an alpha value.
   * @param uniform defines the webGL uniform location where to store the value
   * @param color3 defines the color to store
   * @param alpha defines the alpha component to store
   */
  void setColor4(GL::IGLUniformLocation* uniform, const Color3& color3,
                 float alpha);

  /**
   * @brief Sets a Color4 on a uniform variable.
   * @param uniform defines the uniform location
   * @param color4 defines the value to be set
   */
  void setDirectColor4(GL::IGLUniformLocation* uniform, const Color4& color4);

  /** States **/

  /**
   * @brief Set various states to the webGL context.
   * @param culling defines backface culling state
   * @param zOffset defines the value to apply to zOffset (0 by default)
   * @param force defines if states must be applied even if cache is up to date
   * @param reverseSide defines if culling must be reversed (CCW instead of CW
   * and CW instead of CCW)
   */
  void setState(bool culling, float zOffset = 0.f, bool force = false,
                bool reverseSide = false);

  /**
   * @brief Set the z offset to apply to current rendering.
   * @param value defines the offset to apply
   */
  void setZOffset(float value);

  /**
   * @brief Gets the current value of the zOffset.
   * @returns the current zOffset state
   */
  float getZOffset() const;

  /**
   * @brief Enable or disable depth buffering.
   * @param enable defines the state to set
   */
  void setDepthBuffer(bool enable);

  /**
   * @brief Gets a boolean indicating if depth writing is enabled.
   * @returns the current depth writing state
   */
  bool getDepthWrite() const;

  /**
   * @brief Enable or disable depth writing.
   * @param enable defines the state to set
   */
  void setDepthWrite(bool enable);

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
   * @param mode defines the mode to use (one of the BABYLON.Engine.ALPHA_XXX)
   * @param noDepthWriteChange defines if depth writing state should remains
   * unchanged (false by default)
   * @see
   * http://doc.babylonjs.com/resources/transparency_and_how_meshes_are_rendered
   */
  void setAlphaMode(unsigned int mode, bool noDepthWriteChange = false);

  /**
   * @brief Gets the current alpha mode.
   * @see
   * http://doc.babylonjs.com/resources/transparency_and_how_meshes_are_rendered
   * @returns the current alpha mode
   */
  unsigned int getAlphaMode() const;

  /**
   * @brief Gets the current stensil state.
   * @returns the current stensil state
   */
  _StencilState* stencilState();

  /** Textures **/

  /**
   * @brief Clears the list of texture accessible through engine.
   * This can help preventing texture load conflict due to name collision.
   */
  void clearInternalTexturesCache();

  /**
   * @brief Force the entire cache to be cleared.
   * You should not have to use this function unless your engine needs to share
   * the webGL context with another engine
   * @param bruteForce defines a boolean to force clearing ALL caches (including
   * stencil, detoh and alpha states)
   */
  void wipeCaches(bool bruteForce = false);

  /**
   * @brief Set the compressed texture format to use, based on the formats you
   * have, and the formats supported by the hardware / browser.
   *
   * Khronos Texture Container (.ktx) files are used to support this.  This
   * format has the advantage of being specifically designed for OpenGL.  Header
   * elements directly correspond to API arguments needed to compressed
   * textures.  This puts the burden on the container generator to house the
   * arcane code for determining these for current & future formats.
   *
   * for description see https://www.khronos.org/opengles/sdk/tools/KTX/
   * for file layout see
   * https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
   *
   * Note: The result of this call is not taken into account when a texture is
   * base64.
   *
   * @param formatsAvailable defines the list of those format families you have
   * created on your server.  Syntax: '-' + format family + '.ktx'.  (Case and
   * order do not matter.)
   *
   * Current families are astc, dxt, pvrtc, etc2, & etc1.
   * @returns The extension selected.
   */
  std::string&
  setTextureFormatToUse(const std::vector<std::string>& formatsAvailable);
  std::unique_ptr<GL::IGLTexture> _createTexture();

  /**
   * @brief Hidden
   */
  InternalTexturePtr createTexture(
    const std::vector<std::string>& list, bool noMipmap, bool invertY,
    Scene* scene,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    const std::function<void(InternalTexture*, EventState&)>& onLoad = nullptr,
    const std::function<void()>& onError                             = nullptr,
    const std::variant<ArrayBuffer, Image>& buffer
    = std::variant<ArrayBuffer, Image>());

  /**
   * @brief HUsually called from BABYLON.Texture.ts.
   * Passed information to create a WebGLTexture
   * @param urlArg defines a value which contains one of the following:
   * * A conventional http URL, e.g. 'http://...' or 'file://...'
   * * A base64 string of in-line texture data, e.g.
   * 'data:image/jpg;base64,/...'
   * * An indicator that data being passed using the buffer parameter, e.g.
   * 'data:mytexture.jpg'
   * @param noMipmap defines a boolean indicating that no mipmaps shall be
   * generated.  Ignored for compressed textures.  They must be in the file
   * @param invertY when true, image is flipped when loaded.  You probably want
   * true. Ignored for compressed textures.  Must be flipped in the file
   * @param scene needed for loading to the correct scene
   * @param samplingMode mode with should be used sample / access the texture
   * (Default: BABYLON.Texture.TRILINEAR_SAMPLINGMODE)
   * @param onLoad optional callback to be called upon successful completion
   * @param onError optional callback to be called upon failure
   * @param buffer a source of a file previously fetched as either a base64
   * string, an ArrayBuffer (compressed or image format), HTMLImageElement
   * (image format), or a Blob
   * @param fallback an internal argument in case the function must be called
   * again, due to etc1 not having alpha capabilities
   * @param format internal format.  Default: RGB when extension is '.jpg' else
   * RGBA.  Ignored for compressed textures
   * @returns a InternalTexture for assignment back into BABYLON.Texture
   */
  InternalTexturePtr createTexture(
    const std::string& urlArg, bool noMipmap, bool invertY, Scene* scene,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    const std::function<void(InternalTexture*, EventState&)>& onLoad = nullptr,
    const std::function<void()>& onError                             = nullptr,
    const std::optional<std::variant<ArrayBuffer, Image>>& buffer
    = std::nullopt,
    const InternalTexturePtr& fallBack        = nullptr,
    const std::optional<unsigned int>& format = std::nullopt);

  /**
   * @brief Update a raw texture.
   * @param texture defines the texture to update
   * @param data defines the data to store in the texture
   * @param format defines the format of the data
   * @param invertY defines if data must be stored with Y axis inverted
   * @param compression defines the compression used (null by default)
   * @param type defines the type fo the data
   * (BABYLON.Engine.TEXTURETYPE_UNSIGNED_INT by default)
   */
  void updateRawTexture(const InternalTexturePtr& texture,
                        const Uint8Array& data, unsigned int format,
                        bool invertY                   = true,
                        const std::string& compression = "",
                        unsigned int type
                        = EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief Creates a raw texture.
   * @param data defines the data to store in the texture
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @param format defines the format of the data
   * @param generateMipMaps defines if the engine should generate the mip levels
   * @param invertY defines if data must be stored with Y axis inverted
   * @param samplingMode defines the required sampling mode
   * (BABYLON.Texture.NEAREST_SAMPLINGMODE by default)
   * @param compression defines the compression used (null by default)
   * @param type defines the type fo the data
   * (BABYLON.Engine.TEXTURETYPE_UNSIGNED_INT by default)
   * @returns the raw texture inside an InternalTexture
   */
  InternalTexturePtr createRawTexture(
    const Uint8Array& data, int width, int height, unsigned int format,
    bool generateMipMaps, bool invertY, unsigned int samplingMode,
    const std::string& compression = "",
    unsigned int type              = EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief hidden
   */
  int _getUnpackAlignement() const;

  /**
   * @brief hidden
   */
  void _unpackFlipY(bool value);

  /**
   * @brief hidden
   */
  int _getUnpackAlignement();

  /**
   * @brief Creates a dynamic texture.
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @param generateMipMaps defines if the engine should generate the mip levels
   * @param samplingMode defines the required sampling mode
   * (BABYLON.Texture.NEAREST_SAMPLINGMODE by default)
   * @returns the dynamic texture inside an InternalTexture
   */
  InternalTexturePtr createDynamicTexture(int width, int height,
                                          bool generateMipMaps,
                                          unsigned int samplingMode);

  /**
   * @brief Update the sampling mode of a given texture.
   * @param samplingMode defines the required sampling mode
   * @param texture defines the texture to update
   */
  void updateTextureSamplingMode(unsigned int samplingMode,
                                 const InternalTexturePtr& texture);

  /**
   * @brief Update the content of a dynamic texture.
   * @param texture defines the texture to update
   * @param canvas defines the canvas containing the source
   * @param invertY defines if data must be stored with Y axis inverted
   * @param premulAlpha defines if alpha is stored as premultiplied
   * @param format defines the format of the data
   */
  void updateDynamicTexture(const InternalTexturePtr& texture, ICanvas* canvas,
                            bool invertY, bool premulAlpha = false,
                            unsigned int format
                            = EngineConstants::TEXTUREFORMAT_RGBA);

  /**
   *@brief Updates a depth texture Comparison Mode and Function.
   * If the comparison Function is equal to 0, the mode will be set to none.
   * Otherwise, this only works in webgl 2 and requires a shadow sampler in the
   *shader.
   * @param texture The texture to set the comparison function for
   * @param comparisonFunction The comparison function to set, 0 if no
   *comparison required
   */
  void updateTextureComparisonFunction(const InternalTexturePtr& texture,
                                       int comparisonFunction);

  /**
   * @brief Creates a depth stencil texture.
   * This is only available in WebGL 2 or with the depth texture extension
   * available.
   * @param size The size of face edge in the texture.
   * @param options The options defining the texture.
   * @returns The texture
   */
  InternalTexturePtr
  createDepthStencilTexture(const std::variant<int, ISize>& size,
                            const DepthTextureCreationOptions& options);

  /**
   * @brief Sets the frame buffer Depth / Stencil attachement of the render
   * target to the defined depth stencil texture.
   * @param renderTarget The render target to set the frame buffer for
   */
  void setFrameBufferDepthStencilTexture(RenderTargetTexture* renderTarget);

  /**
   * @brief Creates a new render target texture.
   * @param size defines the size of the texture
   * @param options defines the options used to create the texture
   * @returns a new render target texture stored in an InternalTexture
   */
  InternalTexturePtr
  createRenderTargetTexture(ISize size, const IRenderTargetOptions& options);

  /**
   * @brief Create a multi render target texture.
   * @see http://doc.babylonjs.com/features/webgl2#multiple-render-target
   * @param size defines the size of the texture
   * @param options defines the creation options
   * @returns the cube texture as an InternalTexture
   */
  std::vector<InternalTexturePtr>
  createMultipleRenderTarget(ISize size,
                             const IMultiRenderTargetOptions& options);

  /**
   * @brief Updates the sample count of a render target texture.
   * @see http://doc.babylonjs.com/features/webgl2#multisample-render-targets
   * @param texture defines the texture to update
   * @param samples defines the sample count to set
   * @returns the effective sample count (could be 0 if multisample render
   * targets are not supported)
   */
  unsigned int
  updateRenderTargetTextureSampleCount(const InternalTexturePtr& texture,
                                       unsigned int samples);

  /**
   * @brief Update the sample count for a given multiple render target texture.
   * @see http://doc.babylonjs.com/features/webgl2#multisample-render-targets
   * @param textures defines the textures to update
   * @param samples defines the sample count to set
   * @returns the effective sample count (could be 0 if multisample render
   * targets are not supported)
   */
  unsigned int updateMultipleRenderTargetTextureSampleCount(
    const std::vector<InternalTexturePtr>& textures, unsigned int samples);

  /**
   * @brief Hidden
   */
  void _uploadCompressedDataToTextureDirectly(const InternalTexturePtr& texture,
                                              unsigned int internalFormat,
                                              float width, float height,
                                              const Uint8Array& data,
                                              unsigned int faceIndex = 0,
                                              int lod                = 0);

  /**
   * @brief Hidden
   */
  void _uploadDataToTextureDirectly(const InternalTexturePtr& texture,
                                    const ArrayBufferView& imageData,
                                    unsigned int faceIndex = 0, int lod = 0);

  /**
   * @brief Hidden
   */
  void _uploadArrayBufferViewToTexture(const InternalTexturePtr& texture,
                                       const Uint8Array& imageData,
                                       unsigned int faceIndex = 0, int lod = 0);

  /**
   * @brief Hidden
   */
  void _uploadImageToTexture(const InternalTexturePtr& texture,
                             unsigned int faceIndex, int lod, ICanvas* image);

  /**
   * @brief Creates a new render target cube texture.
   * @param size defines the size of the texture
   * @param options defines the options used to create the texture
   * @returns a new render target cube texture stored in an InternalTexture
   */
  InternalTexturePtr
  createRenderTargetCubeTexture(const ISize& size,
                                const RenderTargetCreationOptions& options);

  /**
   * @brief Create a cube texture from prefiltered data (ie. the mipmaps contain
   * ready to use data for PBR reflection).
   * @param rootUrl defines the url where the file to load is located
   * @param scene defines the current scene
   * @param lodScale defines scale to apply to the mip map selection
   * @param lodOffset defines offset to apply to the mip map selection
   * @param onLoad defines an optional callback raised when the texture is
   * loaded
   * @param onError defines an optional callback raised if there is an issue to
   * load the texture
   * @param format defines the format of the data
   * @param forcedExtension defines the extension to use to pick the right
   * loader
   * @param createPolynomials defines wheter or not to create polynomails
   * harmonics for the texture
   * @returns the cube texture as an InternalTexture
   */
  InternalTexturePtr createPrefilteredCubeTexture(
    const std::string& rootUrl, Scene* scene, float lodScale, float lodOffset,
    const std::function<void(const CubeTextureData& data)>& onLoad = nullptr,
    const std::function<void(const std::string& message,
                             const std::string& exception)>& onError
    = nullptr,
    unsigned int format = 0, const std::string& forcedExtension = "",
    bool createPolynomials = true);

  void _setCubeMapTextureParams(bool loadMipmap);

  /**
   * @brief Creates a cube texture.
   * @param rootUrl defines the url where the files to load is located
   * @param scene defines the current scene
   * @param files defines the list of files to load (1 per face)
   * @param noMipmap defines a boolean indicating that no mipmaps shall be
   * generated (false by default)
   * @param onLoad defines an optional callback raised when the texture is
   * loaded
   * @param onError defines an optional callback raised if there is an issue to
   * load the texture
   * @param format defines the format of the data
   * @param forcedExtension defines the extension to use to pick the right
   * loader
   * @param createPolynomials if a polynomial sphere should be created for the
   * cube texture
   * @param lodScale defines the scale applied to environment texture. This
   * manages the range of LOD level used for IBL according to the roughness
   * @param lodOffset defines the offset applied to environment texture. This
   * manages first LOD level used for IBL according to the roughness
   * @param fallback defines texture to use while falling back when (compressed)
   * texture file not found.
   * @returns the cube texture as an InternalTexture
   */
  InternalTexturePtr createCubeTexture(
    std::string rootUrl, Scene* scene, const std::vector<std::string>& files,
    bool noMipmap,
    const std::function<void(const CubeTextureData& data)>& onLoad = nullptr,
    const std::function<void(const std::string& message,
                             const std::string& exception)>& onError
    = nullptr,
    unsigned int format = 0, const std::string& forcedExtension = "",
    bool createPolynomials = false, float lodScale = 0.f, float lodOffset = 0.f,
    const InternalTexturePtr& fallback = nullptr);

  /**
   * @brief Update a raw cube texture.
   * @param texture defines the texture to udpdate
   * @param data defines the data to store
   * @param format defines the data format
   * @param type defines the type fo the data
   * (BABYLON.Engine.TEXTURETYPE_UNSIGNED_INT by default)
   * @param invertY defines if data must be stored with Y axis inverted
   * @param compression defines the compression used (null by default)
   * @param level defines which level of the texture to update
   */
  void updateRawCubeTexture(const InternalTexturePtr& texture,
                            const std::vector<Uint8Array>& data,
                            unsigned int format, unsigned int type,
                            bool invertY                   = true,
                            const std::string& compression = "",
                            unsigned int level             = 0);

  /**
   * @brief Creates a new raw cube texture.
   * @param data defines the array of data to use to create each face
   * @param size defines the size of the textures
   * @param format defines the format of the data
   * @param type defines the type of the data (like
   * BABYLON.Engine.TEXTURETYPE_UNSIGNED_INT)
   * @param generateMipMaps  defines if the engine should generate the mip
   * levels
   * @param invertY defines if data must be stored with Y axis inverted
   * @param samplingMode defines the required sampling mode (like
   * BABYLON.Texture.NEAREST_SAMPLINGMODE)
   * @param compression defines the compression used (null by default)
   * @returns the cube texture as an InternalTexture
   */
  InternalTexturePtr createRawCubeTexture(const std::vector<Uint8Array> data,
                                          int size, unsigned int format,
                                          unsigned int type,
                                          bool generateMipMaps, bool invertY,
                                          unsigned int samplingMode,
                                          const std::string& compression = "");

  /**
   * @brief Creates a new raw cube texture from a specified url.
   * @param url defines the url where the data is located
   * @param scene defines the current scene
   * @param size defines the size of the textures
   * @param format defines the format of the data
   * @param type defines the type fo the data (like
   * BABYLON.Engine.TEXTURETYPE_UNSIGNED_INT)
   * @param noMipmap defines if the engine should avoid generating the mip
   * levels
   * @param callback defines a callback used to extract texture data from loaded
   * data
   * @param mipmapGenerator defines to provide an optional tool to generate mip
   * levels
   * @param onLoad defines a callback called when texture is loaded
   * @param onError defines a callback called if there is an error
   * @param samplingMode defines the required sampling mode (like
   * BABYLON.Texture.NEAREST_SAMPLINGMODE)
   * @param invertY defines if data must be stored with Y axis inverted
   * @returns the cube texture as an InternalTexture
   */
  InternalTexture* createRawCubeTextureFromUrl(
    const std::string& url, Scene* scene, int size, unsigned int format,
    unsigned int type, bool noMipmap,
    const std::function<ArrayBufferViewArray(const Uint8Array& arrayBuffer)>&
      callback,
    const std::function<std::vector<ArrayBufferViewArray>(
      const ArrayBufferViewArray& faces)>& mipmmapGenerator,
    const std::function<void()>& onLoad  = nullptr,
    const std::function<void()>& onError = nullptr,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    bool invertY              = false);

  /**
   * @brief Update a raw 3D texture.
   * @param texture defines the texture to update
   * @param data defines the data to store
   * @param format defines the data format
   * @param invertY defines if data must be stored with Y axis inverted
   * @param compression defines the used compression (can be null)
   * @param textureType defines the texture Type
   * (Engine.TEXTURETYPE_UNSIGNED_INT, Engine.TEXTURETYPE_FLOAT...)
   */
  void updateRawTexture3D(const InternalTexturePtr& texture,
                          const ArrayBufferView& data, unsigned int format,
                          bool invertY                   = true,
                          const std::string& compression = "",
                          unsigned int textureType
                          = EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief Creates a new raw 3D texture.
   * @param data defines the data used to create the texture
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @param depth defines the depth of the texture
   * @param format defines the format of the texture
   * @param generateMipMaps defines if the engine must generate mip levels
   * @param invertY defines if data must be stored with Y axis inverted
   * @param samplingMode defines the required sampling mode (like
   * BABYLON.Texture.NEAREST_SAMPLINGMODE)
   * @param compression defines the compressed used (can be null)
   * @param textureType defines the compressed used (can be null)
   * @returns a new raw 3D texture (stored in an InternalTexture)
   */
  InternalTexturePtr createRawTexture3D(
    const ArrayBufferView& data, int width, int height, int depth,
    unsigned int format, bool generateMipMaps, bool invertY,
    unsigned int samplingMode, const std::string& compression = "",
    unsigned int textureType = EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief Hidden
   */
  void _releaseFramebufferObjects(InternalTexture* texture);

  /**
   * @brief Hidden
   */
  void _releaseTexture(InternalTexture* texture);

  /**
   * @brief Binds an effect to the webGL context.
   * @param effect defines the effect to bind
   */
  void bindSamplers(Effect* effect);

  /**
   * @brief Hidden
   */
  void _bindTexture(int channel, const InternalTexturePtr& texture);

  /**
   * @brief Sets a texture to the webGL context from a postprocess.
   * @param channel defines the channel to use
   * @param postProcess defines the source postprocess
   */
  void setTextureFromPostProcess(int channel, PostProcess* postProcess);

  /**
   * @brief Binds the output of the passed in post process to the texture
   * channel specified.
   * @param channel The channel the texture should be bound to
   * @param postProcess The post process which's output should be bound
   */
  void setTextureFromPostProcessOutput(int channel, PostProcess* postProcess);

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
  void setTexture(int channel, GL::IGLUniformLocation* uniform,
                  const BaseTexturePtr& texture);

  /**
   * @brief Sets a depth stencil texture from a render target to the according
   * uniform.
   * @param channel The texture channel
   * @param uniform The uniform to set
   * @param texture The render target texture containing the depth stencil
   * texture to apply
   */
  void setDepthStencilTexture(int channel, GL::IGLUniformLocation* uniform,
                              const RenderTargetTexturePtr& texture);

  /**
   * @brief Sets an array of texture to the webGL context.
   * @param channel defines the channel where the texture array must be set
   * @param uniform defines the associated uniform location
   * @param textures defines the array of textures to bind
   */
  void setTextureArray(int channel, GL::IGLUniformLocation* uniform,
                       const std::vector<BaseTexturePtr>& textures);

  /**
   * @brief Hidden
   */
  void _setAnisotropicLevel(unsigned int target, const BaseTexturePtr& texture);

  /**
   * @brief Reads pixels from the current frame buffer. Please note that this
   * function can be slow.
   * @param x defines the x coordinate of the rectangle where pixels must be
   * read
   * @param y defines the y coordinate of the rectangle where pixels must be
   * read
   * @param width defines the width of the rectangle where pixels must be read
   * @param height defines the height of the rectangle where pixels must be read
   * @returns a Uint8Array containing RGBA colors
   */
  Uint8Array readPixels(int x, int y, int width, int height);

  /**
   * @brief Unbind all vertex attributes from the webGL context.
   */
  void unbindAllAttributes();

  /**
   * @brief Force the engine to release all cached effects. This means that next
   * effect compilation will have to be done completely even if a similar effect
   * was already compiled.
   */
  void releaseEffects();

  /**
   * @brief Dispose and release all associated resources.
   */
  void dispose();

  /** Loading screen **/

  /**
   * @brief Display the loading screen.
   * @see http://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  void displayLoadingUI();

  /**
   * @brief Hide the loading screen.
   * @see http://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  void hideLoadingUI();

  /**
   * @brief Gets the current loading screen object.
   * @see http://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  ILoadingScreen* loadingScreen();

  /**
   * @brief Sets the current loading screen object.
   * @see http://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  void setLoadingScreen(ILoadingScreen* loadingScreen);

  /**
   * @brief Sets the current loading screen text.
   * @see http://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  void setLoadingUIText(const std::string& text);

  /**
   * @brief Sets the current loading screen background color.
   * @see http://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  void loadingUIBackgroundColor(const std::string& color);

  /**
   * @brief Gets the source code of the vertex shader associated with a specific
   * webGL program.
   * @param program defines the program to use
   * @returns a string containing the source code of the vertex shader
   * associated with the program
   */
  std::string getVertexShaderSource(GL::IGLProgram* program);

  /**
   * @brief Gets the source code of the fragment shader associated with a
   * specific webGL program.
   * @param program defines the program to use
   * @returns a string containing the source code of the fragment shader
   * associated with the program
   */
  std::string getFragmentShaderSource(GL::IGLProgram* program);

  /**
   * @brief Get the current error code of the webGL context.
   * @returns the error code
   * @see
   * https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getError
   */
  unsigned int getError() const;

  /** FPS **/

  /**
   * @brief Gets the current framerate.
   * @returns a number representing the framerate
   */
  float getFps() const;

  /**
   * @brief Gets the time spent between current and previous frame.
   * @returns a number representing the delta time in ms
   */
  float getDeltaTime() const;

  /** Texture helper functions **/

  /**
   * @brief Hidden
   */
  ArrayBufferView _readTexturePixels(const InternalTexturePtr& texture,
                                     int width, int height, int faceIndex = -1,
                                     int level = 0);

  /**
   * @brief Hidden
   */
  GL::GLenum _getWebGLTextureType(unsigned int type) const;

  /**
   * @brief Hidden
   */
  GL::GLenum
  _getRGBABufferInternalSizedFormat(unsigned int type,
                                    const std::optional<unsigned int>& format
                                    = std::nullopt) const;

  /**
   * @brief Hidden
   */
  GL::GLenum _getRGBAMultiSampleBufferFormat(unsigned int type) const;

  /** Occlusion Queries **/

  /**
   * @brief Create a new webGL query (you must be sure that queries are
   * supported by checking getCaps() function).
   * @return the new query
   */
  GLQueryPtr createQuery();

  /**
   * @brief Delete and release a webGL query.
   * @param query defines the query to delete
   * @return the current engine
   */
  Engine& deleteQuery(const GLQueryPtr& query);

  /**
   * @brief Check if a given query has resolved and got its value.
   * @param query defines the query to check
   * @returns true if the query got its value
   */
  bool isQueryResultAvailable(const GLQueryPtr& query);

  /**
   * @brief Gets the value of a given query.
   * @param query defines the query to check
   * @returns the value of the query
   */
  unsigned int getQueryResult(const GLQueryPtr& query);

  /**
   * @brief Initiates an occlusion query.
   * @param algorithmType defines the algorithm to use
   * @param query defines the query to use
   * @returns the current engine
   * @see http://doc.babylonjs.com/features/occlusionquery
   */
  Engine& beginOcclusionQuery(unsigned int algorithmType,
                              const GLQueryPtr& query);

  /**
   * @brief Ends an occlusion query.
   * @see http://doc.babylonjs.com/features/occlusionquery
   * @param algorithmType defines the algorithm to use
   * @returns the current engine
   */
  Engine& endOcclusionQuery(unsigned int algorithmType);

  /** Time queries **/

  /**
   * @brief Starts a time query (used to measure time spent by the GPU on a
   * specific frame). Please note that only one query can be issued at a time
   * @returns a time token used to track the time span
   */
  std::optional<_TimeToken> startTimeQuery();

  /**
   * @brief Ends a time query.
   * @param token defines the token used to measure the time span
   * @returns the time spent (in ns)
   */
  int endTimeQuery(std::optional<_TimeToken>& token);

  /** Transform feedback **/

  /**
   * @brief Creates a webGL transform feedback object.
   * Please makes sure to check webGLVersion property to check if you are
   * running webGL 2+
   * @returns the webGL transform feedback object
   */
  GLTransformFeedbackPtr createTransformFeedback();

  /**
   * @brief Delete a webGL transform feedback object.
   * @param value defines the webGL transform feedback object to delete
   */
  void deleteTransformFeedback(GL::IGLTransformFeedback* value);

  /**
   * @brief Bind a webGL transform feedback object to the webgl context.
   * @param value defines the webGL transform feedback object to bind
   */
  void bindTransformFeedback(GL::IGLTransformFeedback* value);

  /**
   * @brief Begins a transform feedback operation.
   * @param usePoints defines if points or triangles must be used
   */
  void beginTransformFeedback(bool usePoints = true);

  /**
   * @brief Ends a transform feedback operation.
   */
  void endTransformFeedback();

  /**
   * @brief Specify the varyings to use with transform feedback.
   * @param program defines the associated webGL program
   * @param value defines the list of strings representing the varying names
   */
  void setTranformFeedbackVaryings(GL::IGLProgram* program,
                                   const std::vector<std::string>& value);

  /**
   * @brief Bind a webGL buffer for a transform feedback operation.
   * @param value defines the webGL buffer to bind
   */
  void bindTransformFeedbackBuffer(GL::IGLBuffer* value);

  /**
   * @brief Hidden
   */
  IFileRequest _loadFile(
    const std::string& url,
    const std::function<void(const std::variant<std::string, ArrayBuffer>& data,
                             const std::string& responseURL)>& onSuccess,
    const std::function<void(const ProgressEvent& event)>& onProgress = nullptr,
    bool useArrayBuffer                                               = false,
    const std::function<void(const std::string& message,
                             const std::string& exception)>& onError
    = nullptr);

  /**
   * @brief Hidden
   */
  std::promise<std::string> _loadFileAsync(const std::string& url);

  /** Statics **/

  /**
   * @brief Gets the latest created engine.
   */
  static Engine* LastCreatedEngine();

  /**
   * @brief Gets the latest created scene.
   */
  static Scene* LastCreatedScene();

  /**
   * @brief Will flag all materials in all scenes in all engines as dirty to
   * trigger new shader compilation.
   * @param flag defines which part of the materials must be marked as dirty
   * @param predicate defines a predicate used to filter which materials should
   * be affected
   */
  static void
  MarkAllMaterialsAsDirty(unsigned int flag,
                          const std::function<bool(Material* mat)>& predicate
                          = nullptr);

  /**
   * Hidden
   */
  static std::vector<IInternalTextureLoaderPtr> _TextureLoaders;

  /**
   * @brief Gets a boolean indicating if the engine can be instanciated (ie. if
   * a webGL context can be found).
   * @returns true if the engine can be created
   */
  static bool isSupported();

protected:
  /**
   * @brief Creates a new engine.
   * @param canvasOrContext defines the canvas or WebGL context to use for
   * rendering. If you provide a WebGL context, Babylon.js will not hook events
   * on the canvas (like pointers, keyboards, etc...) so no event observables
   * will be available. This is mostly used when Babylon.js is used as a plugin
   * on a system which alreay used the WebGL context
   * @param antialias defines enable antialiasing (default: false)
   * @param options defines further options to be sent to the getContext()
   * function
   * @param adaptToDeviceRatio defines whether to adapt to the device's viewport
   * characteristics (default: false)
   */
  Engine(ICanvas* canvas, const EngineOptions& options = EngineOptions());

  /**
   * @brief Hidden
   */
  bool _bindTextureDirectly(unsigned int target,
                            const InternalTexturePtr& texture,
                            bool forTextureDataUpdate = false,
                            bool force                = false);

private:
  void _prepareWorkingCanvas();
  void _rebuildInternalTextures();
  void _rebuildEffects();
  void _rebuildBuffers();
  void _initGLContext();
  void _onVRFullScreenTriggered();
  void _getVRDisplays();
  void _bindSamplerUniformToChannel(int sourceSlot, int destination);
  unsigned int _getTextureWrapMode(unsigned int mode) const;
  bool _setTexture(int channel, const BaseTexturePtr& texture,
                   bool isPartOfTextureArray = false,
                   bool depthStencilTexture  = false);
  void _setTextureParameterFloat(unsigned int target, unsigned int parameter,
                                 float value,
                                 const InternalTexturePtr& texture);
  void _setTextureParameterInteger(unsigned int target, unsigned int parameter,
                                   int value,
                                   const InternalTexturePtr& texture = nullptr);
  void bindUnboundFramebuffer(GL::IGLFramebuffer* framebuffer);
  void bindIndexBuffer(GL::IGLBuffer* buffer);
  void bindBuffer(GL::IGLBuffer* buffer, int target);
  void _vertexAttribPointer(GL::IGLBuffer* buffer, unsigned int indx, int size,
                            unsigned int type, bool normalized, int stride,
                            int offset);
  void _bindIndexBufferWithCache(GL::IGLBuffer* indexBuffer);
  void _bindVertexBuffersAttributes(
    const std::unordered_map<std::string, VertexBuffer*>& vertexBuffers,
    Effect* effect);
  void _unbindVertexArrayObject();
  void setProgram(GL::IGLProgram* program);
  void _moveBoundTextureOnTop(const InternalTexturePtr& internalTexture);
  int _getCorrectTextureChannel(int channel,
                                const InternalTexturePtr& internalTexture
                                = nullptr);
  void _linkTrackers(const IInternalTextureTrackerPtr& previous,
                     const IInternalTextureTrackerPtr& next);
  int _removeDesignatedSlot(const InternalTexturePtr& internalTexture);
  void _activateCurrentTexture();
  void _cascadeLoadImgs(
    const std::string& rootUrl, Scene* scene,
    const std::function<void(const std::vector<Image>& images)>& onfinish,
    const std::vector<std::string>& files,
    const std::function<void(const std::string& message,
                             const std::string& exception)>& onError
    = nullptr);
  void _rescaleTexture(const InternalTexturePtr& source,
                       const InternalTexturePtr& destination, Scene* scene,
                       unsigned int internalFormat,
                       const std::function<void()>& onComplete);
  void _setupDepthStencilTexture(InternalTexture* internalTexture,
                                 const std::variant<int, ISize>& size,
                                 bool generateStencil, bool bilinearFiltering,
                                 int comparisonFunction);

  /**
   * @brief Creates a depth stencil texture.
   * This is only available in WebGL 2 or with the depth texture extension
   * available.
   * @param size The size of face edge in the texture.
   * @param options The options defining the texture.
   * @returns The texture
   */
  InternalTexturePtr
  _createDepthStencilTexture(const std::variant<int, ISize>& size,
                             const DepthTextureCreationOptions& options);

  /**
   * @brief Creates a depth stencil cube texture.
   * This is only available in WebGL 2.
   * @param size The size of face edge in the cube texture.
   * @param options The options defining the cube texture.
   * @returns The cube texture
   */
  InternalTexturePtr
  _createDepthStencilCubeTexture(int size,
                                 const DepthTextureCreationOptions& options);

  unsigned int _drawMode(unsigned int fillMode);
  GLShaderPtr _compileShader(const std::string& source, const std::string& type,
                             const std::string& defines,
                             const std::string& shaderVersion);
  GLShaderPtr _compileRawShader(const std::string& source,
                                const std::string& type);
  GLProgramPtr
  _createShaderProgram(const std::unique_ptr<GL::IGLShader>& vertexShader,
                       const std::unique_ptr<GL::IGLShader>& fragmentShader,
                       GL::IGLRenderingContext* context,
                       const std::vector<std::string>& transformFeedbackVaryings
                       = {});
  SamplingParameters _getSamplingParameters(unsigned int samplingMode,
                                            bool generateMipMaps);
  GLRenderBufferPtr
  _setupFramebufferDepthAttachments(bool generateStencilBuffer,
                                    bool generateDepthBuffer, int width,
                                    int height, int samples = 1);
  void _prepareWebGLTextureContinuation(const InternalTexturePtr& texture,
                                        Scene* scene, bool noMipmap,
                                        bool isCompressed,
                                        unsigned int samplingMode);
  void _prepareWebGLTexture(
    const InternalTexturePtr& texture, Scene* scene, int width, int height,
    std::optional<bool> invertY, bool noMipmap, bool isCompressed,
    const std::function<
      bool(int width, int height,
           const std::function<void()>& continuationCallback)>& processFunction,
    unsigned int samplingMode
    = EngineConstants::TEXTURE_TRILINEAR_SAMPLINGMODE);
  ArrayBufferView _convertRGBtoRGBATextureData(const ArrayBufferView& rgbData,
                                               int width, int height,
                                               unsigned int textureType);
  /** VBOs **/
  void _resetVertexBufferBinding();
  void _resetIndexBufferBinding();
  /** FPS **/
  void _measureFps();

  bool _canRenderToFloatFramebuffer();
  bool _canRenderToHalfFloatFramebuffer();
  bool _canRenderToFramebuffer(unsigned int type);
  GL::GLenum _getInternalFormat(unsigned int format) const;

  /** Occlusion Queries **/
  unsigned int getGlAlgorithmType(unsigned int algorithmType) const;

  /** File loading */
  void _cascadeLoadFiles(
    Scene* scene,
    const std::function<void(const std::variant<std::string, ArrayBuffer>& data,
                             const std::string& responseURL)>& onloaddata,
    const std::vector<std::string>& files,
    const std::function<void(const std::string& message,
                             const std::string& exception)>& onError);

public:
  // Public members

  /**
   * Gets or sets a boolean that indicates if textures must be forced to power
   * of 2 size even if not required
   */
  bool forcePOTTextures;

  /**
   * Gets a boolean indicating if the engine is currently rendering in
   * fullscreen mode
   */
  bool isFullscreen;

  /**
   * Gets a boolean indicating if the pointer is currently locked
   */
  bool isPointerLock;

  /**
   * Gets or sets a boolean indicating if back faces must be culled (true by
   * default)
   */
  bool cullBackFaces;

  /**
   * Gets or sets a boolean indicating if the engine must keep rendering even if
   * the window is not in foregroun
   */
  bool renderEvenInBackground;

  /**
   * Gets or sets a boolean indicating that cache can be kept between frames
   */
  bool preventCacheWipeBetweenFrames;

  /**
   * Gets or sets a boolean to enable/disable IndexedDB support and avoid XHR on
   *.manifest
   **/
  bool enableOfflineSupport;

  /**
   * Gets or sets a boolean to enable/disable checking manifest if IndexedDB
   * support is enabled (Babylon.js will always consider the database is up to
   * date)
   **/
  bool disableManifestCheck;

  /**
   * Gets the list of created scenes
   */
  std::vector<Scene*> scenes;

  /**
   * Gets the list of created postprocesses
   */
  std::vector<PostProcessPtr> postProcesses;

  /**
   * Gets or sets a boolean indicating if the engine should validate programs
   * after compilation
   */
  bool validateShaderPrograms;

  // Observables

  /**
   * Observable event triggered each time the rendering canvas is resized.
   */
  Observable<Engine> onResizeObservable;

  /**
   * Observable event triggered each time the canvas lost focus
   */
  Observable<Engine> onCanvasBlurObservable;

  /**
   * Observable event triggered each time the canvas gains focus
   */
  Observable<Engine> onCanvasFocusObservable;

  /**
   * Observable event triggered each time the canvas receives pointerout event
   */
  Observable<Engine> onCanvasPointerOutObservable;

  /**
   * Observable event triggered before each texture is initialized
   */
  Observable<Texture> onBeforeTextureInitObservable;

  /**
   * Observable raised when the engine begins a new frame
   */
  Observable<Engine> onBeginFrameObservable;

  /**
   * Observable raised when the engine ends the current frame
   */
  Observable<Engine> onEndFrameObservable;

  /**
   * Observable raised when the engine is about to compile a shader
   */
  Observable<Engine> onBeforeShaderCompilationObservable;

  /**
   * Observable raised when the engine has jsut compiled a shader
   */
  Observable<Engine> onAfterShaderCompilationObservable;

  /**
   * Hidden
   */
  bool _badOS;

  /**
   * Hidden
   */
  bool _badDesktopOS;

  /**
   * Gets or sets a value indicating if we want to disable texture binding
   * optmization. This could be required on some buggy drivers which wants to
   * have textures bound in a progressive order. By default Babylon.js will try
   * to let textures bound where they are and only update the samplers to point
   * where the texture is
   */
  bool disableTextureBindingOptimization;

  // WebVR

  // The new WebVR uses promises.
  // this promise resolves with the current devices available.
  int vrDisplaysPromise;
  int _vrDisplays;
  bool _vrDisplayEnabled;
  Size _oldSize;
  int _oldHardwareScaleFactor;
  int _vrAnimationFrameHandler;

  // Uniform buffers list

  /**
   * Gets or sets a boolean indicating that uniform buffers must be disabled
   * even if they are supported
   */
  bool disableUniformBuffers;

  /**
   * Hidden
   */
  std::vector<UniformBuffer*> _uniformBuffers;

  /**
   * Hidden
   */
  GL::IGLRenderingContext* _gl;

  /**
   * Gets the audio engine
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  static AudioEngine* audioEngine;

  /**
   * Hidden
   */
  PerfCounter _drawCalls;

  /**
   * Hidden
   */
  PerfCounter _textureCollisions;

  // Lost context

  /**
   * Observable signaled when a context lost event is raised
   */
  Observable<Engine> onContextLostObservable;

  /**
   * Observable signaled when a context restored event is raised
   */
  Observable<Engine> onContextRestoredObservable;

  /**
   * Turn this value on if you want to pause FPS computation when in background
   */
  bool disablePerformanceMonitorInBackground;

  /**
   * Defines whether the engine has been created with the premultipliedAlpha
   * option on or not.
   */
  bool premultipliedAlpha;

protected:
  /**
   * Hidden
   */
  EngineCapabilities _caps;

  // States

  /**
   * Hidden
   */
  std::unique_ptr<_DepthCullingState> _depthCullingState;

  /**
   * Hidden
   */
  std::unique_ptr<_StencilState> _stencilState;

  /**
   * Hidden
   */
  std::unique_ptr<_AlphaState> _alphaState;

  /**
   * Hidden
   */
  unsigned int _alphaMode;

  // Cache
  std::vector<InternalTexturePtr> _internalTexturesCache;

  /**
   * Hidden
   */
  int _activeChannel;

  /**
   * Hidden
   */
  std::unordered_map<int, InternalTexturePtr> _boundTexturesCache;

  /**
   * Hidden
   */
  Effect* _currentEffect;

  /**
   * Hidden
   */
  GL::IGLProgram* _currentProgram;

  /**
   * Hidden
   */
  Viewport* _cachedViewport;

  /**
   * Hidden
   */
  std::unordered_map<std::string, VertexBuffer*> _cachedVertexBuffersMap;

  /**
   * Hidden
   */
  GL::IGLBuffer* _cachedVertexBuffers;

  /**
   * Hidden
   */
  GL::IGLBuffer* _cachedIndexBuffer;

  /**
   * Hidden
   */
  Effect* _cachedEffectForVertexBuffers;

  /**
   * Hidden
   */
  InternalTexturePtr _currentRenderTarget;

  /**
   * Hidden
   */
  GL::IGLFramebuffer* _currentFramebuffer;

private:
  // WebVR
  bool _vrExclusivePointerMode;
  // Private Members
  ICanvas* _renderingCanvas;
  bool _windowIsBackground;
  float _webGLVersion;

  // Focus
  std::function<void()> _onFocus;
  std::function<void()> _onBlur;
  std::function<void()> _onCanvasPointerOut;
  std::function<void()> _onCanvasBlur;

  std::function<void()> _onCanvasFocus;

  std::function<void()> _onFullscreenChange;
  std::function<void()> _onPointerLockChange;

  int _hardwareScalingLevel;
  bool _pointerLockRequested;
  bool _isStencilEnable;
  bool _colorWrite;

  ILoadingScreen* _loadingScreen;

  std::string _glVersion;
  std::string _glRenderer;
  std::string _glVendor;

  bool _videoTextureSupported;

  bool _renderingQueueLaunched;
  std::vector<SA::delegate<void()>> _activeRenderLoops;

  // Deterministic lockstepMaxSteps
  bool _deterministicLockstep;
  unsigned int _lockstepMaxSteps;

  // Lost context
  std::function<void(Event&& evt)> _onContextLost;
  std::function<void(Event&& evt)> _onContextRestored;
  bool _contextWasLost;
  bool _doNotHandleContextLost;

  // FPS
  std::unique_ptr<PerformanceMonitor> _performanceMonitor;
  float _fps;
  float _deltaTime;

  /**
   * Hidden
   */
  int _currentTextureChannel;

  std::unordered_map<std::string, std::unique_ptr<Effect>> _compiledEffects;
  std::vector<bool> _vertexAttribArraysEnabled;
  GL::IGLVertexArrayObject* _cachedVertexArrayObject;
  bool _uintIndicesCurrentlySet;
  std::unordered_map<int, GL::IGLBuffer*> _currentBoundBuffer;
  std::unordered_map<unsigned int, BufferPointer> _currentBufferPointers;
  Int32Array _currentInstanceLocations;
  std::vector<GL::IGLBuffer*> _currentInstanceBuffers;
  Int32Array _textureUnits;
  DummyInternalTextureTrackerPtr _firstBoundInternalTextureTracker;
  DummyInternalTextureTrackerPtr _lastBoundInternalTextureTracker;
  ICanvas* _workingCanvas;
  ICanvasRenderingContext2D* _workingContext;
  PassPostProcessPtr _rescalePostProcess;
  std::unique_ptr<GL::IGLFramebuffer> _dummyFramebuffer;
  std::function<void()> _bindedRenderFunction;
  bool _vaoRecordInProgress;
  bool _mustWipeVertexAttributes;
  InternalTexturePtr _emptyTexture;
  InternalTexturePtr _emptyCubeTexture;
  InternalTexturePtr _emptyTexture3D;
  int _frameHandler;
  // Hardware supported Compressed Textures
  std::vector<std::string> _texturesSupported;
  Int32Array _nextFreeTextureSlots;
  unsigned int _maxSimultaneousTextures;
  std::string _textureFormatInUse;
  std::unordered_map<int, GL::IGLUniformLocation*> _boundUniforms;
  Vector4 _viewportCached;
  std::optional<bool> _unpackFlipYCached;

}; // end of class Engine

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_ENGINE_H
