#ifndef BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H
#define BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H

#include <babylon/babylon_api.h>
#include <babylon/lights/shadows/icustom_shader_options.h>
#include <babylon/lights/shadows/ishadow_generator.h>
#include <babylon/math/isize.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class AbstractMesh;
class Effect;
class IShadowLight;
struct ICustomShaderOptions;
class Mesh;
class PostProcess;
class Scene;
class ShadowGenerator;
class SubMesh;
using AbstractMeshPtr    = std::shared_ptr<AbstractMesh>;
using EffectPtr          = std::shared_ptr<Effect>;
using IShadowLightPtr    = std::shared_ptr<IShadowLight>;
using PostProcessPtr     = std::shared_ptr<PostProcess>;
using ShadowGeneratorPtr = std::shared_ptr<ShadowGenerator>;
using SubMeshPtr         = std::shared_ptr<SubMesh>;

/**
 * @brief Default implementation IShadowGenerator.
 * This is the main object responsible of generating shadows in the framework.
 * Documentation: https://doc.babylonjs.com/babylon101/shadows
 */
class BABYLON_SHARED_EXPORT ShadowGenerator : public IShadowGenerator {

private:
  /** Statics **/
  static constexpr unsigned int _FILTER_NONE                          = 0;
  static constexpr unsigned int _FILTER_EXPONENTIALSHADOWMAP          = 1;
  static constexpr unsigned int _FILTER_POISSONSAMPLING               = 2;
  static constexpr unsigned int _FILTER_BLUREXPONENTIALSHADOWMAP      = 3;
  static constexpr unsigned int _FILTER_CLOSEEXPONENTIALSHADOWMAP     = 4;
  static constexpr unsigned int _FILTER_BLURCLOSEEXPONENTIALSHADOWMAP = 5;
  static constexpr unsigned int _FILTER_PCF                           = 6;
  static constexpr unsigned int _FILTER_PCSS                          = 7;

  static constexpr unsigned int _QUALITY_HIGH   = 0;
  static constexpr unsigned int _QUALITY_MEDIUM = 1;
  static constexpr unsigned int _QUALITY_LOW    = 2;

public:
  /**
   * @brief Shadow generator mode None: no filtering applied.
   */
  static constexpr unsigned int FILTER_NONE()
  {
    return ShadowGenerator::_FILTER_NONE;
  }

  /**
   * @brief Shadow generator mode ESM: Exponential Shadow Mapping.
   * (http://developer.download.nvidia.com/presentations/2008/GDC/GDC08_SoftShadowMapping.pdf)
   */
  static constexpr unsigned int FILTER_EXPONENTIALSHADOWMAP()
  {
    return ShadowGenerator::_FILTER_EXPONENTIALSHADOWMAP;
  }

  /**
   * @brief Shadow generator mode Poisson Sampling: Percentage Closer Filtering.
   * (Multiple Tap around evenly distributed around the pixel are used to
   * evaluate the shadow strength)
   */
  static constexpr unsigned int FILTER_POISSONSAMPLING()
  {
    return ShadowGenerator::_FILTER_POISSONSAMPLING;
  }

  /**
   * @brief Shadow generator mode ESM: Blurred Exponential Shadow Mapping.
   * (http://developer.download.nvidia.com/presentations/2008/GDC/GDC08_SoftShadowMapping.pdf)
   */
  static constexpr unsigned int FILTER_BLUREXPONENTIALSHADOWMAP()
  {
    return ShadowGenerator::_FILTER_BLUREXPONENTIALSHADOWMAP;
  }

  /**
   * @brief Shadow generator mode ESM: Exponential Shadow Mapping using the
   * inverse of the exponential preventing edge artifacts on steep falloff.
   * (http://developer.download.nvidia.com/presentations/2008/GDC/GDC08_SoftShadowMapping.pdf)
   */
  static constexpr unsigned int FILTER_CLOSEEXPONENTIALSHADOWMAP()
  {
    return ShadowGenerator::_FILTER_CLOSEEXPONENTIALSHADOWMAP;
  }

  /**
   * @brief Shadow generator mode ESM: Blurred Exponential Shadow Mapping using
   * the inverse of the exponential preventing edge artifacts on steep falloff.
   * (http://developer.download.nvidia.com/presentations/2008/GDC/GDC08_SoftShadowMapping.pdf)
   */
  static constexpr unsigned int FILTER_BLURCLOSEEXPONENTIALSHADOWMAP()
  {
    return ShadowGenerator::_FILTER_BLURCLOSEEXPONENTIALSHADOWMAP;
  }

