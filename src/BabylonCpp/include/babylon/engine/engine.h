#ifndef BABYLON_ENGINE_ENGINE_H
#define BABYLON_ENGINE_ENGINE_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/core/variant.h>
#include <babylon/engine/engine_capabilities.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/engine/engine_options.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/math/size.h>
#include <babylon/math/viewport.h>
#include <babylon/mesh/buffer_pointer.h>
#include <babylon/tools/ifile_request.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/perf_counter.h>
#include <babylon/tools/performance_monitor.h>

namespace BABYLON {

/**
 * @brief The engine class is responsible for interfacing with all lower-level
 * APIs such as OpenGL and Audio.
 */
class BABYLON_SHARED_EXPORT Engine {

public:
  using ArrayBufferViewArray   = vector_t<ArrayBufferView>;
  using GLBufferPtr            = unique_ptr_t<GL::IGLBuffer>;
  using GLFrameBufferPtr       = unique_ptr_t<GL::IGLFramebuffer>;
  using GLProgramPtr           = unique_ptr_t<GL::IGLProgram>;
  using GLQueryPtr             = unique_ptr_t<GL::IGLQuery>;
  using GLRenderBufferPtr      = unique_ptr_t<GL::IGLRenderbuffer>;
  using GLShaderPtr            = unique_ptr_t<GL::IGLShader>;
  using GLTexturePtr           = unique_ptr_t<GL::IGLTexture>;
  using GLTransformFeedbackPtr = unique_ptr_t<GL::IGLTransformFeedback>;
  using GLUniformLocationPtr   = unique_ptr_t<GL::IGLUniformLocation>;
  using GLVertexArrayObjectPtr = unique_ptr_t<GL::IGLVertexArrayObject>;

  friend class BaseTexture;

public:
  static string_t Version();

  // Updatable statics so stick with vars here
  static float CollisionsEpsilon;
  static string_t CodeRepository;
  static string_t ShadersRepository;

  // Engine instances
  static vector_t<Engine*> Instances;

public:
  template <typename... Ts>
  static unique_ptr_t<Engine> New(Ts&&... args)
  {
    unique_ptr_t<Engine> engine(new Engine(::std::forward<Ts>(args)...));
    return engine;
  }
  virtual ~Engine();

  vector_t<string_t>& texturesSupported();
  string_t textureFormatInUse() const;
  Viewport* currentViewport() const;

  // Empty texture
  InternalTexture* emptyTexture();
  InternalTexture* emptyTexture3D();
  InternalTexture* emptyCubeTexture();

  float webGLVersion() const;
  bool isInVRExclusivePointerMode() const;
  bool supportsUniformBuffers() const;
  bool needPOTTextures() const;
  bool badOS() const;
  bool badDesktopOS() const;
  PerformanceMonitor* performanceMonitor() const;

  /**
   * @brief Returns true if the stencil buffer has been enabled through the
   * creation option of the context.
   */
  bool isStencilEnable() const;
  void resetTextureCache();
  bool isDeterministicLockStep() const;
  unsigned int getLockstepMaxSteps() const;
  GL::GLInfo getGlInfo();
  float getAspectRatio(Camera* camera, bool useScreen = false);
  int getRenderWidth(bool useScreen = false);
  int getRenderHeight(bool useScreen = false);
  ICanvas* getRenderingCanvas();
  Nullable<ClientRect> getRenderingCanvasClientRect();
  void setHardwareScalingLevel(int level);
  int getHardwareScalingLevel() const;
  vector_t<unique_ptr_t<InternalTexture>>& getLoadedTexturesCache();
  EngineCapabilities& getCaps();
  /** The number of draw calls submitted last frame */
  size_t drawCalls() const;
  Nullable<PerfCounter> drawCallsPerfCounter();

  /** Methods **/
  void backupGLState();
  void restoreGLState();
  int getDepthFunction() const;
  void setDepthFunction(int depthFunc);
  void setDepthFunctionToGreater();
  void setDepthFunctionToGreaterOrEqual();
  void setDepthFunctionToLess();
  void setDepthFunctionToLessOrEqual();
  bool getStencilBuffer() const;
  void setStencilBuffer(bool enable);
  unsigned int getStencilMask() const;
  void setStencilMask(unsigned int mask);
  unsigned int getStencilFunction() const;
  int getStencilFunctionReference() const;
  unsigned int getStencilFunctionMask() const;
  void setStencilFunction(unsigned int stencilFunc);
  void setStencilFunctionReference(int reference);
  void setStencilFunctionMask(unsigned int mask);
  unsigned int getStencilOperationFail() const;
  unsigned int getStencilOperationDepthFail() const;
  unsigned int getStencilOperationPass() const;
  void setStencilOperationFail(unsigned int operation);
  void setStencilOperationDepthFail(unsigned int operation);
  void setStencilOperationPass(unsigned int operation);
  void setDitheringState(bool value);
  void setRasterizerState(bool value);

