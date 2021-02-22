#ifndef BABYLON_ENGINES_ENGINE_H
#define BABYLON_ENGINES_ENGINE_H

#include <babylon/babylon_api.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/instrumentation/_time_token.h>
#include <babylon/misc/perf_counter.h>
#include <babylon/misc/performance_monitor.h>

namespace BABYLON {

class Camera;
class EffectFallbacks;
struct IEffectFallbacks;
class Material;
class MultiviewExtension;
class OcclusionQueryExtension;
class TransformFeedbackExtension;
FWD_CLASS_SPTR(AudioEngine)
FWD_CLASS_SPTR(ILoadingScreen)
FWD_CLASS_SPTR(IParticleSystem)
FWD_CLASS_SPTR(PostProcess)
FWD_CLASS_SPTR(RenderTargetTexture)

// GL
using WebGLQuery                = GL::IGLQuery;
using WebGLTransformFeedback    = GL::IGLTransformFeedback;
using WebGLTransformFeedbackPtr = std::shared_ptr<WebGLTransformFeedback>;
using WebGLQueryPtr             = std::shared_ptr<WebGLQuery>;

/**
 * @brief The engine class is responsible for interfacing with all lower-level APIs such as WebGL
 * and Audio.
 */
class BABYLON_SHARED_EXPORT Engine : public ThinEngine {

public:
  // Const statics

  /** Defines that alpha blending is disabled */
  static constexpr unsigned int ALPHA_DISABLE = Constants::ALPHA_DISABLE;
  /** Defines that alpha blending to SRC ALPHA * SRC + DEST */
  static constexpr unsigned int ALPHA_ADD = Constants::ALPHA_ADD;
  /** Defines that alpha blending to SRC ALPHA * SRC + (1 - SRC ALPHA) * DEST */
  static constexpr unsigned int ALPHA_COMBINE = Constants::ALPHA_COMBINE;
  /** Defines that alpha blending to DEST - SRC * DEST */
  static constexpr unsigned int ALPHA_SUBTRACT = Constants::ALPHA_SUBTRACT;
  /** Defines that alpha blending to SRC * DEST */
  static constexpr unsigned int ALPHA_MULTIPLY = Constants::ALPHA_MULTIPLY;
  /** Defines that alpha blending to SRC ALPHA * SRC + (1 - SRC) * DEST */
  static constexpr unsigned int ALPHA_MAXIMIZED = Constants::ALPHA_MAXIMIZED;
  /** Defines that alpha blending to SRC + DEST */
  static constexpr unsigned int ALPHA_ONEONE = Constants::ALPHA_ONEONE;
  /** Defines that alpha blending to SRC + (1 - SRC ALPHA) * DEST */
  static constexpr unsigned int ALPHA_PREMULTIPLIED = Constants::ALPHA_PREMULTIPLIED;
  /**
   * Defines that alpha blending to SRC + (1 - SRC ALPHA) * DEST
   * Alpha will be set to (1 - SRC ALPHA) * DEST ALPHA
   */
  static constexpr unsigned int ALPHA_PREMULTIPLIED_PORTERDUFF
    = Constants::ALPHA_PREMULTIPLIED_PORTERDUFF;
  /** Defines that alpha blending to CST * SRC + (1 - CST) * DEST */
  static constexpr unsigned int ALPHA_INTERPOLATE = Constants::ALPHA_INTERPOLATE;
  /**
   * Defines that alpha blending to SRC + (1 - SRC) * DEST
   * Alpha will be set to SRC ALPHA + (1 - SRC ALPHA) * DEST ALPHA
   */
  static constexpr unsigned int ALPHA_SCREENMODE = Constants::ALPHA_SCREENMODE;

  /** Defines that the ressource is not delayed*/
  static constexpr unsigned int DELAYLOADSTATE_NONE = Constants::DELAYLOADSTATE_NONE;
  /** Defines that the ressource was successfully delay loaded */
  static constexpr unsigned int DELAYLOADSTATE_LOADED = Constants::DELAYLOADSTATE_LOADED;
  /** Defines that the ressource is currently delay loading */
  static constexpr unsigned int DELAYLOADSTATE_LOADING = Constants::DELAYLOADSTATE_LOADING;
  /** Defines that the ressource is delayed and has not started loading */
  static constexpr unsigned int DELAYLOADSTATE_NOTLOADED = Constants::DELAYLOADSTATE_NOTLOADED;

  // Depht or Stencil test Constants::
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will never pass.
   * i.e. Nothing will be drawn */
  static constexpr unsigned int NEVER = Constants::NEVER;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will always pass.
   * i.e. Pixels will be drawn in the order they are drawn */
  static constexpr unsigned int ALWAYS = Constants::ALWAYS;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will pass if the
   * new depth value is less than the stored value */
  static constexpr unsigned int LESS = Constants::LESS;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will pass if the
   * new depth value is equals to the stored value */
  static constexpr unsigned int EQUAL = Constants::EQUAL;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will pass if the
   * new depth value is less than or equal to the stored value */
  static constexpr unsigned int LEQUAL = Constants::LEQUAL;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will pass if the
   * new depth value is greater than the stored value */
  static constexpr unsigned int GREATER = Constants::GREATER;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will pass if the
   * new depth value is greater than or equal to the stored value */
  static constexpr unsigned int GEQUAL = Constants::GEQUAL;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will pass if the
   * new depth value is not equal to the stored value */
  static constexpr unsigned int NOTEQUAL = Constants::NOTEQUAL;