  /**
   * @brief Shadow generator mode PCF: Percentage Closer Filtering.
   * benefits from Webgl 2 shadow samplers. Fallback to Poisson Sampling in
   * Webgl 1 (https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch11.html)
   */
  static constexpr unsigned int FILTER_PCF()
  {
    return ShadowGenerator::_FILTER_PCF;
  }

  /**
   * @brief Shadow generator mode PCSS: Percentage Closering Soft Shadow.
   * benefits from Webgl 2 shadow samplers. Fallback to Poisson Sampling in
   * Webgl 1 Contact Hardening
   */
  static constexpr unsigned int FILTER_PCSS()
  {
    return ShadowGenerator::_FILTER_PCSS;
  }

  /**
   * @brief Reserved for PCF and PCSS.
   * Highest Quality.
   *
   * Execute PCF on a 5*5 kernel improving a lot the shadow aliasing artifacts.
   *
   * Execute PCSS with 32 taps blocker search and 64 taps PCF.
   */
  static constexpr unsigned int QUALITY_HIGH()
  {
    return ShadowGenerator::_QUALITY_HIGH;
  }

  /**
   * @brief Reserved for PCF and PCSS.
   * Good tradeoff for quality/perf cross devices
   *
   * Execute PCF on a 3*3 kernel.
   *
   * Execute PCSS with 16 taps blocker search and 32 taps PCF.
   */
  static constexpr unsigned int QUALITY_MEDIUM()
  {
    return ShadowGenerator::_QUALITY_MEDIUM;
  }

  /**
   * @brief Reserved for PCF and PCSS.
   * The lowest quality but the fastest.
   *
   * Execute PCF on a 1*1 kernel.
   *
   * Execute PCSS with 16 taps blocker search and 16 taps PCF.
   */
  static constexpr unsigned int QUALITY_LOW()
  {
    return ShadowGenerator::_QUALITY_LOW;
  }

  template <typename... Ts>
  static ShadowGeneratorPtr New(Ts&&... args)
  {
    auto shadowGenerator
      = std::shared_ptr<ShadowGenerator>(new ShadowGenerator(std::forward<Ts>(args)...));
    shadowGenerator->addToLight(shadowGenerator);

    return shadowGenerator;
  }
  ~ShadowGenerator() override; // = default

  void addToLight(const ShadowGeneratorPtr& shadowGenerator);

  /**
   * @brief Returns the darkness value (float). This can only decrease the
   * actual darkness of a shadow. 0 means strongest and 1 would means no shadow.
   * @returns the darkness.
   */
  float getDarkness() const;

  /**
   * @brief Sets the darkness value (float). This can only decrease the actual
   * darkness of a shadow.
   * @param darkness The darkness value 0 means strongest and 1 would means no
   * shadow.
   * @returns the shadow generator allowing fluent coding.
   */
  ShadowGenerator& setDarkness(float darkness);

  /**
   * @brief Sets the ability to have transparent shadow (boolean).
   * @param transparent True if transparent else False
   * @returns the shadow generator allowing fluent coding
   */
  ShadowGenerator& setTransparencyShadow(bool transparent);

  /**
   * @brief Gets the main RTT containing the shadow map (usually storing depth
   * from the light point of view).
   * @returns The render target texture if present otherwise, null
   */
  RenderTargetTexturePtr getShadowMap() override;

  /**
   * @brief Gets the RTT used during rendering (can be a blurred version of the
   * shadow map or the shadow map itself).
   * @returns The render target texture if the shadow map is present otherwise,
   * null
   */
  RenderTargetTexturePtr getShadowMapForRendering() override;

  /**
   * @brief Gets the class name of that object.
   * @returns "ShadowGenerator"
   */
  std::string getClassName() const;

  /**
   * @brief Helper function to add a mesh and its descendants to the list of
   * shadow casters.
   * @param mesh Mesh to add
   * @param includeDescendants boolean indicating if the descendants should be
   * added. Default to true
   * @returns the Shadow Generator itself
   */
  ShadowGenerator& addShadowCaster(const AbstractMeshPtr& mesh, bool includeDescendants = true);