  /**
   * @brief Stop executing a render loop function and remove it from the
   * execution array.
   * @param {Function} [renderFunction] the function to be removed. If not
   * provided all functions will be removed.
   */
  void stopRenderLoop();
  void stopRenderLoop(const delegate_t<void()>& renderFunction);
  void _renderLoop();

  /**
   * @brief Register and execute a render loop. The engine can have more than
   * one render function.
   * @param {Function} renderFunction - the function to continuesly execute
   * starting the next render loop.
   * Example:
   * engine.runRenderLoop(function () {
   *      scene.render()
   * })
   */
  void runRenderLoop(const ::std::function<void()>& renderFunction);
  void renderFunction(const ::std::function<void()>& renderFunction);

  /**
   * @brief Toggle full screen mode.
   * @param {boolean} requestPointerLock - should a pointer lock be requested
   * from the user
   */
  void switchFullscreen(bool requestPointerLock);
  void clear(bool depth, bool stencil = false);
  void clear(const Color4& color, bool backBuffer, bool depth,
             bool stencil = false);
  void scissorClear(int x, int y, int width, int height,
                    const Color4& clearColor);

  /**
   * @brief Set the WebGL's viewport.
   * @param {BABYLON.Viewport} viewport - the viewport element to be used.
   * @param {number} [requiredWidth] - the width required for rendering. If not
   * provided the rendering canvas' width is used.
   * @param {number} [requiredHeight] - the height required for rendering. If
   * not provided the rendering canvas' height is used.
   */
  void setViewport(Viewport& viewport, int requiredWidth = 0,
                   int requiredHeight = 0);
  Viewport& setDirectViewport(int x, int y, int width, int height);
  void beginFrame();
  void endFrame();

  /**
   * @brief resize the view according to the canvas' size.
   */
  void resize();

  /**
   * @brief Force a specific size of the canvas.
   * @param {number} width - the new canvas' width
   * @param {number} height - the new canvas' height
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
   */
  void bindFramebuffer(InternalTexture* texture,
                       Nullable<unsigned int> faceIndex       = nullptr,
                       Nullable<int> requiredWidth            = nullptr,
                       Nullable<int> requiredHeight           = nullptr,
                       Nullable<bool> forceFullscreenViewport = nullptr,
                       InternalTexture* depthStencilTexture   = nullptr);
  void unBindFramebuffer(InternalTexture* texture,
                         bool disableGenerateMipMaps = false,
                         const ::std::function<void()>& onBeforeUnbind
                         = nullptr);
  void unBindMultiColorAttachmentFramebuffer(
    const vector_t<InternalTexture*>& textures,
    bool disableGenerateMipMaps                   = false,
    const ::std::function<void()>& onBeforeUnbind = nullptr);
  void generateMipMapsForCubemap(InternalTexture* texture);
  void flushFramebuffer();
  void restoreDefaultFramebuffer();

  /** UBOs **/
  GLBufferPtr createUniformBuffer(const Float32Array& elements);
  GLBufferPtr createDynamicUniformBuffer(const Float32Array& elements);
  void updateUniformBuffer(GL::IGLBuffer* uniformBuffer,
                           const Float32Array& elements, int offset = -1,
                           int count = -1);