  // Stencil Actions Constants::
  /** Passed to stencilOperation to specify that stencil value must be kept */
  static constexpr unsigned int KEEP = Constants::KEEP;
  /** Passed to stencilOperation to specify that stencil value must be replaced */
  static constexpr unsigned int REPLACE = Constants::REPLACE;
  /** Passed to stencilOperation to specify that stencil value must be incremented */
  static constexpr unsigned int INCR = Constants::INCR;
  /** Passed to stencilOperation to specify that stencil value must be decremented */
  static constexpr unsigned int DECR = Constants::DECR;
  /** Passed to stencilOperation to specify that stencil value must be inverted */
  static constexpr unsigned int INVERT = Constants::INVERT;
  /** Passed to stencilOperation to specify that stencil value must be incremented with wrapping */
  static constexpr unsigned int INCR_WRAP = Constants::INCR_WRAP;
  /** Passed to stencilOperation to specify that stencil value must be decremented with wrapping */
  static constexpr unsigned int DECR_WRAP = Constants::DECR_WRAP;

  /** Texture is not repeating outside of 0..1 UVs */
  static constexpr unsigned int TEXTURE_CLAMP_ADDRESSMODE = Constants::TEXTURE_CLAMP_ADDRESSMODE;
  /** Texture is repeating outside of 0..1 UVs */
  static constexpr unsigned int TEXTURE_WRAP_ADDRESSMODE = Constants::TEXTURE_WRAP_ADDRESSMODE;
  /** Texture is repeating and mirrored */
  static constexpr unsigned int TEXTURE_MIRROR_ADDRESSMODE = Constants::TEXTURE_MIRROR_ADDRESSMODE;

  /** ALPHA */
  static constexpr unsigned int TEXTUREFORMAT_ALPHA = Constants::TEXTUREFORMAT_ALPHA;
  /** LUMINANCE */
  static constexpr unsigned int TEXTUREFORMAT_LUMINANCE = Constants::TEXTUREFORMAT_LUMINANCE;
  /** LUMINANCE_ALPHA */
  static constexpr unsigned int TEXTUREFORMAT_LUMINANCE_ALPHA
    = Constants::TEXTUREFORMAT_LUMINANCE_ALPHA;
  /** RGB */
  static constexpr unsigned int TEXTUREFORMAT_RGB = Constants::TEXTUREFORMAT_RGB;
  /** RGBA */
  static constexpr unsigned int TEXTUREFORMAT_RGBA = Constants::TEXTUREFORMAT_RGBA;
  /** RED */
  static constexpr unsigned int TEXTUREFORMAT_RED = Constants::TEXTUREFORMAT_RED;
  /** RED (2nd reference) */
  static constexpr unsigned int TEXTUREFORMAT_R = Constants::TEXTUREFORMAT_R;
  /** RG */
  static constexpr unsigned int TEXTUREFORMAT_RG = Constants::TEXTUREFORMAT_RG;
  /** RED_INTEGER */
  static constexpr unsigned int TEXTUREFORMAT_RED_INTEGER = Constants::TEXTUREFORMAT_RED_INTEGER;
  /** RED_INTEGER (2nd reference) */
  static constexpr unsigned int TEXTUREFORMAT_R_INTEGER = Constants::TEXTUREFORMAT_R_INTEGER;
  /** RG_INTEGER */
  static constexpr unsigned int TEXTUREFORMAT_RG_INTEGER = Constants::TEXTUREFORMAT_RG_INTEGER;
  /** RGB_INTEGER */
  static constexpr unsigned int TEXTUREFORMAT_RGB_INTEGER = Constants::TEXTUREFORMAT_RGB_INTEGER;
  /** RGBA_INTEGER */
  static constexpr unsigned int TEXTUREFORMAT_RGBA_INTEGER = Constants::TEXTUREFORMAT_RGBA_INTEGER;

