#ifndef BABYLON_ENGINES_NULL_ENGINE_H
#define BABYLON_ENGINES_NULL_ENGINE_H

#include <babylon/babylon_api.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/null_engine_options.h>

namespace BABYLON {

/**
 * @brief The null engine class provides support for headless version of babylon.js.
 * This can be used in server side scenario or for testing purposes
 */
class BABYLON_SHARED_EXPORT NullEngine : public Engine {

public:
  template <typename... Ts>
  static std::unique_ptr<NullEngine> New(Ts&&... args)
  {
    std::unique_ptr<NullEngine> engine(new NullEngine(std::forward<Ts>(args)...));
    return engine;
  }
  ~NullEngine() override; // = default

  /**
   * @see Gets a boolean indicating that the engine is running in deterministic lock step mode.
   * @see https://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   * @returns true if engine is in deterministic lock step mode
   */
  bool isDeterministicLockStep() const override;

  /**
   * @see Gets the max steps when engine is running in deterministic lock step.
   * @see https://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   * @returns the max steps
   */
  unsigned int getLockstepMaxSteps() const override;

  /**
   * @brief Gets the current hardware scaling level.
   * By default the hardware scaling level is computed from the window device ratio.
   * if level = 1 then the engine will render at the exact resolution of the canvas. If level = 0.5
   * then the engine will render at twice the size of the canvas.
   * @returns a number indicating the current hardware scaling level
   */
  float getHardwareScalingLevel() const override;

  /**
   * @brief Creates a vertex buffer
   * @param vertices the data for the vertex buffer
   * @returns the new WebGL static buffer
   */
  WebGLDataBufferPtr createVertexBuffer(const Float32Array& vertices) override;

  /**
   * @brief Creates a new index buffer.
   * @param indices defines the content of the index buffer
   * @param updatable defines if the index buffer must be updatable
   * @returns a new webGL buffer
   */
  WebGLDataBufferPtr createIndexBuffer(const IndicesArray& indices,
                                       bool updatable = false) override;

  /**
   * @brief Clear the current render buffer or the current render target (if any is set up).
   * @param color defines the color to use
   * @param backBuffer defines if the back buffer must be cleared
   * @param depth defines if the depth buffer must be cleared
   * @param stencil defines if the stencil buffer must be cleared
   */
  void clear(const std::optional<Color4>& color, bool backBuffer, bool depth,
             bool stencil = false) override;

  /**
   * @brief Gets the current render width.
   * @param useScreen defines if screen size must be used (or the current render target if any)
   * @returns a number defining the current render width
   */
  int getRenderWidth(bool useScreen = false) const override;

  /**
   * @brief Gets the current render height.
   * @param useScreen defines if screen size must be used (or the current render target if any)
   * @returns a number defining the current render height
   */
  int getRenderHeight(bool useScreen = false) const override;

  /**
   * @brief Set the WebGL's viewport.
   * @param viewport defines the viewport element to be used
   * @param requiredWidth defines the width required for rendering. If not provided the rendering
   * canvas' width is used
   * @param requiredHeight defines the height required for rendering. If not provided the rendering
   * canvas' height is used
   */
  void setViewport(const Viewport& viewport, const std::optional<int>& requiredWidth = std::nullopt,
                   const std::optional<int>& requiredHeight = std::nullopt) override;

  /**
   * @brief Hidden
   */
  GL::IGLProgramPtr
  createShaderProgram(const IPipelineContextPtr& pipelineContext, const std::string& vertexCode,
                      const std::string& fragmentCode, const std::string& defines,
                      GL::IGLRenderingContext* context                          = nullptr,
                      const std::vector<std::string>& transformFeedbackVaryings = {}) override;

