#ifndef BABYLON_POSTPROCESS_POST_PROCESS_H
#define BABYLON_POSTPROCESS_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/core/structs.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/math/color4.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT PostProcess {

public:
  PostProcess(
    const string_t& name, const string_t& fragmentUrl,
    const vector_t<string_t>& parameters, const vector_t<string_t>& samplers,
    float renderRatio, Camera* camera,
    unsigned int samplingMode = TextureConstants::NEAREST_SAMPLINGMODE,
    Engine* engine = nullptr, bool reusable = false,
    const string_t& defines   = "",
    unsigned int textureType  = EngineConstants::TEXTURETYPE_UNSIGNED_INT,
    const string_t& vertexUrl = "postprocess",
    const unordered_map_t<string_t, unsigned int>& indexParameters = {},
    bool blockCompilation = false);
  PostProcess(
    const string_t& name, const string_t& fragmentUrl,
    const vector_t<string_t>& parameters, const vector_t<string_t>& samplers,
    const PostProcessOptions& options, Camera* camera,
    unsigned int samplingMode = TextureConstants::NEAREST_SAMPLINGMODE,
    Engine* engine = nullptr, bool reusable = false,
    const string_t& defines   = "",
    unsigned int textureType  = EngineConstants::TEXTURETYPE_UNSIGNED_INT,
    const string_t& vertexUrl = "postprocess",
    const unordered_map_t<string_t, unsigned int>& indexParameters = {},
    bool blockCompilation = false);
  virtual ~PostProcess();

  // Events
  void setOnActivate(
    const ::std::function<void(Camera* camera, EventState&)>& callback);
  void setOnSizeChanged(const ::std::function<void(PostProcess* postProcess,
                                                   EventState&)>& callback);
  void setOnApply(
    const ::std::function<void(Effect* effect, EventState&)>& callback);
  void setOnBeforeRender(
    const ::std::function<void(Effect* effect, EventState&)>& callback);
  void setOnAfterRender(
    const ::std::function<void(Effect* effect, EventState&)>& callback);

  InternalTexture* outputTexture();
  void setOutputTexture(InternalTexture* value);
  Camera* getCamera();
  Vector2 texelSize();
  Engine* getEngine();
  Effect* getEffect();
  PostProcess& shareOutputWith(PostProcess* postProcess);
  void updateEffect(
    const string_t& defines = "", const vector_t<string_t>& uniforms = {},
    const vector_t<string_t>& samplers = {},
    const unordered_map_t<string_t, unsigned int>& indexParameters = {},
    const ::std::function<void(Effect* effect)>& onCompiled = nullptr,
    const ::std::function<void(Effect* effect, const string_t& errors)>& onError
    = nullptr);
  bool isReusable() const;
  /**
   * Invalidate frameBuffer to hint the postprocess to create a depth buffer
   */
  void markTextureDirty();
  void activate(Camera* camera, InternalTexture* sourceTexture = nullptr,
                bool forceDepthStencil = false);
  bool isSupported() const;
  float aspectRatio() const;
  Effect* apply();
  void _disposeTextures();
  virtual void dispose(Camera* camera = nullptr);

public:
  string_t name;
  int width;
  int height;
  unsigned int renderTargetSamplingMode;
  unique_ptr_t<Color4> clearColor;
  bool autoClear;
  unsigned int alphaMode;
  Nullable<Color4> alphaConstants;
  // Enable Pixel Perfect mode where texture is not scaled to be power of 2.
  // Can only be used on a single postprocess or on the last one of a chain.
  bool enablePixelPerfectMode;
  unsigned int scaleMode;
  bool alwaysForcePOT;
  unsigned int samples;
  bool adaptScaleToCurrentViewport;
  vector_t<InternalTexture*> _textures;
  unsigned int _currentRenderTextureInd;
  // Events
  /**
   * An event triggered when the postprocess is activated.
   */
  Observable<Camera> onActivateObservable;
  /**
   * An event triggered when the postprocess changes its size.
   */
  Observable<PostProcess> onSizeChangedObservable;
  /**
   * An event triggered when the postprocess applies its effect.
   */
  Observable<Effect> onApplyObservable;
  /**
   * An event triggered before rendering the postprocess
   */
  Observable<Effect> onBeforeRenderObservable;
  /**
   * An event triggered after rendering the postprocess
   */
  Observable<Effect> onAfterRenderObservable;

protected:
  unordered_map_t<string_t, unsigned int> _indexParameters;

private:
  Camera* _camera;
  Scene* _scene;
  Engine* _engine;
  float _renderRatio;
  PostProcessOptions _options;
  bool _reusable;
  unsigned int _textureType;
  Effect* _effect;
  vector_t<string_t> _samplers;
  string_t _fragmentUrl;
  string_t _vertexUrl;
  vector_t<string_t> _parameters;
  Vector2 _scaleRatio;
  PostProcess* _shareOutputWithPostProcess;
  Vector2 _texelSize;
  InternalTexture* _forcedOutputTexture;
  // Events
  Observer<Camera>::Ptr _onActivateObserver;
  Observer<PostProcess>::Ptr _onSizeChangedObserver;
  Observer<Effect>::Ptr _onApplyObserver;
  Observer<Effect>::Ptr _onBeforeRenderObserver;
  Observer<Effect>::Ptr _onAfterRenderObserver;

}; // end of class PostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_POST_PROCESS_H