  /** UNSIGNED_BYTE */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_BYTE = Constants::TEXTURETYPE_UNSIGNED_BYTE;
  /** UNSIGNED_BYTE (2nd reference) */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INT = Constants::TEXTURETYPE_UNSIGNED_INT;
  /** FLOAT */
  static constexpr unsigned int TEXTURETYPE_FLOAT = Constants::TEXTURETYPE_FLOAT;
  /** HALF_FLOAT */
  static constexpr unsigned int TEXTURETYPE_HALF_FLOAT = Constants::TEXTURETYPE_HALF_FLOAT;
  /** BYTE */
  static constexpr unsigned int TEXTURETYPE_BYTE = Constants::TEXTURETYPE_BYTE;
  /** SHORT */
  static constexpr unsigned int TEXTURETYPE_SHORT = Constants::TEXTURETYPE_SHORT;
  /** UNSIGNED_SHORT */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_SHORT = Constants::TEXTURETYPE_UNSIGNED_SHORT;
  /** INT */
  static constexpr unsigned int TEXTURETYPE_INT = Constants::TEXTURETYPE_INT;
  /** UNSIGNED_INT */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INTEGER
    = Constants::TEXTURETYPE_UNSIGNED_INTEGER;
  /** UNSIGNED_SHORT_4_4_4_4 */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_SHORT_4_4_4_4
    = Constants::TEXTURETYPE_UNSIGNED_SHORT_4_4_4_4;
  /** UNSIGNED_SHORT_5_5_5_1 */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_SHORT_5_5_5_1
    = Constants::TEXTURETYPE_UNSIGNED_SHORT_5_5_5_1;
  /** UNSIGNED_SHORT_5_6_5 */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_SHORT_5_6_5
    = Constants::TEXTURETYPE_UNSIGNED_SHORT_5_6_5;
  /** UNSIGNED_INT_2_10_10_10_REV */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INT_2_10_10_10_REV
    = Constants::TEXTURETYPE_UNSIGNED_INT_2_10_10_10_REV;
  /** UNSIGNED_INT_24_8 */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INT_24_8
    = Constants::TEXTURETYPE_UNSIGNED_INT_24_8;
  /** UNSIGNED_INT_10F_11F_11F_REV */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INT_10F_11F_11F_REV
    = Constants::TEXTURETYPE_UNSIGNED_INT_10F_11F_11F_REV;
  /** UNSIGNED_INT_5_9_9_9_REV */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INT_5_9_9_9_REV
    = Constants::TEXTURETYPE_UNSIGNED_INT_5_9_9_9_REV;
  /** FLOAT_32_UNSIGNED_INT_24_8_REV */
  static constexpr unsigned int TEXTURETYPE_FLOAT_32_UNSIGNED_INT_24_8_REV
    = Constants::TEXTURETYPE_FLOAT_32_UNSIGNED_INT_24_8_REV;

  /** nearest is mag = nearest and min = nearest and mip = linear */
  static constexpr unsigned int TEXTURE_NEAREST_SAMPLINGMODE
    = Constants::TEXTURE_NEAREST_SAMPLINGMODE;
  /** Bilinear is mag = linear and min = linear and mip = nearest */
  static constexpr unsigned int TEXTURE_BILINEAR_SAMPLINGMODE
    = Constants::TEXTURE_BILINEAR_SAMPLINGMODE;
  /** Trilinear is mag = linear and min = linear and mip = linear */
  static constexpr unsigned int TEXTURE_TRILINEAR_SAMPLINGMODE
    = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE;
  /** nearest is mag = nearest and min = nearest and mip = linear */
  static constexpr unsigned int TEXTURE_NEAREST_NEAREST_MIPLINEAR
    = Constants::TEXTURE_NEAREST_NEAREST_MIPLINEAR;
  /** Bilinear is mag = linear and min = linear and mip = nearest */
  static constexpr unsigned int TEXTURE_LINEAR_LINEAR_MIPNEAREST
    = Constants::TEXTURE_LINEAR_LINEAR_MIPNEAREST;
  /** Trilinear is mag = linear and min = linear and mip = linear */
  static constexpr unsigned int TEXTURE_LINEAR_LINEAR_MIPLINEAR
    = Constants::TEXTURE_LINEAR_LINEAR_MIPLINEAR;
  /** mag = nearest and min = nearest and mip = nearest */
  static constexpr unsigned int TEXTURE_NEAREST_NEAREST_MIPNEAREST
    = Constants::TEXTURE_NEAREST_NEAREST_MIPNEAREST;
  /** mag = nearest and min = linear and mip = nearest */
  static constexpr unsigned int TEXTURE_NEAREST_LINEAR_MIPNEAREST
    = Constants::TEXTURE_NEAREST_LINEAR_MIPNEAREST;
  /** mag = nearest and min = linear and mip = linear */
  static constexpr unsigned int TEXTURE_NEAREST_LINEAR_MIPLINEAR
    = Constants::TEXTURE_NEAREST_LINEAR_MIPLINEAR;
  /** mag = nearest and min = linear and mip = none */
  static constexpr unsigned int TEXTURE_NEAREST_LINEAR = Constants::TEXTURE_NEAREST_LINEAR;
  /** mag = nearest and min = nearest and mip = none */
  static constexpr unsigned int TEXTURE_NEAREST_NEAREST = Constants::TEXTURE_NEAREST_NEAREST;
  /** mag = linear and min = nearest and mip = nearest */
  static constexpr unsigned int TEXTURE_LINEAR_NEAREST_MIPNEAREST
    = Constants::TEXTURE_LINEAR_NEAREST_MIPNEAREST;
  /** mag = linear and min = nearest and mip = linear */
  static constexpr unsigned int TEXTURE_LINEAR_NEAREST_MIPLINEAR
    = Constants::TEXTURE_LINEAR_NEAREST_MIPLINEAR;
  /** mag = linear and min = linear and mip = none */
  static constexpr unsigned int TEXTURE_LINEAR_LINEAR = Constants::TEXTURE_LINEAR_LINEAR;
  /** mag = linear and min = nearest and mip = none */
  static constexpr unsigned int TEXTURE_LINEAR_NEAREST = Constants::TEXTURE_LINEAR_NEAREST;