  /** VBOs **/
  GLBufferPtr createVertexBuffer(const Float32Array& vertices);
  GLBufferPtr createDynamicVertexBuffer(const Float32Array& vertices);
  void updateDynamicIndexBuffer(const GLBufferPtr& indexBuffer,
                                const IndicesArray& indices, int offset = 0);
  void updateDynamicVertexBuffer(const GLBufferPtr& vertexBuffer,
                                 const Float32Array& vertices, int offset = -1,
                                 int count = -1);
  GLBufferPtr createIndexBuffer(const IndicesArray& indices,
                                bool updatable = false);
  void bindArrayBuffer(GL::IGLBuffer* buffer);
  void bindUniformBuffer(GL::IGLBuffer* buffer);
  void bindUniformBufferBase(GL::IGLBuffer* buffer, unsigned int location);
  void bindUniformBlock(GL::IGLProgram* shaderProgram, const string_t blockName,
                        unsigned int index);
  void updateArrayBuffer(const Float32Array& data);
  GLVertexArrayObjectPtr recordVertexArrayObject(
    const unordered_map_t<string_t, VertexBuffer*>& vertexBuffers,
    GL::IGLBuffer* indexBuffer, Effect* effect);
  void bindVertexArrayObject(GL::IGLVertexArrayObject* vertexArrayObject,
                             GL::IGLBuffer* indexBuffer);
  void bindBuffersDirectly(GL::IGLBuffer* vertexBuffer,
                           GL::IGLBuffer* indexBuffer,
                           const Float32Array& vertexDeclaration,
                           int vertexStrideSize = 3, Effect* effect = nullptr);
  void
  bindBuffers(const unordered_map_t<string_t, VertexBuffer*>& vertexBuffers,
              GL::IGLBuffer* indexBuffer, Effect* effect);
  void unbindInstanceAttributes();
  void releaseVertexArrayObject(GL::IGLVertexArrayObject* vao);
  bool _releaseBuffer(GL::IGLBuffer* buffer);
  GLBufferPtr createInstancesBuffer(unsigned int capacity);
  void deleteInstancesBuffer(GL::IGLBuffer* buffer);
  void updateAndBindInstancesBuffer(GL::IGLBuffer* instancesBuffer,
                                    const Float32Array& data,
                                    const Uint32Array& offsetLocations);
  void updateAndBindInstancesBuffer(
    GL::IGLBuffer* instancesBuffer, const Float32Array& data,
    const vector_t<InstancingAttributeInfo>& offsetLocations);
  void applyStates();
  void draw(bool useTriangles, int indexStart, int indexCount,
            int instancesCount = 0);
  void drawPointClouds(int verticesStart, int verticesCount,
                       int instancesCount = 0);
  void drawUnIndexed(bool useTriangles, int verticesStart, int verticesCount,
                     int instancesCount = 0);
  void drawElementsType(unsigned int fillMode, int indexStart,
                        int verticesCount, int instancesCount = 0);
  void drawArraysType(unsigned int fillMode, int verticesStart,
                      int verticesCount, int instancesCount = 0);

  /** Shaders **/
  void _releaseEffect(Effect* effect);
  void _deleteProgram(GL::IGLProgram* program);

  /**
   * @param baseName The base name of the effect (The name of file without
   * .fragment.fx or .vertex.fx)
   * @param samplers An array of string used to represent textures
   */
  Effect* createEffect(const string_t& baseName, EffectCreationOptions& options,
                       Engine* engine,
                       const ::std::function<void(Effect* effect)>& onCompiled
                       = nullptr);
  Effect* createEffect(unordered_map_t<string_t, string_t>& baseName,
                       EffectCreationOptions& options, Engine* engine);
  Effect* createEffectForParticles(
    const string_t& fragmentName, const vector_t<string_t>& uniformsNames,
    const vector_t<string_t>& samplers, const string_t& defines = "",
    EffectFallbacks* fallbacks                                    = nullptr,
    const ::std::function<void(const Effect* effect)>& onCompiled = nullptr,
    const ::std::function<void(const Effect* effect, const string_t& errors)>&
      onError
    = nullptr);
  GLProgramPtr createRawShaderProgram(
    const string_t& vertexCode, const string_t& fragmentCode,
    GL::IGLRenderingContext* context                    = nullptr,
    const vector_t<string_t>& transformFeedbackVaryings = {});
  GLProgramPtr
  createShaderProgram(const string_t& vertexCode, const string_t& fragmentCode,
                      const string_t& defines,
                      GL::IGLRenderingContext* context = nullptr,
                      const vector_t<string_t>& transformFeedbackVaryings = {});
  unordered_map_t<string_t, GLUniformLocationPtr>
  getUniforms(GL::IGLProgram* shaderProgram,
              const vector_t<string_t>& uniformsNames);
  Int32Array getAttributes(GL::IGLProgram* shaderProgram,
                           const vector_t<string_t>& attributesNames);
  void enableEffect(Effect* effect);
  void setIntArray(GL::IGLUniformLocation* uniform, const Int32Array& array);
  void setIntArray2(GL::IGLUniformLocation* uniform, const Int32Array& array);
  void setIntArray3(GL::IGLUniformLocation* uniform, const Int32Array& array);
  void setIntArray4(GL::IGLUniformLocation* uniform, const Int32Array& array);
  void setFloatArray(GL::IGLUniformLocation* uniform,
                     const Float32Array& array);
  void setFloatArray2(GL::IGLUniformLocation* uniform,
                      const Float32Array& array);
  void setFloatArray3(GL::IGLUniformLocation* uniform,
                      const Float32Array& array);
  void setFloatArray4(GL::IGLUniformLocation* uniform,
                      const Float32Array& array);
  void setArray(GL::IGLUniformLocation* uniform, const Float32Array& array);
  void setArray2(GL::IGLUniformLocation* uniform, const Float32Array& array);
  void setArray3(GL::IGLUniformLocation* uniform, const Float32Array& array);
  void setArray4(GL::IGLUniformLocation* uniform, const Float32Array& array);
  void setMatrices(GL::IGLUniformLocation* uniform,
                   const Float32Array& matrices);
  void setMatrix(GL::IGLUniformLocation* uniform, const Matrix& matrix);
  void setMatrix3x3(GL::IGLUniformLocation* uniform,
                    const Float32Array& matrix);
  void setMatrix2x2(GL::IGLUniformLocation* uniform,
                    const Float32Array& matrix);
  void setInt(GL::IGLUniformLocation* uniform, int value);
  void setFloat(GL::IGLUniformLocation* uniform, float value);
  void setFloat2(GL::IGLUniformLocation* uniform, float x, float y);
  void setFloat3(GL::IGLUniformLocation* uniform, float x, float y, float z);
  void setBool(GL::IGLUniformLocation* uniform, int value);
  void setFloat4(GL::IGLUniformLocation* uniform, float x, float y, float z,
                 float w);
  void setColor3(GL::IGLUniformLocation* uniform, const Color3& color3);
  void setColor4(GL::IGLUniformLocation* uniform, const Color3& color3,
                 float alpha);