  /**
   * @brief Helper function to remove a mesh and its descendants from the list
   * of shadow casters
   * @param mesh Mesh to remove
   * @param includeDescendants boolean indicating if the descendants should be
   * removed. Default to true
   * @returns the Shadow Generator itself
   */
  ShadowGenerator& removeShadowCaster(const AbstractMeshPtr& mesh, bool includeDescendants = true);

  /**
   * @brief Returns the associated light object.
   * @returns the light generating the shadow
   */
  IShadowLightPtr& getLight();

  /**
   * @brief Forces all the attached effect to compile to enable rendering only
   * once ready vs. lazyly compiling effects.
   * @param onCompiled Callback triggered at the and of the effects compilation
   * @param options Sets of optional options forcing the compilation with
   * different modes
   */
  void forceCompilation(const std::function<void(ShadowGenerator* generator)>& onCompiled,
                        const ShadowGeneratorCompileOptions& options) override;

  /**
   * @brief Determine wheter the shadow generator is ready or not (mainly all
   * effects and related post processes needs to be ready).
   * @param subMesh The submesh we want to render in the shadow map
   * @param useInstances Defines wether will draw in the map using instances
   * @returns true if ready otherwise, false
   */
  bool isReady(SubMesh* subMesh, bool useInstances) override;

  /**
   * @brief Prepare all the defines in a material relying on a shadow map at the
   * specified light index.
   * @param defines Defines of the material we want to update
   * @param lightIndex Index of the light in the enabled light list of the
   * material
   */
  void prepareDefines(MaterialDefines& defines, unsigned int lightIndex) override;

  /**
   * @brief Binds the shadow related information inside of an effect
   * (information like near, far, darkness... defined in the generator but
   * impacting the effect).
   * @param lightIndex Index of the light in the enabled light list of the
   * material owning the effect
   * @param effect The effect we are binfing the information for
   */
  void bindShadowLight(const std::string& lightIndex, const EffectPtr& effect) override;

  /**
   * @brief Gets the transformation matrix used to project the meshes into the
   * map from the light point of view. (eq to shadow prjection matrix * light
   * transform matrix)
   * @returns The transform matrix used to create the shadow map
   */
  Matrix getTransformMatrix() override;

  /**
   * @brief Recreates the shadow map dependencies like RTT and post processes.
   * This can be used during the switch between Cube and 2D textures for
   * instance.
   */
  void recreateShadowMap() override;

  /**
   * @brief Disposes the ShadowGenerator.
   * @returns Nothing.
   */
  void dispose() override;

  /**
   * @brief Serializes the shadow generator setup to a json object.
   * @returns The serialized JSON object
   */
  json serialize() const override;

  /**
   * @brief Parses a serialized ShadowGenerator and returns a new
   * ShadowGenerator.
   * @param parsedShadowGenerator The JSON object to parse
   * @param scene The scene to create the shadow map for
   * @returns The parsed shadow generator
   */
  static ShadowGenerator* Parse(const json& parsedShadowGenerator, Scene* scene);

protected:
  /**
   * @brief Creates a ShadowGenerator object.
   * A ShadowGenerator is the required tool to use the shadows.
   * Each light casting shadows needs to use its own ShadowGenerator.
   * Documentation : https://doc.babylonjs.com/babylon101/shadows
   * @param mapSize The size of the texture what stores the shadows. Example :
   * 1024.
   * @param light The light object generating the shadows.
   * @param usefulFloatFirst By default the generator will try to use half float
   * textures but if you need precision (for self shadowing for instance), you
   * can use this option to enforce full float texture.
   */
  ShadowGenerator(int mapSize, const IShadowLightPtr& light, bool usefulFloatFirst = false);
  ShadowGenerator(const ISize& mapSize, const IShadowLightPtr& light,
                  bool usefulFloatFirst = false);

  /**
   * @brief Gets the bias: offset applied on the depth preventing acnea (in
   * light direction).
   */
  float get_bias() const;

  /**
   * @brief Sets the bias: offset applied on the depth preventing acnea (in
   * light direction).
   */
  void set_bias(float bias);

  /**
   * @brief Gets the normalBias: offset applied on the depth preventing acnea
   * (along side the normal direction and proportinal to the light/normal
   * angle).
   */
  float get_normalBias() const;