  /** Explicit coordinates mode */
  static constexpr unsigned int TEXTURE_EXPLICIT_MODE = Constants::TEXTURE_EXPLICIT_MODE;
  /** Spherical coordinates mode */
  static constexpr unsigned int TEXTURE_SPHERICAL_MODE = Constants::TEXTURE_SPHERICAL_MODE;
  /** Planar coordinates mode */
  static constexpr unsigned int TEXTURE_PLANAR_MODE = Constants::TEXTURE_PLANAR_MODE;
  /** Cubic coordinates mode */
  static constexpr unsigned int TEXTURE_CUBIC_MODE = Constants::TEXTURE_CUBIC_MODE;
  /** Projection coordinates mode */
  static constexpr unsigned int TEXTURE_PROJECTION_MODE = Constants::TEXTURE_PROJECTION_MODE;
  /** Skybox coordinates mode */
  static constexpr unsigned int TEXTURE_SKYBOX_MODE = Constants::TEXTURE_SKYBOX_MODE;
  /** Inverse Cubic coordinates mode */
  static constexpr unsigned int TEXTURE_INVCUBIC_MODE = Constants::TEXTURE_INVCUBIC_MODE;
  /** Equirectangular coordinates mode */
  static constexpr unsigned int TEXTURE_EQUIRECTANGULAR_MODE
    = Constants::TEXTURE_EQUIRECTANGULAR_MODE;
  /** Equirectangular Fixed coordinates mode */
  static constexpr unsigned int TEXTURE_FIXED_EQUIRECTANGULAR_MODE
    = Constants::TEXTURE_FIXED_EQUIRECTANGULAR_MODE;
  /** Equirectangular Fixed Mirrored coordinates mode */
  static constexpr unsigned int TEXTURE_FIXED_EQUIRECTANGULAR_MIRRORED_MODE
    = Constants::TEXTURE_FIXED_EQUIRECTANGULAR_MIRRORED_MODE;

  // Texture rescaling mode
  /** Defines that texture rescaling will use a floor to find the closer power of 2 size */
  static constexpr unsigned int SCALEMODE_FLOOR = Constants::SCALEMODE_FLOOR;
  /** Defines that texture rescaling will look for the nearest power of 2 size */
  static constexpr unsigned int SCALEMODE_NEAREST = Constants::SCALEMODE_NEAREST;
  /** Defines that texture rescaling will use a ceil to find the closer power of 2 size */
  static constexpr unsigned int SCALEMODE_CEILING = Constants::SCALEMODE_CEILING;

public:
  /**
   * Gets the list of created engines
   */
  static std::vector<Engine*>& Instances();

  /**
   * @brief Gets the latest created engine.
   */
  static Engine* LastCreatedEngine();

  /**
   * @brief Gets the latest created scene.
   */
  static Scene* LastCreatedScene();

  /**
   * @brief Will flag all materials in all scenes in all engines as dirty to trigger new shader
   * compilation
   * @param flag defines which part of the materials must be marked as dirty
   * @param predicate defines a predicate used to filter which materials should be affected
   */
  static void MarkAllMaterialsAsDirty(unsigned int flag,
                                      const std::function<bool(Material* mat)>& predicate
                                      = nullptr);

  /**
   * @brief Method called to create the default loading screen.
   * This can be overriden in your own app.
   * @param canvas The rendering canvas element
   * @returns The loading screen
   */
  static ILoadingScreenPtr DefaultLoadingScreenFactory(ICanvas* canvas);

  /**
   * Method called to create the default rescale post process on each engine.
   */
  static std::function<PostProcessPtr(Engine* engine)> _RescalePostProcessFactory;

public:
  template <typename... Ts>
  static std::unique_ptr<Engine> New(Ts&&... args)
  {
    std::unique_ptr<Engine> engine(new Engine(std::forward<Ts>(args)...));
    return engine;
  }
  ~Engine() override; // = default

  // Events

  /**
   * @brief Gets the HTML element used to attach event listeners.
   * @returns a HTML element
   */
  ICanvas* getInputElement() const;

  /**
   * @brief Gets current aspect ratio.
   * @param viewportOwner defines the camera to use to get the aspect ratio
   * @param useScreen defines if screen size must be used (or the current render target if any)
   * @returns a number defining the aspect ratio
   */
  float getAspectRatio(const Camera& camera, bool useScreen = false);

  /**
   * @brief Gets current screen aspect ratio.
   * @returns a number defining the aspect ratio
   */
  float getScreenAspectRatio() const;

  /**
   * @brief Gets the client rect of the HTML canvas attached with the current webGL context.
   * @returns a client rectanglee
   */
  std::optional<ClientRect> getRenderingCanvasClientRect();

  /**
   * @brief Gets the client rect of the HTML element used for events.
   * @returns a client rectanglee
   */
  std::optional<ClientRect> getInputElementClientRect();

  /**
   * @brief Gets a boolean indicating that the engine is running in deterministic lock step mode.
   * @see https://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   * @returns true if engine is in deterministic lock step mode
   */
  virtual bool isDeterministicLockStep() const;

  /**
   * @brief Gets the max steps when engine is running in deterministic lock step.
   * @see https://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   * @returns the max steps
   */
  virtual unsigned int getLockstepMaxSteps() const;