  /**
   * @brief Sets a Color4 on a uniform variable.
   * @param uniform defines the uniform location
   * @param color4 defines the value to be set
   */
  void setDirectColor4(GL::IGLUniformLocation* uniform, const Color4& color4);

  /** States **/
  void setState(bool culling, float zOffset = 0.f, bool force = false,
                bool reverseSide = false);
  void setZOffset(float value);
  float getZOffset() const;
  void setDepthBuffer(bool enable);
  bool getDepthWrite() const;
  void setDepthWrite(bool enable);
  void setColorWrite(bool enable);
  bool getColorWrite() const;
  void setAlphaConstants(float r, float g, float b, float a);
  void setAlphaMode(unsigned int mode, bool noDepthWriteChange = false);
  unsigned int getAlphaMode() const;
  _StencilState* stencilState();

  /** Textures **/
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
   * @param {Array<string>} formatsAvailable- The list of those format families
   * you have created on your server. Syntax: '-' + format family + '.ktx'.
   * (Case and order do not matter.)
   *
   * Current families are astc, dxt, pvrtc, etc2, atc, & etc1.
   * @returns The extension selected.
   */
  string_t& setTextureFormatToUse(const vector_t<string_t>& formatsAvailable);
  unique_ptr_t<GL::IGLTexture> _createTexture();
  InternalTexture* createTexture(
    const vector_t<string_t>& list, bool noMipmap, bool invertY, Scene* scene,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    const ::std::function<void(InternalTexture*, EventState&)>& onLoad
    = nullptr,
    const ::std::function<void()>& onError    = nullptr,
    const Variant<ArrayBuffer, Image>& buffer = Variant<ArrayBuffer, Image>());