  /**
   * @brief Sets the normalBias: offset applied on the depth preventing acnea
   * (along side the normal direction and proportinal to the light/normal
   * angle).
   */
  void set_normalBias(float normalBias);

  /**
   * @brief Gets the blur box offset: offset applied during the blur pass.
   * Only useful if useKernelBlur = false
   */
  int get_blurBoxOffset() const;

  /**
   * @brief Sets the blur box offset: offset applied during the blur pass.
   * Only useful if useKernelBlur = false
   */
  void set_blurBoxOffset(int value);

  /**
   * @brief Gets the blur scale: scale of the blurred texture compared to the
   * main shadow map. 2 means half of the size.
   */
  float get_blurScale() const;

  /**
   * @brief Sets the blur scale: scale of the blurred texture compared to the
   * main shadow map. 2 means half of the size.
   */
  void set_blurScale(float value);

  /**
   * @brief Gets the blur kernel: kernel size of the blur pass.
   * Only useful if useKernelBlur = true
   */
  float get_blurKernel() const;

  /**
   * @brief Sets the blur kernel: kernel size of the blur pass.
   * Only useful if useKernelBlur = true
   */
  void set_blurKernel(float value);

  /**
   * @brief Gets whether the blur pass is a kernel blur (if true) or box blur.
   * Only useful in filtered mode (useBlurExponentialShadowMap...)
   */
  bool get_useKernelBlur() const;

  /**
   * @brief Sets whether the blur pass is a kernel blur (if true) or box blur.
   * Only useful in filtered mode (useBlurExponentialShadowMap...)
   */
  void set_useKernelBlur(bool value);

  /**
   * @brief Gets the depth scale used in ESM mode.
   */
  float get_depthScale() const;

  /**
   * @brief Sets the depth scale used in ESM mode.
   * This can override the scale stored on the light.
   */
  void set_depthScale(float value);

  /**
   * @brief Gets the current mode of the shadow generator (normal, PCF, ESM...).
   * The returned value is a number equal to one of the available mode defined
   * in ShadowMap.FILTER_x like _FILTER_NONE
   */
  unsigned int get_filter() const;

  /**
   * @brief Sets the current mode of the shadow generator (normal, PCF, ESM...).
   * The returned value is a number equal to one of the available mode defined
   * in ShadowMap.FILTER_x like _FILTER_NONE
   */
  void set_filter(unsigned int value);

  /**
   * @brief Gets if the current filter is set to Poisson Sampling.
   */
  bool get_usePoissonSampling() const;

  /**
   * @brief Sets the current filter to Poisson Sampling.
   */
  void set_usePoissonSampling(bool value);

  /**
   * @brief Gets if the current filter is set to ESM.
   */
  bool get_useExponentialShadowMap() const;

  /**
   * @brief Sets the current filter is to ESM.
   */
  void set_useExponentialShadowMap(bool value);

  /**
   * @brief Gets if the current filter is set to filtered ESM.
   */
  bool get_useBlurExponentialShadowMap() const;

  /**
   * @brief Gets if the current filter is set to filtered  ESM.
   */
  void set_useBlurExponentialShadowMap(bool value);

  /**
   * @brief Gets if the current filter is set to "close ESM" (using the inverse
   * of the exponential to prevent steep falloff artifacts).
   */
  bool get_useCloseExponentialShadowMap() const;

  /**
   * @brief Sets the current filter to "close ESM" (using the inverse of the
   * exponential to prevent steep falloff artifacts).
   */
  void set_useCloseExponentialShadowMap(bool value);

  /**
   * @brief Gets if the current filter is set to filtered "close ESM" (using the
   * inverse of the exponential to prevent steep falloff artifacts).
   */
  bool get_useBlurCloseExponentialShadowMap() const;

  /**
   * @brief Sets the current filter to filtered "close ESM" (using the inverse
   * of the exponential to prevent steep falloff artifacts).
   */
  void set_useBlurCloseExponentialShadowMap(bool value);

  /**
   * @brief Gets if the current filter is set to "PCF" (percentage closer
   * filtering).
   */
  bool get_usePercentageCloserFiltering() const;

  /**
   * @brief Sets the current filter to "PCF" (percentage closer filtering).
   */
  void set_usePercentageCloserFiltering(bool value);

  /**
   * @brief Gets the PCF or PCSS Quality.
   * Only valid if usePercentageCloserFiltering or usePercentageCloserFiltering
   * is true.
   */
  unsigned int get_filteringQuality() const;