  /**
   * @brief Returns the time in ms between steps when using deterministic lock step.
   * @returns time step in (ms)
   */
  float getTimeStep() const;

  /**
   * @brief Force the mipmap generation for the given render target texture.
   * @param texture defines the render target texture to use
   * @param unbind defines whether or not to unbind the texture after generation. Defaults to true.
   */
  void generateMipMapsForCubemap(const InternalTexturePtr& texture, bool unbind = true);

  /** States */

  /**
   * @brief Set various states to the webGL context.
   * @param culling defines backface culling state
   * @param zOffset defines the value to apply to zOffset (0 by default)
   * @param force defines if states must be applied even if cache is up to date
   * @param reverseSide defines if culling must be reversed (CCW instead of CW and CW instead of
   * CCW)
   */
  virtual void setState(bool culling, float zOffset = 0.f, bool force = false,
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
   * @brief Sets a boolean indicating if the dithering state is enabled or disabled.
   * @param value defines the dithering state
   */
  void setDitheringState(bool value);

  /**
   * @brief Sets a boolean indicating if the rasterizer state is enabled or disabled.
   * @param value defines the rasterizer state
   */
  void setRasterizerState(bool value);

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
   * @brief Caches the state of the stencil buffer.
   */
  void cacheStencilState();

  /**
   * @brief Restores the state of the stencil buffer.
   */
  void restoreStencilState();

  /**
   * @brief Directly set the WebGL Viewport.
   * @param x defines the x coordinate of the viewport (in screen space)
   * @param y defines the y coordinate of the viewport (in screen space)
   * @param width defines the width of the viewport (in screen space)
   * @param height defines the height of the viewport (in screen space)
   * @return the current viewport Object (if any) that is being replaced by this call. You can
   * restore this viewport later on to go back to the original state
   */
  Viewport setDirectViewport(int x, int y, int width, int height);

  /**
   * @brief Executes a scissor clear (ie. a clear on a specific portion of the screen).
   * @param x defines the x-coordinate of the top left corner of the clear rectangle
   * @param y defines the y-coordinate of the corner of the clear rectangle
   * @param width defines the width of the clear rectangle
   * @param height defines the height of the clear rectangle
   * @param clearColor defines the clear color
   */
  void scissorClear(int x, int y, int width, int height, const Color4& clearColor);

  /**
   * @brief Enable scissor test on a specific rectangle (ie. render will only be executed on a
   * specific portion of the screen).
   * @param x defines the x-coordinate of the top left corner of the clear rectangle
   * @param y defines the y-coordinate of the corner of the clear rectangle
   * @param width defines the width of the clear rectangle
   * @param height defines the height of the clear rectangle
   */
  void enableScissor(int x, int y, int width, int height);

  /**
   * @brief Disable previously set scissor test rectangle.
   */
  void disableScissor();

  /**
   * @brief Initializes a webVR display and starts listening to display change events.
   * The onVRDisplayChangedObservable will be notified upon these changes
   * @returns The onVRDisplayChangedObservable
   */
  void initWebVR();

  /**
   * @brief Hidden
   */
  virtual void _prepareVRComponent();

  /**
   * @brief Hidden
   */
  virtual void _connectVREvents(ICanvas* canvas);

  /**
   * @brief Hidden
   */
  virtual void _submitVRFrame();

  /**
   * @brief Call this function to leave webVR mode.
   * Will do nothing if webVR is not supported or if there is no webVR device
   * @see https://doc.babylonjs.com/how_to/webvr_camera
   */
  virtual void disableVR();

  /**
   * @brief Gets a boolean indicating that the system is in VR mode and is presenting
   * @returns true if VR mode is engaged
   */
  virtual bool isVRPresenting() const;

  /**
   * @brief Hidden
   */
  virtual void _requestVRFrame();

  /**
   * @brief Gets the source code of the vertex shader associated with a specific webGL program.
   * @param program defines the program to use
   * @returns a string containing the source code of the vertex shader associated with the program
   */
  std::string getVertexShaderSource(const WebGLProgramPtr& program);

  /**
   * @brief Gets the source code of the fragment shader associated with a specific webGL program.
   * @param program defines the program to use
   * @returns a string containing the source code of the fragment shader associated with the program
   */
  std::string getFragmentShaderSource(const WebGLProgramPtr& program);

  /**
   * @brief Sets a depth stencil texture from a render target to the according uniform.
   * @param channel The texture channel
   * @param uniform The uniform to set
   * @param texture The render target texture containing the depth stencil texture to apply
   */
  void setDepthStencilTexture(int channel, const WebGLUniformLocationPtr& uniform,
                              const RenderTargetTexturePtr& texture, const std::string& name = "");

  /**
   * @brief Sets a texture to the webGL context from a postprocess.
   * @param channel defines the channel to use
   * @param postProcess defines the source postprocess
   */
  void setTextureFromPostProcess(int channel, const PostProcessPtr& postProcess,
                                 const std::string& name);

  /**
   * @brief Binds the output of the passed in post process to the texture channel specified.
   * @param channel The channel the texture should be bound to
   * @param postProcess The post process which's output should be bound
   */
  void setTextureFromPostProcessOutput(int channel, const PostProcessPtr& postProcess,
                                       const std::string& name);

  /**
   * @brief Hidden
   */
  void _renderFrame();

  /**
   * @brief Hidden
   */
  void _renderLoop() override;

  /**
   * @brief Hidden
   */
  bool _renderViews() const;

  /**
   * @brief Toggle full screen mode.
   * @param requestPointerLock defines if a pointer lock should be requested from the user
   */
  void switchFullscreen(bool requestPointerLock);

  /**
   * @brief Enters full screen mode.
   * @param requestPointerLock defines if a pointer lock should be requested from the user
   */
  void enterFullscreen(bool requestPointerLock);

  /**
   * @brief Exits full screen mode.
   */
  void exitFullscreen();

  /**
   * @brief Enters Pointerlock mode.
   */
  void enterPointerlock();

  /**
   * @brief Exits Pointerlock mode.
   */
  void exitPointerlock();

  /**
   * @brief Begin a new frame.
   */
  void beginFrame() override;

  /**
   * @brief End the current frame.
   */
  void endFrame() override;

  /**
   * @brief Resize the view according to the canvas' size.
   */
  void resize() override;

  /**
   * @brief Force a specific size of the canvas.
   * @param width defines the new canvas' width
   * @param height defines the new canvas' height
   * @returns true if the size was changed
   */
  bool setSize(int width = 0, int height = 0) override;

  /**
   * @brief Hidden
   */
  void _deletePipelineContext(const IPipelineContextPtr& pipelineContext) override;

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
  WebGLProgramPtr
  createShaderProgram(const IPipelineContextPtr& pipelineContext, const std::string& vertexCode,
                      const std::string& fragmentCode, const std::string& defines,
                      WebGLRenderingContext* context                            = nullptr,
                      const std::vector<std::string>& transformFeedbackVaryings = {}) override;

  /**
   * @brief Hidden
   */
  void _releaseTexture(const InternalTexturePtr& texture) override;

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
                       const std::function<void()>& onComplete) override;