  /**
   * @brief Gets the list of webGL uniform locations associated with a specific program based on a
   * list of uniform names.
   * @param pipelineContext defines the pipeline context to use
   * @param uniformsNames defines the list of uniform names
   * @returns an array of webGL uniform locations
   */
  std::unordered_map<std::string, WebGLUniformLocationPtr>
  getUniforms(const IPipelineContextPtr& pipelineContext,
              const std::vector<std::string>& uniformsNames) override;

  /**
   * @brief Gets the lsit of active attributes for a given webGL program
   * @param pipelineContext defines the pipeline context to use
   * @param attributesNames defines the list of attribute names to get
   * @returns an array of indices indicating the offset of each attribute
   */
  Int32Array getAttributes(const IPipelineContextPtr& pipelineContext,
                           const std::vector<std::string>& attributesNames) override;

  /**
   * @brief Binds an effect to the webGL context.
   * @param effect defines the effect to bind
   */
  void bindSamplers(Effect& effect) override;

  /**
   * @brief Activates an effect, mkaing it the current one (ie. the one used for rendering).
   * @param effect defines the effect to activate
   */
  void enableEffect(const EffectPtr& effect) override;

  /**
   * @brief Set various states to the webGL context.
   * @param culling defines backface culling state
   * @param zOffset defines the value to apply to zOffset (0 by default)
   * @param force defines if states must be applied even if cache is up to date
   * @param reverseSide defines if culling must be reversed (CCW instead of CW and CW instead of
   * CCW)
   */
  void setState(bool culling, float zOffset = 0.f, bool force = false,
                bool reverseSide = false) override;

  /**
   * @brief Set the value of an uniform to an array of int32.
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   * @returns true if the value was set
   */
  bool setIntArray(const WebGLUniformLocationPtr& uniform, const Int32Array& array) override;

  /**
   * @brief Set the value of an uniform to an array of int32 (stored as vec2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   * @returns true if the value was set
   */
  bool setIntArray2(const WebGLUniformLocationPtr& uniform, const Int32Array& array) override;

  /**
   * @brief Set the value of an uniform to an array of int32 (stored as vec3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   * @returns true if the value was set
   */
  bool setIntArray3(const WebGLUniformLocationPtr& uniform, const Int32Array& array) override;

  /**
   * @brief Set the value of an uniform to an array of int32 (stored as vec4).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of int32 to store
   * @returns true if the value was set
   */
  bool setIntArray4(const WebGLUniformLocationPtr& uniform, const Int32Array& array) override;