  /**
   * @brief Usually called from BABYLON.Texture.ts.  Passed information to
   * create a WebGLTexture.
   * @param {string} urlArg- This contains one of the following:
   *  1. A conventional http URL, e.g. 'http://...' or 'file://...'
   *  2. A base64 string of in-line texture data, e.g.
   * 'data:image/jpg;base64,/...'
   *  3. An indicator that data being passed using the buffer parameter, e.g.
   * 'data:mytexture.jpg'
   *
   * @param {boolean} noMipmap- When true, no mipmaps shall be generated.
   * Ignored for compressed textures.  They must be in the file.
   * @param {boolean} invertY- When true, image is flipped when loaded.  You
   * probably want true. Ignored for compressed textures.  Must be flipped in
   * the file.
   * @param {Scene} scene- Needed for loading to the correct scene.
   * @param {number} samplingMode- Mode with should be used sample / access the
   * texture. Default: TRILINEAR
   * @param {callback} onLoad- Optional callback to be called upon successful
   * completion.
   * @param {callback} onError- Optional callback to be called upon failure.
   * @param {ArrayBuffer | HTMLImageElement} buffer- A source of a file
   * previously fetched as either an ArrayBuffer (compressed or image format) or
   * HTMLImageElement (image format)
   * @param {WebGLTexture} fallback- An internal argument in case the function
   * must be called again, due to etc1 not having alpha capabilities.
   * @param {number} format-  Internal format.  Default: RGB when extension is
   * '.jpg' else RGBA.  Ignored for compressed textures.
   *
   * @returns {WebGLTexture} for assignment back into BABYLON.Texture
   */
  InternalTexture* createTexture(
    const string_t& urlArg, bool noMipmap, bool invertY, Scene* scene,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    const ::std::function<void(InternalTexture*, EventState&)>& onLoad
    = nullptr,
    const ::std::function<void()>& onError    = nullptr,
    const Variant<ArrayBuffer, Image>& buffer = Variant<ArrayBuffer, Image>(),
    InternalTexture* fallBack                 = nullptr,
    unsigned int format = EngineConstants::TEXTUREFORMAT_RGBA);
  void updateRawTexture(InternalTexture* texture, const Uint8Array& data,
                        unsigned int format, bool invertY = true,
                        const string_t& compression = "",
                        unsigned int type
                        = EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  InternalTexture*
  createRawTexture(const Uint8Array& data, int width, int height,
                   unsigned int format, bool generateMipMaps, bool invertY,
                   unsigned int samplingMode, const string_t& compression = "",
                   unsigned int type
                   = EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  InternalTexture* createDynamicTexture(int width, int height,
                                        bool generateMipMaps,
                                        unsigned int samplingMode);
  void updateTextureSamplingMode(unsigned int samplingMode,
                                 InternalTexture* texture);
  void updateDynamicTexture(InternalTexture* texture, ICanvas* canvas,
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
  void updateTextureComparisonFunction(InternalTexture* texture,
                                       int comparisonFunction);

  /**
   * @brief Creates a depth stencil texture.
   * This is only available in WebGL 2 or with the depth texture extension
   * available.
   * @param size The size of face edge in the texture.
   * @param options The options defining the texture.
   * @returns The texture
   */
  unique_ptr_t<InternalTexture>
  createDepthStencilTexture(const Variant<int, ISize>& size,
                            const DepthTextureCreationOptions& options);

  /**
   * @brief Sets the frame buffer Depth / Stencil attachement of the render
   * target to the defined depth stencil texture.
   * @param renderTarget The render target to set the frame buffer for
   */
  void setFrameBufferDepthStencilTexture(RenderTargetTexture* renderTarget);

  InternalTexture*
  createRenderTargetTexture(ISize size, const IRenderTargetOptions& options);
  vector_t<InternalTexture*>
  createMultipleRenderTarget(ISize size,
                             const IMultiRenderTargetOptions& options);
  unsigned int updateRenderTargetTextureSampleCount(InternalTexture* texture,
                                                    unsigned int samples);
  unsigned int updateMultipleRenderTargetTextureSampleCount(
    const vector_t<InternalTexture*>& textures, unsigned int samples);
  void _uploadDataToTexture(unsigned int target, int lod, int internalFormat,
                            int width, int height, unsigned int format,
                            unsigned int type, const Uint8Array& data);
  void _uploadCompressedDataToTexture(unsigned int target, int lod,
                                      unsigned int internalFormat, int width,
                                      int height, const Uint8Array& data);
  InternalTexture*
  createRenderTargetCubeTexture(const ISize& size,
                                const RenderTargetCreationOptions& options);
  InternalTexture* createPrefilteredCubeTexture(
    const string_t& rootUrl, Scene* scene, float scale, float offset,
    const ::std::function<void(InternalTexture*, EventState&)>& onLoad
    = nullptr,
    const ::std::function<void()>& onError = nullptr, unsigned int format = 0,
    const string_t& forcedExtension = "");
  InternalTexture* createCubeTexture(
    const string_t& rootUrl, Scene* scene, const vector_t<string_t>& extensions,
    bool noMipmap,
    const ::std::function<void(InternalTexture*, EventState&)>& onLoad
    = nullptr,
    const ::std::function<void()>& onError = nullptr, unsigned int format = 0,
    const string_t& forcedExtension = "");
  void updateRawCubeTexture(InternalTexture* texture,
                            const vector_t<Uint8Array>& data,
                            unsigned int format, unsigned int type,
                            bool invertY                = true,
                            const string_t& compression = "",
                            unsigned int level          = 0);
  InternalTexture* createRawCubeTexture(const vector_t<Uint8Array> data,
                                        int size, unsigned int format,
                                        unsigned int type, bool generateMipMaps,
                                        bool invertY, unsigned int samplingMode,
                                        const string_t& compression = "");
  InternalTexture* createRawCubeTextureFromUrl(
    const string_t& url, Scene* scene, int size, unsigned int format,
    unsigned int type, bool noMipmap,
    const ::std::function<ArrayBufferViewArray(const Uint8Array& arrayBuffer)>&
      callback,
    const ::std::function<vector_t<ArrayBufferViewArray>(
      const ArrayBufferViewArray& faces)>& mipmmapGenerator,
    const ::std::function<void()>& onLoad  = nullptr,
    const ::std::function<void()>& onError = nullptr,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    bool invertY              = false);
  void updateRawTexture3D(InternalTexture* texture, const ArrayBufferView& data,
                          unsigned int format, bool invertY = true,
                          const string_t& compression = "");
  InternalTexture* createRawTexture3D(const ArrayBufferView& data, int width,
                                      int height, int depth,
                                      unsigned int format, bool generateMipMaps,
                                      bool invertY, unsigned int samplingMode,
                                      const string_t& compression = "");
  void _releaseFramebufferObjects(InternalTexture* texture);
  void _releaseTexture(InternalTexture* texture);
  void bindSamplers(Effect* effect);
  void _bindTexture(int channel, InternalTexture* texture);
  void setTextureFromPostProcess(int channel, PostProcess* postProcess);

  /**
   * @brief Binds the output of the passed in post process to the texture
   * channel specified.
   * @param channel The channel the texture should be bound to
   * @param postProcess The post process which's output should be bound
   */
  void setTextureFromPostProcessOutput(int channel, PostProcess* postProcess);

  void unbindAllTextures();

  /**
   * @brief Sets a texture to the according uniform.
   * @param channel The texture channel
   * @param uniform The uniform to set
   * @param texture The texture to apply
   */
  void setTexture(int channel, GL::IGLUniformLocation* uniform,
                  BaseTexture* texture);

  /**
   * @brief Sets a depth stencil texture from a render target to the according
   * uniform.
   * @param channel The texture channel
   * @param uniform The uniform to set
   * @param texture The render target texture containing the depth stencil
   * texture to apply
   */
  void setDepthStencilTexture(int channel, GL::IGLUniformLocation* uniform,
                              RenderTargetTexture* texture);

  void setTextureArray(int channel, GL::IGLUniformLocation* uniform,
                       const vector_t<BaseTexture*>& textures);
  void _setAnisotropicLevel(unsigned int key, BaseTexture* texture);
  Uint8Array readPixels(int x, int y, int width, int height);
  void unbindAllAttributes();
  void releaseEffects();

  /** Dispose **/
  void dispose();

  /** Loading screen **/
  void displayLoadingUI();
  void hideLoadingUI();
  ILoadingScreen* loadingScreen();
  void setLoadingScreen(ILoadingScreen* loadingScreen);
  void setLoadingUIText(const string_t& text);
  void loadingUIBackgroundColor(const string_t& color);
  string_t getVertexShaderSource(GL::IGLProgram* program);
  string_t getFragmentShaderSource(GL::IGLProgram* program);
  unsigned int getError() const;

  /** FPS **/
  float getFps() const;
  float getDeltaTime() const;

  /** Texture helper functions **/
  ArrayBufferView _readTexturePixels(InternalTexture* texture, int width,
                                     int height, int faceIndex = -1);
  GL::GLenum _getWebGLTextureType(unsigned int type) const;
  GL::GLenum _getRGBABufferInternalSizedFormat(
    unsigned int type, const Nullable<unsigned int>& format = nullptr) const;
  GL::GLenum _getRGBAMultiSampleBufferFormat(unsigned int type) const;

  /** Occlusion Queries **/
  GLQueryPtr createQuery();
  Engine& deleteQuery(const GLQueryPtr& query);
  bool isQueryResultAvailable(const GLQueryPtr& query);
  unsigned int getQueryResult(const GLQueryPtr& query);
  Engine& beginOcclusionQuery(unsigned int algorithmType,
                              const GLQueryPtr& query);
  Engine& endOcclusionQuery(unsigned int algorithmType);

  /** Time queries **/
  Nullable<_TimeToken> startTimeQuery();
  int endTimeQuery(Nullable<_TimeToken>& token);

  /** Transform feedback **/
  GLTransformFeedbackPtr createTransformFeedback();
  void deleteTransformFeedback(GL::IGLTransformFeedback* value);
  void bindTransformFeedback(GL::IGLTransformFeedback* value);
  void beginTransformFeedback(bool usePoints = true);
  void endTransformFeedback();
  void setTranformFeedbackVaryings(GL::IGLProgram* program,
                                   const vector_t<string_t>& value);
  void bindTransformFeedbackBuffer(GL::IGLBuffer* value);
  IFileRequest _loadFile(
    const string_t& url,
    const ::std::function<void(Variant<string_t, ArrayBuffer>& data,
                               const string_t& responseURL)>& onSuccess);
  Promise<string_t> _loadFileAsync(const string_t& url);

  /** Statics **/
  static Engine* LastCreatedEngine();
  static Scene* LastCreatedScene();

  /**
   * @brief Will flag all materials in all scenes in all engines as dirty to
   * trigger new shader compilation
   */
  static void
  MarkAllMaterialsAsDirty(unsigned int flag,
                          const ::std::function<bool(Material* mat)>& predicate
                          = nullptr);
  static bool isSupported();
  static GLShaderPtr CompileShader(GL::IGLRenderingContext* gl,
                                   const string_t& source, const string_t& type,
                                   const string_t& defines,
                                   const string_t& shaderVersion);
  static GLShaderPtr CompileRawShader(GL::IGLRenderingContext* gl,
                                      const string_t& source,
                                      const string_t& type);
  static SamplingParameters GetSamplingParameters(unsigned int samplingMode,
                                                  bool generateMipMaps);

protected:
  /**
   * @param canvasOrContext defines the canvas or WebGL context to use for
   * rendering
   * @param antialias defines enable antialiasing (default: false)
   * @param options defines further options to be sent to the getContext()
   * function
   * @param adaptToDeviceRatio defines whether to adapt to the device's viewport
   * characteristics (default: false)
   */
  Engine(ICanvas* canvas, const EngineOptions& options = EngineOptions());

  void _bindTextureDirectly(unsigned int target, InternalTexture* texture,
                            bool forTextureDataUpdate = false);

private:
  void _rebuildInternalTextures();
  void _rebuildEffects();
  void _rebuildBuffers();
  void _initGLContext();
  void _onVRFullScreenTriggered();
  void _getVRDisplays();
  void _bindSamplerUniformToChannel(int sourceSlot, int destination);
  bool _setTexture(int channel, BaseTexture* texture,
                   bool isPartOfTextureArray = false,
                   bool depthStencilTexture  = false);
  void bindUnboundFramebuffer(GL::IGLFramebuffer* framebuffer);
  void bindIndexBuffer(GL::IGLBuffer* buffer);
  void bindBuffer(GL::IGLBuffer* buffer, int target);
  void vertexAttribPointer(GL::IGLBuffer* buffer, unsigned int indx, int size,
                           unsigned int type, bool normalized, int stride,
                           int offset);
  void _bindIndexBufferWithCache(GL::IGLBuffer* indexBuffer);
  void _bindVertexBuffersAttributes(
    const unordered_map_t<string_t, VertexBuffer*>& vertexBuffers,
    Effect* effect);
  void _unbindVertexArrayObject();
  void setProgram(GL::IGLProgram* program);
  void _moveBoundTextureOnTop(InternalTexture* internalTexture);
  int _getCorrectTextureChannel(int channel,
                                InternalTexture* internalTexture = nullptr);
  void _linkTrackers(IInternalTextureTracker* previous,
                     IInternalTextureTracker* next);
  int _removeDesignatedSlot(InternalTexture* internalTexture);
  void _activateCurrentTexture();
  void _rescaleTexture(InternalTexture* source, InternalTexture* destination,
                       Scene* scene, unsigned int internalFormat,
                       const ::std::function<void()>& onComplete);
  void _setupDepthStencilTexture(InternalTexture* internalTexture,
                                 const Variant<int, ISize>& size,
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
  unique_ptr_t<InternalTexture>
  _createDepthStencilTexture(const Variant<int, ISize>& size,
                             const DepthTextureCreationOptions& options);

  /**
   * @brief Creates a depth stencil cube texture.
   * This is only available in WebGL 2.
   * @param size The size of face edge in the cube texture.
   * @param options The options defining the cube texture.
   * @returns The cube texture
   */
  unique_ptr_t<InternalTexture>
  _createDepthStencilCubeTexture(int size,
                                 const DepthTextureCreationOptions& options);

  unsigned int _drawMode(unsigned int fillMode);
  GLProgramPtr
  _createShaderProgram(const unique_ptr_t<GL::IGLShader>& vertexShader,
                       const unique_ptr_t<GL::IGLShader>& fragmentShader,
                       GL::IGLRenderingContext* context,
                       const vector_t<string_t>& transformFeedbackVaryings
                       = {});
  void setCubeMapTextureParams(GL::IGLRenderingContext* gl, bool loadMipmap);
  GLRenderBufferPtr
  _setupFramebufferDepthAttachments(bool generateStencilBuffer,
                                    bool generateDepthBuffer, int width,
                                    int height, int samples = 1);
  void _prepareWebGLTextureContinuation(InternalTexture* texture, Scene* scene,
                                        bool noMipmap, bool isCompressed,
                                        unsigned int samplingMode);
  void _prepareWebGLTexture(
    InternalTexture* texture, Scene* scene, int width, int height,
    Nullable<bool> invertY, bool noMipmap, bool isCompressed,
    const ::std::function<bool(
      int width, int height,
      const ::std::function<void()>& continuationCallback)>& processFunction,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE);
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

public:
  // Public members
  bool forcePOTTextures;
  bool isFullscreen;
  bool isPointerLock;
  bool cullBackFaces;
  bool renderEvenInBackground;
  bool preventCacheWipeBetweenFrames;
  // To enable/disable IDB support and avoid XHR on .manifest
  bool enableOfflineSupport;
  vector_t<Scene*> scenes;
  vector_t<PostProcess*> postProcesses;

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
   * Gets or sets a value indicating if we want to disable texture binding
   * optmization. This could be required on some buggy drivers which wants to
   * have textures bound in a progressive order By default Babylon.js will try
   * to let textures bound where they are and only update the samplers to point
   * where the texture is.
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
  bool disableUniformBuffers;
  vector_t<UniformBuffer*> _uniformBuffers;

  static AudioEngine* audioEngine;
  PerfCounter _drawCalls;
  PerfCounter _textureCollisions;

  // Lost context
  Observable<Engine> onContextLostObservable;
  Observable<Engine> onContextRestoredObservable;

  /**
   * Turn this value on if you want to pause FPS computation when in background
   */
  bool disablePerformanceMonitorInBackground;

protected:
  EngineCapabilities _caps;

  // States
  unique_ptr_t<_DepthCullingState> _depthCullingState;
  unique_ptr_t<_StencilState> _stencilState;
  unique_ptr_t<_AlphaState> _alphaState;
  unsigned int _alphaMode;

  // Cache
  int _activeChannel;
  unordered_map_t<int, InternalTexture*> _boundTexturesCache;
  Effect* _currentEffect;
  GL::IGLProgram* _currentProgram;
  Viewport* _cachedViewport;
  unordered_map_t<string_t, VertexBuffer*> _cachedVertexBuffersMap;
  GL::IGLBuffer* _cachedVertexBuffers;
  GL::IGLBuffer* _cachedIndexBuffer;
  Effect* _cachedEffectForVertexBuffers;
  InternalTexture* _currentRenderTarget;
  GL::IGLFramebuffer* _currentFramebuffer;

private:
  // WebVR
  bool _vrExclusivePointerMode;
  // Private Members
  GL::IGLRenderingContext* _gl;
  ICanvas* _renderingCanvas;
  bool _windowIsBackground;
  float _webGLVersion;

  bool _badOS;
  bool _badDesktopOS;

  // Focus
  ::std::function<void()> _onFocus;
  ::std::function<void()> _onBlur;
  ::std::function<void()> _onCanvasPointerOut;
  ::std::function<void()> _onCanvasBlur;

  ::std::function<void()> _onCanvasFocus;

  ::std::function<void()> _onFullscreenChange;
  ::std::function<void()> _onPointerLockChange;

  int _hardwareScalingLevel;
  bool _pointerLockRequested;
  bool _isStencilEnable;
  bool _colorWrite;

  ILoadingScreen* _loadingScreen;

  string_t _glVersion;
  string_t _glRenderer;
  string_t _glVendor;

  bool _videoTextureSupported;

  bool _renderingQueueLaunched;
  vector_t<delegate_t<void()>> _activeRenderLoops;

  // Deterministic lockstepMaxSteps
  bool _deterministicLockstep;
  unsigned int _lockstepMaxSteps;

  // Lost context
  ::std::function<void(Event&& evt)> _onContextLost;
  ::std::function<void(Event&& evt)> _onContextRestored;
  bool _contextWasLost;
  bool _doNotHandleContextLost;

  // FPS
  unique_ptr_t<PerformanceMonitor> _performanceMonitor;
  float _fps;
  float _deltaTime;

  // Cache
  vector_t<unique_ptr_t<InternalTexture>> _internalTexturesCache;
  int _currentTextureChannel;
  unordered_map_t<string_t, unique_ptr_t<Effect>> _compiledEffects;
  vector_t<bool> _vertexAttribArraysEnabled;
  GL::IGLVertexArrayObject* _cachedVertexArrayObject;
  bool _uintIndicesCurrentlySet;
  unordered_map_t<int, GL::IGLBuffer*> _currentBoundBuffer;
  unordered_map_t<unsigned int, BufferPointer> _currentBufferPointers;
  Int32Array _currentInstanceLocations;
  vector_t<GL::IGLBuffer*> _currentInstanceBuffers;
  Int32Array _textureUnits;
  unique_ptr_t<DummyInternalTextureTracker> _firstBoundInternalTextureTracker;
  unique_ptr_t<DummyInternalTextureTracker> _lastBoundInternalTextureTracker;
  ICanvas* _workingCanvas;
  ICanvasRenderingContext2D* _workingContext;
  unique_ptr_t<PassPostProcess> _rescalePostProcess;
  unique_ptr_t<GL::IGLFramebuffer> _dummyFramebuffer;
  ::std::function<void()> _bindedRenderFunction;
  bool _vaoRecordInProgress;
  bool _mustWipeVertexAttributes;
  InternalTexture* _emptyTexture;
  unique_ptr_t<InternalTexture> _emptyCubeTexture;
  InternalTexture* _emptyTexture3D;
  int _frameHandler;
  // Hardware supported Compressed Textures
  vector_t<string_t> _texturesSupported;
  Int32Array _nextFreeTextureSlots;
  unsigned int _maxSimultaneousTextures;
  string_t _textureFormatInUse;
  unordered_map_t<int, GL::IGLUniformLocation*> _boundUniforms;

}; // end of class Engine

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_ENGINE_H