  // FPS

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

  /**
   * @brief Hidden
   */
  virtual void _uploadImageToTexture(const InternalTexturePtr& texture, const Image& image,
                                     unsigned int faceIndex = 0, int lod = 0);

  /**
   * @brief Updates the sample count of a render target texture.
   * @see https://doc.babylonjs.com/features/webgl2#multisample-render-targets
   * @param texture defines the texture to update
   * @param samples defines the sample count to set
   * @returns the effective sample count (could be 0 if multisample render targets are not
   * supported)
   */
  unsigned int updateRenderTargetTextureSampleCount(const InternalTexturePtr& texture,
                                                    unsigned int samples);

  /**
   * @brief Updates a depth texture Comparison Mode and Function.
   * If the comparison Function is equal to 0, the mode will be set to none.
   * Otherwise, this only works in webgl 2 and requires a shadow sampler in the shader.
   * @param texture The texture to set the comparison function for
   * @param comparisonFunction The comparison function to set, 0 if no comparison required
   */
  void updateTextureComparisonFunction(const InternalTexturePtr& texture, int comparisonFunction);

  /**
   * @brief Create an effect to use with particle systems.
   * Please note that some parameters like animation sheets or not being billboard are not supported
   * in this configuration, except if you pass the particle system for which you want to create a
   * custom effect in the last parameter
   * @param fragmentName defines the base name of the effect (The name of file without .fragment.fx)
   * @param uniformsNames defines a list of attribute names
   * @param samplers defines an array of string used to represent textures
   * @param defines defines the string containing the defines to use to compile the shaders
   * @param fallbacks defines the list of potential fallbacks to use if shader compilation fails
   * @param onCompiled defines a function to call when the effect creation is successful
   * @param onError defines a function to call when the effect creation has failed
   * @param particleSystem the particle system you want to create the effect for
   * @returns the new Effect
   */
  EffectPtr createEffectForParticles(
    const std::string& fragmentName, const std::vector<std::string>& uniformsNames,
    std::vector<std::string> samplers, std::string defines, IEffectFallbacks* fallbacks = nullptr,
    const std::function<void(Effect* effect)>& onCompiled                         = nullptr,
    const std::function<void(Effect* effect, const std::string& errors)>& onError = nullptr,
    const IParticleSystemPtr& particleSystem                                      = nullptr);

  /**
   * @brief Creates a webGL buffer to use with instanciation.
   * @param capacity defines the size of the buffer
   * @returns the webGL buffer
   */
  WebGLDataBufferPtr createInstancesBuffer(unsigned int capacity);

  /**
   * @brief Delete a webGL buffer used with instanciation.
   * @param buffer defines the webGL buffer to delete
   */
  void deleteInstancesBuffer(const WebGLDataBufferPtr& buffer);

  /**
   * @brief Dispose and release all associated resources.
   */
  void dispose() override;

  // Loading screen

  /**
   * @brief Display the loading screen.
   * @see https://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  virtual void displayLoadingUI();

  /**
   * @brief Hide the loading screen.
   * @see https://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  virtual void hideLoadingUI();

  /** Pointerlock and fullscreen */