  /**
   * @brief Sets the PCF or PCSS Quality.
   * Only valid if usePercentageCloserFiltering or usePercentageCloserFiltering
   * is true.
   */
  void set_filteringQuality(unsigned int filteringQuality);

  /**
   * @brief Gets if the current filter is set to "PCSS" (contact hardening).
   */
  bool get_useContactHardeningShadow() const;

  /**
   * @brief Sets the current filter to "PCSS" (contact hardening).
   */
  void set_useContactHardeningShadow(bool value);

  /**
   * @brief Gets the Light Size (in shadow map uv unit) used in PCSS to
   * determine the blocker search area and the penumbra size. Using a ratio
   * helps keeping shape stability independently of the map size.
   *
   * It does not account for the light projection as it was having too much
   * instability during the light setup or during light position changes.
   *
   * Only valid if useContactHardeningShadow is true.
   */
  float get_contactHardeningLightSizeUVRatio() const;

  /**
   * @brief Sets the Light Size (in shadow map uv unit) used in PCSS to
   * determine the blocker search area and the penumbra size. Using a ratio
   * helps keeping shape stability independently of the map size.
   *
   * It does not account for the light projection as it was having too much
   * instability during the light setup or during light position changes.
   *
   * Only valid if useContactHardeningShadow is true.
   */
  void set_contactHardeningLightSizeUVRatio(float contactHardeningLightSizeUVRatio);

  /**
   * @brief Gets the actual darkness of a shadow.
   */
  float get_darkness() const;

  /**
   * @brief Sets the actual darkness of a shadow.
   */
  void set_darkness(float value);

  /**
   * @brief Gets the ability to have transparent shadow.
   */
  bool get_transparencyShadow() const;

  /**
   * @brief Sets the ability to have transparent shadow.
   */
  void set_transparencyShadow(bool value);

private:
  void _initializeGenerator();
  void _initializeShadowMap();
  void _initializeBlurRTTAndPostProcesses();
  void _renderForShadowMap(const std::vector<SubMesh*>& opaqueSubMeshes,
                           const std::vector<SubMesh*>& alphaTestSubMeshes,
                           const std::vector<SubMesh*>& transparentSubMeshes,
                           const std::vector<SubMesh*>& depthOnlySubMeshes);
  void _renderSubMeshForShadowMap(SubMesh* subMesh);
  void _applyFilterValues();
  void _disposeBlurPostProcesses();
  void _disposeRTTandPostProcesses();

public:
  /**
   * Gets or sets the custom shader name to use
   */
  std::optional<ICustomShaderOptions> customShaderOptions;

  /**
   * Observable triggered before the shadow is rendered. Can be used to update
   * internal effect state
   */
  Observable<Effect> onBeforeShadowMapRenderObservable;

  /**
   * Observable triggered after the shadow is rendered. Can be used to restore
   * internal effect state
   */
  Observable<Effect> onAfterShadowMapRenderObservable;

  /**
   * Observable triggered before a mesh is rendered in the shadow map.
   * Can be used to update internal effect state (that you can get from the
   * onBeforeShadowMapRenderObservable)
   */
  Observable<Mesh> onBeforeShadowMapRenderMeshObservable;

  /**
   * Observable triggered after a mesh is rendered in the shadow map.
   * Can be used to update internal effect state (that you can get from the
   * onAfterShadowMapRenderObservable)
   */
  Observable<Mesh> onAfterShadowMapRenderMeshObservable;

  /**
   * Gets the bias: offset applied on the depth preventing acnea (in light
   * direction)
   */
  Property<ShadowGenerator, float> bias;

  /**
   * The normalBias: offset applied on the depth preventing acnea (along side
   * the normal direction and proportinal to the light/normal angle)
   */
  Property<ShadowGenerator, float> normalBias;

  /**
   * The blur box offset: offset applied during the blur pass
   * Only useful if useKernelBlur = false
   */
  Property<ShadowGenerator, int> blurBoxOffset;

  /**
   * The blur scale: scale of the blurred texture compared to the main
   * shadow map. 2 means half of the size
   */
  Property<ShadowGenerator, float> blurScale;

  /**
   * The blur kernel: kernel size of the blur pass
   * Only useful if useKernelBlur = true
   */
  Property<ShadowGenerator, float> blurKernel;

