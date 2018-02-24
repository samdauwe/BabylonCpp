#ifndef BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H
#define BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/lights/shadows/ishadow_generator.h>
#include <babylon/math/isize.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Default implementation of @see IShadowGenerator.
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

public:
  /**
   * @brief Shadow generator mode None: no filtering applied.
   */
  static constexpr unsigned int FILTER_NONE()
  {
    return ShadowGenerator::_FILTER_NONE;
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
   * @brief Shadow generator mode ESM: Exponential Shadow Mapping.
   * (http://developer.download.nvidia.com/presentations/2008/GDC/GDC08_SoftShadowMapping.pdf)
   */
  static constexpr unsigned int FILTER_EXPONENTIALSHADOWMAP()
  {
    return ShadowGenerator::_FILTER_EXPONENTIALSHADOWMAP;
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
   * @brief Creates a ShadowGenerator object.
   * A ShadowGenerator is the required tool to use the shadows.
   * Each light casting shadows needs to use its own ShadowGenerator.
   * Documentation : http://doc.babylonjs.com/tutorials/shadows
   * @param mapSize The size of the texture what stores the shadows. Example :
   * 1024.
   * @param light The light object generating the shadows.
   * @param useFullFloatFirst By default the generator will try to use half
   * float textures but if you need precision (for self shadowing for instance),
   * you can use this option to enforce full float texture.
   */
  ShadowGenerator(int mapSize, IShadowLight* light,
                  bool useFullFloatFirst = false);
  ShadowGenerator(const ISize& mapSize, IShadowLight* light,
                  bool useFullFloatFirst = false);
  virtual ~ShadowGenerator();

  /**
   * @brief Gets the bias: offset applied on the depth preventing acnea.
   */
  float bias() const;

  /**
   * @brief Sets the bias: offset applied on the depth preventing acnea.
   */
  void setBias(float bias);

  /**
   * @brief Gets the blur box offset: offset applied during the blur pass.
   * Only usefull if useKernelBlur = false
   */
  int blurBoxOffset() const;

  /**
   * @brief Sets the blur box offset: offset applied during the blur pass.
   * Only usefull if useKernelBlur = false
   */
  void setBlurBoxOffset(int value);

  /**
   * @brief Gets the blur scale: scale of the blurred texture compared to the
   * main shadow map. 2 means half of the size.
   */
  float blurScale() const;

  /**
   * @brief Sets the blur scale: scale of the blurred texture compared to the
   * main shadow map. 2 means half of the size.
   */
  void setBlurScale(float value);

  /**
   * @brief Gets the blur kernel: kernel size of the blur pass.
   * Only usefull if useKernelBlur = true
   */
  float blurKernel() const;

  /**
   * @brief Sets the blur kernel: kernel size of the blur pass.
   * Only usefull if useKernelBlur = true
   */
  void setBlurKernel(float value);

  /**
   * @brief Gets whether the blur pass is a kernel blur (if true) or box blur.
   * Only usefull in filtered mode (useBlurExponentialShadowMap...)
   */
  bool useKernelBlur() const;

  /**
   * @brief Sets whether the blur pass is a kernel blur (if true) or box blur.
   * Only usefull in filtered mode (useBlurExponentialShadowMap...)
   */
  void setUseKernelBlur(bool value);

  /**
   * @brief Gets the depth scale used in ESM mode.
   */
  float depthScale() const;

  /**
   * @brief Sets the depth scale used in ESM mode.
   * This can override the scale stored on the light.
   */
  void setDepthScale(float value);

  /**
   * @brief Gets the current mode of the shadow generator (normal, PCF, ESM...).
   * The returned value is a number equal to one of the available mode defined
   * in ShadowMap.FILTER_x like _FILTER_NONE
   */
  unsigned int filter() const;

  /**
   * @brief Sets the current mode of the shadow generator (normal, PCF, ESM...).
   * The returned value is a number equal to one of the available mode defined
   * in ShadowMap.FILTER_x like _FILTER_NONE
   */
  void setFilter(unsigned int value);

  /**
   * @brief Gets if the current filter is set to Poisson Sampling aka PCF.
   */
  bool usePoissonSampling() const;

  /**
   * @brief Sets the current filter to Poisson Sampling aka PCF.
   */
  void setUsePoissonSampling(bool value);

  /**
   * @brief Gets if the current filter is set to VSM.
   * DEPRECATED. Should use useExponentialShadowMap instead.
   */
  bool useVarianceShadowMap() const;

  /**
   * @brief Sets the current filter is to VSM.
   * DEPRECATED. Should use useExponentialShadowMap instead.
   */
  void setUseVarianceShadowMap(bool value);

  /**
   * @brief Gets if the current filter is set to blurred VSM.
   * DEPRECATED. Should use useBlurExponentialShadowMap instead.
   */
  bool useBlurVarianceShadowMap() const;

  /**
   * @brief Sets the current filter is to blurred VSM.
   * DEPRECATED. Should use useBlurExponentialShadowMap instead.
   */
  void setUseBlurVarianceShadowMap(bool value);

  /**
   * @brief Gets if the current filter is set to ESM.
   */
  bool useExponentialShadowMap() const;

  /**
   * @brief Sets the current filter is to ESM.
   */
  void setUseExponentialShadowMap(bool value);

  /**
   * @brief Gets if the current filter is set to filtered ESM.
   */
  bool useBlurExponentialShadowMap() const;

  /**
   * @brief Gets if the current filter is set to filtered  ESM.
   */
  void setUseBlurExponentialShadowMap(bool value);

  /**
   * @brief Gets if the current filter is set to "close ESM" (using the inverse
   * of the exponential to prevent steep falloff artifacts).
   */
  bool useCloseExponentialShadowMap() const;

  /**
   * @brief Sets the current filter to "close ESM" (using the inverse of the
   * exponential to prevent steep falloff artifacts).
   */
  void setUseCloseExponentialShadowMap(bool value);

  /**
   * @brief Gets if the current filter is set to filtered "close ESM" (using the
   * inverse of the exponential to prevent steep falloff artifacts).
   */
  bool useBlurCloseExponentialShadowMap() const;

  /**
   * @brief Sets the current filter to fileterd "close ESM" (using the inverse
   * of the exponential to prevent steep falloff artifacts).
   */
  void setUseBlurCloseExponentialShadowMap(bool value);

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
  RenderTargetTexture* getShadowMap() override;

  /**
   * @brief Gets the RTT used during rendering (can be a blurred version of the
   * shadow map or the shadow map itself).
   * @returns The render target texture if the shadow map is present otherwise,
   * null
   */
  RenderTargetTexture* getShadowMapForRendering() override;

  /**
   * @brief Helper function to add a mesh and its descendants to the list of
   * shadow casters.
   * @param mesh Mesh to add
   * @param includeDescendants boolean indicating if the descendants should be
   * added. Default to true
   * @returns the Shadow Generator itself
   */
  ShadowGenerator& addShadowCaster(AbstractMesh* mesh,
                                   bool includeDescendants = true);

  /**
   * @brief Helper function to remove a mesh and its descendants from the list
   * of shadow casters
   * @param mesh Mesh to remove
   * @param includeDescendants boolean indicating if the descendants should be
   * removed. Default to true
   * @returns the Shadow Generator itself
   */
  ShadowGenerator& removeShadowCaster(AbstractMesh* mesh,
                                      bool includeDescendants = true);

  /**
   * @brief Returns the associated light object.
   * @returns the light generating the shadow
   */
  IShadowLight* getLight();

  /**
   * @brief Forces all the attached effect to compile to enable rendering only
   * once ready vs. lazyly compiling effects.
   * @param onCompiled Callback triggered at the and of the effects compilation
   * @param options Sets of optional options forcing the compilation with
   * different modes
   */
  void forceCompilation(
    const ::std::function<void(ShadowGenerator* generator)>& onCompiled,
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
  void prepareDefines(MaterialDefines& defines,
                      unsigned int lightIndex) override;

  /**
   * @brief Binds the shadow related information inside of an effect
   * (information like near, far, darkness... defined in the generator but
   * impacting the effect).
   * @param lightIndex Index of the light in the enabled light list of the
   * material owning the effect
   * @param effect The effect we are binfing the information for
   */
  void bindShadowLight(const string_t& lightIndex, Effect* effect) override;

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
  Json::object serialize() const override;

  /**
   * @brief Parses a serialized ShadowGenerator and returns a new
   * ShadowGenerator.
   * @param parsedShadowGenerator The JSON object to parse
   * @param scene The scene to create the shadow map for
   * @returns The parsed shadow generator
   */
  static ShadowGenerator* Parse(const Json::value& parsedShadowGenerator,
                                Scene* scene);

private:
  void _initializeGenerator();
  void _initializeShadowMap();
  void _initializeBlurRTTAndPostProcesses();
  void _renderForShadowMap(const vector_t<SubMesh*>& opaqueSubMeshes,
                           const vector_t<SubMesh*>& alphaTestSubMeshes,
                           const vector_t<SubMesh*>& transparentSubMeshes,
                           const vector_t<SubMesh*>& depthOnlySubMeshes);
  void _renderSubMeshForShadowMap(SubMesh* subMesh);
  void _applyFilterValues();
  void _disposeBlurPostProcesses();
  void _disposeRTTandPostProcesses();

public:
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
  int _blurBoxOffset;
  float _blurScale;
  float _blurKernel;
  bool _useKernelBlur;
  Nullable<float> _depthScale;
  unsigned int _filter;
  float _darkness;
  bool _transparencyShadow;
  unique_ptr_t<RenderTargetTexture> _shadowMap;
  unique_ptr_t<RenderTargetTexture> _shadowMap2;
  IShadowLight* _light;
  Scene* _scene;
  Vector3 _lightDirection;
  Effect* _effect;
  Matrix _viewMatrix;
  Matrix _projectionMatrix;
  Matrix _transformMatrix;
  bool _cacheInitialized;
  Vector3 _cachedPosition;
  Vector3 _cachedDirection;
  string_t _cachedDefines;
  int _currentRenderID;
  unique_ptr_t<PostProcess> _boxBlurPostprocess;
  unique_ptr_t<PostProcess> _kernelBlurXPostprocess;
  unique_ptr_t<PostProcess> _kernelBlurYPostprocess;
  vector_t<PostProcess*> _blurPostProcesses;
  ISize _mapSize;
  unsigned int _currentFaceIndex;
  unsigned int _currentFaceIndexCache;
  bool _useFullFloat;
  unsigned int _textureType;
  Matrix _defaultTextureMatrix;

}; // end of class ShadowGenerator

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H