  /**
   * Ask the browser to promote the current element to pointerlock mode
   * @param element defines the DOM element to promote
   */
  static void _RequestPointerlock(ICanvas* element);

  /**
   * Asks the browser to exit pointerlock mode
   */
  static void _ExitPointerlock();

  /**
   * Ask the browser to promote the current element to fullscreen rendering mode
   * @param element defines the DOM element to promote
   */
  static void _RequestFullscreen(ICanvas* element);

  /**
   * Asks the browser to exit fullscreen mode
   */
  static void _ExitFullscreen();

  //------------------------------------------------------------------------------------------------
  //                              Multiview Extension
  //------------------------------------------------------------------------------------------------

  /**
   * @brief Creates a new multiview render target.
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @returns the created multiview texture
   */
  InternalTexturePtr createMultiviewRenderTargetTexture(int width, int height);

  /**
   * @brief Binds a multiview framebuffer to be drawn to.
   * @param multiviewTexture texture to bind
   */
  void bindMultiviewFramebuffer(const InternalTexturePtr& multiviewTexture);

  //------------------------------------------------------------------------------------------------
  //                              Occlusion Query Extension
  //------------------------------------------------------------------------------------------------

  /**
   * @brief Create a new webGL query (you must be sure that queries are supported by checking
   * getCaps() function).
   * @return the new query
   */
  WebGLQueryPtr createQuery();

  /**
   * @brief Delete and release a webGL query.
   * @param query defines the query to delete
   * @return the current engine
   */
  Engine& deleteQuery(const WebGLQueryPtr& query);

  /**
   * @brief Check if a given query has resolved and got its value.
   * @param query defines the query to check
   * @returns true if the query got its value
   */
  bool isQueryResultAvailable(const WebGLQueryPtr& query);

  /**
   * @brief Gets the value of a given query.
   * @param query defines the query to check
   * @returns the value of the query
   */
  unsigned int getQueryResult(const WebGLQueryPtr& query);

  /**
   * @brief Initiates an occlusion query.
   * @param algorithmType defines the algorithm to use
   * @param query defines the query to use
   * @returns the current engine
   * @see https://doc.babylonjs.com/features/occlusionquery
   */
  Engine& beginOcclusionQuery(unsigned int algorithmType, const WebGLQueryPtr& query);

  /**
   * @brief Ends an occlusion query.
   * @see https://doc.babylonjs.com/features/occlusionquery
   * @param algorithmType defines the algorithm to use
   * @returns the current engine
   */
  Engine& endOcclusionQuery(unsigned int algorithmType);

  /**
   * @brief Starts a time query (used to measure time spent by the GPU on a specific frame)
   * Please note that only one query can be issued at a time
   * @returns a time token used to track the time span
   */
  std::optional<_TimeToken> startTimeQuery();

  /**
   * @brief Ends a time query.
   * @param token defines the token used to measure the time span
   * @returns the time spent (in ns)
   */
  int endTimeQuery(std::optional<_TimeToken>& token);

  /**
   * @brief Hidden
   */
  WebGLQueryPtr _createTimeQuery();

  /**
   * @brief Hidden
   */
  void _deleteTimeQuery(const WebGLQueryPtr& query);

  /**
   * @brief Hidden
   */
  unsigned int _getGlAlgorithmType(unsigned int algorithmType) const;

  /**
   * @brief Hidden
   */
  unsigned int _getTimeQueryResult(const WebGLQueryPtr& query);

  /**
   * @brief Hidden
   */
  bool _getTimeQueryAvailability(const WebGLQueryPtr& query);

  //------------------------------------------------------------------------------------------------
  //                              Transform Feedback Extension
  //------------------------------------------------------------------------------------------------

  /**
   * @brief Creates a webGL transform feedback object.
   * Please makes sure to check webGLVersion property to check if you are running webGL 2+
   * @returns the webGL transform feedback object
   */
  WebGLTransformFeedbackPtr createTransformFeedback();

  /**
   * @brief Delete a webGL transform feedback object.
   * @param value defines the webGL transform feedback object to delete
   */
  void deleteTransformFeedback(const WebGLTransformFeedbackPtr& value);

  /**
   * @brief Bind a webGL transform feedback object to the webgl context.
   * @param value defines the webGL transform feedback object to bind
   */
  void bindTransformFeedback(const WebGLTransformFeedbackPtr& value);

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
  void setTranformFeedbackVaryings(const WebGLProgramPtr& program,
                                   const std::vector<std::string>& value);

  /**
   * @brief Bind a webGL buffer for a transform feedback operation.
   * @param value defines the webGL buffer to bind
   */
  void bindTransformFeedbackBuffer(const WebGLDataBufferPtr& value);

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
  Engine(ICanvas* canvas, const EngineOptions& options = EngineOptions());

  /**
   * @brief Hidden
   */
  bool get__supportsHardwareTextureRescaling() const override;

  /**
   * @brief Gets the performance monitor attached to this engine.
   * @see https://doc.babylonjs.com/how_to/optimizing_your_scene#engineinstrumentation
   */
  std::unique_ptr<PerformanceMonitor>& get_performanceMonitor();