  /**
   * @brief Set the value of an uniform to an array of float32.
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of float32 to store
   */
  void setFloatArray(const WebGLUniformLocationPtr& uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of float32 (stored as vec2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of float32 to store
   */
  void setFloatArray2(const WebGLUniformLocationPtr& uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of float32 (stored as vec3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of float32 to store
   */
  void setFloatArray3(const WebGLUniformLocationPtr& uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of float32 (stored as vec4).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of float32 to store
   */
  void setFloatArray4(const WebGLUniformLocationPtr& uniform, const Float32Array& array);

  /**
   * @brief Set the value of an uniform to an array of number.
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   * @returns true if the value was set
   */
  bool setArray(const WebGLUniformLocationPtr& uniform, const Float32Array& array) override;

  /**
   * @brief Set the value of an uniform to an array of number (stored as vec2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   * @returns true if the value was set
   */
  bool setArray2(const WebGLUniformLocationPtr& uniform, const Float32Array& array) override;

  /**
   * @brief Set the value of an uniform to an array of number (stored as vec3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   * @returns true if the value was set
   */
  bool setArray3(const WebGLUniformLocationPtr& uniform, const Float32Array& array) override;

  /**
   * @brief Set the value of an uniform to an array of number (stored as vec4).
   * @param uniform defines the webGL uniform location where to store the value
   * @param array defines the array of number to store
   * @returns true if the value was set
   */
  bool setArray4(const WebGLUniformLocationPtr& uniform, const Float32Array& array) override;

  /**
   * @brief Set the value of an uniform to an array of float32 (stored as matrices).
   * @param uniform defines the webGL uniform location where to store the value
   * @param matrices defines the array of float32 to store
   * @returns true if the value was set
   */
  bool setMatrices(const WebGLUniformLocationPtr& uniform, const Float32Array& matrices) override;

  /**
   * @brief Set the value of an uniform to a matrix (3x3).
   * @param uniform defines the webGL uniform location where to store the value
   * @param matrix defines the Float32Array representing the 3x3 matrix to store
   * @returns true if the value was set
   */
  bool setMatrix3x3(const WebGLUniformLocationPtr& uniform, const Float32Array& matrix) override;

  /**
   * @brief Set the value of an uniform to a matrix (2x2).
   * @param uniform defines the webGL uniform location where to store the value
   * @param matrix defines the Float32Array representing the 2x2 matrix to store
   * @returns true if the value was set
   */
  bool setMatrix2x2(const WebGLUniformLocationPtr& uniform, const Float32Array& matrix) override;

  /**
   * @brief Set the value of an uniform to a number (float).
   * @param uniform defines the webGL uniform location where to store the value
   * @param value defines the float number to store
   * @returns true if the value was transfered
   */
  bool setFloat(const WebGLUniformLocationPtr& uniform, float value) override;

  /**
   * @brief Set the value of an uniform to a vec2.
   * @param uniform defines the webGL uniform location where to store the value
   * @param x defines the 1st component of the value
   * @param y defines the 2nd component of the value
   * @returns true if the value was set
   */
  bool setFloat2(const WebGLUniformLocationPtr& uniform, float x, float y) override;

  /**
   * @brief Set the value of an uniform to a vec3.
   * @param uniform defines the webGL uniform location where to store the value
   * @param x defines the 1st component of the value
   * @param y defines the 2nd component of the value
   * @param z defines the 3rd component of the value
   * @returns true if the value was set
   */
  bool setFloat3(const WebGLUniformLocationPtr& uniform, float x, float y, float z) override;

  /**
   * @brief Set the value of an uniform to a boolean.
   * @param uniform defines the webGL uniform location where to store the value
   * @param bool defines the boolean to store
   */
  void setBool(const WebGLUniformLocationPtr& uniform, int value);

  /**
   * @brief Set the value of an uniform to a vec4.
   * @param uniform defines the webGL uniform location where to store the value
   * @param x defines the 1st component of the value
   * @param y defines the 2nd component of the value
   * @param z defines the 3rd component of the value
   * @param w defines the 4th component of the value
   * @returns true if the value was set
   */
  bool setFloat4(const WebGLUniformLocationPtr& uniform, float x, float y, float z,
                 float w) override;

  /**
   * @brief Sets the current alpha mode.
   * @param mode defines the mode to use (one of the Engine.ALPHA_XXX)
   * @param noDepthWriteChange defines if depth writing state should remains unchanged (false by
   * default)
   * @see https://doc.babylonjs.com/resources/transparency_and_how_meshes_are_rendered
   */
  void setAlphaMode(unsigned int mode, bool noDepthWriteChange = false) override;

  /**
   * @brief Bind webGl buffers directly to the webGL context.
   * @param vertexBuffers defines the vertex buffer to bind
   * @param indexBuffer defines the index buffer to bind
   * @param vertexDeclaration defines the vertex declaration to use with the vertex buffer
   * @param vertexStrideSize defines the vertex stride of the vertex buffer
   * @param effect defines the effect associated with the vertex buffer
   */
  void bindBuffers(const std::unordered_map<std::string, VertexBufferPtr>& vertexBuffers,
                   const WebGLDataBufferPtr& indexBuffer, const EffectPtr& effect) override;

  /**
   * @brief Force the entire cache to be cleared.
   * You should not have to use this function unless your engine needs to share the webGL context
   * with another engine
   * @param bruteForce defines a boolean to force clearing ALL caches (including stencil, detoh and
   * alpha states)
   */
  void wipeCaches(bool bruteForce = false) override;

  /**
   * @brief Send a draw order
   * @param useTriangles defines if triangles must be used to draw (else wireframe will be used)
   * @param indexStart defines the starting index
   * @param indexCount defines the number of index to draw
   * @param instancesCount defines the number of instances to draw (if instanciation is enabled)
   */
  void draw(bool useTriangles, int indexStart, int indexCount, int instancesCount = 0) override;

  /**
   * @brief Draw a list of indexed primitives.
   * @param fillMode defines the primitive to use
   * @param indexStart defines the starting index
   * @param indexCount defines the number of index to draw
   * @param instancesCount defines the number of instances to draw (if instanciation is enabled)
   */
  void drawElementsType(unsigned int fillMode, int indexStart, int verticesCount,
                        int instancesCount = 0) override;

  /**
   * @brief Draw a list of unindexed primitives.
   * @param fillMode defines the primitive to use
   * @param verticesStart defines the index of first vertex to draw
   * @param verticesCount defines the count of vertices to draw
   * @param instancesCount defines the number of instances to draw (if instanciation is enabled)
   */
  void drawArraysType(unsigned int fillMode, int verticesStart, int verticesCount,
                      int instancesCount = 0) override;

  /**
   * @brief Hidden
   */
  WebGLTexturePtr _createTexture() override;

  /**
   * @brief Hidden
   */
  void _releaseTexture(const InternalTexturePtr& texture) override;

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
   * @param mimeType defines an optional mime type
   * @returns a InternalTexture for assignment back into BABYLON.Texture
   */
  InternalTexturePtr createTexture(
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
    const LoaderOptionsPtr& loaderOptions = nullptr) override;

  /**
   * @brief Creates a new render target texture
   * @param size defines the size of the texture
   * @param options defines the options used to create the texture
   * @returns a new render target texture stored in an InternalTexture
   */
  InternalTexturePtr
  createRenderTargetTexture(const std::variant<int, RenderTargetSize, float>& size,
                            const IRenderTargetOptions& options) override;

  /**
   * @brief Update the sampling mode of a given texture.
   * @param samplingMode defines the required sampling mode
   * @param texture defines the texture to update
   */
  void updateTextureSamplingMode(unsigned int samplingMode, const InternalTexturePtr& texture,
                                 bool generateMipMaps = false) override;

  /**
   * @brief Binds the frame buffer to the specified texture.
   * @param texture The texture to render to or null for the default canvas
   * @param faceIndex The face of the texture to render to in case of cube texture
   * @param requiredWidth The width of the target to render to
   * @param requiredHeight The height of the target to render to
   * @param forceFullscreenViewport Forces the viewport to be the entire texture/screen if true
   * @param lodLevel defines le lod level to bind to the frame buffer
   */
  void bindFramebuffer(const InternalTexturePtr& texture, unsigned int faceIndex = 0,
                       std::optional<int> requiredWidth            = std::nullopt,
                       std::optional<int> requiredHeight           = std::nullopt,
                       std::optional<bool> forceFullscreenViewport = std::nullopt, int lodLevel = 0,
                       int layer = 0) override;

  /**
   * @brief Unbind the current render target texture from the webGL context.
   * @param texture defines the render target texture to unbind
   * @param disableGenerateMipMaps defines a boolean indicating that mipmaps must not be generated
   * @param onBeforeUnbind defines a function which will be called before the effective unbind
   */
  void unBindFramebuffer(const InternalTexturePtr& texture, bool disableGenerateMipMaps = false,
                         const std::function<void()>& onBeforeUnbind = nullptr) override;

  /**
   * @brief Creates a dynamic vertex buffer.
   * @param vertices the data for the dynamic vertex buffer
   * @returns the new WebGL dynamic buffer
   */
  WebGLDataBufferPtr createDynamicVertexBuffer(const Float32Array& vertices) override;

  /**
   * @brief Update the content of a dynamic texture.
   * @param texture defines the texture to update
   * @param canvas defines the canvas containing the source
   * @param invertY defines if data must be stored with Y axis inverted
   * @param premulAlpha defines if alpha is stored as premultiplied
   * @param format defines the format of the data
   * @param forceBindTexture if the texture should be forced to be bound eg. after a graphics
   * context loss (Default: false)
   */
  void updateDynamicTexture(const InternalTexturePtr& texture, ICanvas* canvas, bool invertY,
                            bool premulAlpha                   = false,
                            std::optional<unsigned int> format = std::nullopt,
                            bool forceBindTexture              = false) override;

  /**
   * @brief Gets a boolean indicating if all created effects are ready.
   * @returns true if all effects are ready
   */
  bool areAllEffectsReady() const override;

  /**
   * @brief Hidden
   */
  unsigned int getError() const override;

  /**
   * @brief Hidden
   */
  int _getUnpackAlignement() override;

  /**
   * @brief Hidden
   */
  void _unpackFlipY(bool value) override;

  /**
   * @brief Update a dynamic index buffer.
   * @param indexBuffer defines the target index buffer
   * @param indices defines the data to update
   * @param offset defines the offset in the target index buffer where update should start
   */
  void updateDynamicIndexBuffer(const WebGLDataBufferPtr& indexBuffer, const IndicesArray& indices,
                                int offset = 0) override;

  /**
   * @brief Updates a dynamic vertex buffer.
   * @param vertexBuffer the vertex buffer to update
   * @param vertices the data used to update the vertex buffer
   * @param byteOffset the byte offset of the data (optional)
   * @param byteLength the byte length of the data (optional)
   */
  void updateDynamicVertexBuffer(const WebGLDataBufferPtr& vertexBuffer, const Float32Array& data,
                                 int byteOffset = -1, int byteLength = -1) override;

  /**
   * @brief Hidden
   */
  bool _bindTextureDirectly(unsigned int target, const InternalTexturePtr& texture,
                            bool forTextureDataUpdate = false, bool force = false) override;

  /**
   * @brief Hidden
   */
  void _bindTexture(int channel, const InternalTexturePtr& texture) override;

  /**
   * @brief Force the engine to release all cached effects. This means that next effect compilation
   * will have to be done completely even if a similar effect was already compiled.
   */
  void releaseEffects() override;

  /**
   * @brief Hidden
   */
  void displayLoadingUI() override;

  /**
   * @brief Hidden
   */
  void hideLoadingUI() override;

  /**
   * @brief Hidden
   */
  void _uploadCompressedDataToTextureDirectly(const InternalTexturePtr& texture,
                                              unsigned int internalFormat, int width, int height,
                                              const Uint8Array& data, unsigned int faceIndex = 0,
                                              int lod = 0) override;

  /**
   * @brief Hidden
   */
  void _uploadDataToTextureDirectly(const InternalTexturePtr& texture,
                                    const ArrayBufferView& imageData, unsigned int faceIndex = 0,
                                    int lod = 0, int babylonInternalFormat = -1,
                                    bool useTextureWidthAndHeight = false) override;

  /**
   * @brief Hidden
   */
  void _uploadArrayBufferViewToTexture(const InternalTexturePtr& texture,
                                       const Uint8Array& imageData, unsigned int faceIndex = 0,
                                       int lod = 0) override;

  /**
   * @brief Hidden
   */
  void _uploadImageToTexture(const InternalTexturePtr& texture, const Image& image,
                             unsigned int faceIndex = 0, int lod = 0) override;

protected:
  NullEngine(const NullEngineOptions& options = NullEngineOptions{});

  void _deleteBuffer(GL::IGLBuffer* buffer);

private:
  NullEngineOptions _options;

}; // end of class Engine

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_ENGINE_H