  /**
   * Whether the blur pass is a kernel blur (if true) or box blur
   * Only useful in filtered mode (useBlurExponentialShadowMap...)
   */
  Property<ShadowGenerator, bool> useKernelBlur;

  /**
   * The depth scale used in ESM mode
   */
  Property<ShadowGenerator, float> depthScale;

  /**
   * The current mode of the shadow generator (normal, PCF, ESM...)
   * The returned value is a number equal to one of the available mode defined
   * in ShadowMap.FILTER_x like _FILTER_NONE
   */
  Property<ShadowGenerator, unsigned int> filter;

  /**
   * If the current filter is set to Poisson Sampling
   */
  Property<ShadowGenerator, bool> usePoissonSampling;

  /**
   * If the current filter is set to ESM
   */
  Property<ShadowGenerator, bool> useExponentialShadowMap;

  /**
   * If the current filter is set to filtered ESM
   */
  Property<ShadowGenerator, bool> useBlurExponentialShadowMap;

  /**
   * If the current filter is set to "close ESM" (using the inverse of the
   * exponential to prevent steep falloff artifacts).
   */
  Property<ShadowGenerator, bool> useCloseExponentialShadowMap;

  /**
   * If the current filter is set to filtered "close ESM" (using the
   * inverse of the exponential to prevent steep falloff artifacts).
   */
  Property<ShadowGenerator, bool> useBlurCloseExponentialShadowMap;

  /**
   * The current filter is set to "PCF" (percentage closer filtering).
   */
  Property<ShadowGenerator, bool> usePercentageCloserFiltering;

  /**
   * The PCF or PCSS Quality
   * Only valid if usePercentageCloserFiltering or usePercentageCloserFiltering
   * is true.
   */
  Property<ShadowGenerator, unsigned int> filteringQuality;

  /**
   * The current filter is set to "PCSS" (contact hardening)
   */
  Property<ShadowGenerator, bool> useContactHardeningShadow;

  /**
   * The Light Size (in shadow map uv unit) used in PCSS to determine the
   * blocker search area and the penumbra size. Using a ratio helps keeping
   * shape stability independently of the map size
   *
   * It does not account for the light projection as it was having too much
   * instability during the light setup or during light position changes.
   *
   * Only valid if useContactHardeningShadow is true.
   */
  Property<ShadowGenerator, float> contactHardeningLightSizeUVRatio;

  /**
   * Gets or sets the actual darkness of a shadow
   */
  Property<ShadowGenerator, float> darkness;

  /**
   * Gets or sets the ability to have transparent shadow
   */
  Property<ShadowGenerator, bool> transparencyShadow;

  /**
   * If true the shadow map is generated by rendering the back face of the mesh
   * instead of the front face. This can help with self-shadowing as the
   * geometry making up the back of objects is slightly offset. It might on the
   * other hand introduce peter panning.
   */
  float frustumEdgeFalloff;
  bool forceBackFacesOnly;

private:
  float _bias;
  float _normalBias;
  int _blurBoxOffset;
  float _blurScale;
  float _blurKernel;
  bool _useKernelBlur;
  std::optional<float> _depthScale;
  unsigned int _filter;
  unsigned int _filteringQuality;
  float _contactHardeningLightSizeUVRatio;
  float _darkness;
  bool _transparencyShadow;
  RenderTargetTexturePtr _shadowMap;
  RenderTargetTexturePtr _shadowMap2;
  IShadowLightPtr _light;
  Scene* _scene;
  Vector3 _lightDirection;
  EffectPtr _effect;
  Matrix _viewMatrix;
  Matrix _projectionMatrix;
  Matrix _transformMatrix;
  bool _cacheInitialized;
  Vector3 _cachedPosition;
  Vector3 _cachedDirection;
  std::string _cachedDefines;
  int _currentRenderID;
  PostProcessPtr _boxBlurPostprocess;
  PostProcessPtr _kernelBlurXPostprocess;
  PostProcessPtr _kernelBlurYPostprocess;
  std::vector<PostProcessPtr> _blurPostProcesses;
  ISize _mapSize;
  unsigned int _currentFaceIndex;
  unsigned int _currentFaceIndexCache;
  bool _useFullFloat;
  unsigned int _textureType;
  Matrix _defaultTextureMatrix;

}; // end of class ShadowGenerator

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H