  /**
   * @brief Gets the current loading screen object.
   * @see https://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  ILoadingScreenPtr& get_loadingScreen();

  /**
   * @brief Sets the current loading screen object.
   * @see https://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  void set_loadingScreen(const ILoadingScreenPtr& loadingScreen);

  /**
   * @brief Sets the current loading screen text.
   * @see https://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  void set_loadingUIText(std::string text);

  /**
   * @brief Sets the current loading screen background color.
   * @see https://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  void set_loadingUIBackgroundColor(std::string color);

  void _reportDrawCall() override;
  void _rebuildBuffers() override;
  WebGLProgramPtr
  _createShaderProgram(const WebGLPipelineContextPtr& pipelineContext,
                       const WebGLShaderPtr& vertexShader, const WebGLShaderPtr& fragmentShader,
                       WebGLRenderingContext* context,
                       const std::vector<std::string>& transformFeedbackVaryings = {}) override;

private:
  void _measureFps();
  void _disableTouchAction();

public:
  // Members

  /**
   * Gets or sets a boolean to enable/disable IndexedDB support and avoid XHR on .manifest
   **/
  bool enableOfflineSupport = false;

  /**
   * Gets or sets a boolean to enable/disable checking manifest if IndexedDB support is enabled (js
   *will always consider the database is up to date)
   **/
  bool disableManifestCheck = false;

  /**
   * Gets the list of created scenes
   */
  std::vector<Scene*> scenes;

  /**
   * Event raised when a new scene is created
   */
  Observable<Scene> onNewSceneAddedObservable;

  /**
   * Gets the list of created postprocesses
   */
  std::vector<PostProcessPtr> postProcesses;

  /**
   * Gets a boolean indicating if the pointer is currently locked
   */
  bool isPointerLock = false;

  // Observables

  /**
   * Observable event triggered each time the rendering canvas is resized
   */
  Observable<Engine> onResizeObservable;

  /**
   * Observable event triggered each time the canvas loses focus
   */
  Observable<Engine> onCanvasBlurObservable;

  /**
   * Observable event triggered each time the canvas gains focus
   */
  Observable<Engine> onCanvasFocusObservable;

  /**
   * Observable event triggered each time the canvas receives pointerout event
   */
  Observable<PointerEvent> onCanvasPointerOutObservable;

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
   * Gets the audio engine
   * @see https://doc.babylonjs.com/how_to/playing_sounds_and_music
   * @ignorenaming
   */
  static AudioEnginePtr audioEngine;

  /** @hidden */
  PerfCounter _drawCalls;

  /**
   * Gets or sets the tab index to set to the rendering canvas. 1 is the minimum value to set to be
   * able to capture keyboard events
   */
  int canvasTabIndex = 1;

  /**
   * Turn this value on if you want to pause FPS computation when in background
   */
  bool disablePerformanceMonitorInBackground = false;

  /**
   * Gets the performance monitor attached to this engine
   * @see https://doc.babylonjs.com/how_to/optimizing_your_scene#engineinstrumentation
   */
  ReadOnlyProperty<Engine, std::unique_ptr<PerformanceMonitor>> performanceMonitor;

  /**
   * Gets or sets the current loading screen object.
   * @see https://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  Property<Engine, ILoadingScreenPtr> loadingScreen;

  /**
   * Sets the current loading screen text.
   * @see https://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  WriteOnlyProperty<Engine, std::string> loadingUIText;

  /**
   * @brief Sets the current loading screen background color.
   * @see https://doc.babylonjs.com/how_to/creating_a_custom_loading_screen
   */
  WriteOnlyProperty<Engine, std::string> loadingUIBackgroundColor;

private:
  ILoadingScreenPtr _loadingScreen = nullptr;
  bool _pointerLockRequested       = false;
  PostProcessPtr _rescalePostProcess;

  // Deterministic lockstepMaxSteps
  bool _deterministicLockstep    = false;
  unsigned int _lockstepMaxSteps = 4;
  float _timeStep                = 1.f / 60.f;

  // FPS
  float _fps                                              = 60.f;
  float _deltaTime                                        = 0.f;
  std::unique_ptr<PerformanceMonitor> _performanceMonitor = nullptr;

  // Focus
  std::function<void()> _onFocus                            = nullptr;
  std::function<void()> _onBlur                             = nullptr;
  std::function<void(PointerEvent* ev)> _onCanvasPointerOut = nullptr;
  std::function<void()> _onCanvasBlur                       = nullptr;
  std::function<void()> _onCanvasFocus                      = nullptr;

  std::function<void()> _onFullscreenChange  = nullptr;
  std::function<void()> _onPointerLockChange = nullptr;

  bool _cachedStencilBuffer;
  unsigned int _cachedStencilFunction;
  unsigned int _cachedStencilMask;
  unsigned int _cachedStencilOperationPass;
  unsigned int _cachedStencilOperationFail;
  unsigned int _cachedStencilOperationDepthFail;
  int _cachedStencilReference;

  /** Extensions */
  std::unique_ptr<MultiviewExtension> _multiviewExtension;
  std::unique_ptr<OcclusionQueryExtension> _occlusionQueryExtension;
  std::unique_ptr<TransformFeedbackExtension> _transformFeedbackExtension;

}; // end of class Engine

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_ENGINE_H
