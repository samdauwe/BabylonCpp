#ifndef BABYLON_ENGINES_NULL_ENGINE_H
#define BABYLON_ENGINES_NULL_ENGINE_H

#include <babylon/babylon_api.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/null_engine_options.h>

namespace BABYLON {

/**
 * @brief The null engine class provides support for headless version of
 * babylon.js. This can be used in server side scenario or for testing purposes.
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
   * @see https://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   */
  [[nodiscard]] bool isDeterministicLockStep() const;

  /**
   * @see https://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   */
  [[nodiscard]] bool getLockstepMaxSteps() const;

  /**
   * @brief Sets hardware scaling, used to save performance if needed.
   * @see https://doc.babylonjs.com/how_to/how_to_use_sceneoptimizer
   */
  [[nodiscard]] float getHardwareScalingLevel() const;

  GLBufferPtr createVertexBuffer(const Float32Array& vertices) override;
  GLBufferPtr createIndexBuffer(const IndicesArray& indices, bool updatable = false) override;
  void clear(const Color4& color, bool backBuffer, bool depth, bool stencil = false) override;
  [[nodiscard]] int getRenderWidth(bool useScreen = false) const override;
  [[nodiscard]] int getRenderHeight(bool useScreen = false) const override;
  void setViewport(Viewport& viewport, const std::optional<int>& requiredWidth = std::nullopt,
                   const std::optional<int>& requiredHeight = std::nullopt) override;
  GL::IGLProgramPtr createShaderProgram(const IPipelineContextPtr& pipelineContext,
                                        const std::string& vertexCode,
                                        const std::string& fragmentCode, const std::string& defines,
                                        GL::IGLRenderingContext* context = nullptr) override;
  std::unordered_map<std::string, GLUniformLocationPtr>
  getUniforms(const IPipelineContextPtr& pipelineContext,
              const std::vector<std::string>& uniformsNames) override;
  Int32Array getAttributes(const IPipelineContextPtr& pipelineContext,
                           const std::vector<std::string>& attributesNames) override;
  void bindSamplers(Effect& effect) override;
  void enableEffect(const EffectPtr& effect) override;
  void setState(bool culling, float zOffset = 0.f, bool force = false,
                bool reverseSide = false) override;
  void setIntArray(GL::IGLUniformLocation* uniform, const Int32Array& array) override;
  void setIntArray2(GL::IGLUniformLocation* uniform, const Int32Array& array) override;
  void setIntArray3(GL::IGLUniformLocation* uniform, const Int32Array& array) override;
  void setIntArray4(GL::IGLUniformLocation* uniform, const Int32Array& array) override;
  void setFloatArray(GL::IGLUniformLocation* uniform, const Float32Array& array) override;
  void setFloatArray2(GL::IGLUniformLocation* uniform, const Float32Array& array) override;
  void setFloatArray3(GL::IGLUniformLocation* uniform, const Float32Array& array) override;
  void setFloatArray4(GL::IGLUniformLocation* uniform, const Float32Array& array) override;
  void setArray(GL::IGLUniformLocation* uniform, const Float32Array& array) override;
  void setArray2(GL::IGLUniformLocation* uniform, const Float32Array& array) override;
  void setArray3(GL::IGLUniformLocation* uniform, const Float32Array& array) override;
  void setArray4(GL::IGLUniformLocation* uniform, const Float32Array& array) override;
  void setMatrices(GL::IGLUniformLocation* uniform, const Float32Array& matrices) override;
  void setMatrix(GL::IGLUniformLocation* uniform, const Matrix& matrix) override;
  void setMatrix3x3(GL::IGLUniformLocation* uniform, const Float32Array& matrix) override;
  void setMatrix2x2(GL::IGLUniformLocation* uniform, const Float32Array& matrix) override;
  void setInt(GL::IGLUniformLocation* uniform, int value) override;
  void setFloat(GL::IGLUniformLocation* uniform, float value) override;
  void setFloat2(GL::IGLUniformLocation* uniform, float x, float y) override;
  void setFloat3(GL::IGLUniformLocation* uniform, float x, float y, float z) override;
  void setBool(GL::IGLUniformLocation* uniform, int value) override;
  void setFloat4(GL::IGLUniformLocation* uniform, float x, float y, float z, float w) override;
  void setColor3(GL::IGLUniformLocation* uniform, const Color3& color3) override;
  void setColor4(GL::IGLUniformLocation* uniform, const Color3& color3, float alpha) override;
  void setAlphaMode(unsigned int mode, bool noDepthWriteChange = false) override;
  void bindBuffers(const std::unordered_map<std::string, VertexBufferPtr>& vertexBuffers,
                   GL::IGLBuffer* indexBuffer, const EffectPtr& effect) override;
  void wipeCaches(bool bruteForce = false) override;
  void draw(bool useTriangles, int indexStart, int indexCount, int instancesCount = 0) override;
  void drawElementsType(unsigned int fillMode, int indexStart, int verticesCount,
                        int instancesCount = 0) override;
  void drawArraysType(unsigned int fillMode, int verticesStart, int verticesCount,
                      int instancesCount = 0) override;
  GLTexturePtr _createTexture() override;
  void _releaseTexture(InternalTexture* texture) override;
  InternalTexturePtr createTexture(
    const std::string& urlArg, bool noMipmap, bool invertY, Scene* scene,
    unsigned int samplingMode = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE,
    const std::function<void(InternalTexture*, EventState&)>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr,
    const std::optional<std::variant<std::string, ArrayBuffer, Image>>& buffer = std::nullopt,
    const InternalTexturePtr& fallBack                                         = nullptr,
    const std::optional<unsigned int>& format = std::nullopt) override;
  InternalTexturePtr createRenderTargetTexture(const std::variant<ISize, float>& size,
                                               const IRenderTargetOptions& options) override;
  void updateTextureSamplingMode(unsigned int samplingMode,
                                 const InternalTexturePtr& texture) override;
  void bindFramebuffer(const InternalTexturePtr& texture,
                       std::optional<unsigned int> faceIndex       = std::nullopt,
                       std::optional<int> requiredWidth            = std::nullopt,
                       std::optional<int> requiredHeight           = std::nullopt,
                       std::optional<bool> forceFullscreenViewport = std::nullopt,
                       InternalTexture* depthStencilTexture = nullptr, int lodLevel = 0) override;
  void unBindFramebuffer(const InternalTexturePtr& texture, bool disableGenerateMipMaps = false,
                         const std::function<void()>& onBeforeUnbind = nullptr) override;
  GLBufferPtr createDynamicVertexBuffer(const Float32Array& vertices) override;
  void updateDynamicTexture(const InternalTexturePtr& texture, ICanvas* canvas, bool invertY,
                            bool premulAlpha    = false,
                            unsigned int format = Constants::TEXTUREFORMAT_RGBA) override;
  [[nodiscard]] bool areAllEffectsReady() const override;
  [[nodiscard]] unsigned int getError() const override;
  int _getUnpackAlignement() override;
  void _unpackFlipY(bool value) override;
  void updateDynamicIndexBuffer(const GLBufferPtr& indexBuffer, const IndicesArray& indices,
                                int offset = 0) override;
  void updateDynamicVertexBuffer(const GLBufferPtr& vertexBuffer, const Float32Array& data,
                                 int byteOffset = -1, int byteLength = -1) override;
  bool _bindTextureDirectly(unsigned int target, const InternalTexturePtr& texture,
                            bool forTextureDataUpdate = false, bool force = false) override;
  void _bindTexture(int channel, const InternalTexturePtr& texture) override;
  bool _releaseBuffer(GL::IGLBuffer* buffer) override;
  void releaseEffects() override;
  void displayLoadingUI() override;
  void hideLoadingUI() override;
  void _uploadCompressedDataToTextureDirectly(const InternalTexturePtr& texture,
                                              unsigned int internalFormat, float width,
                                              float height, const Uint8Array& data,
                                              unsigned int faceIndex = 0, int lod = 0) override;
  void _uploadDataToTextureDirectly(const InternalTexturePtr& texture,
                                    const ArrayBufferView& imageData, unsigned int faceIndex = 0,
                                    int lod = 0) override;
  void _uploadArrayBufferViewToTexture(const InternalTexturePtr& texture,
                                       const Uint8Array& imageData, unsigned int faceIndex = 0,
                                       int lod = 0) override;
  void _uploadImageToTexture(const InternalTexturePtr& texture, const Image& image,
                             unsigned int faceIndex = 0, int lod = 0) override;

protected:
  NullEngine(const NullEngineOptions& options = NullEngineOptions{});

private:
  NullEngineOptions _options;

}; // end of class Engine

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_ENGINE_H
